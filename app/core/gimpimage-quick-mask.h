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

#ifndef __GIMP_IMAGE_QMASK_H__
#define __GIMP_IMAGE_QMASK_H__


void       gimp_image_set_qmask_state (GimpImage       *gimage,
                                       gboolean         qmask_state);
gboolean   gimp_image_get_qmask_state (const GimpImage *gimage);

void       gimp_image_qmask_invert    (GimpImage       *gimage);


#endif /* __GIMP_IMAGE_QMASK_H__ */
