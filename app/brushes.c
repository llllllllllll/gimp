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

#include <gtk/gtk.h>

#include "apptypes.h"

#include "brush_select.h"
#include "brushes.h"
#include "gimpbrush.h"
#include "gimpbrushgenerated.h"
#include "gimpbrushpipe.h"
#include "gimpcontext.h"
#include "gimpdatalist.h"
#include "gimprc.h"


/*  global variables  */
GimpContainer *global_brush_list = NULL;


/*  public functions  */

void
brushes_init (gboolean no_data)
{
  if (global_brush_list)
    brushes_free ();
  else
    global_brush_list = GIMP_CONTAINER (gimp_data_list_new (GIMP_TYPE_BRUSH));

  if (brush_path != NULL && !no_data)
    {
      gchar *common_brush_path;

      brush_select_freeze_all ();

      common_brush_path = g_strconcat (brush_path,
				       G_SEARCHPATH_SEPARATOR_S,
				       brush_vbr_path,
				       NULL);

      gimp_data_list_load (GIMP_DATA_LIST (global_brush_list),
			   common_brush_path,

			   (GimpDataObjectLoaderFunc) gimp_brush_load,
			   GIMP_BRUSH_FILE_EXTENSION,

			   (GimpDataObjectLoaderFunc) gimp_brush_load,
			   GIMP_BRUSH_PIXMAP_FILE_EXTENSION,

			   (GimpDataObjectLoaderFunc) gimp_brush_generated_load,
			   GIMP_BRUSH_GENERATED_FILE_EXTENSION,

			   (GimpDataObjectLoaderFunc) gimp_brush_pipe_load,
			   GIMP_BRUSH_PIPE_FILE_EXTENSION,

			   NULL);

      g_free (common_brush_path);

      brush_select_thaw_all ();
    }

  gimp_context_refresh_brushes ();
}

void
brushes_free (void)
{
  if (! global_brush_list)
    return;

  brush_select_freeze_all ();

  gimp_data_list_save_and_clear (GIMP_DATA_LIST (global_brush_list),
				 brush_vbr_path,
				 GIMP_BRUSH_GENERATED_FILE_EXTENSION);

  brush_select_thaw_all ();
}

GimpBrush *
brushes_get_standard_brush (void)
{
  static GimpBrush *standard_brush = NULL;

  if (! standard_brush)
    {
      standard_brush =
	GIMP_BRUSH (gimp_brush_generated_new (5.0, 0.5, 0.0, 1.0));

      gimp_object_set_name (GIMP_OBJECT (standard_brush), "Standard");

      /*  set ref_count to 2 --> never swap the standard brush  */
      gtk_object_ref (GTK_OBJECT (standard_brush));
      gtk_object_ref (GTK_OBJECT (standard_brush));
      gtk_object_sink (GTK_OBJECT (standard_brush));
    }

  return standard_brush;
}
