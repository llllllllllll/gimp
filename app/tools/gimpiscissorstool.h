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

#ifndef __GIMP_ISCISSORS_TOOL_H__
#define __GIMP_ISCISSORS_TOOL_H__


#include "gimpselectiontool.h"


/*  The possible states...  */
typedef enum
{
  NO_ACTION,
  SEED_PLACEMENT,
  SEED_ADJUSTMENT,
  WAITING
} IscissorsState;

/*  The possible drawing states...  */
typedef enum
{
  DRAW_NOTHING      = 0x0,
  DRAW_CURRENT_SEED = 0x1,
  DRAW_CURVE        = 0x2,
  DRAW_ACTIVE_CURVE = 0x4,
  DRAW_LIVEWIRE     = 0x8
} IscissorsDraw;

/*  For oper_update & cursor_update  */
typedef enum
{
  ISCISSORS_OP_NONE,
  ISCISSORS_OP_SELECT,
  ISCISSORS_OP_MOVE_POINT,
  ISCISSORS_OP_ADD_POINT,
  ISCISSORS_OP_IMPOSSIBLE,
} IscissorsOps;

typedef struct _ICurve ICurve;


#define GIMP_TYPE_ISCISSORS_TOOL            (gimp_iscissors_tool_get_type ())
#define GIMP_ISCISSORS_TOOL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_ISCISSORS_TOOL, GimpIscissorsTool))
#define GIMP_ISCISSORS_TOOL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_ISCISSORS_TOOL, GimpIscissorsToolClass))
#define GIMP_IS_ISCISSORS_TOOL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_ISCISSORS_TOOL))
#define GIMP_IS_ISCISSORS_TOOL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_ISCISSORS_TOOL))
#define GIMP_ISCISSORS_TOOL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_ISCISSORS_TOOL, GimpIscissorsToolClass))


typedef struct _GimpIscissorsTool      GimpIscissorsTool;
typedef struct _GimpIscissorsToolClass GimpIscissorsToolClass;

struct _GimpIscissorsTool
{
  GimpSelectionTool  parent_instance;

  IscissorsOps    op;

  gint            x, y;         /*  upper left hand coordinate            */
  gint            ix, iy;       /*  initial coordinates                   */
  gint            nx, ny;       /*  new coordinates                       */

  TempBuf        *dp_buf;       /*  dynamic programming buffer            */

  ICurve         *livewire;     /*  livewire boundary curve               */

  ICurve         *curve1;       /*  1st curve connected to current point  */
  ICurve         *curve2;       /*  2nd curve connected to current point  */

  GSList         *curves;       /*  the list of curves                    */

  gboolean        first_point;  /*  is this the first point?              */
  gboolean        connected;    /*  is the region closed?                 */

  IscissorsState  state;        /*  state of iscissors                    */
  IscissorsDraw   draw;         /*  items to draw on a draw request       */

  /* XXX might be useful */
  GimpChannel    *mask;         /*  selection mask                        */
  TileManager    *gradient_map; /*  lazily filled gradient map            */
};

struct _GimpIscissorsToolClass
{
  GimpSelectionToolClass parent_class;
};


void    gimp_iscissors_tool_register (GimpToolRegisterCallback  callback,
                                      Gimp                     *gimp);

GType   gimp_iscissors_tool_get_type (void) G_GNUC_CONST;


#endif  /*  __GIMP_ISCISSORS_TOOL_H__  */
