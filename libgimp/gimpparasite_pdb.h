/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimpparasite_pdb.h
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

#ifndef __GIMP_PARASITE_PDB_H__
#define __GIMP_PARASITE_PDB_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* For information look into the C source or the html documentation */


GimpParasite * gimp_parasite_find                (const gchar        *name);
void           gimp_parasite_attach              (const GimpParasite *parasite);
void           gimp_parasite_detach              (const gchar        *name);

GimpParasite * gimp_drawable_parasite_find       (gint32              drawable_ID,
						  const gchar        *name);
void           gimp_drawable_parasite_attach     (gint32              drawable_ID,
						  const GimpParasite *parasite);
void           gimp_drawable_parasite_detach     (gint32              drawable_ID,
						  const gchar        *name);
GimpParasite * gimp_image_parasite_find          (gint32              image_ID,
						  const gchar        *name);
void           gimp_image_parasite_attach        (gint32              image_ID,
						  const GimpParasite *parasite);
void           gimp_image_parasite_detach        (gint32              image_ID,
						  const gchar        *name);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GIMP_PARASITE_PDB_H__ */
