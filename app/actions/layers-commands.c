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

#include "libgimpmath/gimpmath.h"
#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "actions-types.h"

#include "config/gimpcoreconfig.h"

#include "core/gimp.h"
#include "core/gimpchannel-select.h"
#include "core/gimpcontext.h"
#include "core/gimpimage.h"
#include "core/gimpimage-merge.h"
#include "core/gimpimage-undo.h"
#include "core/gimpitemundo.h"
#include "core/gimplayer.h"
#include "core/gimplayer-floating-sel.h"
#include "core/gimplayermask.h"
#include "core/gimplist.h"
#include "core/gimptoolinfo.h"
#include "core/gimpundostack.h"
#include "core/gimpprogress.h"

#include "text/gimptext.h"
#include "text/gimptextlayer.h"

#include "widgets/gimpaction.h"
#include "widgets/gimpdock.h"
#include "widgets/gimphelp-ids.h"
#include "widgets/gimpprogressdialog.h"

#include "display/gimpdisplay.h"
#include "display/gimpdisplayshell.h"

#include "tools/gimptexttool.h"
#include "tools/tool_manager.h"

#include "dialogs/layer-add-mask-dialog.h"
#include "dialogs/layer-options-dialog.h"
#include "dialogs/resize-dialog.h"
#include "dialogs/scale-dialog.h"

#include "actions.h"
#include "layers-commands.h"

#include "gimp-intl.h"


static const GimpLayerModeEffects layer_modes[] =
{
  GIMP_NORMAL_MODE,
  GIMP_DISSOLVE_MODE,
  GIMP_MULTIPLY_MODE,
  GIMP_DIVIDE_MODE,
  GIMP_SCREEN_MODE,
  GIMP_OVERLAY_MODE,
  GIMP_DODGE_MODE,
  GIMP_BURN_MODE,
  GIMP_HARDLIGHT_MODE,
  GIMP_SOFTLIGHT_MODE,
  GIMP_GRAIN_EXTRACT_MODE,
  GIMP_GRAIN_MERGE_MODE,
  GIMP_DIFFERENCE_MODE,
  GIMP_ADDITION_MODE,
  GIMP_SUBTRACT_MODE,
  GIMP_DARKEN_ONLY_MODE,
  GIMP_LIGHTEN_ONLY_MODE,
  GIMP_HUE_MODE,
  GIMP_SATURATION_MODE,
  GIMP_COLOR_MODE,
  GIMP_VALUE_MODE
};


typedef struct _ResizeLayerOptions ResizeLayerOptions;

struct _ResizeLayerOptions
{
  GimpLayer    *layer;
  GimpContext  *context;
  ResizeDialog *dialog;
};


/*  local function prototypes  */

static void   layers_new_layer_response    (GtkWidget             *widget,
                                            gint                   response_id,
                                            LayerOptionsDialog    *dialog);
static void   layers_edit_layer_response   (GtkWidget             *widget,
                                            gint                   response_id,
                                            LayerOptionsDialog    *dialog);
static void   layers_add_mask_response     (GtkWidget             *widget,
                                            gint                   response_id,
                                            LayerAddMaskDialog    *dialog);
static void   layers_scale_layer_callback  (GtkWidget             *dialog,
                                            GimpViewable          *viewable,
                                            gint                   width,
                                            gint                   height,
                                            GimpUnit               unit,
                                            GimpInterpolationType  interpolation,
                                            gdouble                xresolution,
                                            gdouble                yresolution,
                                            GimpUnit               resolution_unit,
                                            gpointer               data);
static void   layers_resize_layer_callback (GtkWidget             *widget,
                                            gpointer               data);
static gint   layers_mode_index            (GimpLayerModeEffects   layer_mode);


/*  private variables  */

static GimpFillType     layer_fill_type     = GIMP_TRANSPARENT_FILL;
static gchar           *layer_name          = NULL;
static GimpAddMaskType  layer_add_mask_type = GIMP_ADD_WHITE_MASK;
static gboolean         layer_mask_invert   = FALSE;


/*  public functions  */

