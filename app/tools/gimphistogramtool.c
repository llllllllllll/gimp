/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#ifdef __GNUC__
#warning GTK_DISABLE_DEPRECATED
#endif
#undef GTK_DISABLE_DEPRECATED

#include <gtk/gtk.h>

#include "libgimpmath/gimpmath.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "tools-types.h"

#include "base/gimphistogram.h"
#include "base/pixel-region.h"

#include "core/gimpdrawable.h"
#include "core/gimpimage.h"
#include "core/gimptoolinfo.h"

#include "widgets/gimpenummenu.h"
#include "widgets/gimphistogrambox.h"
#include "widgets/gimphistogramview.h"
#include "widgets/gimpviewabledialog.h"

#include "display/gimpdisplay.h"

#include "gimphistogramtool.h"
#include "tool_manager.h"

#include "app_procs.h"

#include "libgimp/gimpintl.h"


#define TEXT_WIDTH      45
#define GRADIENT_HEIGHT 15


typedef struct _HistogramToolDialog HistogramToolDialog;

struct _HistogramToolDialog
{
  GtkWidget            *shell;

  GtkWidget            *info_labels[6];
  GtkWidget            *channel_menu;
  GimpHistogramBox     *histogram_box;
  GimpHistogram        *hist;
  GtkWidget            *gradient;

  gdouble               mean;
  gdouble               std_dev;
  gdouble               median;
  gdouble               pixels;
  gdouble               count;
  gdouble               percentile;

  GimpDrawable         *drawable;
  GimpHistogramChannel  channel;
  gboolean              color;
};


/*  local function prototypes  */

static void   gimp_histogram_tool_class_init (GimpHistogramToolClass *klass);
static void   gimp_histogram_tool_init       (GimpHistogramTool      *hist_tool);

static void   gimp_histogram_tool_initialize (GimpTool             *tool,
					      GimpDisplay          *gdisp);
static void   gimp_histogram_tool_control    (GimpTool             *tool,
					      GimpToolAction        action,
					      GimpDisplay          *gdisp);

static HistogramToolDialog *  histogram_tool_dialog_new (GimpToolInfo *tool_info);

static void   histogram_tool_close_callback   (GtkWidget            *widget,
					       gpointer              data);
static gboolean histogram_set_sensitive_callback 
                                              (gpointer              item_data,
                                               HistogramToolDialog  *htd);
static void   histogram_tool_channel_callback (GtkWidget            *widget,
					       gpointer              data);
static void   histogram_tool_gradient_draw    (GtkWidget            *gradient,
					       GimpHistogramChannel  channel);

static void   histogram_tool_dialog_update    (HistogramToolDialog  *htd,
					       gint                  start,
					       gint                  end);

static void   histogram_tool_histogram_range  (GimpHistogramView    *view,
                                               gint                  start,
                                               gint                  end,
                                               gpointer              data);


static HistogramToolDialog * histogram_dialog = NULL;

static GimpToolClass *parent_class = NULL;


/*  public functions  */

void
gimp_histogram_tool_register (GimpToolRegisterCallback  callback,
                              gpointer                  data)
{
  (* callback) (GIMP_TYPE_HISTOGRAM_TOOL,
                NULL,
                FALSE,
                "gimp-histogram-tool",
                _("Histogram"),
                _("View image histogram"),
                N_("/Layer/Colors/Histogram..."), NULL,
                NULL, "tools/histogram.html",
                GIMP_STOCK_TOOL_HISTOGRAM,
                data);
}

