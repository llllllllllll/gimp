/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2000 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"


#include <gtk/gtk.h>

#include "core/core-types.h"
#include "procedural_db.h"

#include "appenums.h"
#include "core/gimpimage.h"
#include "core/gimplayer.h"
#include "core/gimplayermask.h"
#include "core/gimplist.h"
#include "drawable.h"
#include "floating_sel.h"
#include "pdb_glue.h"
#include "undo.h"

static ProcRecord layer_new_proc;
static ProcRecord layer_copy_proc;
static ProcRecord layer_create_mask_proc;
static ProcRecord layer_scale_proc;
static ProcRecord layer_resize_proc;
static ProcRecord layer_delete_proc;
static ProcRecord layer_translate_proc;
static ProcRecord layer_add_alpha_proc;
static ProcRecord layer_set_offsets_proc;
static ProcRecord layer_mask_proc;
static ProcRecord layer_is_floating_sel_proc;
static ProcRecord layer_get_name_proc;
static ProcRecord layer_set_name_proc;
static ProcRecord layer_get_visible_proc;
static ProcRecord layer_set_visible_proc;
static ProcRecord layer_get_preserve_trans_proc;
static ProcRecord layer_set_preserve_trans_proc;
static ProcRecord layer_get_opacity_proc;
static ProcRecord layer_set_opacity_proc;
static ProcRecord layer_get_mode_proc;
static ProcRecord layer_set_mode_proc;
static ProcRecord layer_get_linked_proc;
static ProcRecord layer_set_linked_proc;
static ProcRecord layer_get_tattoo_proc;
static ProcRecord layer_set_tattoo_proc;

void
register_layer_procs (void)
{
  procedural_db_register (&layer_new_proc);
  procedural_db_register (&layer_copy_proc);
  procedural_db_register (&layer_create_mask_proc);
  procedural_db_register (&layer_scale_proc);
  procedural_db_register (&layer_resize_proc);
  procedural_db_register (&layer_delete_proc);
  procedural_db_register (&layer_translate_proc);
  procedural_db_register (&layer_add_alpha_proc);
  procedural_db_register (&layer_set_offsets_proc);
  procedural_db_register (&layer_mask_proc);
  procedural_db_register (&layer_is_floating_sel_proc);
  procedural_db_register (&layer_get_name_proc);
  procedural_db_register (&layer_set_name_proc);
  procedural_db_register (&layer_get_visible_proc);
  procedural_db_register (&layer_set_visible_proc);
  procedural_db_register (&layer_get_preserve_trans_proc);
  procedural_db_register (&layer_set_preserve_trans_proc);
  procedural_db_register (&layer_get_opacity_proc);
  procedural_db_register (&layer_set_opacity_proc);
  procedural_db_register (&layer_get_mode_proc);
  procedural_db_register (&layer_set_mode_proc);
  procedural_db_register (&layer_get_linked_proc);
  procedural_db_register (&layer_set_linked_proc);
  procedural_db_register (&layer_get_tattoo_proc);
  procedural_db_register (&layer_set_tattoo_proc);
}

