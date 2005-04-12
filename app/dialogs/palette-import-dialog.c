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

#include <string.h>

#include <gtk/gtk.h>

#include "libgimpwidgets/gimpwidgets.h"

#include "dialogs-types.h"

#include "core/gimp.h"
#include "core/gimpcontainer.h"
#include "core/gimpcontext.h"
#include "core/gimpdatafactory.h"
#include "core/gimpgradient.h"
#include "core/gimpimage.h"
#include "core/gimppalette.h"
#include "core/gimppalette-import.h"

#include "file/file-utils.h"

#include "widgets/gimpcontainercombobox.h"
#include "widgets/gimpdnd.h"
#include "widgets/gimphelp-ids.h"
#include "widgets/gimpview.h"
#include "widgets/gimpviewabledialog.h"

#include "palette-import-dialog.h"

#include "gimp-intl.h"


#define IMPORT_PREVIEW_SIZE 80


typedef enum
{
  GRADIENT_IMPORT,
  IMAGE_IMPORT,
  FILE_IMPORT
} ImportType;


typedef struct _ImportDialog ImportDialog;

struct _ImportDialog
{
  GtkWidget      *dialog;

  ImportType      import_type;
  GimpContext    *context;
  GimpPalette    *palette;

  GtkWidget      *gradient_combo;
  GtkWidget      *image_combo;
  GtkWidget      *filename_entry;

  GtkWidget      *entry;

  GtkWidget      *image_radio;
  GtkWidget      *gradient_radio;
  GtkWidget      *palettefile_radio;

  GtkAdjustment  *threshold;
  GtkAdjustment  *num_colors;
  GtkAdjustment  *columns;

  GtkWidget      *preview;
};


static ImportDialog * palette_import_dialog_new   (Gimp          *gimp);
static void   palette_import_response             (GtkWidget     *widget,
                                                   gint           response_id,
                                                   ImportDialog  *import_dialog);
static void   palette_import_gradient_changed     (GimpContext   *context,
                                                   GimpGradient  *gradient,
                                                   ImportDialog  *import_dialog);
static void   palette_import_image_changed        (GimpContext   *context,
                                                   GimpImage     *gimage,
                                                   ImportDialog  *import_dialog);
static void   palette_import_filename_changed     (GimpFileEntry *file_entry,
                                                   ImportDialog  *import_dialog);
static void   import_dialog_drop_callback         (GtkWidget     *widget,
                                                   gint           x,
                                                   gint           y,
                                                   GimpViewable  *viewable,
                                                   gpointer       data);
static void   palette_import_grad_callback        (GtkWidget     *widget,
                                                   ImportDialog  *import_dialog);
static void   palette_import_image_callback       (GtkWidget     *widget,
                                                   ImportDialog  *import_dialog);
static void   palette_import_file_callback        (GtkWidget     *widget,
                                                   ImportDialog  *import_dialog);
static void   palette_import_columns_changed      (GtkAdjustment *adjustment,
                                                   ImportDialog  *import_dialog);
static void   palette_import_image_add            (GimpContainer *container,
                                                   GimpImage     *gimage,
                                                   ImportDialog  *import_dialog);
static void   palette_import_image_remove         (GimpContainer *container,
                                                   GimpImage     *gimage,
                                                   ImportDialog  *import_dialog);
static void   palette_import_make_palette         (ImportDialog  *import_dialog);


static ImportDialog *the_import_dialog = NULL;


/*  public functions  */

void
palette_import_dialog_show (Gimp *gimp)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));

  if (! the_import_dialog)
    the_import_dialog = palette_import_dialog_new (gimp);

  gtk_window_present (GTK_WINDOW (the_import_dialog->dialog));
}

void
palette_import_dialog_destroy (void)
{
  if (the_import_dialog)
    palette_import_response (NULL, GTK_RESPONSE_CANCEL, the_import_dialog);
}


/*  private functions  */

/*  the palette import dialog constructor  ***********************************/