GType
gimp_histogram_tool_get_type (void)
{
  static GType tool_type = 0;

  if (! tool_type)
    {
      static const GTypeInfo tool_info =
      {
        sizeof (GimpHistogramToolClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gimp_histogram_tool_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data     */
        sizeof (GimpHistogramTool),
        0,              /* n_preallocs    */
        (GInstanceInitFunc) gimp_histogram_tool_init,
      };

      tool_type = g_type_register_static (GIMP_TYPE_IMAGE_MAP_TOOL,
                                          "GimpHistogramTool", 
                                          &tool_info, 0);
    }

  return tool_type;
}

static void
gimp_histogram_tool_class_init (GimpHistogramToolClass *klass)
{
  GimpToolClass *tool_class;

  tool_class = GIMP_TOOL_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  tool_class->initialize = gimp_histogram_tool_initialize;
  tool_class->control    = gimp_histogram_tool_control;
}

static void
gimp_histogram_tool_init (GimpHistogramTool *hist_tool)
{
}

static void
gimp_histogram_tool_initialize (GimpTool    *tool,
				GimpDisplay *gdisp)
{
  GimpDrawable *drawable;
  PixelRegion   PR;

  if (! gdisp)
    {
      if (histogram_dialog)
        histogram_tool_close_callback (NULL, histogram_dialog);
      return;
    }

  if (gimp_drawable_is_indexed (gimp_image_active_drawable (gdisp->gimage)))
    {
      g_message (_("Histogram does not operate on indexed drawables."));
      return;
    }

  /*  The histogram_tool dialog  */
  if (! histogram_dialog)
    histogram_dialog = histogram_tool_dialog_new (tool->tool_info);

  drawable = gimp_image_active_drawable (gdisp->gimage);

  gimp_viewable_dialog_set_viewable (GIMP_VIEWABLE_DIALOG (histogram_dialog->shell),
                                     GIMP_VIEWABLE (drawable));

  gtk_widget_show (histogram_dialog->shell);

  histogram_dialog->drawable = drawable;
  histogram_dialog->color    = gimp_drawable_is_rgb (drawable);

  gimp_option_menu_set_sensitive (GTK_OPTION_MENU (histogram_dialog->channel_menu),
                                  (GimpOptionMenuSensitivityCallback) histogram_set_sensitive_callback,
                                  histogram_dialog);
  gimp_option_menu_set_history (GTK_OPTION_MENU (histogram_dialog->channel_menu),
                                GINT_TO_POINTER (histogram_dialog->channel));

  /* calculate the histogram */
  pixel_region_init (&PR, gimp_drawable_data (drawable),
		     0, 0,
		     gimp_drawable_width (drawable),
		     gimp_drawable_height (drawable),
		     FALSE);
  gimp_histogram_calculate (histogram_dialog->hist, &PR, NULL);

  gimp_histogram_view_update (histogram_dialog->histogram_box->histogram,
                              histogram_dialog->hist);
  gimp_histogram_view_set_range (histogram_dialog->histogram_box->histogram,
                                 0, 255);
}

static void
gimp_histogram_tool_control (GimpTool       *tool,
			     GimpToolAction  action,
			     GimpDisplay    *gdisp)
{
  switch (action)
    {
    case PAUSE:
      break;

    case RESUME:
      break;

    case HALT:
      if (histogram_dialog)
        histogram_tool_close_callback (NULL, histogram_dialog);
      break;

    default:
      break;
    }

  GIMP_TOOL_CLASS (parent_class)->control (tool, action, gdisp);
}


/*  histogram_tool machinery  */

static void
histogram_tool_histogram_range (GimpHistogramView *widget,
				gint               start,
				gint               end,
				gpointer           data)
{
  HistogramToolDialog *htd;
  gdouble              pixels;
  gdouble              count;

  htd = (HistogramToolDialog *) data;

  if (htd == NULL || htd->hist == NULL ||
      gimp_histogram_nchannels (htd->hist) <= 0)
    return;

  pixels = gimp_histogram_get_count (htd->hist, 0, 255);
  count  = gimp_histogram_get_count (htd->hist, start, end);

  htd->mean       = gimp_histogram_get_mean (htd->hist, htd->channel,
					     start, end);
  htd->std_dev    = gimp_histogram_get_std_dev (htd->hist, htd->channel,
						start, end);
  htd->median     = gimp_histogram_get_median (htd->hist, htd->channel,
					       start, end);
  htd->pixels     = pixels;
  htd->count      = count;
  htd->percentile = count / pixels;

  if (htd->shell)
    histogram_tool_dialog_update (htd, start, end);
}

static void
histogram_tool_dialog_update (HistogramToolDialog *htd,
			      gint                 start,
			      gint                 end)
{
  gchar text[12];

  /*  mean  */
  g_snprintf (text, sizeof (text), "%3.1f", htd->mean);
  gtk_label_set_text (GTK_LABEL (htd->info_labels[0]), text);

  /*  std dev  */
  g_snprintf (text, sizeof (text), "%3.1f", htd->std_dev);
  gtk_label_set_text (GTK_LABEL (htd->info_labels[1]), text);

  /*  median  */
  g_snprintf (text, sizeof (text), "%3.1f", htd->median);
  gtk_label_set_text (GTK_LABEL (htd->info_labels[2]), text);

  /*  pixels  */
  g_snprintf (text, sizeof (text), "%8.1f", htd->pixels);
  gtk_label_set_text (GTK_LABEL (htd->info_labels[3]), text);

  /*  count  */
  g_snprintf (text, sizeof (text), "%8.1f", htd->count);
  gtk_label_set_text (GTK_LABEL (htd->info_labels[4]), text);

  /*  percentile  */
  g_snprintf (text, sizeof (text), "%2.2f", htd->percentile * 100);
  gtk_label_set_text (GTK_LABEL (htd->info_labels[5]), text);
}

/***************************/
/*  Histogram Tool dialog  */
/***************************/

static HistogramToolDialog *
histogram_tool_dialog_new (GimpToolInfo *tool_info)
{
  HistogramToolDialog *htd;
  GtkWidget *hbox;
  GtkWidget *vbox;
  GtkWidget *frame;
  GtkWidget *table;
  GtkWidget *label;
  gint       i;
  gint       x, y;

  static const gchar *histogram_info_names[] =
  {
    N_("Mean:"),
    N_("Std Dev:"),
    N_("Median:"),
    N_("Pixels:"),
    N_("Count:"),
    N_("Percentile:")
  };

  htd = g_new0 (HistogramToolDialog, 1);
  htd->channel = GIMP_HISTOGRAM_VALUE;
  htd->hist    = gimp_histogram_new ();

  /*  The shell and main vbox  */
  htd->shell =
    gimp_viewable_dialog_new (NULL,
                              tool_info->blurb,
                              GIMP_OBJECT (tool_info)->name,
                              tool_info->stock_id,
                              _("View Image Histogram"),
                              tool_manager_help_func, NULL,

                              GTK_STOCK_CLOSE, histogram_tool_close_callback,
                              htd, NULL, NULL, TRUE, TRUE,

                              NULL);

  hbox = gtk_hbox_new (TRUE, 0);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (htd->shell)->vbox), hbox);
  gtk_widget_show (hbox);

  vbox = gtk_vbox_new (FALSE, 4);
  gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);;
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, FALSE, 0);
  gtk_widget_show (vbox);

  /*  The option menu for selecting channels  */
  hbox = gtk_hbox_new (FALSE, 6);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);
  gtk_widget_show (hbox);

  label = gtk_label_new (_("Information on Channel:"));
  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
  gtk_widget_show (label);

  htd->channel_menu = 
    gimp_enum_option_menu_new (GIMP_TYPE_HISTOGRAM_CHANNEL,
                               G_CALLBACK (histogram_tool_channel_callback),
                               htd);
  gtk_box_pack_start (GTK_BOX (hbox), htd->channel_menu, FALSE, FALSE, 0);
  gtk_widget_show (htd->channel_menu);

  /*  The histogram tool histogram  */
  htd->histogram_box =
    GIMP_HISTOGRAM_BOX (gimp_histogram_box_new (_("Intensity Range:")));
  gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (htd->histogram_box),
                      FALSE, FALSE, 0);
  gtk_widget_show (GTK_WIDGET (htd->histogram_box));

  g_signal_connect (G_OBJECT (htd->histogram_box->histogram), "range_changed",
                    G_CALLBACK (histogram_tool_histogram_range),
                    htd);

  /*  The gradient below the histogram */
  frame = gtk_frame_new (NULL);
  gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_IN);
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  htd->gradient = gtk_preview_new (GTK_PREVIEW_COLOR);
  gtk_preview_size (GTK_PREVIEW (htd->gradient), 
		    GIMP_HISTOGRAM_VIEW_WIDTH, GRADIENT_HEIGHT);
  gtk_container_add (GTK_CONTAINER (frame), GTK_WIDGET(htd->gradient));
  gtk_widget_show (htd->gradient);

  histogram_tool_gradient_draw (htd->gradient, GIMP_HISTOGRAM_VALUE);

  /*  The table containing histogram information  */
  table = gtk_table_new (3, 4, TRUE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 6);
  gtk_box_pack_start (GTK_BOX (vbox), table, FALSE, FALSE, 0);
  gtk_widget_show (table);

  /*  the labels for histogram information  */
  for (i = 0; i < 6; i++)
    {
      y = (i % 3);
      x = (i / 3) * 2;

      label = gtk_label_new (gettext(histogram_info_names[i]));
      gtk_misc_set_alignment (GTK_MISC (label), 1.0, 0.5);
      gtk_table_attach (GTK_TABLE (table), label, x, x + 1, y, y + 1,
			GTK_FILL, GTK_FILL, 2, 2);
      gtk_widget_show (label);

      htd->info_labels[i] = gtk_label_new ("0");
      gtk_misc_set_alignment (GTK_MISC (htd->info_labels[i]), 0.0, 0.5);
      gtk_table_attach (GTK_TABLE (table), htd->info_labels[i],
			x + 1, x + 2, y, y + 1,
			GTK_FILL, GTK_FILL, 2, 2);
      gtk_widget_show (htd->info_labels[i]);
    }

  return htd;
}