static Argument *
layer_new_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *gimage;
  gint32 width;
  gint32 height;
  gint32 type;
  gchar *name;
  gdouble opacity_arg;
  gint32 mode;
  GimpLayer *layer = NULL;
  int opacity;

  gimage = gimp_image_get_by_ID (args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  width = args[1].value.pdb_int;
  if (width <= 0)
    success = FALSE;

  height = args[2].value.pdb_int;
  if (height <= 0)
    success = FALSE;

  type = args[3].value.pdb_int;
  if (type < RGB_GIMAGE || type > INDEXEDA_GIMAGE)
    success = FALSE;

  name = (gchar *) args[4].value.pdb_pointer;

  opacity_arg = args[5].value.pdb_float;
  if (opacity_arg < 0.0 || opacity_arg > 100.0)
    success = FALSE;

  mode = args[6].value.pdb_int;
  if (mode < NORMAL_MODE || mode > HARDLIGHT_MODE)
    success = FALSE;

  if (success)
    {
      opacity = (int) ((opacity_arg * 255) / 100);
      layer = gimp_layer_new (gimage, width, height,
			      (GimpImageType) type, name,
			      opacity, (LayerModeEffects) mode);
      success = layer != NULL;
    }

  return_args = procedural_db_return_args (&layer_new_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_drawable_get_ID (GIMP_DRAWABLE (layer));

  return return_args;
}

static ProcArg layer_new_inargs[] =
{
  {
    PDB_IMAGE,
    "image",
    "The image to which to add the layer"
  },
  {
    PDB_INT32,
    "width",
    "The layer width: (0 < width)"
  },
  {
    PDB_INT32,
    "height",
    "The layer height: (0 < height)"
  },
  {
    PDB_INT32,
    "type",
    "The layer type: { RGB_IMAGE (0), RGBA_IMAGE (1), GRAY_IMAGE (2), GRAYA_IMAGE (3), INDEXED_IMAGE (4), INDEXEDA_IMAGE (5) }"
  },
  {
    PDB_STRING,
    "name",
    "The layer name"
  },
  {
    PDB_FLOAT,
    "opacity",
    "The layer opacity: (0 <= opacity <= 100)"
  },
  {
    PDB_INT32,
    "mode",
    "The layer combination mode: { NORMAL_MODE (0), DISSOLVE_MODE (1), BEHIND_MODE (2), MULTIPLY_MODE (3), SCREEN_MODE (4), OVERLAY_MODE (5), DIFFERENCE_MODE (6), ADDITION_MODE (7), SUBTRACT_MODE (8), DARKEN_ONLY_MODE (9), LIGHTEN_ONLY_MODE (10), HUE_MODE (11), SATURATION_MODE (12), COLOR_MODE (13), VALUE_MODE (14), DIVIDE_MODE (15), DODGE_MODE (16), BURN_MODE (17), HARDLIGHT_MODE (18) }"
  }
};

static ProcArg layer_new_outargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The newly created layer"
  }
};

static ProcRecord layer_new_proc =
{
  "gimp_layer_new",
  "Create a new layer.",
  "This procedure creates a new layer with the specified width, height, and type. Name, opacity, and mode are also supplied parameters. The new layer still needs to be added to the image, as this is not automatic. Add the new layer with the 'gimp_image_add_layer' command. Other attributes such as layer mask modes, and offsets should be set with explicit procedure calls.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  7,
  layer_new_inargs,
  1,
  layer_new_outargs,
  { { layer_new_invoker } }
};

static Argument *
layer_copy_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;
  gboolean add_alpha;
  GimpLayer *copy = NULL;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  add_alpha = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    success = (copy = gimp_layer_copy (layer, add_alpha)) != NULL;

  return_args = procedural_db_return_args (&layer_copy_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_drawable_get_ID (GIMP_DRAWABLE (copy));

  return return_args;
}

static ProcArg layer_copy_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer to copy"
  },
  {
    PDB_INT32,
    "add_alpha",
    "Add an alpha channel to the copied layer"
  }
};

static ProcArg layer_copy_outargs[] =
{
  {
    PDB_LAYER,
    "layer_copy",
    "The newly copied layer"
  }
};

static ProcRecord layer_copy_proc =
{
  "gimp_layer_copy",
  "Copy a layer.",
  "This procedure copies the specified layer and returns the copy. The newly copied layer is for use within the original layer's image. It should not be subsequently added to any other image. The copied layer can optionally have an added alpha channel. This is useful if the background layer in an image is being copied and added to the same image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  layer_copy_inargs,
  1,
  layer_copy_outargs,
  { { layer_copy_invoker } }
};

static Argument *
layer_create_mask_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;
  gint32 mask_type;
  GimpLayerMask *mask = NULL;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  mask_type = args[1].value.pdb_int;
  if (mask_type < ADD_WHITE_MASK || mask_type > ADD_INV_SELECTION_MASK)
    success = FALSE;

  if (success)
    success = (mask = gimp_layer_create_mask (layer, (AddMaskType) mask_type)) != NULL;

  return_args = procedural_db_return_args (&layer_create_mask_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_drawable_get_ID (GIMP_DRAWABLE (mask));

  return return_args;
}

static ProcArg layer_create_mask_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer to which to add the mask"
  },
  {
    PDB_INT32,
    "mask_type",
    "The type of mask: { WHITE_MASK (0), BLACK_MASK (1), ALPHA_MASK (2), SELECTION_MASK (3), INV_SELECTION_MASK (4) }"
  }
};