static ImportDialog *
palette_import_dialog_new (Gimp *gimp)
{
  ImportDialog *import_dialog;
  GimpGradient *gradient;
  GtkWidget    *main_hbox;
  GtkWidget    *frame;
  GtkWidget    *vbox;
  GtkWidget    *table;
  GtkWidget    *abox;
  GSList       *group;

  gradient = gimp_context_get_gradient (gimp_get_user_context (gimp));

  import_dialog = g_new0 (ImportDialog, 1);

  import_dialog->import_type = GRADIENT_IMPORT;
  import_dialog->context     = gimp_context_new (gimp, "Palette Import",
                                                 gimp_get_user_context (gimp));

  import_dialog->dialog =
    gimp_viewable_dialog_new (NULL, _("Import Palette"), "gimp-palette-import",
                              GTK_STOCK_CONVERT,
                              _("Import a New Palette"),
                              NULL,
                              gimp_standard_help_func,
                              GIMP_HELP_PALETTE_IMPORT,

                              GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                              _("_Import"),     GTK_RESPONSE_OK,

                              NULL);

  gtk_dialog_set_alternative_button_order (GTK_DIALOG (import_dialog->dialog),
                                           GTK_RESPONSE_OK,
                                           GTK_RESPONSE_CANCEL,
                                           -1);

  g_signal_connect (import_dialog->dialog, "response",
                    G_CALLBACK (palette_import_response),
                    import_dialog);

  gimp_dnd_viewable_dest_add (import_dialog->dialog,
                              GIMP_TYPE_GRADIENT,
                              import_dialog_drop_callback,
                              import_dialog);
  gimp_dnd_viewable_dest_add (import_dialog->dialog,
                              GIMP_TYPE_IMAGE,
                              import_dialog_drop_callback,
                              import_dialog);

  main_hbox = gtk_hbox_new (FALSE, 12);
  gtk_container_set_border_width (GTK_CONTAINER (main_hbox), 12);
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG (import_dialog->dialog)->vbox),
                     main_hbox);
  gtk_widget_show (main_hbox);

  vbox = gtk_vbox_new (FALSE, 12);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, TRUE, TRUE, 0);
  gtk_widget_show (vbox);


  /*  The "Source" frame  */
  frame = gimp_frame_new (_("Select Source"));
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  table = gtk_table_new (2, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  gtk_table_set_row_spacings (GTK_TABLE (table), 2);
  gtk_container_add (GTK_CONTAINER (frame), table);
  gtk_widget_show (table);

  import_dialog->gradient_radio =
    gtk_radio_button_new_with_mnemonic (NULL, _("_Gradient"));
  gtk_table_attach_defaults (GTK_TABLE (table), import_dialog->gradient_radio,
                             0, 1, 0, 1);
  gtk_widget_show (import_dialog->gradient_radio);

  g_signal_connect (import_dialog->gradient_radio, "toggled",
                    G_CALLBACK (palette_import_grad_callback),
                    import_dialog);

  group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (import_dialog->gradient_radio));

  import_dialog->image_radio =
    gtk_radio_button_new_with_mnemonic (group, _("I_mage"));
  gtk_table_attach_defaults (GTK_TABLE (table), import_dialog->image_radio,
                             0, 1, 1, 2);
  gtk_widget_show (import_dialog->image_radio);

  g_signal_connect (import_dialog->image_radio, "toggled",
                    G_CALLBACK (palette_import_image_callback),
                    import_dialog);

  gtk_widget_set_sensitive (import_dialog->image_radio,
                            ! gimp_container_is_empty (gimp->images));

  group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (import_dialog->image_radio));

  import_dialog->palettefile_radio =
    gtk_radio_button_new_with_mnemonic (group, _("Palette _file"));
  gtk_table_attach_defaults (GTK_TABLE (table),
                             import_dialog->palettefile_radio,
                             0, 1, 2, 3);
  gtk_widget_show (import_dialog->palettefile_radio);

  g_signal_connect (import_dialog->palettefile_radio, "toggled",
                    G_CALLBACK (palette_import_file_callback),
                    import_dialog);

  /*  The gradient menu  */
  import_dialog->gradient_combo =
    gimp_container_combo_box_new (gimp->gradient_factory->container,
                                  import_dialog->context, 24, 1);
  gimp_table_attach_aligned (GTK_TABLE (table), 0, 0,
                             NULL, 0.0, 0.5,
                             import_dialog->gradient_combo, 1, FALSE);

  /*  The image menu  */
  import_dialog->image_combo =
    gimp_container_combo_box_new (gimp->images, import_dialog->context, 24, 1);
  gimp_table_attach_aligned (GTK_TABLE (table), 0, 1,
                             NULL, 0.0, 0.5,
                             import_dialog->image_combo, 1, FALSE);

  /*  Palette file name entry  */
  import_dialog->filename_entry =
    gimp_file_entry_new (_("Select palette file"), NULL, FALSE, FALSE);
  gimp_table_attach_aligned (GTK_TABLE (table), 0, 2,
                             NULL, 0.0, 0.5,
                             import_dialog->filename_entry, 1, FALSE);

  gtk_widget_show (import_dialog->filename_entry);

  {
    gint focus_line_width;
    gint focus_padding;
    gint ythickness;
    gint ysize;

    gtk_widget_style_get (import_dialog->gradient_combo,
                          "focus_line_width", &focus_line_width,
                          "focus_padding",    &focus_padding,
                          NULL);

    ythickness = import_dialog->gradient_combo->style->ythickness;

    ysize = 24 + (2 * (1 /* CHILD_SPACING */ +
                       ythickness            +
                       focus_padding         +
                       focus_line_width));

    gtk_widget_set_size_request (import_dialog->gradient_combo, -1, ysize);
    gtk_widget_set_size_request (import_dialog->image_combo,    -1, ysize);
  }


  /*  The "Import" frame  */
  frame = gimp_frame_new (_("Import Options"));
  gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  table = gtk_table_new (4, 3, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (table), 4);
  gtk_table_set_row_spacings (GTK_TABLE (table), 2);
  gtk_container_add (GTK_CONTAINER (frame), table);
  gtk_widget_show (table);

  /*  The source's name  */
  import_dialog->entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (import_dialog->entry),
                      gradient ? GIMP_OBJECT (gradient)->name : _("New import"));
  gimp_table_attach_aligned (GTK_TABLE (table), 0, 0,
                             _("Palette _Name:"), 0.0, 0.5,
                             import_dialog->entry, 2, FALSE);

  /*  The # of colors  */
  import_dialog->num_colors =
    GTK_ADJUSTMENT (gimp_scale_entry_new (GTK_TABLE (table), 0, 1,
                                          _("N_umber of colors:"), -1, 5,
                                          256, 2, 10000, 1, 10, 0,
                                          TRUE, 0.0, 0.0,
                                          NULL, NULL));

  g_signal_connect_swapped (import_dialog->num_colors,
                            "value_changed",
                            G_CALLBACK (palette_import_make_palette),
                            import_dialog);

  /*  The columns  */
  import_dialog->columns =
    GTK_ADJUSTMENT (gimp_scale_entry_new (GTK_TABLE (table), 0, 2,
                                          _("C_olumns:"), -1, 5,
                                          16, 0, 64, 1, 8, 0,
                                          TRUE, 0.0, 0.0,
                                          NULL, NULL));

  g_signal_connect (import_dialog->columns, "value_changed",
                    G_CALLBACK (palette_import_columns_changed),
                    import_dialog);

  /*  The interval  */
  import_dialog->threshold =
    GTK_ADJUSTMENT (gimp_scale_entry_new (GTK_TABLE (table), 0, 3,
                                          _("I_nterval:"), -1, 5,
                                          1, 1, 128, 1, 8, 0,
                                          TRUE, 0.0, 0.0,
                                          NULL, NULL));
  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->threshold), FALSE);

  g_signal_connect_swapped (import_dialog->threshold, "value_changed",
                            G_CALLBACK (palette_import_make_palette),
                            import_dialog);


  /*  The "Preview" frame  */
  frame = gimp_frame_new (_("Preview"));
  gtk_box_pack_start (GTK_BOX (main_hbox), frame, FALSE, FALSE, 0);
  gtk_widget_show (frame);

  abox = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  gtk_container_add (GTK_CONTAINER (frame), abox);
  gtk_widget_show (abox);

  import_dialog->preview = gimp_view_new_full_by_types (GIMP_TYPE_VIEW,
                                                        GIMP_TYPE_PALETTE,
                                                        192, 192, 1,
                                                        TRUE, FALSE, FALSE);
  gtk_container_add (GTK_CONTAINER (abox), import_dialog->preview);
  gtk_widget_show (import_dialog->preview);

  /*  keep the dialog up-to-date  */

  g_signal_connect (gimp->images, "add",
                    G_CALLBACK (palette_import_image_add),
                    import_dialog);
  g_signal_connect (gimp->images, "remove",
                    G_CALLBACK (palette_import_image_remove),
                    import_dialog);

  g_signal_connect (import_dialog->context, "gradient_changed",
		    G_CALLBACK (palette_import_gradient_changed),
		    import_dialog);
  g_signal_connect (import_dialog->context, "image_changed",
		    G_CALLBACK (palette_import_image_changed),
		    import_dialog);
  g_signal_connect (import_dialog->filename_entry, "filename_changed",
                    G_CALLBACK (palette_import_filename_changed),
                    import_dialog);

  palette_import_make_palette (import_dialog);

  palette_import_grad_callback (import_dialog->gradient_radio, import_dialog);

  return import_dialog;
}


