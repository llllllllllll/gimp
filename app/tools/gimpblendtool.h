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

#ifndef  __GIMP_BLEND_TOOL_H__
#define  __GIMP_BLEND_TOOL_H__


#include "gimpdrawtool.h"


typedef enum
{
  LINEAR,
  BILINEAR,
  RADIAL,
  SQUARE,
  CONICAL_SYMMETRIC,
  CONICAL_ASYMMETRIC,
  SHAPEBURST_ANGULAR,
  SHAPEBURST_SPHERICAL,
  SHAPEBURST_DIMPLED,
  SPIRAL_CLOCKWISE,
  SPIRAL_ANTICLOCKWISE,
  GRADIENT_TYPE_LAST  /*< skip >*/
} GradientType;

typedef enum  /*< chop=_MODE >*/
{
  FG_BG_RGB_MODE,
  FG_BG_HSV_MODE,
  FG_TRANS_MODE,
  CUSTOM_MODE,
  BLEND_MODE_LAST /*< skip >*/
} BlendMode;

typedef enum
{
  REPEAT_NONE,
  REPEAT_SAWTOOTH,
  REPEAT_TRIANGULAR,
  REPEAT_LAST /*< skip >*/
} RepeatMode;


#define GIMP_TYPE_BLEND_TOOL            (gimp_blend_tool_get_type ())
#define GIMP_BLEND_TOOL(obj)            (GTK_CHECK_CAST ((obj), GIMP_TYPE_BLEND_TOOL, GimpBlendTool))
#define GIMP_IS_BLEND_TOOL(obj)         (GTK_CHECK_TYPE ((obj), GIMP_TYPE_BLEND_TOOL))
#define GIMP_BLEND_TOOL_CLASS(klass)    (GTK_CHECK_CLASS_CAST ((klass), GIMP_TYPE_BLEND_TOOL, GimpBlendToolClass))
#define GIMP_IS_BLEND_TOOL_CLASS(klass) (GTK_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_BLEND_TOOL))


typedef struct _GimpBlendTool      GimpBlendTool;
typedef struct _GimpBlendToolClass GimpBlendToolClass;

struct _GimpBlendTool
{
  GimpDrawTool  parent_instance;

  gint          startx;      /*  starting x coord     */
  gint          starty;      /*  starting y coord     */

  gint          endx;        /*  ending x coord       */
  gint          endy;        /*  ending y coord       */
  guint         context_id;  /*  for the statusbar    */
};

struct _GimpBlendToolClass
{
  GimpDrawToolClass  parent_class;
};


GtkType   gimp_blend_tool_get_type (void);

void      gimp_blend_tool_register (void);

void      blend                    (GimpImage        *gimage,
                                    GimpDrawable     *drawable,
                                    BlendMode         blend_mode,
                                    gint              paint_mode,
                                    GradientType      gradient_type,
                                    gdouble           opacity,
                                    gdouble           offset,
                                    RepeatMode        repeat,
                                    gint              supersample,
                                    gint              max_depth,
                                    gdouble           threshold,
                                    gdouble           startx,
                                    gdouble           starty,
                                    gdouble           endx,
                                    gdouble           endy,
                                    GimpProgressFunc  progress_callback,
                                    gpointer          progress_data);


#endif  /*  __GIMP_BLEND_TOOL_H__  */
