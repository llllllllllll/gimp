/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimpbrushes_pdb.c
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "gimp.h"

gchar *
gimp_brushes_get_brush_data (gchar                 *brush_name,
			     gdouble               *opacity,
			     gint                  *spacing,
			     GimpLayerModeEffects  *paint_mode,
			     gint                  *width,
			     gint                  *height,
			     guint8               **mask_data)
{
  GParam *return_vals;
  gint nreturn_vals;
  gint num_mask_data;

  return_vals = gimp_run_procedure ("gimp_brushes_get_brush_data",
				    &nreturn_vals,
				    PARAM_STRING, brush_name,
				    PARAM_END);

  brush_name = NULL;
  if (return_vals[0].data.d_status == STATUS_SUCCESS)
    {
      brush_name = g_strdup (return_vals[1].data.d_string);
      *opacity = return_vals[2].data.d_float;
      *spacing = return_vals[3].data.d_int32;
      *paint_mode = return_vals[4].data.d_int32;
      *width = return_vals[5].data.d_int32;
      *height = return_vals[6].data.d_int32;
      num_mask_data = return_vals[7].data.d_int32;
      *mask_data = g_new (guint8, num_mask_data);
      memcpy (*mask_data, return_vals[8].data.d_int8array,
	      num_mask_data * sizeof (guint8));
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return brush_name;
}
