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

#include "libgimpmath/gimpmath.h"
#include "libgimpconfig/gimpconfig.h"
#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "tools-types.h"

#include "core/gimpchannel.h"
#include "core/gimpchannel-select.h"
#include "core/gimpimage.h"
#include "core/gimpimage-crop.h"
#include "core/gimptoolinfo.h"
#include "core/gimp-utils.h"

#include "widgets/gimphelp-ids.h"
#include "widgets/gimpwidgets-utils.h"

#include "display/gimpcanvas.h"
#include "display/gimpdisplay.h"
#include "display/gimpdisplayshell.h"
#include "display/gimpdisplayshell-transform.h"

#include "gimpselectiontool.h"
#include "gimpselectionoptions.h"
#include "gimprectangletool.h"
#include "gimprectangleoptions.h"
#include "gimptoolcontrol.h"

#include "gimp-intl.h"


/*  speed of key movement  */
#define ARROW_VELOCITY 25

static void     gimp_rectangle_tool_class_init     (GimpRectangleToolClass *klass);
static void     gimp_rectangle_tool_init           (GimpRectangleTool      *rectangle_tool);

static void     gimp_rectangle_tool_control        (GimpTool          *tool,
                                                    GimpToolAction     action,
                                                    GimpDisplay       *gdisp);
static void     gimp_rectangle_tool_button_press   (GimpTool          *tool,
                                                    GimpCoords        *coords,
                                                    guint32            time,
                                                    GdkModifierType    state,
                                                    GimpDisplay       *gdisp);
static void     gimp_rectangle_tool_button_release (GimpTool          *tool,
                                                    GimpCoords        *coords,
                                                    guint32            time,
                                                    GdkModifierType    state,
                                                    GimpDisplay       *gdisp);
static void     gimp_rectangle_tool_motion         (GimpTool          *tool,
                                                    GimpCoords        *coords,
                                                    guint32            time,
                                                    GdkModifierType    state,
                                                    GimpDisplay       *gdisp);
static gboolean gimp_rectangle_tool_key_press      (GimpTool          *tool,
                                                    GdkEventKey       *kevent,
                                                    GimpDisplay       *gdisp);
static void     gimp_rectangle_tool_modifier_key   (GimpTool          *tool,
                                                    GdkModifierType    key,
                                                    gboolean           press,
                                                    GdkModifierType    state,
                                                    GimpDisplay       *gdisp);
static void     gimp_rectangle_tool_oper_update    (GimpTool          *tool,
                                                    GimpCoords        *coords,
                                                    GdkModifierType    state,
                                                    GimpDisplay       *gdisp);
static void     gimp_rectangle_tool_cursor_update  (GimpTool          *tool,
                                                    GimpCoords        *coords,
                                                    GdkModifierType    state,
                                                    GimpDisplay       *gdisp);

static void     gimp_rectangle_tool_draw           (GimpDrawTool      *draw_tool);

/*  Rectangle helper functions  */
static void     rectangle_recalc                   (GimpRectangleTool *rectangle);
static void     rectangle_tool_start               (GimpRectangleTool *rectangle);

/*  Rectangle dialog functions  */
static void     rectangle_info_update              (GimpRectangleTool *rectangle);
static void     rectangle_response                 (GtkWidget         *widget,
                                                    gint               response_id,
                                                    GimpRectangleTool *rectangle);

static void     rectangle_selection_callback       (GtkWidget         *widget,
                                                    GimpRectangleTool *rectangle);
static void     rectangle_automatic_callback       (GtkWidget         *widget,
                                                    GimpRectangleTool *rectangle);

static void     rectangle_origin_changed           (GtkWidget         *widget,
                                                    GimpRectangleTool *rectangle);
static void     rectangle_size_changed             (GtkWidget         *widget,
                                                    GimpRectangleTool *rectangle);
static void     rectangle_aspect_changed           (GtkWidget         *widget,
                                                    GimpRectangleTool *rectangle);

static void  rectangle_tool_execute                (GimpRectangleTool *rect_tool,
                                                    gint               x,
                                                    gint               y,
                                                    gint               w,
                                                    gint               h);

static void   gimp_rectangle_tool_real_execute      (GimpRectangleTool *rect_tool,
                                                     gint               x,
                                                     gint               y,
                                                     gint               w,
                                                     gint               h);

static GimpDrawToolClass *parent_class = NULL;


