/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimpbrush_pdb.h
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

/* NOTE: This file is auto-generated by pdbgen.pl */

#ifndef __GIMP_BRUSH_PDB_H__
#define __GIMP_BRUSH_PDB_H__

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


gchar*                  gimp_brush_new              (const gchar              *name);
gchar*                  gimp_brush_duplicate        (const gchar              *name);
gboolean                gimp_brush_is_generated     (const gchar              *name);
gchar*                  gimp_brush_rename           (const gchar              *name,
                                                     const gchar              *new_name);
gboolean                gimp_brush_delete           (const gchar              *name);
gboolean                gimp_brush_is_editable      (const gchar              *name);
gboolean                gimp_brush_get_info         (const gchar              *name,
                                                     gint                     *width,
                                                     gint                     *height,
                                                     gint                     *mask_bpp,
                                                     gint                     *color_bpp);
gboolean                gimp_brush_get_pixels       (const gchar              *name,
                                                     gint                     *width,
                                                     gint                     *height,
                                                     gint                     *mask_bpp,
                                                     gint                     *num_mask_bytes,
                                                     guint8                  **mask_bytes,
                                                     gint                     *color_bpp,
                                                     gint                     *num_color_bytes,
                                                     guint8                  **color_bytes);
gboolean                gimp_brush_get_spacing      (const gchar              *name,
                                                     gint                     *spacing);
gboolean                gimp_brush_set_spacing      (const gchar              *name,
                                                     gint                      spacing);
GimpBrushGeneratedShape gimp_brush_get_shape        (const gchar              *name);
gdouble                 gimp_brush_get_radius       (const gchar              *name);
gint                    gimp_brush_get_spikes       (const gchar              *name);
gdouble                 gimp_brush_get_hardness     (const gchar              *name);
gdouble                 gimp_brush_get_aspect_ratio (const gchar              *name);
gdouble                 gimp_brush_get_angle        (const gchar              *name);
GimpBrushGeneratedShape gimp_brush_set_shape        (const gchar              *name,
                                                     GimpBrushGeneratedShape   shape_in);
gdouble                 gimp_brush_set_radius       (const gchar              *name,
                                                     gdouble                   radius_in);
gint                    gimp_brush_set_spikes       (const gchar              *name,
                                                     gint                      spikes_in);
gdouble                 gimp_brush_set_hardness     (const gchar              *name,
                                                     gdouble                   hardness_in);
gdouble                 gimp_brush_set_aspect_ratio (const gchar              *name,
                                                     gdouble                   aspect_ratio_in);
gdouble                 gimp_brush_set_angle        (const gchar              *name,
                                                     gdouble                   angle_in);


G_END_DECLS

#endif /* __GIMP_BRUSH_PDB_H__ */
