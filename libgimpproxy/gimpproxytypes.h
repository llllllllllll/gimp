
/* Generated data (by gimp-mkproxy) */

#ifndef __GIMP_PROXY_TYPES_H__
#define __GIMP_PROXY_TYPES_H__


/* autogenerated from "./../app/core/core-enums.h" */
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

typedef enum  /*< proxy-resume >*/ /*< skip >*/
{
  GIMP_CHANNEL_OP_ADD,
  GIMP_CHANNEL_OP_SUBTRACT,
  GIMP_CHANNEL_OP_REPLACE,
  GIMP_CHANNEL_OP_INTERSECT
} GimpChannelOps;


/* autogenerated from "./../app/core/core-types.h" */
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

typedef struct _GimpObject          GimpObject; /*< proxy-include >*/
typedef struct _GimpDrawable        GimpDrawable; /*< proxy-resume >*/

typedef struct _GimpChannel         GimpChannel;

typedef struct _GimpLayer           GimpLayer;
typedef struct _GimpLayerMask       GimpLayerMask;

typedef struct _GimpImage           GimpImage;


/*  data objects  */

typedef struct _GimpCoords          GimpCoords; /*< proxy-include >*/

/* autogenerated from "./../app/widgets/widgets-enums.h" */
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

typedef enum  /*< skip, proxy-resume >*/
{
  GIMP_TOOL_CURSOR_NONE,
  GIMP_RECT_SELECT_TOOL_CURSOR,
  GIMP_ELLIPSE_SELECT_TOOL_CURSOR,
  GIMP_FREE_SELECT_TOOL_CURSOR,
  GIMP_FUZZY_SELECT_TOOL_CURSOR,
  GIMP_BEZIER_SELECT_TOOL_CURSOR,
  GIMP_SCISSORS_TOOL_CURSOR,
  GIMP_MOVE_TOOL_CURSOR,
  GIMP_ZOOM_TOOL_CURSOR,
  GIMP_CROP_TOOL_CURSOR,
  GIMP_RESIZE_TOOL_CURSOR,
  GIMP_ROTATE_TOOL_CURSOR,
  GIMP_SHEAR_TOOL_CURSOR,
  GIMP_PERSPECTIVE_TOOL_CURSOR,
  GIMP_FLIP_HORIZONTAL_TOOL_CURSOR,
  GIMP_FLIP_VERTICAL_TOOL_CURSOR,
  GIMP_TEXT_TOOL_CURSOR,
  GIMP_COLOR_PICKER_TOOL_CURSOR,
  GIMP_BUCKET_FILL_TOOL_CURSOR,
  GIMP_BLEND_TOOL_CURSOR,
  GIMP_PENCIL_TOOL_CURSOR,
  GIMP_PAINTBRUSH_TOOL_CURSOR,
  GIMP_AIRBRUSH_TOOL_CURSOR,
  GIMP_INK_TOOL_CURSOR,
  GIMP_CLONE_TOOL_CURSOR,
  GIMP_ERASER_TOOL_CURSOR,
  GIMP_SMUDGE_TOOL_CURSOR,
  GIMP_BLUR_TOOL_CURSOR,
  GIMP_DODGE_TOOL_CURSOR,
  GIMP_BURN_TOOL_CURSOR,
  GIMP_MEASURE_TOOL_CURSOR,
  GIMP_LAST_STOCK_TOOL_CURSOR_ENTRY
} GimpToolCursorType;

typedef enum  /*< skip >*/
{
  GIMP_CURSOR_MODIFIER_NONE,
  GIMP_CURSOR_MODIFIER_PLUS,
  GIMP_CURSOR_MODIFIER_MINUS,
  GIMP_CURSOR_MODIFIER_INTERSECT,
  GIMP_CURSOR_MODIFIER_MOVE,
  GIMP_CURSOR_MODIFIER_RESIZE,
  GIMP_CURSOR_MODIFIER_CONTROL,
  GIMP_CURSOR_MODIFIER_ANCHOR,
  GIMP_CURSOR_MODIFIER_FOREGROUND,
  GIMP_CURSOR_MODIFIER_BACKGROUND,
  GIMP_CURSOR_MODIFIER_PATTERN,
  GIMP_CURSOR_MODIFIER_HAND,
  GIMP_LAST_CURSOR_MODIFIER_ENTRY
} GimpCursorModifier;


/* autogenerated from "./../app/display/display-types.h" */
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
 
typedef struct _GimpDisplay       GimpDisplay; /*< proxy-resume >*/
typedef struct _GimpDisplayShell  GimpDisplayShell;


#endif /* __GIMP_PROXY_TYPES_H__ */


/* Generated data ends here */

