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
#ifndef  __COLOR_PICKER_H__
#define  __COLOR_PICKER_H__

#include "gimpimageF.h"
#include "gimpdrawableF.h"
#include "tools.h"

int         pick_color (GimpImage *, GimpDrawable *, int , int,
			gboolean, gboolean, double, int);

Tool *      tools_new_color_picker   (void);
void        tools_free_color_picker  (Tool *);

extern int col_value[5];

#endif  /*  __COLOR_PICKER_H__  */
