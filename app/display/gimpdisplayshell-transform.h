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

#ifndef __GIMP_DISPLAY_SHELL_TRANSFORM_H__
#define __GIMP_DISPLAY_SHELL_TRANSFORM_H__


void  gimp_display_shell_transform_coords   (GimpDisplayShell *shell,
					     GimpCoords       *image_coords,
					     GimpCoords       *display_coords);
void  gimp_display_shell_untransform_coords (GimpDisplayShell *shell,
					     GimpCoords       *display_coords,
					     GimpCoords       *image_coords);

void  gimp_display_shell_transform_xy       (GimpDisplayShell *shell,
					     gint              x,
					     gint              y,
					     gint             *nx,
					     gint             *ny,
					     gboolean          use_offsets);
void  gimp_display_shell_untransform_xy     (GimpDisplayShell *shell,
					     gint              x,
					     gint              y,
					     gint             *nx,
					     gint             *ny,
					     gboolean          round,
					     gboolean          use_offsets);

void  gimp_display_shell_transform_xy_f     (GimpDisplayShell *shell,
					     gdouble           x,
					     gdouble           y,
					     gdouble          *nx,
					     gdouble          *ny,
					     gboolean          use_offsets);
void  gimp_display_shell_untransform_xy_f   (GimpDisplayShell *shell,
					     gdouble           x,
					     gdouble           y,
					     gdouble          *nx,
					     gdouble          *ny,
					     gboolean          use_offsets);


#endif /* __GIMP_DISPLAY_SHELL_TRANSFORM_H__ */
