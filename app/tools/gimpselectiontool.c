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

#include "config.h"

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "tools-types.h"

#include "core/gimpimage.h"
#include "core/gimpimage-mask.h"

#include "display/gimpdisplay.h"

#include "gimpdrawtool.h"
#include "gimpselectiontool.h"


static void   gimp_selection_tool_class_init    (GimpSelectionToolClass *klass);
static void   gimp_selection_tool_init          (GimpSelectionTool      *selection_tool);

static void   gimp_selection_tool_cursor_update   (GimpTool          *tool,
                                                   GdkEventMotion    *mevent,
                                                   GimpDisplay       *gdisp);
static void   gimp_selection_tool_oper_update     (GimpTool          *tool,
                                                   GdkEventMotion    *mevent,
                                                   GimpDisplay       *gdisp);
static void   gimp_selection_tool_modifier_key    (GimpTool          *tool,
                                                   GdkEventKey       *kevent,
                                                   GimpDisplay       *gdisp);

static void   gimp_selection_tool_update_op_state (GimpSelectionTool *selection_tool,
                                                   gint               x,
                                                   gint               y,
                                                   gint               state,  
                                                   GimpDisplay       *gdisp);


static GimpDrawToolClass *parent_class = NULL;


GType
gimp_selection_tool_get_type (void)
{
  static GType tool_type = 0;

  if (! tool_type)
    {
      static const GTypeInfo tool_info =
      {
        sizeof (GimpSelectionToolClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_selection_tool_class_init,
	NULL,           /* class_finalize */
	NULL,           /* class_data     */
	sizeof (GimpSelectionTool),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_selection_tool_init,
      };

      tool_type = g_type_register_static (GIMP_TYPE_DRAW_TOOL,
					  "GimpSelectionTool", 
                                          &tool_info, 0);
    }

  return tool_type;
}

static void
gimp_selection_tool_class_init (GimpSelectionToolClass *klass)
{
  GimpToolClass *tool_class;

  tool_class = GIMP_TOOL_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  tool_class->cursor_update = gimp_selection_tool_cursor_update;
  tool_class->oper_update   = gimp_selection_tool_oper_update;
  tool_class->modifier_key  = gimp_selection_tool_modifier_key;
}

static void
gimp_selection_tool_init (GimpSelectionTool *selection_tool)
{
  GimpTool *tool;

  tool = GIMP_TOOL (selection_tool);
  
  selection_tool->current_x = 0;
  selection_tool->current_y = 0;
  selection_tool->op        = SELECTION_REPLACE;

  tool->preserve = FALSE;  /*  Don't preserve on drawable change  */
}

static void
gimp_selection_tool_cursor_update (GimpTool       *tool,
                                   GdkEventMotion *mevent,
                                   GimpDisplay    *gdisp)
{
  GimpSelectionTool *selection_tool;

  selection_tool = GIMP_SELECTION_TOOL (tool);

  switch (selection_tool->op)
    {
    case SELECTION_ADD:
      gdisplay_install_tool_cursor (gdisp,
				    GIMP_MOUSE_CURSOR,
				    tool->tool_cursor,
				    GIMP_CURSOR_MODIFIER_PLUS);
      break;
    case SELECTION_SUB:
      gdisplay_install_tool_cursor (gdisp,
				    GIMP_MOUSE_CURSOR,
				    tool->tool_cursor,
				    GIMP_CURSOR_MODIFIER_MINUS);
      break;
    case SELECTION_INTERSECT: 
      gdisplay_install_tool_cursor (gdisp,
				    GIMP_MOUSE_CURSOR,
				    tool->tool_cursor,
				    GIMP_CURSOR_MODIFIER_INTERSECT);
      break;
    case SELECTION_REPLACE:
      gdisplay_install_tool_cursor (gdisp,
				    GIMP_MOUSE_CURSOR,
				    tool->tool_cursor,
				    GIMP_CURSOR_MODIFIER_NONE);
      break;
    case SELECTION_MOVE_MASK:
      gdisplay_install_tool_cursor (gdisp,
				    GIMP_MOUSE_CURSOR,
				    tool->tool_cursor,
				    GIMP_CURSOR_MODIFIER_MOVE);
      break;
    case SELECTION_MOVE:
      gdisplay_install_tool_cursor (gdisp,
				    GIMP_MOUSE_CURSOR,
				    GIMP_MOVE_TOOL_CURSOR,
				    GIMP_CURSOR_MODIFIER_NONE);
      break;
    case SELECTION_ANCHOR:
      gdisplay_install_tool_cursor (gdisp,
				    GIMP_MOUSE_CURSOR,
				    tool->tool_cursor,
				    GIMP_CURSOR_MODIFIER_ANCHOR);
      break;
    }
}

static void
gimp_selection_tool_oper_update (GimpTool       *tool,
                                 GdkEventMotion *mevent,
                                 GimpDisplay    *gdisp)
{
  GimpSelectionTool *selection_tool;

  selection_tool = GIMP_SELECTION_TOOL (tool);

  selection_tool->current_x = mevent->x;
  selection_tool->current_y = mevent->y;

  gimp_selection_tool_update_op_state (selection_tool,
                                       selection_tool->current_x,
                                       selection_tool->current_y,
                                       mevent->state,
                                       gdisp);
}

static void
gimp_selection_tool_modifier_key (GimpTool    *tool,
                                  GdkEventKey *kevent,
                                  GimpDisplay *gdisp)
{
  GimpSelectionTool *selection_tool;
  gint               state;

  selection_tool = GIMP_SELECTION_TOOL (tool);

  state = kevent->state;

  switch (kevent->keyval)
    {
    case GDK_Alt_L: case GDK_Alt_R:
      if (state & GDK_MOD1_MASK)
	state &= ~GDK_MOD1_MASK;
      else
	state |= GDK_MOD1_MASK;
      break;

    case GDK_Shift_L: case GDK_Shift_R:
      if (state & GDK_SHIFT_MASK)
	state &= ~GDK_SHIFT_MASK;
      else
	state |= GDK_SHIFT_MASK;
      break;

    case GDK_Control_L: case GDK_Control_R:
      if (state & GDK_CONTROL_MASK)
	state &= ~GDK_CONTROL_MASK;
      else
	state |= GDK_CONTROL_MASK;
      break;
    }

  gimp_selection_tool_update_op_state (selection_tool,
                                       selection_tool->current_x,
                                       selection_tool->current_y,
                                       state, gdisp);
}

static void
gimp_selection_tool_update_op_state (GimpSelectionTool *selection_tool,
                                     gint               x,
                                     gint               y,
                                     gint               state,  
                                     GimpDisplay       *gdisp)
{
  GimpLayer *layer;
  GimpLayer *floating_sel;
  gint       tx, ty;

  if (GIMP_TOOL (selection_tool)->state == ACTIVE)
    return;

  gdisplay_untransform_coords (gdisp, x, y, &tx, &ty, FALSE, FALSE);

  layer        = gimp_image_pick_correlate_layer (gdisp->gimage, tx, ty);
  floating_sel = gimp_image_floating_sel (gdisp->gimage);

  if (state & GDK_MOD1_MASK &&
      !gimage_mask_is_empty (gdisp->gimage))
    {
      selection_tool->op = SELECTION_MOVE_MASK; /* move just the selection mask */
    }
  else if (!(state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK)) &&
	   layer &&
	   (layer == floating_sel ||
	    (gdisplay_mask_value (gdisp, x, y) &&
	     floating_sel == NULL)))
    {
      selection_tool->op = SELECTION_MOVE;      /* move the selection */
    }
  else if ((state & GDK_SHIFT_MASK) &&
	   !(state & GDK_CONTROL_MASK))
    {
      selection_tool->op = SELECTION_ADD;       /* add to the selection */
    }
  else if ((state & GDK_CONTROL_MASK) &&
	   !(state & GDK_SHIFT_MASK))
    {
      selection_tool->op = SELECTION_SUB;       /* subtract from the selection */
    }
  else if ((state & GDK_CONTROL_MASK) &&
	   (state & GDK_SHIFT_MASK))
    {
      selection_tool->op = SELECTION_INTERSECT; /* intersect with selection */
    }
  else if (floating_sel)
    {
      selection_tool->op = SELECTION_ANCHOR;    /* anchor the selection */
    }
  else
    {
      selection_tool->op = SELECTION_REPLACE;   /* replace the selection */
    }
}