void
layers_text_tool_cmd_callback (GtkAction *action,
                               gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  GtkWidget *widget;
  GimpTool  *active_tool;
  return_if_no_layer (gimage, layer, data);
  return_if_no_widget (widget, data);

  if (! gimp_drawable_is_text_layer (GIMP_DRAWABLE (layer)))
    {
      layers_edit_attributes_cmd_callback (action, data);
      return;
    }

  active_tool = tool_manager_get_active (gimage->gimp);

  if (! GIMP_IS_TEXT_TOOL (active_tool))
    {
      GimpToolInfo *tool_info;

      tool_info = (GimpToolInfo *)
        gimp_container_get_child_by_name (gimage->gimp->tool_info_list,
                                          "gimp-text-tool");

      if (GIMP_IS_TOOL_INFO (tool_info))
        {
          gimp_context_set_tool (action_data_get_context (data), tool_info);
          active_tool = tool_manager_get_active (gimage->gimp);
        }
    }

  if (GIMP_IS_TEXT_TOOL (active_tool))
    gimp_text_tool_set_layer (GIMP_TEXT_TOOL (active_tool), layer);
}

void
layers_edit_attributes_cmd_callback (GtkAction *action,
				     gpointer   data)
{
  LayerOptionsDialog *dialog;
  GimpImage          *gimage;
  GimpLayer          *layer;
  GtkWidget          *widget;
  return_if_no_layer (gimage, layer, data);
  return_if_no_widget (widget, data);

  dialog = layer_options_dialog_new (gimp_item_get_image (GIMP_ITEM (layer)),
                                     action_data_get_context (data),
                                     layer, widget,
                                     gimp_object_get_name (GIMP_OBJECT (layer)),
                                     layer_fill_type,
                                     _("Layer Attributes"),
                                     "gimp-layer-edit",
                                     GIMP_STOCK_EDIT,
                                     _("Edit Layer Attributes"),
                                     GIMP_HELP_LAYER_EDIT);

  g_signal_connect (dialog->dialog, "response",
                    G_CALLBACK (layers_edit_layer_response),
                    dialog);

  gtk_widget_show (dialog->dialog);
}

void
layers_new_cmd_callback (GtkAction *action,
			 gpointer   data)
{
  LayerOptionsDialog *dialog;
  GimpImage          *gimage;
  GtkWidget          *widget;
  GimpLayer          *floating_sel;
  return_if_no_image (gimage, data);
  return_if_no_widget (widget, data);

  /*  If there is a floating selection, the new command transforms
   *  the current fs into a new layer
   */
  if ((floating_sel = gimp_image_floating_sel (gimage)))
    {
      floating_sel_to_layer (floating_sel);
      gimp_image_flush (gimage);
      return;
    }

  dialog = layer_options_dialog_new (gimage, action_data_get_context (data),
                                     NULL, widget,
                                     layer_name ? layer_name : _("New Layer"),
                                     layer_fill_type,
                                     _("New Layer"),
                                     "gimp-layer-new",
                                     GIMP_STOCK_LAYER,
                                     _("Create a New Layer"),
                                     GIMP_HELP_LAYER_NEW);

  g_signal_connect (dialog->dialog, "response",
                    G_CALLBACK (layers_new_layer_response),
                    dialog);

  gtk_widget_show (dialog->dialog);
}