static ProcArg layer_create_mask_outargs[] =
{
  {
    PDB_CHANNEL,
    "mask",
    "The newly created mask"
  }
};

static ProcRecord layer_create_mask_proc =
{
  "gimp_layer_create_mask",
  "Create a layer mask for the specified specified layer.",
  "This procedure creates a layer mask for the specified layer. Layer masks serve as an additional alpha channel for a layer. Three different types of masks are allowed initially: completely white masks (which will leave the layer fully visible), completely black masks (which will give the layer complete transparency, and the layer's already existing alpha channel (which will leave the layer fully visible, but which may be more useful than a white mask). The layer mask still needs to be added to the layer. This can be done with a call to 'gimage_add_layer_mask'.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  layer_create_mask_inargs,
  1,
  layer_create_mask_outargs,
  { { layer_create_mask_invoker } }
};

static Argument *
layer_scale_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gint32 new_width;
  gint32 new_height;
  gboolean local_origin;
  GimpImage *gimage;
  GimpLayer *floating_layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  new_width = args[1].value.pdb_int;
  if (new_width <= 0)
    success = FALSE;

  new_height = args[2].value.pdb_int;
  if (new_height <= 0)
    success = FALSE;

  local_origin = args[3].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      if ((gimage = GIMP_DRAWABLE(layer)->gimage))
	{
	  floating_layer = gimp_image_floating_sel (gimage);
    
	  undo_push_group_start (gimage, LAYER_SCALE_UNDO);
    
	  if (floating_layer)
	    floating_sel_relax (floating_layer, TRUE);
    
	  gimp_layer_scale (layer, new_width, new_height, local_origin);
    
	  if (floating_layer)
	    floating_sel_rigor (floating_layer, TRUE);
    
	  undo_push_group_end (gimage);
	}
      else
	success = FALSE;
    }

  return procedural_db_return_args (&layer_scale_proc, success);
}

static ProcArg layer_scale_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "new_width",
    "New layer width: (0 < new_width)"
  },
  {
    PDB_INT32,
    "new_height",
    "New layer height: (0 < new_height)"
  },
  {
    PDB_INT32,
    "local_origin",
    "Use a local origin (as opposed to the image origin)"
  }
};

static ProcRecord layer_scale_proc =
{
  "gimp_layer_scale",
  "Scale the layer to the specified extents.",
  "This procedure scales the layer so that it's new width and height are equal to the supplied parameters. The \"local_origin\" parameter specifies whether to scale from the center of the layer, or from the image origin. This operation only works if the layer has been added to an image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  4,
  layer_scale_inargs,
  0,
  NULL,
  { { layer_scale_invoker } }
};

static Argument *
layer_resize_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gint32 new_width;
  gint32 new_height;
  gint32 offx;
  gint32 offy;
  GimpImage *gimage;
  GimpLayer *floating_layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  new_width = args[1].value.pdb_int;
  if (new_width <= 0)
    success = FALSE;

  new_height = args[2].value.pdb_int;
  if (new_height <= 0)
    success = FALSE;

  offx = args[3].value.pdb_int;

  offy = args[4].value.pdb_int;

  if (success)
    {
      if ((gimage = GIMP_DRAWABLE(layer)->gimage))
	{
	  floating_layer = gimp_image_floating_sel (gimage);
    
	  undo_push_group_start (gimage, LAYER_RESIZE_UNDO);
    
	  if (floating_layer)
	    floating_sel_relax (floating_layer, TRUE);
    
	  gimp_layer_resize (layer, new_width, new_height, offx, offy);
    
	  if (floating_layer)
	    floating_sel_rigor (floating_layer, TRUE);
    
	  undo_push_group_end (gimage);
	}
      else
	success = FALSE;
    }

  return procedural_db_return_args (&layer_resize_proc, success);
}

static ProcArg layer_resize_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "new_width",
    "New layer width: (0 < new_width)"
  },
  {
    PDB_INT32,
    "new_height",
    "New layer height: (0 < new_height)"
  },
  {
    PDB_INT32,
    "offx",
    "x offset between upper left corner of old and new layers: (new - old)"
  },
  {
    PDB_INT32,
    "offy",
    "y offset between upper left corner of old and new layers: (new - old)"
  }
};