static void
histogram_tool_close_callback (GtkWidget *widget,
			       gpointer   data)
{
  HistogramToolDialog *htd;
  GimpTool            *active_tool;

  htd = (HistogramToolDialog *) data;

  gtk_widget_hide (htd->shell);

  active_tool = tool_manager_get_active (the_gimp);

  active_tool->gdisp    = NULL;
  active_tool->drawable = NULL;
}

static void
histogram_tool_channel_callback (GtkWidget *widget,
				 gpointer   data)
{
  HistogramToolDialog *htd;

  htd = (HistogramToolDialog *) data;

  gimp_menu_item_update (widget, &htd->channel);

  gimp_histogram_view_channel (htd->histogram_box->histogram, htd->channel);
  histogram_tool_gradient_draw (htd->gradient, htd->channel);
}

static gboolean
histogram_set_sensitive_callback (gpointer             item_data,
                                  HistogramToolDialog *htd)
{
  GimpHistogramChannel  channel = GPOINTER_TO_INT (item_data);
  
  switch (channel)
    {
    case GIMP_HISTOGRAM_VALUE:
      return TRUE;
    case GIMP_HISTOGRAM_RED:
    case GIMP_HISTOGRAM_GREEN:
    case GIMP_HISTOGRAM_BLUE:
      return htd->color;
    case GIMP_HISTOGRAM_ALPHA:
      return gimp_drawable_has_alpha (htd->drawable);
    }
  
  return FALSE;
}