void
layers_new_last_vals_cmd_callback (GtkAction *action,
                                   gpointer   data)
{
  GimpImage            *gimage;
  GimpLayer            *floating_sel;
  GimpLayer            *new_layer;
  gint                  width, height;
  gint                  off_x, off_y;
  gdouble               opacity;
  GimpLayerModeEffects  mode;
  return_if_no_image (gimage, data);

  /*  If there is a floating selection, the new command transforms
   *  the current fs into a new layer
   */
  if ((floating_sel = gimp_image_floating_sel (gimage)))
    {
      floating_sel_to_layer (floating_sel);
      gimp_image_flush (gimage);
      return;
    }

  if (GIMP_IS_LAYER (GIMP_ACTION (action)->viewable))
    {
      GimpLayer *template = GIMP_LAYER (GIMP_ACTION (action)->viewable);

      gimp_item_offsets (GIMP_ITEM (template), &off_x, &off_y);
      width   = gimp_item_width  (GIMP_ITEM (template));
      height  = gimp_item_height (GIMP_ITEM (template));
      opacity = template->opacity;
      mode    = template->mode;
    }
  else
    {
      width   = gimp_image_get_width (gimage);
      height  = gimp_image_get_height (gimage);
      off_x   = 0;
      off_y   = 0;
      opacity = 1.0;
      mode    = GIMP_NORMAL_MODE;
    }

  gimp_image_undo_group_start (gimage, GIMP_UNDO_GROUP_EDIT_PASTE,
                               _("New Layer"));

  new_layer = gimp_layer_new (gimage, width, height,
                              gimp_image_base_type_with_alpha (gimage),
                              layer_name ? layer_name : _("New Layer"),
                              opacity, mode);

  gimp_drawable_fill_by_type (GIMP_DRAWABLE (new_layer),
                              action_data_get_context (data),
                              layer_fill_type);
  gimp_item_translate (GIMP_ITEM (new_layer), off_x, off_y, FALSE);

  gimp_image_add_layer (gimage, new_layer, -1);

  gimp_image_undo_group_end (gimage);

  gimp_image_flush (gimage);
}

void
layers_select_cmd_callback (GtkAction *action,
                            gint       value,
                            gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  GimpLayer *new_layer;
  return_if_no_image (gimage, data);

  layer = gimp_image_get_active_layer (gimage);

  new_layer = (GimpLayer *) action_select_object ((GimpActionSelectType) value,
                                                  gimage->layers,
                                                  (GimpObject *) layer);

  if (new_layer && new_layer != layer)
    {
      gimp_image_set_active_layer (gimage, new_layer);
      gimp_image_flush (gimage);
    }
}

void
layers_raise_cmd_callback (GtkAction *action,
			   gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  gimp_image_raise_layer (gimage, layer);
  gimp_image_flush (gimage);
}

void
layers_raise_to_top_cmd_callback (GtkAction *action,
				  gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  gimp_image_raise_layer_to_top (gimage, layer);
  gimp_image_flush (gimage);
}

void
layers_lower_cmd_callback (GtkAction *action,
			   gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  gimp_image_lower_layer (gimage, layer);
  gimp_image_flush (gimage);
}

void
layers_lower_to_bottom_cmd_callback (GtkAction *action,
				     gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  gimp_image_lower_layer_to_bottom (gimage, layer);
  gimp_image_flush (gimage);
}

void
layers_duplicate_cmd_callback (GtkAction *action,
			       gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  GimpLayer *new_layer;
  return_if_no_layer (gimage, layer, data);

  new_layer =
    GIMP_LAYER (gimp_item_duplicate (GIMP_ITEM (layer),
                                     G_TYPE_FROM_INSTANCE (layer),
                                     TRUE));
  gimp_image_add_layer (gimage, new_layer, -1);

  gimp_image_flush (gimage);
}

void
layers_anchor_cmd_callback (GtkAction *action,
			    gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  if (gimp_layer_is_floating_sel (layer))
    {
      floating_sel_anchor (layer);
      gimp_image_flush (gimage);
    }
}

void
layers_merge_down_cmd_callback (GtkAction *action,
				gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  gimp_image_merge_down (gimage, layer, action_data_get_context (data),
                         GIMP_EXPAND_AS_NECESSARY);
  gimp_image_flush (gimage);
}

void
layers_delete_cmd_callback (GtkAction *action,
			    gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  if (gimp_layer_is_floating_sel (layer))
    floating_sel_remove (layer);
  else
    gimp_image_remove_layer (gimage, layer);

  gimp_image_flush (gimage);
}

void
layers_text_discard_cmd_callback (GtkAction *action,
                                  gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  if (GIMP_IS_TEXT_LAYER (layer))
    gimp_text_layer_discard (GIMP_TEXT_LAYER (layer));
}