static ProcRecord layer_resize_proc =
{
  "gimp_layer_resize",
  "Resize the layer to the specified extents.",
  "This procedure resizes the layer so that it's new width and height are equal to the supplied parameters. Offsets are also provided which describe the position of the previous layer's content. This operation only works if the layer has been added to an image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  5,
  layer_resize_inargs,
  0,
  NULL,
  { { layer_resize_invoker } }
};

static Argument *
layer_delete_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  if (success)
    gtk_object_sink (GTK_OBJECT (layer));

  return procedural_db_return_args (&layer_delete_proc, success);
}

static ProcArg layer_delete_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer to delete"
  }
};

static ProcRecord layer_delete_proc =
{
  "gimp_layer_delete",
  "Delete a layer.",
  "This procedure deletes the specified layer. This must not be done if the gimage containing this layer was already deleted or if the layer was already removed from the image. The only case in which this procedure is useful is if you want to get rid of a layer which has not yet been added to an image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_delete_inargs,
  0,
  NULL,
  { { layer_delete_invoker } }
};

static Argument *
layer_translate_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gint32 offx;
  gint32 offy;
  GimpImage *gimage;
  GimpLayer *floating_layer;
  GimpLayer *tmp_layer;
  GList *layer_list;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  offx = args[1].value.pdb_int;

  offy = args[2].value.pdb_int;

  if (success)
    {
      if ((gimage = GIMP_DRAWABLE(layer)->gimage))
	{
	  floating_layer = gimp_image_floating_sel (gimage);
    
	  undo_push_group_start (gimage, LINKED_LAYER_UNDO);
    
	  if (floating_layer)
	    floating_sel_relax (floating_layer, TRUE);
    
	  for (layer_list = GIMP_LIST (gimage->layers)->list;
	       layer_list;
	       layer_list = g_list_next (layer_list))
	    {
	      tmp_layer = (GimpLayer *) layer_list->data;
	
	      if ((tmp_layer == layer) || gimp_layer_get_linked (tmp_layer))
		gimp_layer_translate (tmp_layer, offx, offy);
	    }
    
	  if (floating_layer)
	    floating_sel_rigor (floating_layer, TRUE);
    
	  undo_push_group_end (gimage);
	}
      else
	success = FALSE;
    }

  return procedural_db_return_args (&layer_translate_proc, success);
}

static ProcArg layer_translate_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "offx",
    "Offset in x direction"
  },
  {
    PDB_INT32,
    "offy",
    "Offset in y direction"
  }
};

static ProcRecord layer_translate_proc =
{
  "gimp_layer_translate",
  "Translate the layer by the specified offsets.",
  "This procedure translates the layer by the amounts specified in the x and y arguments. These can be negative, and are considered offsets from the current position. This command only works if the layer has been added to an image. All additional layers contained in the image which have the linked flag set to TRUE w ill also be translated by the specified offsets.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  3,
  layer_translate_inargs,
  0,
  NULL,
  { { layer_translate_invoker } }
};

static Argument *
layer_add_alpha_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  if (success)
    gimp_layer_add_alpha (layer);

  return procedural_db_return_args (&layer_add_alpha_proc, success);
}

static ProcArg layer_add_alpha_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcRecord layer_add_alpha_proc =
{
  "gimp_layer_add_alpha",
  "Add an alpha channel to the layer if it doesn't already have one.",
  "This procedure adds an additional component to the specified layer if it does not already possess an alpha channel. An alpha channel makes it possible to move a layer from the bottom of the layer stack and to clear and erase to transparency, instead of the background color. This transforms images of type RGB to RGBA, GRAY to GRAYA, and INDEXED to INDEXEDA.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_add_alpha_inargs,
  0,
  NULL,
  { { layer_add_alpha_invoker } }
};

static Argument *
layer_set_offsets_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gint32 offx;
  gint32 offy;
  GimpImage *gimage;
  GimpLayer *floating_layer;
  GimpLayer *tmp_layer;
  GList *layer_list;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  offx = args[1].value.pdb_int;

  offy = args[2].value.pdb_int;

  if (success)
    {
      if ((gimage = GIMP_DRAWABLE(layer)->gimage))
	{
	  floating_layer = gimp_image_floating_sel (gimage);
    
	  undo_push_group_start (gimage, LINKED_LAYER_UNDO);
    
	  if (floating_layer)
	    floating_sel_relax (floating_layer, TRUE);
    
	  for (layer_list = GIMP_LIST (gimage->layers)->list;
	       layer_list;
	       layer_list = g_list_next (layer_list))
	    {
	      tmp_layer = (GimpLayer *) layer_list->data;
	
	      if ((tmp_layer == layer) || gimp_layer_get_linked (tmp_layer))
		gimp_layer_translate (tmp_layer,
				(offx - GIMP_DRAWABLE (layer)->offset_x),
				(offy - GIMP_DRAWABLE (layer)->offset_y));
	    }
    
	  if (floating_layer)
	    floating_sel_rigor (floating_layer, TRUE);
    
	  undo_push_group_end (gimage);
	}
      else
	success = FALSE;
    }

  return procedural_db_return_args (&layer_set_offsets_proc, success);
}

