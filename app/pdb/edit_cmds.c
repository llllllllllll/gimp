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
#include "core/gimpimage-mask.h"
#include "core/gimplayer.h"
#include "drawable.h"
#include "global_edit.h"

extern TileManager *global_buf;

static ProcRecord edit_cut_proc;
static ProcRecord edit_copy_proc;
static ProcRecord edit_paste_proc;
static ProcRecord edit_clear_proc;
static ProcRecord edit_fill_proc;
static ProcRecord edit_stroke_proc;

void
register_edit_procs (void)
{
  procedural_db_register (&edit_cut_proc);
  procedural_db_register (&edit_copy_proc);
  procedural_db_register (&edit_paste_proc);
  procedural_db_register (&edit_clear_proc);
  procedural_db_register (&edit_fill_proc);
  procedural_db_register (&edit_stroke_proc);
}

static Argument *
edit_cut_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  GimpImage *gimage;

  drawable = gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  if (success)
    {
      gimage = gimp_drawable_gimage (GIMP_DRAWABLE (drawable));
      success = edit_cut (gimage, drawable) != NULL;
    }

  return procedural_db_return_args (&edit_cut_proc, success);
}

static ProcArg edit_cut_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable to cut from"
  }
};

static ProcRecord edit_cut_proc =
{
  "gimp_edit_cut",
  "Cut from the specified drawable.",
  "If there is a selection in the image, then the area specified by the selection is cut from the specified drawable and placed in an internal GIMP edit buffer. It can subsequently be retrieved using the 'gimp-edit-paste' command. If there is no selection, then the specified drawable will be removed and its contents stored in the internal GIMP edit buffer. The drawable MUST belong to the specified image, or an error is returned.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  edit_cut_inargs,
  0,
  NULL,
  { { edit_cut_invoker } }
};

static Argument *
edit_copy_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  GimpImage *gimage;

  drawable = gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  if (success)
    {
      gimage = gimp_drawable_gimage (GIMP_DRAWABLE (drawable));
      success = edit_copy (gimage, drawable) != NULL;
    }

  return procedural_db_return_args (&edit_copy_proc, success);
}

static ProcArg edit_copy_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable to copy from"
  }
};

static ProcRecord edit_copy_proc =
{
  "gimp_edit_copy",
  "Copy from the specified drawable.",
  "If there is a selection in the image, then the area specified by the selection is copied from the specified drawable and placed in an internal GIMP edit buffer. It can subsequently be retrieved using the 'gimp-edit-paste' command. If there is no selection, then the specified drawable's contents will be stored in the internal GIMP edit buffer. The drawable MUST belong to the specified image, or an error is returned.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  edit_copy_inargs,
  0,
  NULL,
  { { edit_copy_invoker } }
};

static Argument *
edit_paste_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gboolean paste_into;
  GimpLayer *layer = NULL;
  GimpImage *gimage;

  drawable = gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  paste_into = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      gimage = gimp_drawable_gimage (GIMP_DRAWABLE (drawable));
      layer = edit_paste (gimage, drawable, global_buf, paste_into);
      success = layer != NULL;
    }

  return_args = procedural_db_return_args (&edit_paste_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_drawable_get_ID (GIMP_DRAWABLE (layer));

  return return_args;
}

static ProcArg edit_paste_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable to paste to"
  },
  {
    PDB_INT32,
    "paste_into",
    "Clear selection, or paste behind it?"
  }
};

static ProcArg edit_paste_outargs[] =
{
  {
    PDB_LAYER,
    "floating_sel",
    "The new floating selection"
  }
};

static ProcRecord edit_paste_proc =
{
  "gimp_edit_paste",
  "Paste buffer to the specified drawable.",
  "This procedure pastes a copy of the internal GIMP edit buffer to the specified drawable. The GIMP edit buffer will be empty unless a call was previously made to either 'gimp-edit-cut' or 'gimp-edit-copy'. The \"paste_into\" option specifies whether to clear the current image selection, or to paste the buffer \"behind\" the selection. This allows the selection to act as a mask for the pasted buffer. Anywhere that the selection mask is non-zero, the pasted buffer will show through. The pasted buffer will be a new layer in the image which is designated as the image floating selection. If the image has a floating selection at the time of pasting, the old floating selection will be anchored to it's drawable before the new floating selection is added. This procedure returns the new floating layer. The resulting floating selection will already be attached to the specified drawable, and a subsequent call to floating_sel_attach is not needed.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  edit_paste_inargs,
  1,
  edit_paste_outargs,
  { { edit_paste_invoker } }
};

static Argument *
edit_clear_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  GimpImage *gimage;

  drawable = gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  if (success)
    {
      gimage = gimp_drawable_gimage (GIMP_DRAWABLE (drawable));
      success = edit_clear (gimage, drawable);
    }

  return procedural_db_return_args (&edit_clear_proc, success);
}

static ProcArg edit_clear_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable to clear from"
  }
};

static ProcRecord edit_clear_proc =
{
  "gimp_edit_clear",
  "Clear selected area of drawable.",
  "This procedure clears the specified drawable. If the drawable has an alpha channel, the cleared pixels will become transparent. If the drawable does not have an alpha channel, cleared pixels will be set to the background color. This procedure only affects regions within a selection if there is a selection active.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  edit_clear_inargs,
  0,
  NULL,
  { { edit_clear_invoker } }
};

static Argument *
edit_fill_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 fill_type;
  GimpImage *gimage;

  drawable = gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  fill_type = args[1].value.pdb_int;
  if (fill_type < FOREGROUND_FILL || fill_type > NO_FILL)
    success = FALSE;

  if (success)
    {
      gimage = gimp_drawable_gimage (GIMP_DRAWABLE (drawable));
      success = edit_fill (gimage, drawable, (GimpFillType) fill_type);
    }

  return procedural_db_return_args (&edit_fill_proc, success);
}

static ProcArg edit_fill_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable to fill to"
  },
  {
    PDB_INT32,
    "fill_type",
    "The type of fill: FG_IMAGE_FILL (0), BG_IMAGE_FILL (1), WHITE_IMAGE_FILL (2), TRANS_IMAGE_FILL (3), NO_IMAGE_FILL (4)"
  }
};

static ProcRecord edit_fill_proc =
{
  "gimp_edit_fill",
  "Fill selected area of drawable.",
  "This procedure fills the specified drawable with the fill mode. If the fill mode is foreground, the current foreground color is used. If the fill mode is background, the current background color is used. Other fill modes should not be used. This procedure only affects regions within a selection if there is a selection active.",
  "Spencer Kimball & Peter Mattis & Raphael Quinet",
  "Spencer Kimball & Peter Mattis",
  "1995-2000",
  PDB_INTERNAL,
  2,
  edit_fill_inargs,
  0,
  NULL,
  { { edit_fill_invoker } }
};

static Argument *
edit_stroke_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  GimpImage *gimage;

  drawable = gimp_drawable_get_by_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  if (success)
    {
      gimage = gimp_drawable_gimage (GIMP_DRAWABLE (drawable));
      success = gimage_mask_stroke (gimage, drawable);
    }

  return procedural_db_return_args (&edit_stroke_proc, success);
}

static ProcArg edit_stroke_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable to stroke to"
  }
};

static ProcRecord edit_stroke_proc =
{
  "gimp_edit_stroke",
  "Stroke the current selection",
  "This procedure strokes the current selection, painting along the selection boundary with the active brush and foreground color. The paint is applied to the specified drawable regardless of the active selection.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  edit_stroke_inargs,
  0,
  NULL,
  { { edit_stroke_invoker } }
};
