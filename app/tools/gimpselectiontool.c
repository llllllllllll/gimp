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
#include "display/gimpdisplayshell.h"

#include "gimpdrawtool.h"
#include "gimpselectiontool.h"


static void   gimp_selection_tool_class_init    (GimpSelectionToolClass *klass);
static void   gimp_selection_tool_init          (GimpSelectionTool      *selection_tool);

static void   gimp_selection_tool_oper_update     (GimpTool          *tool,
                                                   GimpCoords        *coords,
                                                   GdkModifierType    state,
                                                   GimpDisplay       *gdisp);
static void   gimp_selection_tool_cursor_update   (GimpTool          *tool,
                                                   GimpCoords        *coords,
                                                   GdkModifierType    state,
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

  tool_class->oper_update   = gimp_selection_tool_oper_update;
  tool_class->cursor_update = gimp_selection_tool_cursor_update;
}

static void
gimp_selection_tool_init (GimpSelectionTool *selection_tool)
{
  GimpTool *tool;

  tool = GIMP_TOOL (selection_tool);
  
  selection_tool->op = SELECTION_REPLACE;
}

static void
gimp_selection_tool_oper_update (GimpTool        *tool,
                                 GimpCoords      *coords,
                                 GdkModifierType  state,
                                 GimpDisplay     *gdisp)
{
  GimpSelectionTool *selection_tool;
  GimpLayer         *layer;
  GimpLayer         *floating_sel;

  selection_tool = GIMP_SELECTION_TOOL (tool);

  if (tool->state == ACTIVE)
    return;

  layer        = gimp_image_pick_correlate_layer (gdisp->gimage,
                                                  coords->x, coords->y);
  floating_sel = gimp_image_floating_sel (gdisp->gimage);

  if ((state & GDK_MOD1_MASK) && ! gimage_mask_is_empty (gdisp->gimage))
    {
      selection_tool->op = SELECTION_MOVE_MASK; /* move the selection mask */
    }
  else if (! (state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK)) &&
           layer &&
	   (layer == floating_sel ||
	    (gimage_mask_value (gdisp->gimage, coords->x, coords->y) &&
	     floating_sel == NULL)))
    {
      selection_tool->op = SELECTION_MOVE;      /* move the selection */
    }
  else if ((state & GDK_CONTROL_MASK) && (state & GDK_SHIFT_MASK))
    {
      selection_tool->op = SELECTION_INTERSECT; /* intersect with selection */
    }
  else if (state & GDK_SHIFT_MASK)
    {
      selection_tool->op = SELECTION_ADD;       /* add to the selection */
    }
  else if (state & GDK_CONTROL_MASK)
    {
      selection_tool->op = SELECTION_SUB;       /* subtract from the selection */
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

static void
gimp_selection_tool_cursor_update (GimpTool        *tool,
                                   GimpCoords      *coords,
                                   GdkModifierType  state,
                                   GimpDisplay     *gdisp)
{
  GimpSelectionTool *selection_tool;
  GimpDisplayShell  *shell;

  selection_tool = GIMP_SELECTION_TOOL (tool);

  shell = GIMP_DISPLAY_SHELL (gdisp->shell);

  switch (selection_tool->op)
    {
    case SELECTION_ADD:
      gimp_display_shell_install_tool_cursor (shell,
                                              GIMP_MOUSE_CURSOR,
                                              tool->tool_cursor,
                                              GIMP_CURSOR_MODIFIER_PLUS);
      break;
    case SELECTION_SUB:
      gimp_display_shell_install_tool_cursor (shell,
                                              GIMP_MOUSE_CURSOR,
                                              tool->tool_cursor,
                                              GIMP_CURSOR_MODIFIER_MINUS);
      break;
    case SELECTION_INTERSECT: 
      gimp_display_shell_install_tool_cursor (shell,
                                              GIMP_MOUSE_CURSOR,
                                              tool->tool_cursor,
                                              GIMP_CURSOR_MODIFIER_INTERSECT);
      break;
    case SELECTION_REPLACE:
      gimp_display_shell_install_tool_cursor (shell,
                                              GIMP_MOUSE_CURSOR,
                                              tool->tool_cursor,
                                              GIMP_CURSOR_MODIFIER_NONE);
      break;
    case SELECTION_MOVE_MASK:
      gimp_display_shell_install_tool_cursor (shell,
                                              GIMP_MOUSE_CURSOR,
                                              tool->tool_cursor,
                                              GIMP_CURSOR_MODIFIER_MOVE);
      break;
    case SELECTION_MOVE:
      gimp_display_shell_install_tool_cursor (shell,
                                              GIMP_MOUSE_CURSOR,
                                              GIMP_MOVE_TOOL_CURSOR,
                                              GIMP_CURSOR_MODIFIER_NONE);
      break;
    case SELECTION_ANCHOR:
      gimp_display_shell_install_tool_cursor (shell,
                                              GIMP_MOUSE_CURSOR,
                                              tool->tool_cursor,
                                              GIMP_CURSOR_MODIFIER_ANCHOR);
      break;
    }
}