static ProcArg layer_set_offsets_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "offx",
    "Offset in x direction"
  },
  {
    PDB_INT32,
    "offy",
    "Offset in y direction"
  }
};

static ProcRecord layer_set_offsets_proc =
{
  "gimp_layer_set_offsets",
  "Set the layer offsets.",
  "This procedure sets the offsets for the specified layer. The offsets are relative to the image origin and can be any values. This operation is valid only on layers which have been added to an image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  3,
  layer_set_offsets_inargs,
  0,
  NULL,
  { { layer_set_offsets_invoker } }
};

static Argument *
layer_mask_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_mask_proc, success);

  if (success)
    return_args[1].value.pdb_int = layer->mask ? gimp_drawable_get_ID (GIMP_DRAWABLE (layer->mask)) : -1;

  return return_args;
}

static ProcArg layer_mask_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_mask_outargs[] =
{
  {
    PDB_CHANNEL,
    "mask",
    "The layer mask"
  }
};

static ProcRecord layer_mask_proc =
{
  "gimp_layer_mask",
  "Get the specified layer's mask if it exists.",
  "This procedure returns the specified layer's mask, or -1 if none exists.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_mask_inargs,
  1,
  layer_mask_outargs,
  { { layer_mask_invoker } }
};

static Argument *
layer_is_floating_sel_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_is_floating_sel_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_layer_is_floating_sel (layer);

  return return_args;
}

static ProcArg layer_is_floating_sel_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_is_floating_sel_outargs[] =
{
  {
    PDB_INT32,
    "is_floating_sel",
    "Non-zero if the layer is a floating selection"
  }
};

static ProcRecord layer_is_floating_sel_proc =
{
  "gimp_layer_is_floating_sel",
  "Is the specified layer a floating selection?",
  "This procedure returns whether the layer is a floating selection. Floating selections are special cases of layers which are attached to a specific drawable.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_is_floating_sel_inargs,
  1,
  layer_is_floating_sel_outargs,
  { { layer_is_floating_sel_invoker } }
};

static Argument *
layer_get_name_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_get_name_proc, success);

  if (success)
    return_args[1].value.pdb_pointer = g_strdup (gimp_layer_get_name (layer));

  return return_args;
}

static ProcArg layer_get_name_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_get_name_outargs[] =
{
  {
    PDB_STRING,
    "name",
    "The layer name"
  }
};

static ProcRecord layer_get_name_proc =
{
  "gimp_layer_get_name",
  "Get the name of the specified layer.",
  "This procedure returns the specified layer's name.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_get_name_inargs,
  1,
  layer_get_name_outargs,
  { { layer_get_name_invoker } }
};

static Argument *
layer_set_name_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gchar *name;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  name = (gchar *) args[1].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  if (success)
    gimp_layer_set_name (layer, name);

  return procedural_db_return_args (&layer_set_name_proc, success);
}

static ProcArg layer_set_name_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_STRING,
    "name",
    "The new layer name"
  }
};

static ProcRecord layer_set_name_proc =
{
  "gimp_layer_set_name",
  "Set the name of the specified layer.",
  "This procedure sets the specified layer's name.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  layer_set_name_inargs,
  0,
  NULL,
  { { layer_set_name_invoker } }
};

static Argument *
layer_get_visible_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_get_visible_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_layer_get_visible (layer);

  return return_args;
}

static ProcArg layer_get_visible_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_get_visible_outargs[] =
{
  {
    PDB_INT32,
    "visible",
    "The layer visibility"
  }
};