/*  public functions  */

GType
gimp_rectangle_tool_get_type (void)
{
  static GType tool_type = 0;

  if (! tool_type)
    {
      static const GTypeInfo tool_info =
      {
        sizeof (GimpRectangleToolClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gimp_rectangle_tool_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data     */
        sizeof (GimpRectangleTool),
        0,              /* n_preallocs    */
        (GInstanceInitFunc) gimp_rectangle_tool_init,
      };

      tool_type = g_type_register_static (GIMP_TYPE_SELECTION_TOOL,
                                          "GimpRectangleTool",
                                          &tool_info, 0);
    }

  return tool_type;
}

static void
gimp_rectangle_tool_class_init (GimpRectangleToolClass *klass)
{
  GimpToolClass     *tool_class      = GIMP_TOOL_CLASS (klass);
  GimpDrawToolClass *draw_tool_class = GIMP_DRAW_TOOL_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  tool_class->control        = gimp_rectangle_tool_control;
  tool_class->button_press   = gimp_rectangle_tool_button_press;
  tool_class->button_release = gimp_rectangle_tool_button_release;
  tool_class->motion         = gimp_rectangle_tool_motion;
  tool_class->key_press      = gimp_rectangle_tool_key_press;
  tool_class->modifier_key   = gimp_rectangle_tool_modifier_key;
  tool_class->oper_update    = gimp_rectangle_tool_oper_update;
  tool_class->cursor_update  = gimp_rectangle_tool_cursor_update;

  draw_tool_class->draw      = gimp_rectangle_tool_draw;
  klass->execute             = gimp_rectangle_tool_real_execute;
}

static void
gimp_rectangle_tool_init (GimpRectangleTool *rectangle_tool)
{
  GimpTool *tool = GIMP_TOOL (rectangle_tool);

  gimp_tool_control_set_preserve    (tool->control, FALSE);
  gimp_tool_control_set_dirty_mask  (tool->control, GIMP_DIRTY_IMAGE_SIZE);
}

static void
gimp_rectangle_tool_control (GimpTool       *tool,
                             GimpToolAction  action,
                             GimpDisplay    *gdisp)
{
  GimpRectangleTool *rectangle = GIMP_RECTANGLE_TOOL (tool);

  switch (action)
    {
    case PAUSE:
      break;

    case RESUME:
      rectangle_recalc (rectangle);
      break;

    case HALT:
      rectangle_response (NULL, GTK_RESPONSE_CANCEL, rectangle);
      break;

    default:
      break;
    }

  GIMP_TOOL_CLASS (parent_class)->control (tool, action, gdisp);
}

static void
gimp_rectangle_tool_button_press (GimpTool        *tool,
                                  GimpCoords      *coords,
                                  guint32          time,
                                  GdkModifierType  state,
                                  GimpDisplay     *gdisp)
{
  GimpRectangleTool     *rectangle  = GIMP_RECTANGLE_TOOL (tool);
  GimpDrawTool          *draw_tool  = GIMP_DRAW_TOOL (tool);
  GimpRectangleOptions  *options;

  options = GIMP_RECTANGLE_OPTIONS (tool->tool_info->tool_options);

  if (gdisp != tool->gdisp)
    {
      if (gimp_draw_tool_is_active (draw_tool))
        gimp_draw_tool_stop (draw_tool);

      rectangle->function = RECT_CREATING;
      gimp_tool_control_set_snap_offsets (tool->control, 0, 0, 0, 0);

      tool->gdisp = gdisp;

      rectangle->x2 = rectangle->x1 = ROUND (coords->x);
      rectangle->y2 = rectangle->y1 = ROUND (coords->y);

      rectangle_tool_start (rectangle);
    }

  rectangle->lastx = rectangle->startx = ROUND (coords->x);
  rectangle->lasty = rectangle->starty = ROUND (coords->y);

  gimp_tool_control_activate (tool->control);
}

static void
gimp_rectangle_tool_button_release (GimpTool        *tool,
                                    GimpCoords      *coords,
                                    guint32          time,
                                    GdkModifierType  state,
                                    GimpDisplay     *gdisp)
{
  GimpRectangleTool *rectangle = GIMP_RECTANGLE_TOOL (tool);
  GimpRectangleOptions  *options;

  options = GIMP_RECTANGLE_OPTIONS (tool->tool_info->tool_options);

  gimp_tool_control_halt (tool->control);
  gimp_tool_pop_status (tool);

  if (! (state & GDK_BUTTON3_MASK))
    {
      if (rectangle->function == RECT_CREATING)
        {
          if ( (rectangle->lastx == rectangle->startx) &&
               (rectangle->lasty == rectangle->starty))
            {
              rectangle_response (NULL, GTK_RESPONSE_CANCEL, rectangle);
            }
        }
      else if (rectangle->function == RECT_EXECUTING)
        {
          rectangle_response (NULL, GIMP_RECTANGLE_MODE_EXECUTE, rectangle);
        }
      rectangle_info_update (rectangle);
    }
}

static void
gimp_rectangle_tool_motion (GimpTool        *tool,
                            GimpCoords      *coords,
                            guint32          time,
                            GdkModifierType  state,
                            GimpDisplay     *gdisp)
{
  GimpRectangleTool *rectangle = GIMP_RECTANGLE_TOOL (tool);
  gint               x1, y1, x2, y2;
  gint               curx, cury;
  gint               inc_x, inc_y;
  gint               min_x, min_y, max_x, max_y;

  /*  This is the only case when the motion events should be ignored--
      we're just waiting for the button release event to execute  */
  if (rectangle->function == RECT_EXECUTING)
    return;

  curx = ROUND (coords->x);
  cury = ROUND (coords->y);

  x1 = rectangle->startx;
  y1 = rectangle->starty;
  x2 = curx;
  y2 = cury;

  inc_x = (x2 - x1);
  inc_y = (y2 - y1);

  /*  If there have been no changes... return  */
  if (rectangle->lastx == x2 && rectangle->lasty == y2)
    return;

  gimp_draw_tool_pause (GIMP_DRAW_TOOL (tool));

  min_x = min_y = 0;
  max_x = gdisp->gimage->width;
  max_y = gdisp->gimage->height;

  switch (rectangle->function)
    {
    case RECT_CREATING:
      rectangle->change_aspect_ratio = TRUE;
      break;

    case RECT_RESIZING_LEFT:
      x1 = rectangle->x1 + inc_x;
      y1 = rectangle->y1 + inc_y;
      x2 = MAX (x1, rectangle->x2);
      y2 = MAX (y1, rectangle->y2);
      rectangle->startx = curx;
      rectangle->starty = cury;
      break;

    case RECT_RESIZING_RIGHT:
      x2 = rectangle->x2 + inc_x;
      y2 = rectangle->y2 + inc_y;
      x1 = MIN (rectangle->x1, x2);
      y1 = MIN (rectangle->y1, y2);
      rectangle->startx = curx;
      rectangle->starty = cury;
      break;

    case RECT_MOVING:
      x1 = rectangle->x1 + inc_x;
      x2 = rectangle->x2 + inc_x;
      y1 = rectangle->y1 + inc_y;
      y2 = rectangle->y2 + inc_y;
      rectangle->startx = curx;
      rectangle->starty = cury;
      break;
    }

  /*  make sure that the coords are in bounds  */
  rectangle->x1 = MIN (x1, x2);
  rectangle->y1 = MIN (y1, y2);
  rectangle->x2 = MAX (x1, x2);
  rectangle->y2 = MAX (y1, y2);

  rectangle->lastx = curx;
  rectangle->lasty = cury;

  /*  recalculate the coordinates for rectangle_draw based on the new values  */
  rectangle_recalc (rectangle);

  switch (rectangle->function)
    {
    case RECT_RESIZING_LEFT:
      gimp_tool_control_set_snap_offsets (tool->control,
                                          rectangle->x1 - coords->x,
                                          rectangle->y1 - coords->y,
                                          0, 0);
      break;

    case RECT_RESIZING_RIGHT:
      gimp_tool_control_set_snap_offsets (tool->control,
                                          rectangle->x2 - coords->x,
                                          rectangle->y2 - coords->y,
                                          0, 0);
      break;

    case RECT_MOVING:
      gimp_tool_control_set_snap_offsets (tool->control,
                                          rectangle->x1 - coords->x,
                                          rectangle->y1 - coords->y,
                                          rectangle->x2 - rectangle->x1,
                                          rectangle->y2 - rectangle->y1);
      break;

    default:
      break;
    }

  if (rectangle->function == RECT_CREATING      ||
      rectangle->function == RECT_RESIZING_LEFT ||
      rectangle->function == RECT_RESIZING_RIGHT)
    {
      gimp_tool_pop_status (tool);

      gimp_tool_push_status_coords (tool,
                                    _("Rectangle: "),
                                    rectangle->x2 - rectangle->x1,
                                    " x ",
                                    rectangle->y2 - rectangle->y1);
    }

  gimp_draw_tool_resume (GIMP_DRAW_TOOL (tool));
}

static gboolean
gimp_rectangle_tool_key_press (GimpTool    *tool,
                               GdkEventKey *kevent,
                               GimpDisplay *gdisp)
{
  GimpRectangleTool *rectangle = GIMP_RECTANGLE_TOOL (tool);
  gint               inc_x, inc_y;
  gint               min_x, min_y;
  gint               max_x, max_y;

  if (gdisp != tool->gdisp)
    return FALSE;

  inc_x = inc_y = 0;

  switch (kevent->keyval)
    {
    case GDK_Up:
      inc_y = -1;
      break;
    case GDK_Left:
      inc_x = -1;
      break;
    case GDK_Right:
      inc_x = 1;
      break;
    case GDK_Down:
      inc_y = 1;
      break;

    case GDK_KP_Enter:
    case GDK_Return:
      rectangle_response (NULL, GIMP_RECTANGLE_MODE_EXECUTE, rectangle);
      return TRUE;

    case GDK_Escape:
      rectangle_response (NULL, GTK_RESPONSE_CANCEL, rectangle);
      return TRUE;

    default:
      return FALSE;
    }

  /*  If the shift key is down, move by an accelerated increment  */
  if (kevent->state & GDK_SHIFT_MASK)
    {
      inc_y *= ARROW_VELOCITY;
      inc_x *= ARROW_VELOCITY;
    }

  gimp_draw_tool_pause (GIMP_DRAW_TOOL (tool));

  min_x = min_y = 0;
  max_x = gdisp->gimage->width;
  max_y = gdisp->gimage->height;

  rectangle->x1 += inc_x;
  rectangle->x2 += inc_x;
  rectangle->y1 += inc_y;
  rectangle->y2 += inc_y;

  rectangle_recalc (rectangle);

  gimp_draw_tool_resume (GIMP_DRAW_TOOL (tool));

  return TRUE;
}

static void
gimp_rectangle_tool_modifier_key (GimpTool        *tool,
                                  GdkModifierType  key,
                                  gboolean         press,
                                  GdkModifierType  state,
                                  GimpDisplay     *gdisp)
{
}

static void
gimp_rectangle_tool_oper_update (GimpTool        *tool,
                                 GimpCoords      *coords,
                                 GdkModifierType  state,
                                 GimpDisplay     *gdisp)
{
  GimpRectangleTool *rectangle = GIMP_RECTANGLE_TOOL (tool);
  GimpDrawTool      *draw_tool = GIMP_DRAW_TOOL (tool);

  if (tool->gdisp != gdisp)
    return;

  /*  If the cursor is in either the upper left or lower right boxes,
   *  The new function will be to resize the current rectangle area
   */
  if (gimp_draw_tool_on_handle (draw_tool, gdisp,
                                coords->x, coords->y,
                                GIMP_HANDLE_SQUARE,
                                rectangle->x1, rectangle->y1,
                                rectangle->dcw, rectangle->dch,
                                GTK_ANCHOR_NORTH_WEST,
                                FALSE))
    {
      rectangle->function = RECT_RESIZING_LEFT;

      gimp_tool_control_set_snap_offsets (tool->control,
                                          rectangle->x1 - coords->x,
                                          rectangle->y1 - coords->y,
                                          0, 0);
    }
  else if (gimp_draw_tool_on_handle (draw_tool, gdisp,
                                     coords->x, coords->y,
                                     GIMP_HANDLE_SQUARE,
                                     rectangle->x2, rectangle->y2,
                                     rectangle->dcw, rectangle->dch,
                                     GTK_ANCHOR_SOUTH_EAST,
                                     FALSE))
    {
      rectangle->function = RECT_RESIZING_RIGHT;

      gimp_tool_control_set_snap_offsets (tool->control,
                                          rectangle->x2 - coords->x,
                                          rectangle->y2 - coords->y,
                                          0, 0);
    }
  /*  If the cursor is in either the upper right or lower left boxes,
   *  The new function will be to translate the current rectangle area
   */
  else if  (gimp_draw_tool_on_handle (draw_tool, gdisp,
                                      coords->x, coords->y,
                                      GIMP_HANDLE_SQUARE,
                                      rectangle->x2, rectangle->y1,
                                      rectangle->dcw, rectangle->dch,
                                      GTK_ANCHOR_NORTH_EAST,
                                      FALSE) ||
            gimp_draw_tool_on_handle (draw_tool, gdisp,
                                      coords->x, coords->y,
                                      GIMP_HANDLE_SQUARE,
                                      rectangle->x1, rectangle->y2,
                                      rectangle->dcw, rectangle->dch,
                                      GTK_ANCHOR_SOUTH_WEST,
                                      FALSE))
    {
      rectangle->function = RECT_MOVING;

      gimp_tool_control_set_snap_offsets (tool->control,
                                          rectangle->x1 - coords->x,
                                          rectangle->y1 - coords->y,
                                          rectangle->x2 - rectangle->x1,
                                          rectangle->y2 - rectangle->y1);
    }
  /*  If the pointer is in the rectangular region, execute or resize it!
   */
  else if (coords->x > rectangle->x1 &&
           coords->x < rectangle->x2 &&
           coords->y > rectangle->y1 &&
           coords->y < rectangle->y2)
    {
      rectangle->function = RECT_EXECUTING;
    }
  /*  otherwise, the new function will be creating, since we want
   *  to start a new
   */
  else
    {
      rectangle->function = RECT_CREATING;

      gimp_tool_control_set_snap_offsets (tool->control, 0, 0, 0, 0);
    }
}

static void
gimp_rectangle_tool_cursor_update (GimpTool        *tool,
                                   GimpCoords      *coords,
                                   GdkModifierType  state,
                                   GimpDisplay     *gdisp)
{
  GimpRectangleTool     *rectangle = GIMP_RECTANGLE_TOOL (tool);
  GimpRectangleOptions  *options;
  GimpCursorType         cursor    = GIMP_CURSOR_CROSSHAIR_SMALL;
  GimpCursorModifier     modifier  = GIMP_CURSOR_MODIFIER_NONE;

  options = GIMP_RECTANGLE_OPTIONS (tool->tool_info->tool_options);

  if (tool->gdisp == gdisp)
    {
      switch (rectangle->function)
        {
        case RECT_MOVING:
          modifier = GIMP_CURSOR_MODIFIER_MOVE;
          break;

        case RECT_RESIZING_LEFT:
        case RECT_RESIZING_RIGHT:
          modifier = GIMP_CURSOR_MODIFIER_RESIZE;
          break;

        default:
          break;
        }
    }

  gimp_tool_control_set_cursor (tool->control, cursor);
  gimp_tool_control_set_tool_cursor (tool->control,
                                     GIMP_TOOL_CURSOR_CROP);
  gimp_tool_control_set_cursor_modifier (tool->control, modifier);

  GIMP_TOOL_CLASS (parent_class)->cursor_update (tool, coords, state, gdisp);
}

static void
gimp_rectangle_tool_draw (GimpDrawTool *draw)
{
  GimpRectangleTool *rectangle = GIMP_RECTANGLE_TOOL (draw);
  GimpTool          *tool      = GIMP_TOOL (draw);
  GimpDisplayShell  *shell     = GIMP_DISPLAY_SHELL (tool->gdisp->shell);
  GimpCanvas        *canvas    = GIMP_CANVAS (shell->canvas);

  gimp_canvas_draw_line (canvas, GIMP_CANVAS_STYLE_XOR,
                         rectangle->dx1, rectangle->dy1,
                         shell->disp_width, rectangle->dy1);
  gimp_canvas_draw_line (canvas, GIMP_CANVAS_STYLE_XOR,
                         rectangle->dx1, rectangle->dy1,
                         rectangle->dx1, shell->disp_height);
  gimp_canvas_draw_line (canvas, GIMP_CANVAS_STYLE_XOR,
                         rectangle->dx2, rectangle->dy2,
                         0, rectangle->dy2);
  gimp_canvas_draw_line (canvas, GIMP_CANVAS_STYLE_XOR,
                         rectangle->dx2, rectangle->dy2,
                         rectangle->dx2, 0);

  gimp_draw_tool_draw_handle (draw,
                              GIMP_HANDLE_FILLED_SQUARE,
                              rectangle->x1, rectangle->y1,
                              rectangle->dcw, rectangle->dch,
                              GTK_ANCHOR_NORTH_WEST,
                              FALSE);
  gimp_draw_tool_draw_handle (draw,
                              GIMP_HANDLE_FILLED_SQUARE,
                              rectangle->x2, rectangle->y1,
                              rectangle->dcw, rectangle->dch,
                              GTK_ANCHOR_NORTH_EAST,
                              FALSE);
  gimp_draw_tool_draw_handle (draw,
                              GIMP_HANDLE_FILLED_SQUARE,
                              rectangle->x1, rectangle->y2,
                              rectangle->dcw, rectangle->dch,
                              GTK_ANCHOR_SOUTH_WEST,
                              FALSE);
  gimp_draw_tool_draw_handle (draw,
                              GIMP_HANDLE_FILLED_SQUARE,
                              rectangle->x2, rectangle->y2,
                              rectangle->dcw, rectangle->dch,
                              GTK_ANCHOR_SOUTH_EAST,
                              FALSE);
}

static void
rectangle_recalc (GimpRectangleTool *rectangle)
{
  GimpTool              *tool     = GIMP_TOOL (rectangle);
  GimpDisplayShell      *shell    = GIMP_DISPLAY_SHELL (tool->gdisp->shell);
  GimpRectangleOptions  *options;

  options = GIMP_RECTANGLE_OPTIONS (tool->tool_info->tool_options);

  if (! tool->gdisp)
    return;

  if (options->highlight)
    {
      GdkRectangle rect;

      rect.x      = rectangle->x1;
      rect.y      = rectangle->y1;
      rect.width  = rectangle->x2 - rectangle->x1;
      rect.height = rectangle->y2 - rectangle->y1;

      gimp_display_shell_set_highlight (shell, &rect);
    }

  gimp_display_shell_transform_xy (shell,
                                   rectangle->x1, rectangle->y1,
                                   &rectangle->dx1, &rectangle->dy1,
                                   FALSE);
  gimp_display_shell_transform_xy (shell,
                                   rectangle->x2, rectangle->y2,
                                   &rectangle->dx2, &rectangle->dy2,
                                   FALSE);

#define SRW 10
#define SRH 10

  rectangle->dcw = ((rectangle->dx2 - rectangle->dx1) < SRW) ?
    (rectangle->dx2 - rectangle->dx1) : SRW;

  rectangle->dch = ((rectangle->dy2 - rectangle->dy1) < SRH) ?
    (rectangle->dy2 - rectangle->dy1) : SRH;

#undef SRW
#undef SRH

  if ((rectangle->y2 - rectangle->y1) != 0)
    {
      if (rectangle->change_aspect_ratio)
        rectangle->aspect_ratio = ((gdouble) (rectangle->x2 - rectangle->x1) /
                                     (gdouble) (rectangle->y2 - rectangle->y1));
    }
  else
    {
      rectangle->aspect_ratio = 0.0;
    }

  rectangle_info_update (rectangle);
}

static void
rectangle_tool_start (GimpRectangleTool *rectangle)
{
  GimpTool         *tool  = GIMP_TOOL (rectangle);

  rectangle_recalc (rectangle);

  /* initialize the statusbar display */
  gimp_tool_push_status_coords (tool, _("Rectangle: "), 0, " x ", 0);

  gimp_draw_tool_start (GIMP_DRAW_TOOL (tool), tool->gdisp);
}


static void
rectangle_info_update (GimpRectangleTool *rectangle)
{
  rectangle->orig_vals[0] = rectangle->x1;
  rectangle->orig_vals[1] = rectangle->y1;
  rectangle->size_vals[0] = rectangle->x2 - rectangle->x1;
  rectangle->size_vals[1] = rectangle->y2 - rectangle->y1;

}

static void
rectangle_response (GtkWidget         *widget,
                    gint               response_id,
                    GimpRectangleTool *rectangle)
{
  GimpTool *tool = GIMP_TOOL (rectangle);

  switch (response_id)
    {
    case GIMP_RECTANGLE_MODE_EXECUTE:
      gimp_display_shell_set_highlight (GIMP_DISPLAY_SHELL (tool->gdisp->shell),
                                        NULL);

      rectangle_tool_execute (GIMP_RECTANGLE_TOOL (tool),
                              rectangle->x1, rectangle->y1,
                              rectangle->x2 - rectangle->x1,
                              rectangle->y2 - rectangle->y1);

      gimp_image_flush (tool->gdisp->gimage);
      break;

    default:
      break;
    }

  if (tool->gdisp)
    gimp_display_shell_set_highlight (GIMP_DISPLAY_SHELL (tool->gdisp->shell),
                                      NULL);

  if (gimp_draw_tool_is_active (GIMP_DRAW_TOOL (rectangle)))
    gimp_draw_tool_stop (GIMP_DRAW_TOOL (rectangle));

  if (gimp_tool_control_is_active (GIMP_TOOL (rectangle)->control))
    gimp_tool_control_halt (GIMP_TOOL (rectangle)->control);

  tool->gdisp    = NULL;
  tool->drawable = NULL;
}

static void
rectangle_selection_callback (GtkWidget         *widget,
                              GimpRectangleTool *rectangle)
{
  GimpRectangleOptions *options;
  GimpDisplay          *gdisp;

  options = GIMP_RECTANGLE_OPTIONS (GIMP_TOOL (rectangle)->tool_info->tool_options);
  gdisp   = GIMP_TOOL (rectangle)->gdisp;

  gimp_draw_tool_pause (GIMP_DRAW_TOOL (rectangle));

  if (! gimp_channel_bounds (gimp_image_get_mask (gdisp->gimage),
                             &rectangle->x1, &rectangle->y1,
                             &rectangle->x2, &rectangle->y2))
    {
      rectangle->x1 = rectangle->y1 = 0;
      rectangle->x2 = gdisp->gimage->width;
      rectangle->y2 = gdisp->gimage->height;
    }

  /* force change of aspect ratio */
  if ((rectangle->y2 - rectangle->y1) != 0)
    rectangle->aspect_ratio = ((gdouble) (rectangle->x2 - rectangle->x1) /
                          (gdouble) (rectangle->y2 - rectangle->y1));

  rectangle_recalc (rectangle);

  gimp_draw_tool_resume (GIMP_DRAW_TOOL (rectangle));
}

static void
rectangle_automatic_callback (GtkWidget         *widget,
                              GimpRectangleTool *rectangle)
{
  GimpRectangleOptions *options;
  GimpDisplay          *gdisp;
  gint                  offset_x, offset_y;
  gint                  width, height;
  gint                  x1, y1, x2, y2;
  gint                  shrunk_x1;
  gint                  shrunk_y1;
  gint                  shrunk_x2;
  gint                  shrunk_y2;

  options = GIMP_RECTANGLE_OPTIONS (GIMP_TOOL (rectangle)->tool_info->tool_options);
  gdisp   = GIMP_TOOL (rectangle)->gdisp;

  width    = gdisp->gimage->width;
  height   = gdisp->gimage->height;
  offset_x = 0;
  offset_y = 0;

  x1 = rectangle->x1 - offset_x  > 0      ? rectangle->x1 - offset_x : 0;
  x2 = rectangle->x2 - offset_x  < width  ? rectangle->x2 - offset_x : width;
  y1 = rectangle->y1 - offset_y  > 0      ? rectangle->y1 - offset_y : 0;
  y2 = rectangle->y2 - offset_y  < height ? rectangle->y2 - offset_y : height;

  if (gimp_image_crop_auto_shrink (gdisp->gimage,
                                   x1, y1, x2, y2,
                                   FALSE,
                                   &shrunk_x1,
                                   &shrunk_y1,
                                   &shrunk_x2,
                                   &shrunk_y2))
    {
      gimp_draw_tool_pause (GIMP_DRAW_TOOL (rectangle));

      rectangle->x1 = offset_x + shrunk_x1;
      rectangle->x2 = offset_x + shrunk_x2;
      rectangle->y1 = offset_y + shrunk_y1;
      rectangle->y2 = offset_y + shrunk_y2;

      rectangle_recalc (rectangle);

      gimp_draw_tool_resume (GIMP_DRAW_TOOL (rectangle));
    }
}

static void
rectangle_origin_changed (GtkWidget         *widget,
                          GimpRectangleTool *rectangle)
{
  gint origin_x;
  gint origin_y;

  origin_x = RINT (gimp_size_entry_get_refval (GIMP_SIZE_ENTRY (widget), 0));
  origin_y = RINT (gimp_size_entry_get_refval (GIMP_SIZE_ENTRY (widget), 1));

  if ((origin_x != rectangle->x1) ||
      (origin_y != rectangle->y1))
    {
      gimp_draw_tool_pause (GIMP_DRAW_TOOL (rectangle));

      rectangle->x2 = rectangle->x2 + (origin_x - rectangle->x1);
      rectangle->x1 = origin_x;
      rectangle->y2 = rectangle->y2 + (origin_y - rectangle->y1);
      rectangle->y1 = origin_y;

      rectangle_recalc (rectangle);

      gimp_draw_tool_resume (GIMP_DRAW_TOOL (rectangle));
    }
}

static void
rectangle_size_changed (GtkWidget         *widget,
                        GimpRectangleTool *rectangle)
{
  gint size_x = gimp_size_entry_get_refval (GIMP_SIZE_ENTRY (widget), 0);
  gint size_y = gimp_size_entry_get_refval (GIMP_SIZE_ENTRY (widget), 1);

  if ((size_x != (rectangle->x2 - rectangle->x1)) ||
      (size_y != (rectangle->y2 - rectangle->y1)))
    {
      gimp_draw_tool_pause (GIMP_DRAW_TOOL (rectangle));

      rectangle->x2 = size_x + rectangle->x1;
      rectangle->y2 = size_y + rectangle->y1;

      rectangle_recalc (rectangle);

      gimp_draw_tool_resume (GIMP_DRAW_TOOL (rectangle));
    }
}

static void
rectangle_aspect_changed (GtkWidget         *widget,
                          GimpRectangleTool *rectangle)
{
  rectangle->aspect_ratio = GTK_ADJUSTMENT (widget)->value;

  gimp_draw_tool_pause (GIMP_DRAW_TOOL (rectangle));

  rectangle->y2 = rectangle->y1 + ((gdouble) (rectangle->x2 - rectangle->x1)
                                   / rectangle->aspect_ratio);

  rectangle->change_aspect_ratio = FALSE;
  rectangle_recalc (rectangle);
  rectangle->change_aspect_ratio = TRUE;

  gimp_draw_tool_resume (GIMP_DRAW_TOOL (rectangle));
}


static void
rectangle_tool_execute (GimpRectangleTool *rect_tool,
                        gint               x,
                        gint               y,
                        gint               w,
                        gint               h)
{
  GimpTool             *tool;
  GimpSelectionOptions *options;

  g_return_if_fail (GIMP_IS_RECTANGLE_TOOL (rect_tool));

  tool    = GIMP_TOOL (rect_tool);
  options = GIMP_SELECTION_OPTIONS (tool->tool_info->tool_options);

  if (options->auto_shrink)
    {
      gint off_x = 0;
      gint off_y = 0;
      gint x2, y2;

      if (! gimp_rectangle_intersect (x, y, w, h,
                                      0, 0,
                                      tool->gdisp->gimage->width,
                                      tool->gdisp->gimage->height,
                                      &x, &y, &w, &h))
        {
          return;
        }

      if (! options->shrink_merged)
        {
          GimpItem *item;
          gint      width, height;

          item = GIMP_ITEM (gimp_image_active_drawable (tool->gdisp->gimage));

          gimp_item_offsets (item, &off_x, &off_y);
          width  = gimp_item_width  (item);
          height = gimp_item_height (item);

          if (! gimp_rectangle_intersect (x, y, w, h,
                                          off_x, off_y, width, height,
                                          &x, &y, &w, &h))
            {
              return;
            }

          x -= off_x;
          y -= off_y;
        }

      if (gimp_image_crop_auto_shrink (tool->gdisp->gimage,
                                       x, y,
                                       x + w, y + h,
                                       ! options->shrink_merged,
                                       &x, &y,
                                       &x2, &y2))
        {
          w = x2 - x;
          h = y2 - y;
        }

      x += off_x;
      y += off_y;
    }

  GIMP_RECTANGLE_TOOL_GET_CLASS (rect_tool)->execute (rect_tool, x, y, w, h);
}

static void
gimp_rectangle_tool_real_execute (GimpRectangleTool *rect_tool,
                                  gint               x,
                                  gint               y,
                                  gint               w,
                                  gint               h)
{
}