static void
histogram_tool_gradient_draw (GtkWidget            *gradient,
			      GimpHistogramChannel  channel)
{
  guchar buf[GIMP_HISTOGRAM_VIEW_WIDTH * 3];
  guchar r, g, b;
  gint   i;

  r = g = b = 0;
  switch (channel)
    {
    case GIMP_HISTOGRAM_VALUE:
    case GIMP_HISTOGRAM_ALPHA:  r = g = b = 1;
      break;
    case GIMP_HISTOGRAM_RED:    r = 1;
      break;
    case GIMP_HISTOGRAM_GREEN:  g = 1;
      break;
    case GIMP_HISTOGRAM_BLUE:   b = 1;
      break;
    default:
      g_warning ("unknown channel type, can't happen\n");
      break;
    }

  for (i = 0; i < GIMP_HISTOGRAM_VIEW_WIDTH; i++)
    {
      buf[3 * i + 0] = i * r;
      buf[3 * i + 1] = i * g;
      buf[3 * i + 2] = i * b;
    }

  for (i = 0; i < GRADIENT_HEIGHT; i++)
    gtk_preview_draw_row (GTK_PREVIEW (gradient),
			  buf, 0, i, GIMP_HISTOGRAM_VIEW_WIDTH);

  gtk_widget_queue_draw (gradient);
}