void
layers_resize_cmd_callback (GtkAction *action,
			    gpointer   data)
{
  ResizeLayerOptions *options;
  GimpImage          *gimage;
  GimpLayer          *layer;
  GtkWidget          *widget;
  GimpDisplay        *gdisp;
  return_if_no_layer (gimage, layer, data);
  return_if_no_widget (widget, data);

  gdisp = GIMP_IS_DISPLAY (data) ? data : NULL;

  options = g_new0 (ResizeLayerOptions, 1);

  options->context = action_data_get_context (data);
  options->layer   = layer;

  options->dialog =
    resize_dialog_new (GIMP_VIEWABLE (layer), widget,
                       RESIZE_DIALOG,
		       gimp_item_width  (GIMP_ITEM (layer)),
		       gimp_item_height (GIMP_ITEM (layer)),
		       gimage->xresolution,
		       gimage->yresolution,
		       (gdisp ?
                        GIMP_DISPLAY_SHELL (gdisp->shell)->unit :
                        GIMP_UNIT_PIXEL),
		       G_CALLBACK (layers_resize_layer_callback),
                       options);

  g_object_weak_ref (G_OBJECT (options->dialog->shell),
		     (GWeakNotify) g_free,
		     options);

  gtk_widget_show (options->dialog->shell);
}

void
layers_resize_to_image_cmd_callback (GtkAction *action,
				     gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  gimp_layer_resize_to_image (layer, action_data_get_context (data));
  gimp_image_flush (gimage);
}

void
layers_scale_cmd_callback (GtkAction *action,
			   gpointer   data)
{
  GimpImage   *gimage;
  GimpLayer   *layer;
  GtkWidget   *widget;
  GimpDisplay *gdisp;
  GtkWidget   *dialog;
  GimpUnit     unit;
  return_if_no_layer (gimage, layer, data);
  return_if_no_widget (widget, data);

  gdisp = action_data_get_display (data);

  unit = gdisp ? GIMP_DISPLAY_SHELL (gdisp->shell)->unit : GIMP_UNIT_PIXEL;

  dialog = scale_dialog_new (GIMP_VIEWABLE (layer),
                             _("Scale Layer"), "gimp-layer-scale",
                             widget,
                             gimp_standard_help_func, GIMP_HELP_LAYER_SCALE,
                             unit, gimage->gimp->config->interpolation_type,
                             layers_scale_layer_callback,
                             gdisp);

  gtk_widget_show (dialog);
}

void
layers_crop_cmd_callback (GtkAction *action,
                          gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  gint       x1, y1, x2, y2;
  gint       off_x, off_y;
  return_if_no_layer (gimage, layer, data);

  if (! gimp_channel_bounds (gimp_image_get_mask (gimage),
                             &x1, &y1, &x2, &y2))
    {
      g_message (_("Cannot crop because the current selection is empty."));
      return;
    }

  gimp_item_offsets (GIMP_ITEM (layer), &off_x, &off_y);

  off_x -= x1;
  off_y -= y1;

  gimp_image_undo_group_start (gimage, GIMP_UNDO_GROUP_ITEM_RESIZE,
                               _("Crop Layer"));

  gimp_item_resize (GIMP_ITEM (layer), action_data_get_context (data),
                    x2 - x1, y2 - y1, off_x, off_y);

  gimp_image_undo_group_end (gimage);

  gimp_image_flush (gimage);
}

void
layers_mask_add_cmd_callback (GtkAction *action,
                              gpointer   data)
{
  LayerAddMaskDialog *dialog;
  GimpImage          *gimage;
  GimpLayer          *layer;
  GtkWidget          *widget;
  return_if_no_layer (gimage, layer, data);
  return_if_no_widget (widget, data);

  dialog = layer_add_mask_dialog_new (layer, widget,
                                      layer_add_mask_type, layer_mask_invert);

  g_signal_connect (dialog->dialog, "response",
                    G_CALLBACK (layers_add_mask_response),
                    dialog);

  gtk_widget_show (dialog->dialog);
}

void
layers_mask_apply_cmd_callback (GtkAction *action,
                                gint       value,
                                gpointer   data)
{
  GimpImage         *gimage;
  GimpLayer         *layer;
  GimpMaskApplyMode  mode;
  return_if_no_layer (gimage, layer, data);

  mode = (GimpMaskApplyMode) value;

  if (gimp_layer_get_mask (layer))
    {
      gimp_layer_apply_mask (layer, mode, TRUE);
      gimp_image_flush (gimage);
    }
}