static ProcRecord layer_get_visible_proc =
{
  "gimp_layer_get_visible",
  "Get the visibility of the specified layer.",
  "This procedure returns the specified layer's visibility.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_get_visible_inargs,
  1,
  layer_get_visible_outargs,
  { { layer_get_visible_invoker } }
};

static Argument *
layer_set_visible_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gboolean visible;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  visible = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    gimp_layer_set_visible (layer, visible);

  return procedural_db_return_args (&layer_set_visible_proc, success);
}

static ProcArg layer_set_visible_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "visible",
    "The new layer visibility"
  }
};

static ProcRecord layer_set_visible_proc =
{
  "gimp_layer_set_visible",
  "Set the visibility of the specified layer.",
  "This procedure sets the specified layer's visibility.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  layer_set_visible_inargs,
  0,
  NULL,
  { { layer_set_visible_invoker } }
};

static Argument *
layer_get_preserve_trans_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_get_preserve_trans_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_layer_get_preserve_trans (layer);

  return return_args;
}

static ProcArg layer_get_preserve_trans_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_get_preserve_trans_outargs[] =
{
  {
    PDB_INT32,
    "preserve_trans",
    "The layer's preserve transperancy setting"
  }
};

static ProcRecord layer_get_preserve_trans_proc =
{
  "gimp_layer_get_preserve_trans",
  "Get the preserve transperancy setting of the specified layer.",
  "This procedure returns the specified layer's preserve transperancy setting.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_get_preserve_trans_inargs,
  1,
  layer_get_preserve_trans_outargs,
  { { layer_get_preserve_trans_invoker } }
};

static Argument *
layer_set_preserve_trans_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gboolean preserve_trans;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  preserve_trans = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    gimp_layer_set_preserve_trans (layer, preserve_trans);

  return procedural_db_return_args (&layer_set_preserve_trans_proc, success);
}

static ProcArg layer_set_preserve_trans_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "preserve_trans",
    "The new layer's preserve transperancy setting"
  }
};

static ProcRecord layer_set_preserve_trans_proc =
{
  "gimp_layer_set_preserve_trans",
  "Set the preserve transperancy setting of the specified layer.",
  "This procedure sets the specified layer's preserve transperancy setting.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  layer_set_preserve_trans_inargs,
  0,
  NULL,
  { { layer_set_preserve_trans_invoker } }
};

static Argument *
layer_get_opacity_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_get_opacity_proc, success);

  if (success)
    return_args[1].value.pdb_float = gimp_layer_get_opacity (layer) * 100.0;

  return return_args;
}

static ProcArg layer_get_opacity_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_get_opacity_outargs[] =
{
  {
    PDB_FLOAT,
    "opacity",
    "The layer opacity"
  }
};

static ProcRecord layer_get_opacity_proc =
{
  "gimp_layer_get_opacity",
  "Get the opacity of the specified layer.",
  "This procedure returns the specified layer's opacity.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_get_opacity_inargs,
  1,
  layer_get_opacity_outargs,
  { { layer_get_opacity_invoker } }
};

static Argument *
layer_set_opacity_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gdouble opacity;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  opacity = args[1].value.pdb_float;
  if (opacity < 0.0 || opacity > 100.0)
    success = FALSE;

  if (success)
    gimp_layer_set_opacity (layer, opacity / 100.0);

  return procedural_db_return_args (&layer_set_opacity_proc, success);
}

static ProcArg layer_set_opacity_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_FLOAT,
    "opacity",
    "The new layer opacity (0 <= opacity <= 100)"
  }
};

static ProcRecord layer_set_opacity_proc =
{
  "gimp_layer_set_opacity",
  "Set the opacity of the specified layer.",
  "This procedure sets the specified layer's opacity.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  layer_set_opacity_inargs,
  0,
  NULL,
  { { layer_set_opacity_invoker } }
};

static Argument *
layer_get_mode_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_get_mode_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_layer_get_mode (layer);

  return return_args;
}

static ProcArg layer_get_mode_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_get_mode_outargs[] =
{
  {
    PDB_INT32,
    "mode",
    "The layer combination mode"
  }
};

static ProcRecord layer_get_mode_proc =
{
  "gimp_layer_get_mode",
  "Get the combination mode of the specified layer.",
  "This procedure returns the specified layer's combination mode.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  layer_get_mode_inargs,
  1,
  layer_get_mode_outargs,
  { { layer_get_mode_invoker } }
};