/*  the palette import response callback  ************************************/

static void
palette_import_response (GtkWidget    *widget,
                         gint          response_id,
                         ImportDialog *import_dialog)
{
  Gimp *gimp = import_dialog->context->gimp;

  g_signal_handlers_disconnect_by_func (gimp->images,
                                        palette_import_image_add,
                                        import_dialog);
  g_signal_handlers_disconnect_by_func (gimp->images,
                                        palette_import_image_remove,
                                        import_dialog);

  if (response_id == GTK_RESPONSE_OK && import_dialog->palette)
    {
      const gchar *name = gtk_entry_get_text (GTK_ENTRY (import_dialog->entry));

      if (name && *name)
        gimp_object_set_name (GIMP_OBJECT (import_dialog->palette), name);

      gimp_container_add (gimp->palette_factory->container,
                          GIMP_OBJECT (import_dialog->palette));
    }

  g_object_unref (import_dialog->context);

  if (import_dialog->palette)
    g_object_unref (import_dialog->palette);

  gtk_widget_destroy (import_dialog->dialog);
  g_free (import_dialog);

  the_import_dialog = NULL;
}


/*  functions to create & update the import dialog's gradient selection  *****/

static void
palette_import_gradient_changed (GimpContext  *context,
                                 GimpGradient *gradient,
                                 ImportDialog *import_dialog)
{
  if (gradient && import_dialog->import_type == GRADIENT_IMPORT)
    {
      gtk_entry_set_text (GTK_ENTRY (import_dialog->entry),
			  GIMP_OBJECT (gradient)->name);

      palette_import_make_palette (import_dialog);
    }
}

