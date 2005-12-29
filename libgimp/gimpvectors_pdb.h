/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimpvectors_pdb.h
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

/* NOTE: This file is autogenerated by pdbgen.pl */

#ifndef __GIMP_VECTORS_PDB_H__
#define __GIMP_VECTORS_PDB_H__

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


gint*    gimp_vectors_get_strokes        (gint32        vectors_ID,
					  gint         *num_strokes);
gint32   gimp_vectors_get_image          (gint32        vectors_ID);
gboolean gimp_vectors_get_linked         (gint32        vectors_ID);
gboolean gimp_vectors_set_linked         (gint32        vectors_ID,
					  gboolean      linked);
gboolean gimp_vectors_get_visible        (gint32        vectors_ID);
gboolean gimp_vectors_set_visible        (gint32        vectors_ID,
					  gboolean      visible);
gchar*   gimp_vectors_get_name           (gint32        vectors_ID);
gboolean gimp_vectors_set_name           (gint32        vectors_ID,
					  const gchar  *name);
gint     gimp_vectors_get_tattoo         (gint32        vectors_ID);
gboolean gimp_vectors_set_tattoo         (gint32        vectors_ID,
					  gint          tattoo);
gdouble  gimp_vectors_stroke_get_length  (gint32        vectors_ID,
					  gint          stroke_id,
					  gdouble       prescision);
gboolean gimp_vectors_stroke_remove      (gint32        vectors_ID,
					  gint          stroke_id);
gboolean gimp_vectors_stroke_translate   (gint32        vectors_ID,
					  gint          stroke_id,
					  gint          off_x,
					  gint          off_y);
gboolean gimp_vectors_stroke_scale       (gint32        vectors_ID,
					  gint          stroke_id,
					  gdouble       scale_x,
					  gdouble       scale_y);
gboolean gimp_vectors_stroke_interpolate (gint32        vectors_ID,
					  gint          stroke_id,
					  gdouble       prescision,
					  gint         *num_coords,
					  gdouble     **coords);


G_END_DECLS

#endif /* __GIMP_VECTORS_PDB_H__ */
