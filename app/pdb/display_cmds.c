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

#include "core/gimpimage.h"
#include "gdisplay.h"

static ProcRecord display_new_proc;
static ProcRecord display_delete_proc;
static ProcRecord displays_flush_proc;

void
register_display_procs (void)
{
  procedural_db_register (&display_new_proc);
  procedural_db_register (&display_delete_proc);
  procedural_db_register (&displays_flush_proc);
}

static Argument *
display_new_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpImage *gimage;
  GDisplay *gdisp = NULL;
  guint scale = 0x101;

  gimage = gimp_image_get_by_ID (args[0].value.pdb_int);
  if (gimage == NULL)
    success = FALSE;

  if (success)
    {
      if (! gimp_image_is_empty (gimage))
	success = (gdisp = gdisplay_new (gimage, scale)) != NULL;
      else
	success = FALSE;
    }

  return_args = procedural_db_return_args (&display_new_proc, success);

  if (success)
    return_args[1].value.pdb_int = gdisp->ID;

  return return_args;
}

static ProcArg display_new_inargs[] =
{
  {
    PDB_IMAGE,
    "image",
    "The image"
  }
};

static ProcArg display_new_outargs[] =
{
  {
    PDB_DISPLAY,
    "display",
    "The new display"
  }
};

static ProcRecord display_new_proc =
{
  "gimp_display_new",
  "Create a new display for the specified image.",
  "Creates a new display for the specified image. If the image already has a display, another is added. Multiple displays are handled transparently by the GIMP. The newly created display is returned and can be subsequently destroyed with a call to 'gimp-display-delete'. This procedure only makes sense for use with the GIMP UI.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  display_new_inargs,
  1,
  display_new_outargs,
  { { display_new_invoker } }
};

static Argument *
display_delete_invoker (Argument *args)
{
  gboolean success = TRUE;
  GDisplay *gdisp;

  gdisp = gdisplay_get_by_ID (args[0].value.pdb_int);
  if (gdisp == NULL)
    success = FALSE;

  if (success)
    gtk_widget_destroy (gdisp->shell);

  return procedural_db_return_args (&display_delete_proc, success);
}

static ProcArg display_delete_inargs[] =
{
  {
    PDB_DISPLAY,
    "display",
    "The display to delete"
  }
};

static ProcRecord display_delete_proc =
{
  "gimp_display_delete",
  "Delete the specified display.",
  "This procedure removes the specified display. If this is the last remaining display for the underlying image, then the image is deleted also.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  display_delete_inargs,
  0,
  NULL,
  { { display_delete_invoker } }
};

static Argument *
displays_flush_invoker (Argument *args)
{
  gdisplays_flush ();
  return procedural_db_return_args (&displays_flush_proc, TRUE);
}

static ProcRecord displays_flush_proc =
{
  "gimp_displays_flush",
  "Flush all internal changes to the user interface",
  "This procedure takes no arguments and returns nothing except a success status. Its purpose is to flush all pending updates of image manipulations to the user interface. It should be called whenever appropriate.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  0,
  NULL,
  0,
  NULL,
  { { displays_flush_invoker } }
};