static void
palette_import_image_changed (GimpContext  *context,
                              GimpImage    *gimage,
                              ImportDialog *import_dialog)
{
  if (gimage && import_dialog->import_type == IMAGE_IMPORT)
    {
      gchar *basename;
      gchar *label;

      basename = file_utils_uri_to_utf8_basename (gimp_image_get_uri (gimage));
      label = g_strdup_printf ("%s-%d", basename, gimp_image_get_ID (gimage));
      g_free (basename);

      gtk_entry_set_text (GTK_ENTRY (import_dialog->entry), label);

      g_free (label);

      palette_import_make_palette (import_dialog);
    }
}

static void
palette_import_filename_changed (GimpFileEntry *file_entry,
                                 ImportDialog  *import_dialog)
{
  gchar *filename;

  if (import_dialog->import_type != FILE_IMPORT)
    return;

  filename = gimp_file_entry_get_filename (file_entry);

  if (filename && filename[0])
    {
      gchar *basename = g_path_get_basename (filename);

      /* TODO: strip filename extension */
      gtk_entry_set_text (GTK_ENTRY (import_dialog->entry), basename);
      g_free (basename);
    }
  else
    {
      gtk_entry_set_text (GTK_ENTRY (import_dialog->entry), "");
    }

  g_free (filename);

  palette_import_make_palette (import_dialog);
}