void
layers_mask_edit_cmd_callback (GtkAction *action,
                               gpointer   data)
{
  GimpImage     *gimage;
  GimpLayer     *layer;
  GimpLayerMask *mask;
  return_if_no_layer (gimage, layer, data);

  mask = gimp_layer_get_mask (layer);

  if (mask)
    {
      gboolean active;

      active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

      gimp_layer_mask_set_edit (mask, active);
      gimp_image_flush (gimage);
    }
}

void
layers_mask_show_cmd_callback (GtkAction *action,
                               gpointer   data)
{
  GimpImage     *gimage;
  GimpLayer     *layer;
  GimpLayerMask *mask;
  return_if_no_layer (gimage, layer, data);

  mask = gimp_layer_get_mask (layer);

  if (mask)
    {
      gboolean active;

      active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

      gimp_layer_mask_set_show (mask, active);
      gimp_image_flush (gimage);
    }
}

void
layers_mask_disable_cmd_callback (GtkAction *action,
                                  gpointer   data)
{
  GimpImage     *gimage;
  GimpLayer     *layer;
  GimpLayerMask *mask;
  return_if_no_layer (gimage, layer, data);

  mask = gimp_layer_get_mask (layer);

  if (mask)
    {
      gboolean active;

      active = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

      gimp_layer_mask_set_apply (mask, ! active);
      gimp_image_flush (gimage);
    }
}

void
layers_mask_to_selection_cmd_callback (GtkAction *action,
                                       gint       value,
                                       gpointer   data)
{
  GimpChannelOps  op;
  GimpImage      *gimage;
  GimpLayer      *layer;
  GimpLayerMask  *mask;
  return_if_no_layer (gimage, layer, data);

  op = (GimpChannelOps) value;

  mask = gimp_layer_get_mask (layer);

  if (mask)
    {
      gint off_x, off_y;

      gimp_item_offsets (GIMP_ITEM (mask), &off_x, &off_y);

      gimp_channel_select_channel (gimp_image_get_mask (gimage),
                                   _("Layer Mask to Selection"),
                                   GIMP_CHANNEL (mask),
                                   off_x, off_y,
                                   op, FALSE, 0.0, 0.0);
      gimp_image_flush (gimage);
    }
}

void
layers_alpha_add_cmd_callback (GtkAction *action,
                               gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  return_if_no_layer (gimage, layer, data);

  if (! gimp_drawable_has_alpha (GIMP_DRAWABLE (layer)))
    {
      gimp_layer_add_alpha (layer);
      gimp_image_flush (gimage);
    }
}

void
layers_alpha_to_selection_cmd_callback (GtkAction *action,
                                        gint       value,
                                        gpointer   data)
{
  GimpChannelOps  op;
  GimpImage      *gimage;
  GimpLayer      *layer;
  return_if_no_layer (gimage, layer, data);

  op = (GimpChannelOps) value;

  gimp_channel_select_alpha (gimp_image_get_mask (gimage),
                             GIMP_DRAWABLE (layer),
                             op, FALSE, 0.0, 0.0);
  gimp_image_flush (gimage);
}

void
layers_opacity_cmd_callback (GtkAction *action,
                             gint       value,
                             gpointer   data)
{
  GimpImage      *gimage;
  GimpLayer      *layer;
  gdouble         opacity;
  GimpUndo       *undo;
  gboolean        push_undo = TRUE;
  return_if_no_layer (gimage, layer, data);

  undo = gimp_image_undo_can_compress (gimage, GIMP_TYPE_ITEM_UNDO,
                                       GIMP_UNDO_LAYER_OPACITY);

  if (undo && GIMP_ITEM_UNDO (undo)->item == GIMP_ITEM (layer))
    push_undo = FALSE;

  opacity = action_select_value ((GimpActionSelectType) value,
                                 gimp_layer_get_opacity (layer),
                                 0.0, 1.0,
                                 0.01, 0.1, FALSE);
  gimp_layer_set_opacity (layer, opacity, push_undo);
  gimp_image_flush (gimage);
}