static Argument *
layer_set_mode_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gint32 mode;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  mode = args[1].value.pdb_int;
  if (mode < NORMAL_MODE || mode > HARDLIGHT_MODE)
    success = FALSE;

  if (success)
    gimp_layer_set_mode (layer, mode);

  return procedural_db_return_args (&layer_set_mode_proc, success);
}

static ProcArg layer_set_mode_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "mode",
    "The new layer combination mode"
  }
};

static ProcRecord layer_set_mode_proc =
{
  "gimp_layer_set_mode",
  "Set the combination mode of the specified layer.",
  "This procedure sets the specified layer's combination mode.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  layer_set_mode_inargs,
  0,
  NULL,
  { { layer_set_mode_invoker } }
};

static Argument *
layer_get_linked_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_get_linked_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_layer_get_linked (layer);

  return return_args;
}

static ProcArg layer_get_linked_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_get_linked_outargs[] =
{
  {
    PDB_INT32,
    "linked",
    "The layer linked state (for moves)"
  }
};

static ProcRecord layer_get_linked_proc =
{
  "gimp_layer_get_linked",
  "Get the linked state of the specified layer.",
  "This procedure returns the specified layer's linked state.",
  "Wolfgang Hofer",
  "Wolfgang Hofer",
  "1998",
  PDB_INTERNAL,
  1,
  layer_get_linked_inargs,
  1,
  layer_get_linked_outargs,
  { { layer_get_linked_invoker } }
};

static Argument *
layer_set_linked_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gboolean linked;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  linked = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    gimp_layer_set_linked (layer, linked);

  return procedural_db_return_args (&layer_set_linked_proc, success);
}

static ProcArg layer_set_linked_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "linked",
    "The new layer linked state"
  }
};

static ProcRecord layer_set_linked_proc =
{
  "gimp_layer_set_linked",
  "Set the linked state of the specified layer.",
  "This procedure sets the specified layer's linked state.",
  "Wolfgang Hofer",
  "Wolfgang Hofer",
  "1998",
  PDB_INTERNAL,
  2,
  layer_set_linked_inargs,
  0,
  NULL,
  { { layer_set_linked_invoker } }
};

static Argument *
layer_get_tattoo_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpLayer *layer;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  return_args = procedural_db_return_args (&layer_get_tattoo_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_layer_get_tattoo (layer);

  return return_args;
}

static ProcArg layer_get_tattoo_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  }
};

static ProcArg layer_get_tattoo_outargs[] =
{
  {
    PDB_INT32,
    "tattoo",
    "The layer tattoo"
  }
};

static ProcRecord layer_get_tattoo_proc =
{
  "gimp_layer_get_tattoo",
  "Get the tattoo of the specified layer.",
  "This procedure returns the specified layer's tattoo. A tattoo is a unique and permanent identifier attached to a layer that can be used to uniquely identify a layer within an image even between sessions",
  "Jay Cox",
  "Jay Cox",
  "1998",
  PDB_INTERNAL,
  1,
  layer_get_tattoo_inargs,
  1,
  layer_get_tattoo_outargs,
  { { layer_get_tattoo_invoker } }
};

static Argument *
layer_set_tattoo_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpLayer *layer;
  gint32 tattoo;

  layer = (GimpLayer *) gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (layer == NULL)
    success = FALSE;

  tattoo = args[1].value.pdb_int;
  if (tattoo == 0)
    success = FALSE;

  if (success)
    gimp_layer_set_tattoo (layer, tattoo);

  return procedural_db_return_args (&layer_set_tattoo_proc, success);
}

static ProcArg layer_set_tattoo_inargs[] =
{
  {
    PDB_LAYER,
    "layer",
    "The layer"
  },
  {
    PDB_INT32,
    "tattoo",
    "The new layer tattoo"
  }
};

static ProcRecord layer_set_tattoo_proc =
{
  "gimp_layer_set_tattoo",
  "Set the tattoo of the specified layer.",
  "This procedure sets the specified layer's tattoo. A tattoo is a unique and permanent identifier attached to a layer that can be used to uniquely identify a layer within an image even between sessions",
  "Jay Cox",
  "Jay Cox",
  "1998",
  PDB_INTERNAL,
  2,
  layer_set_tattoo_inargs,
  0,
  NULL,
  { { layer_set_tattoo_invoker } }
};