static void
import_dialog_drop_callback (GtkWidget    *widget,
                             gint          x,
                             gint          y,
                             GimpViewable *viewable,
                             gpointer      data)
{
  ImportDialog *import_dialog;

  import_dialog = (ImportDialog *) data;

  gimp_context_set_by_type (import_dialog->context,
                            G_TYPE_FROM_INSTANCE (viewable),
                            GIMP_OBJECT (viewable));

  if (GIMP_IS_GRADIENT (viewable) &&
      import_dialog->import_type != GRADIENT_IMPORT)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (import_dialog->gradient_radio),
                                    TRUE);
    }
  else if (GIMP_IS_IMAGE (viewable) &&
           import_dialog->import_type != IMAGE_IMPORT)
    {
      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (import_dialog->image_radio),
                                    TRUE);
    }
}



/*  the import source menu item callbacks  ***********************************/

static void
palette_import_grad_callback (GtkWidget    *widget,
			      ImportDialog *import_dialog)
{
  GimpGradient *gradient;

  if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    return;

  import_dialog->import_type = GRADIENT_IMPORT;

  gradient = gimp_context_get_gradient (import_dialog->context);

  gtk_widget_set_sensitive (import_dialog->gradient_combo, TRUE);
  gtk_widget_set_sensitive (import_dialog->image_combo,    FALSE);
  gtk_widget_set_sensitive (import_dialog->filename_entry, FALSE);

  gtk_entry_set_text (GTK_ENTRY (import_dialog->entry),
                      GIMP_OBJECT (gradient)->name);

  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->threshold),  FALSE);
  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->num_colors), TRUE);
  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->columns),    TRUE);

  palette_import_make_palette (import_dialog);
}

static void
palette_import_image_callback (GtkWidget    *widget,
			       ImportDialog *import_dialog)
{
  GimpImage *gimage;

  if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    return;

  import_dialog->import_type = IMAGE_IMPORT;

  gimage = gimp_context_get_image (import_dialog->context);

  if (! gimage)
    gimage = (GimpImage *)
      gimp_container_get_child_by_index (import_dialog->context->gimp->images,
                                         0);

  gtk_widget_set_sensitive (import_dialog->gradient_combo, FALSE);
  gtk_widget_set_sensitive (import_dialog->image_combo,    TRUE);
  gtk_widget_set_sensitive (import_dialog->filename_entry, FALSE);

  palette_import_image_changed (import_dialog->context, gimage, import_dialog);

  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->threshold),  TRUE);
  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->num_colors), TRUE);
  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->columns),    TRUE);
}

