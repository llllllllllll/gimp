/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
 *
 * gimppalette_pdb.h
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

#ifndef __GIMP_PALETTE_PDB_H__
#define __GIMP_PALETTE_PDB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* For information look into the C source or the html documentation */


gboolean gimp_palette_get_foreground     (GimpRGB *foreground);
gboolean gimp_palette_get_background     (GimpRGB *background);
gboolean gimp_palette_set_foreground     (GimpRGB *foreground);
gboolean gimp_palette_set_background     (GimpRGB *background);
gboolean gimp_palette_set_default_colors (void);
gboolean gimp_palette_swap_colors        (void);
gboolean gimp_palette_refresh            (void);
gchar**  gimp_palette_list               (gint    *num_palettes);
gchar*   gimp_palette_get_palette        (gint    *num_colors);
gboolean gimp_palette_set_palette        (gchar   *name);
gboolean gimp_palette_get_entry          (gint     entry_num,
					  GimpRGB *color);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GIMP_PALETTE_PDB_H__ */