void
layers_mode_cmd_callback (GtkAction *action,
                          gint       value,
                          gpointer   data)
{
  GimpImage            *gimage;
  GimpLayer            *layer;
  GimpLayerModeEffects  layer_mode;
  gint                  index;
  GimpUndo             *undo;
  gboolean              push_undo = TRUE;
  return_if_no_layer (gimage, layer, data);

  undo = gimp_image_undo_can_compress (gimage, GIMP_TYPE_ITEM_UNDO,
                                       GIMP_UNDO_LAYER_MODE);

  if (undo && GIMP_ITEM_UNDO (undo)->item == GIMP_ITEM (layer))
    push_undo = FALSE;

  layer_mode = gimp_layer_get_mode (layer);

  index = action_select_value ((GimpActionSelectType) value,
                               layers_mode_index (layer_mode),
                               0, G_N_ELEMENTS (layer_modes) - 1,
                               1.0, 1.0, FALSE);
  gimp_layer_set_mode (layer, layer_modes[index], push_undo);
  gimp_image_flush (gimage);
}

void
layers_preserve_trans_cmd_callback (GtkAction *action,
                                    gpointer   data)
{
  GimpImage *gimage;
  GimpLayer *layer;
  gboolean   preserve;
  return_if_no_layer (gimage, layer, data);

  preserve = gtk_toggle_action_get_active (GTK_TOGGLE_ACTION (action));

  if (preserve != gimp_layer_get_preserve_trans (layer))
    {
      GimpUndo *undo;
      gboolean  push_undo = TRUE;

      undo = gimp_image_undo_can_compress (gimage, GIMP_TYPE_ITEM_UNDO,
                                           GIMP_UNDO_LAYER_PRESERVE_TRANS);

      if (undo && GIMP_ITEM_UNDO (undo)->item == GIMP_ITEM (layer))
        push_undo = FALSE;

      gimp_layer_set_preserve_trans (layer, preserve, push_undo);
      gimp_image_flush (gimage);
    }
}


/*  private functions  */

static void
layers_new_layer_response (GtkWidget          *widget,
                           gint                response_id,
                           LayerOptionsDialog *dialog)
{
  if (response_id == GTK_RESPONSE_OK)
    {
      GimpLayer *layer;

      if (layer_name)
        g_free (layer_name);
      layer_name =
        g_strdup (gtk_entry_get_text (GTK_ENTRY (dialog->name_entry)));

      layer_fill_type = dialog->fill_type;

      dialog->xsize =
        RINT (gimp_size_entry_get_refval (GIMP_SIZE_ENTRY (dialog->size_se),
                                          0));
      dialog->ysize =
        RINT (gimp_size_entry_get_refval (GIMP_SIZE_ENTRY (dialog->size_se),
                                          1));

      layer = gimp_layer_new (dialog->gimage,
                              dialog->xsize,
                              dialog->ysize,
                              gimp_image_base_type_with_alpha (dialog->gimage),
                              layer_name,
                              GIMP_OPACITY_OPAQUE, GIMP_NORMAL_MODE);

      if (layer)
        {
          gimp_drawable_fill_by_type (GIMP_DRAWABLE (layer),
                                      dialog->context,
                                      layer_fill_type);
          gimp_image_add_layer (dialog->gimage, layer, -1);

          gimp_image_flush (dialog->gimage);
        }
      else
        {
          g_message ("new_layer_query_response: "
                     "could not allocate new layer");
        }
    }

  gtk_widget_destroy (dialog->dialog);
}