static void
palette_import_file_callback (GtkWidget    *widget,
                              ImportDialog *import_dialog)
{
  gchar *filename;

  if (! gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
    return;

  import_dialog->import_type = FILE_IMPORT;

  gtk_widget_set_sensitive (import_dialog->gradient_combo, FALSE);
  gtk_widget_set_sensitive (import_dialog->image_combo,    FALSE);
  gtk_widget_set_sensitive (import_dialog->filename_entry, TRUE);

  filename = gimp_file_entry_get_filename (GIMP_FILE_ENTRY (import_dialog->filename_entry));

  if (filename && *filename)
    {
      gchar *basename = g_path_get_basename (filename);

      /* TODO: strip filename extension */
      gtk_entry_set_text (GTK_ENTRY (import_dialog->entry), basename);
      g_free (basename);
    }
  else
    {
      gtk_entry_set_text (GTK_ENTRY (import_dialog->entry), "");
    }

  g_free (filename);

  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->threshold),  FALSE);
  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->num_colors), FALSE);
  gimp_scale_entry_set_sensitive (GTK_OBJECT (import_dialog->columns),    FALSE);
}

static void
palette_import_columns_changed (GtkAdjustment *adj,
                                ImportDialog  *import_dialog)
{
  if (import_dialog->palette)
    gimp_palette_set_columns (import_dialog->palette, ROUND (adj->value));
}

/*  functions & callbacks to keep the import dialog uptodate  ****************/

static void
palette_import_image_add (GimpContainer *container,
			  GimpImage     *gimage,
			  ImportDialog  *import_dialog)
{
  if (! GTK_WIDGET_IS_SENSITIVE (import_dialog->image_radio))
    {
      gtk_widget_set_sensitive (import_dialog->image_radio, TRUE);
      gimp_context_set_image (import_dialog->context, gimage);
    }
}

static void
palette_import_image_remove (GimpContainer *container,
                             GimpImage     *gimage,
                             ImportDialog  *import_dialog)
{
  if (! gimp_container_num_children (import_dialog->context->gimp->images))
    {
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (import_dialog->image_radio)))
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (import_dialog->gradient_radio),
                                      TRUE);

      gtk_widget_set_sensitive (import_dialog->image_radio, FALSE);
    }
}

static void
palette_import_make_palette (ImportDialog *import_dialog)
{
  GimpPalette  *palette = NULL;
  const gchar  *palette_name;
  gint          n_colors;
  gint          n_columns;
  gint          threshold;

  palette_name = gtk_entry_get_text (GTK_ENTRY (import_dialog->entry));

  if (! palette_name || ! strlen (palette_name))
    palette_name = _("Untitled");

  n_colors  = ROUND (import_dialog->num_colors->value);
  n_columns = ROUND (import_dialog->columns->value);
  threshold = ROUND (import_dialog->threshold->value);

  switch (import_dialog->import_type)
    {
    case GRADIENT_IMPORT:
      {
        GimpGradient *gradient;

        gradient = gimp_context_get_gradient (import_dialog->context);

        palette = gimp_palette_import_from_gradient (gradient,
                                                     FALSE,
                                                     palette_name,
                                                     n_colors);
      }
      break;

    case IMAGE_IMPORT:
      {
        GimpImage *gimage;

        gimage = gimp_context_get_image (import_dialog->context);

        if (gimp_image_base_type (gimage) == GIMP_INDEXED)
          {
            palette = gimp_palette_import_from_indexed_image (gimage,
                                                              palette_name);
          }
        else
          {
            palette = gimp_palette_import_from_image (gimage,
                                                      palette_name,
                                                      n_colors,
                                                      threshold);
          }
      }
      break;

    case FILE_IMPORT:
      {
        gchar  *filename;
        GError *error = NULL;

        filename = gimp_file_entry_get_filename (GIMP_FILE_ENTRY (import_dialog->filename_entry));

        palette = gimp_palette_import_from_file (filename,
                                                 palette_name, &error);

        if (! palette)
          {
            g_message (error->message);
            g_error_free (error);
          }

        g_free (filename);
      }
      break;
    }

  if (palette)
    {
      if (import_dialog->palette)
        g_object_unref (import_dialog->palette);

      palette->n_columns = n_columns;

      gimp_view_set_viewable (GIMP_VIEW (import_dialog->preview),
                              GIMP_VIEWABLE (palette));

      import_dialog->palette = palette;
    }
}
