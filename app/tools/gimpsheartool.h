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

#ifndef __GIMP_SHEAR_TOOL_H__
#define __GIMP_SHEAR_TOOL_H__


#include "gimptransformtool.h"


#define GIMP_TYPE_SHEAR_TOOL            (gimp_shear_tool_get_type ())
#define GIMP_SHEAR_TOOL(obj)            (GTK_CHECK_CAST ((obj), GIMP_TYPE_SHEAR_TOOL, GimpShearTool))
#define GIMP_IS_SHEAR_TOOL(obj)         (GTK_CHECK_TYPE ((obj), GIMP_TYPE_SHEAR_TOOL))
#define GIMP_SHEAR_TOOL_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GIMP_TYPE_SHEAR_TOOL, GimpShearToolClass))
#define GIMP_IS_SHEAR_TOOL_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_SHEAR_TOOL))


typedef struct _GimpShearTool      GimpShearTool;
typedef struct _GimpShearToolClass GimpShearToolClass;

struct _GimpShearTool
{
  GimpTransformTool  parent_instance;
};

struct _GimpShearToolClass
{
  GimpTransformToolClass  parent_class;
};


GtkType       gimp_shear_tool_get_type (void);

void          gimp_shear_tool_register (void);


TileManager * gimp_shear_tool_shear  (GimpImage      *gimage,
				      GimpDrawable   *drawable,
				      GDisplay       *gdisp,
				      TileManager    *float_tiles,
				      gboolean        interpolation,
				      GimpMatrix3     matrix);


#endif  /*  __GIMP_SHEAR_TOOL_H__  */