static void
layers_edit_layer_response (GtkWidget          *widget,
                            gint                response_id,
                            LayerOptionsDialog *dialog)
{
  if (response_id == GTK_RESPONSE_OK)
    {
      GimpLayer   *layer = dialog->layer;
      const gchar *new_name;

      new_name = gtk_entry_get_text (GTK_ENTRY (dialog->name_entry));

      if (strcmp (new_name, gimp_object_get_name (GIMP_OBJECT (layer))))
        {
          gimp_item_rename (GIMP_ITEM (layer), new_name);
          gimp_image_flush (dialog->gimage);
        }

      if (dialog->rename_toggle &&
          gimp_drawable_is_text_layer (GIMP_DRAWABLE (layer)))
        {
          g_object_set (layer,
                        "auto-rename",
                        GTK_TOGGLE_BUTTON (dialog->rename_toggle)->active,
                        NULL);
        }
    }

  gtk_widget_destroy (dialog->dialog);
}

static void
layers_add_mask_response (GtkWidget          *widget,
                          gint                response_id,
                          LayerAddMaskDialog *dialog)
{
  if (response_id == GTK_RESPONSE_OK)
    {
      GimpLayer     *layer  = dialog->layer;
      GimpImage     *gimage = gimp_item_get_image (GIMP_ITEM (layer));
      GimpLayerMask *mask;

      layer_add_mask_type = dialog->add_mask_type;
      layer_mask_invert   = dialog->invert;

      gimp_image_undo_group_start (gimage, GIMP_UNDO_GROUP_LAYER_ADD_MASK,
                                   _("Add Layer Mask"));

      mask = gimp_layer_create_mask (layer, layer_add_mask_type);

      if (layer_mask_invert)
        gimp_channel_invert (GIMP_CHANNEL (mask), FALSE);

      gimp_layer_add_mask (layer, mask, TRUE);

      gimp_image_undo_group_end (gimage);

      gimp_image_flush (gimage);
    }

  gtk_widget_destroy (dialog->dialog);
}

static void
layers_scale_layer_callback (GtkWidget             *dialog,
                             GimpViewable          *viewable,
                             gint                   width,
                             gint                   height,
                             GimpUnit               unit,
                             GimpInterpolationType  interpolation,
                             gdouble                xresolution,    /* unused */
                             gdouble                yresolution,    /* unused */
                             GimpUnit               resolution_unit,/* unused */
                             gpointer               data)
{
  GimpDisplay *gdisp = GIMP_DISPLAY (data);

  if (width > 0 && height > 0)
    {
      GimpItem     *item = GIMP_ITEM (viewable);
      GimpProgress *progress;
      GtkWidget    *progress_dialog = NULL;

      gtk_widget_destroy (dialog);

      if (width == gimp_item_width (item) && height == gimp_item_height (item))
        return;

      if (gdisp)
        {
          progress = GIMP_PROGRESS (gdisp);
        }
      else
        {
          progress_dialog = gimp_progress_dialog_new ();
          progress = GIMP_PROGRESS (progress_dialog);
        }

      progress = gimp_progress_start (progress, _("Scaling..."), FALSE);

      gimp_item_scale_by_origin (item,
                                 width, height, interpolation,
                                 progress, TRUE);

      if (progress)
        gimp_progress_end (progress);

      if (progress_dialog)
        gtk_widget_destroy (progress_dialog);

      gimp_image_flush (gimp_item_get_image (item));
    }
  else
    {
      g_message (_("Invalid width or height. Both must be positive."));
    }
}

static void
layers_resize_layer_callback (GtkWidget *widget,
                              gpointer   data)
{
  ResizeLayerOptions *options = data;

  if (options->dialog->width > 0 && options->dialog->height > 0)
    {
      GimpImage *gimage = gimp_item_get_image (GIMP_ITEM (options->layer));

      gtk_widget_set_sensitive (options->dialog->shell, FALSE);

      gimp_item_resize (GIMP_ITEM (options->layer),
                        options->context,
                        options->dialog->width,
                        options->dialog->height,
                        options->dialog->offset_x,
                        options->dialog->offset_y);

      gimp_image_flush (gimage);

      gtk_widget_destroy (options->dialog->shell);
    }
  else
    {
      g_message (_("Invalid width or height. Both must be positive."));
    }
}

static gint
layers_mode_index (GimpLayerModeEffects layer_mode)
{
  gint i = 0;

  while (i < (G_N_ELEMENTS (layer_modes) - 1) && layer_modes[i] != layer_mode)
    i++;

  return i;
}
