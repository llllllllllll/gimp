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

#include <stdlib.h>
#include <stdio.h>     /* temporary for debugging */
#include <string.h>

#include "appenv.h"
#include "brush_scale.h"
#include "devices.h"
#include "drawable.h"
#include "errors.h"
#include "gdisplay.h"
#include "gimage_mask.h"
#include "gimpbrushpipe.h"
#include "gimprc.h"
#include "gradient.h"  /* for grad_get_color_at() */
#include "paint_funcs.h"
#include "paint_core.h"
#include "selection.h"
#include "tools.h"
#include "undo.h"
#include "cursorutil.h"

#include "libgimp/gimpintl.h"
#include "libgimp/gimpmath.h"

#include "tile.h"			/* ick. */

/*  target size  */
#define  TARGET_HEIGHT  15
#define  TARGET_WIDTH   15

#define  EPSILON  0.00001

#define  STATUSBAR_SIZE 128

/*  global variables--for use in the various paint tools  */
PaintCore  non_gui_paint_core;

/*  local function prototypes  */
static void      paint_core_calculate_brush_size (MaskBuf *, double, int *, int *);
static MaskBuf * paint_core_subsample_mask  (MaskBuf *, double, double);
static MaskBuf * paint_core_pressurize_mask (MaskBuf *, double, double, double);
static MaskBuf * paint_core_solidify_mask   (MaskBuf *);
static MaskBuf * paint_core_scale_mask      (MaskBuf *, gdouble);
static MaskBuf * paint_core_scale_pixmap    (MaskBuf *, gdouble);
static MaskBuf * paint_core_get_brush_mask  (PaintCore *, BrushApplicationMode, gdouble);
static void      paint_core_paste           (PaintCore *, MaskBuf *,
					     GimpDrawable *, int, int,
					     LayerModeEffects,
					     PaintApplicationMode);
static void      paint_core_replace         (PaintCore *, MaskBuf *,
					     GimpDrawable *, int, int,
					     PaintApplicationMode);
static void      brush_to_canvas_tiles      (PaintCore *, MaskBuf *, int);
static void      canvas_tiles_to_canvas_buf (PaintCore *);
static void      brush_to_canvas_buf        (PaintCore *, MaskBuf *, int);
static void      set_undo_tiles             (GimpDrawable *, int, int, int, int);
static void      set_canvas_tiles           (int, int, int, int);
static void      paint_core_invalidate_cache  (GimpBrush *brush, gpointer *blah);

/***********************************************************************/


/*  undo blocks variables  */
static TileManager *  undo_tiles = NULL;
static TileManager *  canvas_tiles = NULL;


/***********************************************************************/


/*  paint buffers variables  */
static TempBuf *  orig_buf = NULL;
static TempBuf *  canvas_buf = NULL;


/*  brush buffers  */
static MaskBuf *  pressure_brush;
static MaskBuf *  solid_brush;
static MaskBuf *  scale_brush = NULL;
static MaskBuf *  scale_pixmap = NULL;
static MaskBuf *  kernel_brushes[5][5];


/*  paint buffers utility functions  */
static void        free_paint_buffers     (void);

/*  brush pipe utility functions  */
static void        paint_line_pixmap_mask (GImage *, GimpDrawable *,
					   TempBuf *, TempBuf *, guchar *,
					   int, int, int, int, int);

/***********************************************************************/


#define KERNEL_WIDTH   3
#define KERNEL_HEIGHT  3

/*  Brush pixel subsampling kernels  */
static const int subsample[5][5][9] = {
	{
		{ 64, 64, 0, 64, 64, 0, 0, 0, 0, },
		{ 32, 96, 0, 32, 96, 0, 0, 0, 0, },
		{ 0, 128, 0, 0, 128, 0, 0, 0, 0, },
		{ 0, 96, 32, 0, 96, 32, 0, 0, 0, },
		{ 0, 64, 64, 0, 64, 64, 0, 0, 0, },
	},
	{
		{ 32, 32, 0, 96, 96, 0, 0, 0, 0, },
		{ 16, 48, 0, 48, 144, 0, 0, 0, 0, },
		{ 0, 64, 0, 0, 192, 0, 0, 0, 0, },
		{ 0, 48, 16, 0, 144, 48, 0, 0, 0, },
		{ 0, 32, 32, 0, 96, 96, 0, 0, 0, },
	},
	{
		{ 0, 0, 0, 128, 128, 0, 0, 0, 0, },
		{ 0, 0, 0, 64, 192, 0, 0, 0, 0, },
		{ 0, 0, 0, 0, 256, 0, 0, 0, 0, },
		{ 0, 0, 0, 0, 192, 64, 0, 0, 0, },
		{ 0, 0, 0, 0, 128, 128, 0, 0, 0, },
	},
	{
		{ 0, 0, 0, 96, 96, 0, 32, 32, 0, },
		{ 0, 0, 0, 48, 144, 0, 16, 48, 0, },
		{ 0, 0, 0, 0, 192, 0, 0, 64, 0, },
		{ 0, 0, 0, 0, 144, 48, 0, 48, 16, },
		{ 0, 0, 0, 0, 96, 96, 0, 32, 32, },
	},
	{
		{ 0, 0, 0, 64, 64, 0, 64, 64, 0, },
		{ 0, 0, 0, 32, 96, 0, 32, 96, 0, },
		{ 0, 0, 0, 0, 128, 0, 0, 128, 0, },
		{ 0, 0, 0, 0, 96, 32, 0, 96, 32, },
		{ 0, 0, 0, 0, 64, 64, 0, 64, 64, },
	},
};

static void
paint_core_sample_color (GimpDrawable *drawable, 
			 int           x, 
			 int           y, 
			 int           state)
{
  guchar *color;

  if ((color = gimp_drawable_get_color_at (drawable, x, y)))
    {
      if ((state & GDK_CONTROL_MASK))
	gimp_context_set_foreground (gimp_context_get_user (),
				     color[RED_PIX],
				     color[GREEN_PIX],
				     color[BLUE_PIX]);
      else
	gimp_context_set_background (gimp_context_get_user (),
				     color[RED_PIX],
				     color[GREEN_PIX],
				     color[BLUE_PIX]);

      g_free (color);
    }
}


void
paint_core_button_press (Tool           *tool, 
			 GdkEventButton *bevent, 
			 gpointer        gdisp_ptr)
{
  PaintCore * paint_core;
  GDisplay * gdisp;
  gboolean draw_line;
  double x, y;
  GimpDrawable *drawable;

  gdisp = (GDisplay *) gdisp_ptr;
  paint_core = (PaintCore *) tool->private;

  gdisplay_untransform_coords_f (gdisp, (double) bevent->x, (double) bevent->y, &x, &y, TRUE);
  drawable = gimage_active_drawable (gdisp->gimage);

  if (! paint_core_init (paint_core, drawable, x, y))
    return;

  draw_line = FALSE;

  paint_core->curpressure = bevent->pressure;
  paint_core->curxtilt = bevent->xtilt;
  paint_core->curytilt = bevent->ytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
  paint_core->curwheel = bevent->wheel;
#endif /* GTK_HAVE_SIX_VALUATORS */ 
  paint_core->state = bevent->state;
  
  if (gdisp_ptr != tool->gdisp_ptr)
    {
      /* initialize the statusbar display */
      paint_core->context_id = 
	gtk_statusbar_get_context_id (GTK_STATUSBAR (gdisp->statusbar), "paint");
    }
 /*  if this is a new image, reinit the core vals  */
  if ((gdisp_ptr != tool->gdisp_ptr) || ! (bevent->state & GDK_SHIFT_MASK))
    {
      /*  initialize some values  */
      paint_core->startx = paint_core->lastx = paint_core->curx = x;
      paint_core->starty = paint_core->lasty = paint_core->cury = y;
      paint_core->startpressure = paint_core->lastpressure = paint_core->curpressure;
      paint_core->startytilt = paint_core->lastytilt = paint_core->curytilt;
      paint_core->startxtilt = paint_core->lastxtilt = paint_core->curxtilt;
#ifdef GTK_HAVE_SIX_VALUATORS
      paint_core->startwheel = paint_core->lastwheel = paint_core->curwheel;
#endif /* GTK_HAVE_SIX_VALUATORS */
    }

  /*  If shift is down and this is not the first paint
   *  stroke, then draw a line from the last coords to the pointer
   */
  else if (bevent->state & GDK_SHIFT_MASK)
    {
      draw_line = TRUE;
      paint_core->startx = paint_core->lastx;
      paint_core->starty = paint_core->lasty;
      paint_core->startpressure = paint_core->lastpressure;
      paint_core->startxtilt = paint_core->lastxtilt;
      paint_core->startytilt = paint_core->lastytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
      paint_core->startwheel = paint_core->lastwheel;
#endif /* GTK_HAVE_SIX_VALUATORS */

      /* restrict to horizontal/vertical lines, if modifiers are pressed */
      if (bevent->state & GDK_MOD1_MASK)
	{
	  if (bevent->state & GDK_CONTROL_MASK)
	    {
	      double dx, dy;
	
	      dx = paint_core->curx - paint_core->lastx;
	      dy = paint_core->cury - paint_core->lasty;

	      paint_core->curx = paint_core->lastx + 
		(dx > 0 ? MAX (fabs (dx), fabs (dy)) : - MAX (fabs (dx), fabs (dy)));
	      paint_core->cury = paint_core->lasty + 
		(dy > 0  ? MAX (fabs (dx), fabs (dy)) : - MAX (fabs (dx), fabs (dy)));
	    }
	  else
	    paint_core->curx = paint_core->lastx;
	}
      else if (bevent->state & GDK_CONTROL_MASK)
	paint_core->cury = paint_core->lasty;
    }

  tool->state = ACTIVE;
  tool->gdisp_ptr = gdisp_ptr;
  tool->paused_count = 0;

  /*  pause the current selection and grab the pointer  */
  gdisplays_selection_visibility (gdisp->gimage, SelectionPause);

  /* add motion memory if perfectmouse is set */
  if (perfectmouse != 0)
    gdk_pointer_grab (gdisp->canvas->window, FALSE,
		      GDK_BUTTON1_MOTION_MASK | GDK_BUTTON_RELEASE_MASK,
		      NULL, NULL, bevent->time);
  else
    gdk_pointer_grab (gdisp->canvas->window, FALSE,
		      GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON1_MOTION_MASK |
		      GDK_BUTTON_RELEASE_MASK,
		      NULL, NULL, bevent->time);
  
  /*  Let the specific painting function initialize itself  */
  (* paint_core->paint_func) (paint_core, drawable, INIT_PAINT);

  if (paint_core->pick_colors
      && !(bevent->state & GDK_SHIFT_MASK) 
      && (bevent->state & (GDK_CONTROL_MASK | GDK_MOD1_MASK)))
    {
      paint_core_sample_color (drawable, x, y, bevent->state);
      paint_core->pick_state = TRUE;
      return;
    }
  else
    paint_core->pick_state = FALSE;

  /*  Paint to the image  */
  if (draw_line)
    {
      draw_core_pause (paint_core->core, tool);
      paint_core_interpolate (paint_core, drawable);
      paint_core->lastx = paint_core->curx;
      paint_core->lasty = paint_core->cury;
      paint_core->lastpressure = paint_core->curpressure;
      paint_core->lastxtilt = paint_core->curxtilt;
      paint_core->lastytilt = paint_core->curytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
      paint_core->lastwheel = paint_core->curwheel;
#endif /* GTK_HAVE_SIX_VALUATORS */
    }
  else
    {
      /* If we current point == last point, check if the brush
       * wants to be painted in that case. (Direction dependent
       * pixmap brush pipes don't, as they don't know which
       * pixmap to select.)
       */
      if (paint_core->lastx != paint_core->curx
	  || paint_core->lasty != paint_core->cury
	  || (* GIMP_BRUSH_CLASS (GTK_OBJECT (paint_core->brush)
				  ->klass)->want_null_motion) (paint_core))
	{
	  if (paint_core->flags & TOOL_CAN_HANDLE_CHANGING_BRUSH)
	    paint_core->brush =
	      (* GIMP_BRUSH_CLASS (GTK_OBJECT (paint_core->brush)
				   ->klass)->select_brush) (paint_core);
	  (* paint_core->paint_func) (paint_core, drawable, MOTION_PAINT);
	}
    }

  gdisplay_flush_now (gdisp);
}

void
paint_core_button_release (Tool           *tool, 
			   GdkEventButton *bevent, 
			   gpointer        gdisp_ptr)
{
  GDisplay * gdisp;
  GImage * gimage;
  PaintCore * paint_core;

  gdisp = (GDisplay *) gdisp_ptr;
  gimage = gdisp->gimage;
  paint_core = (PaintCore *) tool->private;

  /*  resume the current selection and ungrab the pointer  */
  gdisplays_selection_visibility (gdisp->gimage, SelectionResume);

  gdk_pointer_ungrab (bevent->time);
  gdk_flush ();

  /*  Let the specific painting function finish up  */
  (* paint_core->paint_func) (paint_core, gimage_active_drawable (gdisp->gimage), FINISH_PAINT);

  /*  Set tool state to inactive -- no longer painting */
  draw_core_stop (paint_core->core, tool);
  tool->state = INACTIVE;

  paint_core->pick_state = FALSE;

  paint_core_finish (paint_core, gimage_active_drawable (gdisp->gimage), tool->ID);
  gdisplays_flush ();
}

void
paint_core_motion (Tool           *tool, 
		   GdkEventMotion *mevent, 
		   gpointer        gdisp_ptr)
{
  GDisplay * gdisp;
  PaintCore * paint_core;

  gdisp = (GDisplay *) gdisp_ptr;
  paint_core = (PaintCore *) tool->private;

  gdisplay_untransform_coords_f (gdisp, (double) mevent->x, (double) mevent->y,
				 &paint_core->curx, &paint_core->cury, TRUE);

  if (paint_core->pick_state)
  {
    paint_core_sample_color (gimage_active_drawable (gdisp->gimage),
			     paint_core->curx, paint_core->cury, mevent->state);
    return;
  }

  paint_core->curpressure = mevent->pressure;
  paint_core->curxtilt = mevent->xtilt;
  paint_core->curytilt = mevent->ytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
  paint_core->curwheel = mevent->wheel;
#endif /* GTK_HAVE_SIX_VALUATORS */
  paint_core->state = mevent->state;

  paint_core_interpolate (paint_core, gimage_active_drawable (gdisp->gimage));

  gdisplay_flush_now (gdisp);

  paint_core->lastx = paint_core->curx;
  paint_core->lasty = paint_core->cury;
  paint_core->lastpressure = paint_core->curpressure;
  paint_core->lastxtilt = paint_core->curxtilt;
  paint_core->lastytilt = paint_core->curytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
  paint_core->lastwheel = paint_core->curwheel;
#endif /* GTK_HAVE_SIX_VALUATORS */
}

void
paint_core_cursor_update (Tool           *tool, 
			  GdkEventMotion *mevent, 
			  gpointer        gdisp_ptr)
{
  GDisplay *gdisp;
  Layer *layer;
  PaintCore * paint_core;
  GdkCursorType ctype = GDK_TOP_LEFT_ARROW;
  int x, y;
  gchar status_str[STATUSBAR_SIZE];

  gdisp = (GDisplay *) gdisp_ptr;
  paint_core = (PaintCore *) tool->private;

  /*  undraw the current tool  */
  draw_core_pause (paint_core->core, tool);
  if (paint_core->context_id)
    gtk_statusbar_pop (GTK_STATUSBAR (gdisp->statusbar), paint_core->context_id);

  if ((layer = gimage_get_active_layer (gdisp->gimage)))
    {
      /* If shift is down and this is not the first paint stroke, draw a line */
      if (gdisp_ptr == tool->gdisp_ptr && (mevent->state & GDK_SHIFT_MASK))
	{ 
	  gdouble dx, dy, d;

	  ctype = GDK_PENCIL;	  
	  /*  Get the current coordinates */ 
	  gdisplay_untransform_coords_f (gdisp, 
					 (double) mevent->x, 
					 (double) mevent->y,
					 &paint_core->curx, 
					 &paint_core->cury, TRUE);

	  dx = paint_core->curx - paint_core->lastx;
	  dy = paint_core->cury - paint_core->lasty;
		  
	  /* restrict to horizontal/vertical lines, if modifiers are pressed */
	  if (mevent->state & GDK_MOD1_MASK)
	    {
	      if (mevent->state & GDK_CONTROL_MASK)
		{
		  dx = paint_core->curx - paint_core->lastx;
		  dy = paint_core->cury - paint_core->lasty;

		  paint_core->curx = paint_core->lastx + 
		    (dx > 0 ? MAX (fabs (dx), fabs (dy)) : - MAX (fabs (dx), fabs (dy)));
		  paint_core->cury = paint_core->lasty + 
		    (dy > 0  ? MAX (fabs (dx), fabs (dy)) : - MAX (fabs (dx), fabs (dy)));
		}
	      else
		paint_core->curx = paint_core->lastx;
	    }
	  else if (mevent->state & GDK_CONTROL_MASK)
	    paint_core->cury = paint_core->lasty;

	  /*  show distance in statusbar  */
	  dx = paint_core->curx - paint_core->lastx;
	  dy = paint_core->cury - paint_core->lasty;

	  if (gdisp->dot_for_dot)
	    {
	      d = sqrt (SQR (dx) + SQR (dy));
	      g_snprintf (status_str, STATUSBAR_SIZE, "%.1f %s", d, _("pixels"));
	    }
	  else
	    {
	      gchar *format_str = g_strdup_printf ("%%.%df %s",
						   gimp_unit_get_digits (gdisp->gimage->unit),
						   gimp_unit_get_symbol (gdisp->gimage->unit));
	      d = gimp_unit_get_factor (gdisp->gimage->unit) * 
		sqrt (SQR (dx / gdisp->gimage->xresolution) + SQR (dy / gdisp->gimage->yresolution));
	      
	      g_snprintf (status_str, STATUSBAR_SIZE, format_str, d);
	      g_free (format_str);
	    }
	  gtk_statusbar_push (GTK_STATUSBAR (gdisp->statusbar), paint_core->context_id,
			      status_str);

	  if (paint_core->core->gc == NULL)
	    draw_core_start (paint_core->core, gdisp->canvas->window, tool);
	  else
	    {
	      /* is this a bad hack ? */
	      paint_core->core->paused_count = 0;
	      draw_core_resume (paint_core->core, tool);
	    }
	} 
      /* If Ctrl or Mod1 is pressed, pick colors */ 
      else if (paint_core->pick_colors
	  && !(mevent->state & GDK_SHIFT_MASK) 
	  && (mevent->state & (GDK_CONTROL_MASK | GDK_MOD1_MASK)))
        {
	  ctype = GIMP_COLOR_PICKER_CURSOR;
	}
      /* Normal operation -- no modifier pressed or first stroke */
      else 
	{
	  int off_x, off_y;

	  drawable_offsets (GIMP_DRAWABLE(layer), &off_x, &off_y);
	  gdisplay_untransform_coords (gdisp, (double) mevent->x, (double) mevent->y,
				       &x, &y, TRUE, FALSE);
 
	  if (x >= off_x && y >= off_y &&
	       x < (off_x + drawable_width (GIMP_DRAWABLE(layer))) &&
	       y < (off_y + drawable_height (GIMP_DRAWABLE(layer))))
	    {
	      /*  One more test--is there a selected region?
	       *  if so, is cursor inside?
	       */
	      if (gimage_mask_is_empty (gdisp->gimage))
		ctype = GDK_PENCIL;
	      else if (gimage_mask_value (gdisp->gimage, x, y))
		ctype = GDK_PENCIL;
	    }
	}
      gdisplay_install_tool_cursor (gdisp, ctype);
    }
}

void
paint_core_control (Tool       *tool, 
		    ToolAction  action, 
		    gpointer    gdisp_ptr)
{
  PaintCore * paint_core;
  GDisplay *gdisp;
  GimpDrawable *drawable;

  gdisp = (GDisplay *) gdisp_ptr;
  paint_core = (PaintCore *) tool->private;
  drawable = gimage_active_drawable (gdisp->gimage);

  switch (action)
    {
    case PAUSE:
      break;

    case RESUME:
      break;

    case HALT:
      (* paint_core->paint_func) (paint_core, drawable, FINISH_PAINT);
      draw_core_stop (paint_core->core, tool);
      paint_core_cleanup ();
      break;

    default:
      break;
    }
}

void
paint_core_draw (Tool *tool)
{
  GDisplay *gdisp;
  PaintCore * paint_core;
  int tx1, ty1, tx2, ty2;

  paint_core = (PaintCore *) tool->private;

  /* if shift was never used, paint_core->core->gc is NULL 
     and we don't care about a redraw                       */
  if (paint_core->core->gc != NULL)
    {
      gdisp = (GDisplay *) tool->gdisp_ptr;

      gdisplay_transform_coords (gdisp, paint_core->lastx, paint_core->lasty,
				 &tx1, &ty1, 1);
      gdisplay_transform_coords (gdisp, paint_core->curx, paint_core->cury,
				 &tx2, &ty2, 1);

      /*  Draw start target  */
      gdk_draw_line (gdisp->canvas->window, paint_core->core->gc,
		     tx1 - (TARGET_WIDTH >> 1), ty1,
		     tx1 + (TARGET_WIDTH >> 1), ty1);
      gdk_draw_line (gdisp->canvas->window, paint_core->core->gc,
		     tx1, ty1 - (TARGET_HEIGHT >> 1),
		     tx1, ty1 + (TARGET_HEIGHT >> 1));
      
      /*  Draw end target  */
      gdk_draw_line (gdisp->canvas->window, paint_core->core->gc,
		     tx2 - (TARGET_WIDTH >> 1), ty2,
		     tx2 + (TARGET_WIDTH >> 1), ty2);
      gdk_draw_line (gdisp->canvas->window, paint_core->core->gc,
		     tx2, ty2 - (TARGET_HEIGHT >> 1),
		     tx2, ty2 + (TARGET_HEIGHT >> 1));
      
      /*  Draw the line between the start and end coords  */
      gdk_draw_line (gdisp->canvas->window, paint_core->core->gc,
		     tx1, ty1, tx2, ty2);
    }
  return;
}	      

Tool *
paint_core_new (ToolType type)
{
  Tool * tool;
  PaintCore * private;

  tool = tools_new_tool (type);
  private = g_new (PaintCore, 1);

  private->core = draw_core_new (paint_core_draw);

  private->pick_colors = FALSE;
  private->flags = 0;
  private->context_id = 0;

  tool->private = (void *) private;

  tool->button_press_func   = paint_core_button_press;
  tool->button_release_func = paint_core_button_release;
  tool->motion_func         = paint_core_motion;
  tool->cursor_update_func  = paint_core_cursor_update;
  tool->control_func        = paint_core_control;

  return tool;
}

void
paint_core_free (Tool *tool)
{
  PaintCore * paint_core;

  paint_core = (PaintCore *) tool->private;

  /*  Make sure the selection core is not visible  */
  if (tool->state == ACTIVE && paint_core->core)
    draw_core_stop (paint_core->core, tool);

  /*  Free the selection core  */
  if (paint_core->core)
    draw_core_free (paint_core->core);

  /*  Cleanup memory  */
  paint_core_cleanup ();

  /*  Free the paint core  */
  g_free (paint_core);
}

int
paint_core_init (PaintCore    *paint_core, 
		 GimpDrawable *drawable, 
		 double        x, 
		 double        y)
{
  static GimpBrush *brush = NULL;
  
  paint_core->curx = x;
  paint_core->cury = y;

  /* Set up some defaults for non-gui use */
  if (paint_core == &non_gui_paint_core)
    {
      paint_core->startpressure = paint_core->lastpressure = paint_core->curpressure = 0.5;
      paint_core->startxtilt = paint_core->lastxtilt = paint_core->curxtilt = 0;
      paint_core->startytilt = paint_core->lastytilt = paint_core->curytilt = 0;
#ifdef GTK_HAVE_SIX_VALUATORS
      paint_core->startwheel = paint_core->lastwheel = paint_core->curwheel = 0.5;
#endif /* GTK_HAVE_SIX_VALUATORS */
    }

  /*  Each buffer is the same size as the maximum bounds of the active brush... */
  if (brush && brush != gimp_context_get_brush (NULL))
    {
      gtk_signal_disconnect_by_func (GTK_OBJECT (brush),
				     GTK_SIGNAL_FUNC (paint_core_invalidate_cache),
				     NULL);
      gtk_object_unref (GTK_OBJECT (brush));
    }
  if (!(brush = gimp_context_get_brush (NULL)))
    {
      g_message (_("No brushes available for use with this tool."));
      return FALSE;
    }
  gtk_object_ref (GTK_OBJECT (brush));
  gtk_signal_connect (GTK_OBJECT (brush), "dirty",
		      GTK_SIGNAL_FUNC (paint_core_invalidate_cache),
		      NULL);

  paint_core->spacing = (double) gimp_brush_get_spacing (brush) / 100.0;

  paint_core->brush = brush;

  /*  free the block structures  */
  if (undo_tiles)
    tile_manager_destroy (undo_tiles);
  if (canvas_tiles)
    tile_manager_destroy (canvas_tiles);

  /*  Allocate the undo structure  */
  undo_tiles = tile_manager_new (drawable_width (drawable),
				 drawable_height (drawable),
				 drawable_bytes (drawable));

  /*  Allocate the canvas blocks structure  */
  canvas_tiles = tile_manager_new (drawable_width (drawable),
				   drawable_height (drawable), 1);

  /*  Get the initial undo extents  */
  paint_core->x1 = paint_core->x2 = paint_core->curx;
  paint_core->y1 = paint_core->y2 = paint_core->cury;
  paint_core->distance = 0.0;
  paint_core->pixel_dist = 0.0;

  return TRUE;
}

void
paint_core_get_color_from_gradient (PaintCore *paint_core, 
				    double     gradient_length, 
				    double    *r, 
				    double    *g, 
				    double    *b, 
				    double    *a, 
				    int        mode)
{
  double y;
  double distance;      /* distance in current brush stroke */
  double temp_opacity;  /* so i can blank out stuff */

  distance = paint_core->pixel_dist;
  y = ((double) distance / gradient_length);
  temp_opacity = 1.0;

  /* for the once modes, set alpha to 0.0 after the first chunk */
  if (y >= 1.0 && (mode == ONCE_FORWARD || mode == ONCE_BACKWARDS))
    temp_opacity = 0.0; 

  if ( ((int)y & 1 && mode != LOOP_SAWTOOTH) || mode == ONCE_BACKWARDS )
    y = 1.0 - (y - (int)y);
  else
    y = y - (int)y;

  gradient_get_color_at (gimp_context_get_gradient (NULL), y, r, g, b, a);
  *a = (temp_opacity * *a);
}
  

void
paint_core_interpolate (PaintCore    *paint_core, 
			GimpDrawable *drawable)
{
  double n;
  vector2d delta;
#ifdef GTK_HAVE_SIX_VALUATORS
  double dpressure, dxtilt, dytilt, dwheel;
#else /* !GTK_HAVE_SIX_VALUATORS */
  double dpressure, dxtilt, dytilt;
#endif /* GTK_HAVE_SIX_VALUATORS */
  /*   double spacing; */
  /*   double lastscale, curscale; */
  double left;
  double t;
  double initial;
  double dist;
  double total;
  double pixel_dist;
  double pixel_initial;
  double xd, yd;
  double mag;
  delta.x = paint_core->curx - paint_core->lastx;
  delta.y = paint_core->cury - paint_core->lasty;
  dpressure = paint_core->curpressure - paint_core->lastpressure;
  dxtilt = paint_core->curxtilt - paint_core->lastxtilt;
  dytilt = paint_core->curytilt - paint_core->lastytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
  dwheel = paint_core->curwheel - paint_core->lastwheel;
#endif /* GTK_HAVE_SIX_VALUATORS */

/* return if there has been no motion */
#ifdef GTK_HAVE_SIX_VALUATORS
  if (!delta.x && !delta.y && !dpressure && !dxtilt && !dytilt && !dwheel)
#else /* !GTK_HAVE_SIX_VALUATORS */
  if (!delta.x && !delta.y && !dpressure && !dxtilt && !dytilt)
#endif /* GTK_HAVE_SIX_VALUATORS */
    return;

/* calculate the distance traveled in the coordinate space of the brush */
  mag = vector2d_magnitude (&(paint_core->brush->x_axis));
  xd = vector2d_dot_product(&delta, &(paint_core->brush->x_axis)) / (mag*mag);

  mag = vector2d_magnitude (&(paint_core->brush->y_axis));
  yd = vector2d_dot_product(&delta, &(paint_core->brush->y_axis)) / (mag*mag);

  dist = 0.5 * sqrt (xd*xd + yd*yd); 
  total = dist + paint_core->distance;
  initial = paint_core->distance;

  pixel_dist = vector2d_magnitude (&delta);
  pixel_initial = paint_core->pixel_dist;  

  /*  FIXME: need to adapt the spacing to the size  */
  /*   lastscale = MIN (paint_core->lastpressure, 1/256); */
  /*   curscale = MIN (paint_core->curpressure,  1/256); */
  /*   spacing = paint_core->spacing * sqrt (0.5 * (lastscale + curscale)); */

  while (paint_core->distance < total)
    {
      n = (int) (paint_core->distance / paint_core->spacing + 1.0 + EPSILON);
      left = n * paint_core->spacing - paint_core->distance;
      
      paint_core->distance += left;

      if (paint_core->distance <= (total+EPSILON))
	{
	  t = (paint_core->distance - initial) / dist;

	  paint_core->curx = paint_core->lastx + delta.x * t;
	  paint_core->cury = paint_core->lasty + delta.y * t;
	  paint_core->pixel_dist = pixel_initial + pixel_dist * t;
	  paint_core->curpressure = paint_core->lastpressure + dpressure * t;
	  paint_core->curxtilt = paint_core->lastxtilt + dxtilt * t;
	  paint_core->curytilt = paint_core->lastytilt + dytilt * t;
#ifdef GTK_HAVE_SIX_VALUATORS
          paint_core->curwheel = paint_core->lastwheel + dwheel * t;
#endif /* GTK_HAVE_SIX_VALUATORS */
	  if (paint_core->flags & TOOL_CAN_HANDLE_CHANGING_BRUSH)
	    paint_core->brush =
	      (* GIMP_BRUSH_CLASS (GTK_OBJECT (paint_core->brush)
				   ->klass)->select_brush) (paint_core);
	  (* paint_core->paint_func) (paint_core, drawable, MOTION_PAINT);
	}
    }
  paint_core->distance = total;
  paint_core->pixel_dist = pixel_initial + pixel_dist;
  paint_core->curx = paint_core->lastx + delta.x;
  paint_core->cury = paint_core->lasty + delta.y;
  paint_core->curpressure = paint_core->lastpressure + dpressure;
  paint_core->curxtilt = paint_core->lastxtilt + dxtilt;
  paint_core->curytilt = paint_core->lastytilt + dytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
  paint_core->curwheel = paint_core->lastwheel + dwheel;
#endif /* GTK_HAVE_SIX_VALUATORS */
}

void
paint_core_finish (PaintCore    *paint_core, 
		   GimpDrawable *drawable, 
		   int           tool_id)
{
  GImage *gimage;
  PaintUndo *pu;

  if (! (gimage = drawable_gimage (drawable)))
    return;

  /*  Determine if any part of the image has been altered--
   *  if nothing has, then just return...
   */

  if ((paint_core->x2 == paint_core->x1) || (paint_core->y2 == paint_core->y1))
    return;

  undo_push_group_start (gimage, PAINT_CORE_UNDO);

  pu = (PaintUndo *) g_malloc (sizeof (PaintUndo));
  pu->tool_ID = tool_id;
  pu->lastx = paint_core->startx;
  pu->lasty = paint_core->starty;
  pu->lastpressure = paint_core->startpressure;
  pu->lastxtilt = paint_core->startxtilt;
  pu->lastytilt = paint_core->startytilt;
#ifdef GTK_HAVE_SIX_VALUATORS
  pu->lastwheel = paint_core->startwheel;
#endif /* GTK_HAVE_SIX_VALUATORS */

  /*  Push a paint undo  */
  undo_push_paint (gimage, pu);

  /*  push an undo  */
  drawable_apply_image (drawable, paint_core->x1, paint_core->y1,
			paint_core->x2, paint_core->y2, undo_tiles, TRUE);
  undo_tiles = NULL;

  /*  push the group end  */
  undo_push_group_end (gimage);

  /*  invalidate the drawable--have to do it here, because
   *  it is not done during the actual painting.
   */
  drawable_invalidate_preview (drawable);
}

void
paint_core_cleanup ()
{
  /*  CLEANUP  */
  /*  If the undo tiles exist, nuke them  */
  if (undo_tiles)
    {
      tile_manager_destroy (undo_tiles);
      undo_tiles = NULL;
    }

  /*  If the canvas blocks exist, nuke them  */
  if (canvas_tiles)
    {
      tile_manager_destroy (canvas_tiles);
      canvas_tiles = NULL;
    }

  /*  Free the temporary buffers if they exist  */
  free_paint_buffers ();
}


/************************/
/*  Painting functions  */
/************************/

TempBuf *
paint_core_get_paint_area (PaintCore    *paint_core, 
			   GimpDrawable *drawable,
			   gdouble       scale)
{
  int x, y;
  int x1, y1, x2, y2;
  int bytes;
  int dwidth, dheight;
  int bwidth, bheight;

  bytes = drawable_has_alpha (drawable) ?
    drawable_bytes (drawable) : drawable_bytes (drawable) + 1;

  paint_core_calculate_brush_size (paint_core->brush->mask, scale,
				   &bwidth, &bheight);

  /*  adjust the x and y coordinates to the upper left corner of the brush  */
  x = (int) paint_core->curx - (bwidth >> 1);
  y = (int) paint_core->cury - (bheight >> 1);

  dwidth = drawable_width (drawable);
  dheight = drawable_height (drawable);

  x1 = BOUNDS (x - 1, 0, dwidth);
  y1 = BOUNDS (y - 1, 0, dheight);
  x2 = BOUNDS (x + bwidth + 1, 0, dwidth);
  y2 = BOUNDS (y + bheight + 1, 0, dheight);

  /*  configure the canvas buffer  */
  if ((x2 - x1) && (y2 - y1))
    canvas_buf = temp_buf_resize (canvas_buf, bytes, x1, y1,
				  (x2 - x1), (y2 - y1));
  else
    return NULL;

  return canvas_buf;
}

TempBuf *
paint_core_get_orig_image (PaintCore    *paint_core, 
			   GimpDrawable *drawable, 
			   int x1, int y1, int x2, int y2)
{
  PixelRegion srcPR, destPR;
  Tile *undo_tile;
  int h;
  int refd;
  int pixelwidth;
  int dwidth, dheight;
  unsigned char * s, * d;
  void * pr;

  orig_buf = temp_buf_resize (orig_buf, drawable_bytes (drawable),
			      x1, y1, (x2 - x1), (y2 - y1));

  dwidth = drawable_width (drawable);
  dheight = drawable_height (drawable);

  x1 = BOUNDS (x1, 0, dwidth);
  y1 = BOUNDS (y1, 0, dheight);
  x2 = BOUNDS (x2, 0, dwidth);
  y2 = BOUNDS (y2, 0, dheight);

  /*  configure the pixel regions  */
  pixel_region_init (&srcPR, drawable_data (drawable), x1, y1,
		     (x2 - x1), (y2 - y1), FALSE);
  destPR.bytes = orig_buf->bytes;
  destPR.x = 0; destPR.y = 0;
  destPR.w = (x2 - x1); destPR.h = (y2 - y1);
  destPR.rowstride = orig_buf->bytes * orig_buf->width;
  destPR.data = temp_buf_data (orig_buf) +
    (y1 - orig_buf->y) * destPR.rowstride + (x1 - orig_buf->x) * destPR.bytes;

  for (pr = pixel_regions_register (2, &srcPR, &destPR);
       pr != NULL;
       pr = pixel_regions_process (pr))
    {
      /*  If the undo tile corresponding to this location is valid, use it  */
      undo_tile = tile_manager_get_tile (undo_tiles, srcPR.x, srcPR.y,
					 FALSE, FALSE);
      if (tile_is_valid (undo_tile) == TRUE)
	{
	  refd = 1;
	  undo_tile = tile_manager_get_tile (undo_tiles, srcPR.x, srcPR.y,
					     TRUE, FALSE);
	  s = (unsigned char*)tile_data_pointer (undo_tile, 0, 0) +
	    srcPR.rowstride * (srcPR.y % TILE_HEIGHT) +
	    srcPR.bytes * (srcPR.x % TILE_WIDTH); /* dubious... */
	}
      else
	{
	  refd = 0;
	  s = srcPR.data;
	}

      d = destPR.data;
      pixelwidth = srcPR.w * srcPR.bytes;
      h = srcPR.h;
      while (h --)
	{
	  memcpy (d, s, pixelwidth);
	  s += srcPR.rowstride;
	  d += destPR.rowstride;
	}

      if (refd)
	tile_release (undo_tile, FALSE);
    }

  return orig_buf;
}

void
paint_core_paste_canvas (PaintCore	     *paint_core, 
			 GimpDrawable	     *drawable, 
			 int		      brush_opacity, 
			 int		      image_opacity, 
			 LayerModeEffects     paint_mode,
			 BrushApplicationMode brush_hardness,
			 gdouble              brush_scale,
			 PaintApplicationMode mode)
{
  MaskBuf *brush_mask;

  /*  get the brush mask  */
  brush_mask = paint_core_get_brush_mask (paint_core, brush_hardness, brush_scale);

  /*  paste the canvas buf  */
  paint_core_paste (paint_core, brush_mask, drawable,
		    brush_opacity, image_opacity, paint_mode, mode);
}

/* Similar to paint_core_paste_canvas, but replaces the alpha channel
   rather than using it to composite (i.e. transparent over opaque
   becomes transparent rather than opauqe. */
void
paint_core_replace_canvas (PaintCore	       *paint_core, 
			   GimpDrawable	       *drawable, 
			   int			brush_opacity, 
			   int			image_opacity, 
			   BrushApplicationMode brush_hardness,
			   gdouble              brush_scale,
			   PaintApplicationMode mode)
{
  MaskBuf *brush_mask;

  /*  get the brush mask  */
  brush_mask = paint_core_get_brush_mask (paint_core, brush_hardness, brush_scale);

  /*  paste the canvas buf  */
  paint_core_replace (paint_core, brush_mask, drawable,
		      brush_opacity, image_opacity, mode);
}

static MaskBuf  *last_brush_mask = NULL;
static gboolean  cache_invalid   = FALSE;

static void
paint_core_invalidate_cache (GimpBrush *brush,
			     gpointer  *blah)
{
  /* Make sure we don't cache data for a brush that has changed */
  if (last_brush_mask == brush->mask)
    cache_invalid = TRUE;
}

/************************************************************
 *             LOCAL FUNCTION DEFINITIONS                   *
 ************************************************************/

static void
paint_core_calculate_brush_size (MaskBuf   *mask,
				 double     scale,
				 int       *width,
				 int       *height)
{
  if (current_device == GDK_CORE_POINTER)
    {
      *width  = mask->width;
      *height = mask->height;
    }
  else
    {
      double ratio;

      if (scale < 1/256)
	ratio = 1/16;
      else
	ratio = sqrt (scale);
      
      *width  = MAX ((int)(mask->width  * ratio + 0.5), 1);
      *height = MAX ((int)(mask->height * ratio + 0.5), 1);
    }
}  

static MaskBuf *
paint_core_subsample_mask (MaskBuf *mask, 
			   double   x, 
			   double   y)
{
  MaskBuf * dest;
  double left;
  unsigned char * m, * d;
  const int * k;
  int index1, index2;
  const int * kernel;
  int new_val;
  int i, j;
  int r, s;

  x += (x < 0) ? mask->width : 0;
  left = x - floor(x) + 0.125;
  index1 = (int) (left * 4);

  y += (y < 0) ? mask->height : 0;
  left = y - floor(y) + 0.125;
  index2 = (int) (left * 4);

  kernel = subsample[index2][index1];

  if ((mask == last_brush_mask) && kernel_brushes[index2][index1] &&
      !cache_invalid)
    return kernel_brushes[index2][index1];
  else if (mask != last_brush_mask || cache_invalid)
    for (i = 0; i < 5; i++)
      for (j = 0; j < 5; j++)
	{
	  if (kernel_brushes[i][j])
	    mask_buf_free (kernel_brushes[i][j]);
	  kernel_brushes[i][j] = NULL;
	}

  last_brush_mask = mask;
  cache_invalid = FALSE;
  kernel_brushes[index2][index1] = mask_buf_new (mask->width + 2, mask->height + 2);
  dest = kernel_brushes[index2][index1];

  m = mask_buf_data (mask);
  for (i = 0; i < mask->height; i++)
    {
      for (j = 0; j < mask->width; j++)
	{
	  k = kernel;
	  for (r = 0; r < KERNEL_HEIGHT; r++)
	    {
	      d = mask_buf_data (dest) + (i+r) * dest->width + j;
	      s = KERNEL_WIDTH;
	      while (s--)
		{
		  new_val = *d + ((*m * *k++ + 128) >> 8);
		  *d++ = MINIMUM (new_val, 255);
		}
	    }
	  m++;
	}
    }

  return dest;
}

/* #define FANCY_PRESSURE */

static MaskBuf *
paint_core_pressurize_mask (MaskBuf *brush_mask, 
			    double   x, 
			    double   y, 
			    double   pressure)
{
  static MaskBuf *last_brush = NULL;
  static unsigned char mapi[256];
  unsigned char *source;
  unsigned char *dest;
  MaskBuf *subsample_mask;
  int i;
#ifdef FANCY_PRESSURE
  static double map[256];
  double ds,s,c;
#endif

  /* Get the raw subsampled mask */
  subsample_mask = paint_core_subsample_mask (brush_mask, x, y);

  /* Special case pressure = 0.5 */
  if ((int)(pressure*100+0.5) == 50)
    return subsample_mask;

  /* Make sure we have the right sized buffer */
  if (brush_mask != last_brush)
    {
      if (pressure_brush)
	mask_buf_free (pressure_brush);
      pressure_brush = mask_buf_new (brush_mask->width + 2,
				     brush_mask->height +2);
    }

#ifdef FANCY_PRESSURE
  /* Create the pressure profile

     It is: I'(I) = tanh(20*(pressure-0.5)*I) : pressure > 0.5
            I'(I) = 1 - tanh(20*(0.5-pressure)*(1-I)) : pressure < 0.5

     It looks like:

        low pressure      medium pressure     high pressure

             |                   /                 --
             |    		/                 /
            /     	       /                 |
          --                  /	                 |

  */

  ds = (pressure - 0.5)*(20./256.);
  s = 0;
  c = 1.0;

  if (ds > 0)
    {
      for (i=0;i<256;i++)
	{
	  map[i] = s/c;
	  s += c*ds;
	  c += s*ds;
	}
      for (i=0;i<256;i++)
	mapi[i] = (int)(255*map[i]/map[255]);
    }
  else
    {
      ds = -ds;
      for (i=255;i>=0;i--)
	{
	  map[i] = s/c;
	  s += c*ds;
	  c += s*ds;
	}
      for (i=0;i<256;i++)
	mapi[i] = (int)(255*(1-map[i]/map[0]));
    }
#else /* ! FANCY_PRESSURE */

  for (i=0;i<256;i++)
    {
      int tmp = (pressure/0.5)*i;
      if (tmp > 255)
	mapi[i] = 255;
      else
	mapi[i] = tmp;
    }

#endif /* FANCY_PRESSURE */

  /* Now convert the brush */

  source = mask_buf_data (subsample_mask);
  dest = mask_buf_data (pressure_brush);

  i = subsample_mask->width * subsample_mask->height;
  while (i--)
    {
      *dest++ = mapi[(*source++)];
    }

  return pressure_brush;
}

static MaskBuf *
paint_core_solidify_mask (MaskBuf *brush_mask)
{
  static MaskBuf *last_brush = NULL;
  int i, j;
  unsigned char * data, * src;

  if (brush_mask == last_brush && !cache_invalid)
    return solid_brush;
  last_brush = brush_mask;

  if (solid_brush)
    mask_buf_free (solid_brush);

  solid_brush = mask_buf_new (brush_mask->width + 2, brush_mask->height + 2);

  /*  get the data and advance one line into it  */
  data = mask_buf_data (solid_brush) + solid_brush->width;
  src = mask_buf_data (brush_mask);

  for (i = 0; i < brush_mask->height; i++)
    {
      data++;
      for (j = 0; j < brush_mask->width; j++)
	{
	  *data++ = (*src++) ? OPAQUE_OPACITY : TRANSPARENT_OPACITY;
	}
      data++;
    }

  return solid_brush;
}

static MaskBuf *
paint_core_scale_mask (MaskBuf *brush_mask, 
		       gdouble  scale)
{
  static MaskBuf *last_brush = NULL;
  static gint last_width  = 0.0; 
  static gint last_height = 0.0; 
  gint dest_width, dest_height;

  if (scale == 0.0) 
    return NULL;

  if (scale == 1.0)
    return brush_mask;
  
  paint_core_calculate_brush_size (brush_mask, scale, 
				   &dest_width, &dest_height);

  if (brush_mask == last_brush && !cache_invalid &&
      dest_width == last_width && dest_height == last_height)
    return scale_brush;

  if (scale_brush)
    mask_buf_free (scale_brush);

  last_brush  = brush_mask;
  last_width  = dest_width;
  last_height = dest_height;
 
  scale_brush = brush_scale_mask (brush_mask, dest_width, dest_height);
  cache_invalid = TRUE;

  return scale_brush;
}

static MaskBuf *
paint_core_scale_pixmap (MaskBuf *brush_mask, 
			 gdouble  scale)
{
  static MaskBuf *last_brush = NULL;
  static gint last_width  = 0.0; 
  static gint last_height = 0.0; 
  gint dest_width, dest_height;

  if (scale == 0.0) 
    return NULL;

  if (scale == 1.0)
    return brush_mask;
  
  paint_core_calculate_brush_size (brush_mask, scale, 
				   &dest_width, &dest_height);

  if (brush_mask == last_brush && !cache_invalid &&
      dest_width == last_width && dest_height == last_height)
    return scale_pixmap;

  if (scale_pixmap)
    mask_buf_free (scale_pixmap);

  last_brush  = brush_mask;
  last_width  = dest_width;
  last_height = dest_height;
 
  scale_pixmap = brush_scale_pixmap (brush_mask, dest_width, dest_height);
  cache_invalid = TRUE;

  return scale_pixmap;
}

static MaskBuf *
paint_core_get_brush_mask (PaintCore	       *paint_core, 
			   BrushApplicationMode brush_hardness,
			   gdouble              scale)
{
  MaskBuf * mask;

  if (current_device == GDK_CORE_POINTER)
    mask = paint_core->brush->mask;
  else
    mask = paint_core_scale_mask (paint_core->brush->mask, scale);
  
  switch (brush_hardness)
    {
    case SOFT:
      mask = paint_core_subsample_mask (mask, paint_core->curx, paint_core->cury);
      break;
    case HARD:
      mask = paint_core_solidify_mask (mask);
      break;
    case PRESSURE:
      mask = paint_core_pressurize_mask (mask, paint_core->curx, paint_core->cury, 
					 paint_core->curpressure);
      break;
    default:
      break;
    }

  return mask;
}

static void
paint_core_paste (PaintCore	      *paint_core, 
		  MaskBuf	      *brush_mask, 
		  GimpDrawable	      *drawable, 
		  int		       brush_opacity, 
		  int		       image_opacity, 
		  LayerModeEffects     paint_mode, 
		  PaintApplicationMode mode)
{
  GImage *gimage;
  PixelRegion srcPR;
  TileManager *alt = NULL;
  int offx, offy;

  if (! (gimage = drawable_gimage (drawable)))
    return;

  /*  set undo blocks  */
  set_undo_tiles (drawable,
		  canvas_buf->x, canvas_buf->y,
		  canvas_buf->width, canvas_buf->height);

  /*  If the mode is CONSTANT:
   *   combine the canvas buf, the brush mask to the canvas tiles
   */
  if (mode == CONSTANT)
    {
      /*  initialize any invalid canvas tiles  */
      set_canvas_tiles (canvas_buf->x, canvas_buf->y,
			canvas_buf->width, canvas_buf->height);

      brush_to_canvas_tiles (paint_core, brush_mask, brush_opacity);
      canvas_tiles_to_canvas_buf (paint_core);
      alt = undo_tiles;
    }
  /*  Otherwise:
   *   combine the canvas buf and the brush mask to the canvas buf
   */
  else  /*  mode != CONSTANT  */
    brush_to_canvas_buf (paint_core, brush_mask, brush_opacity);

  /*  intialize canvas buf source pixel regions  */
  srcPR.bytes = canvas_buf->bytes;
  srcPR.x = 0; srcPR.y = 0;
  srcPR.w = canvas_buf->width;
  srcPR.h = canvas_buf->height;
  srcPR.rowstride = canvas_buf->width * canvas_buf->bytes;
  srcPR.data = temp_buf_data (canvas_buf);

  /*  apply the paint area to the gimage  */
  gimage_apply_image (gimage, drawable, &srcPR,
		      FALSE, image_opacity, paint_mode,
		      alt,  /*  specify an alternative src1  */
		      canvas_buf->x, canvas_buf->y);

  /*  Update the undo extents  */
  paint_core->x1 = MINIMUM (paint_core->x1, canvas_buf->x);
  paint_core->y1 = MINIMUM (paint_core->y1, canvas_buf->y);
  paint_core->x2 = MAXIMUM (paint_core->x2, (canvas_buf->x + canvas_buf->width));
  paint_core->y2 = MAXIMUM (paint_core->y2, (canvas_buf->y + canvas_buf->height));

  /*  Update the gimage--it is important to call gdisplays_update_area
   *  instead of drawable_update because we don't want the drawable
   *  preview to be constantly invalidated
   */
  drawable_offsets (drawable, &offx, &offy);
  gdisplays_update_area (gimage, canvas_buf->x + offx, canvas_buf->y + offy,
			 canvas_buf->width, canvas_buf->height);
}

/* This works similarly to paint_core_paste. However, instead of combining
   the canvas to the paint core drawable using one of the combination
   modes, it uses a "replace" mode (i.e. transparent pixels in the 
   canvas erase the paint core drawable).

   When not drawing on alpha-enabled images, it just paints using NORMAL
   mode.
*/
static void
paint_core_replace (PaintCore		*paint_core, 
		    MaskBuf		*brush_mask, 
		    GimpDrawable	*drawable, 
		    int			 brush_opacity, 
		    int			 image_opacity, 
		    PaintApplicationMode mode)
{
  GImage *gimage;
  PixelRegion srcPR, maskPR;
  TileManager *alt = NULL;
  int offx, offy;

  if (!drawable_has_alpha (drawable))
    {
      paint_core_paste (paint_core, brush_mask, drawable,
			brush_opacity, image_opacity, NORMAL_MODE,
			mode);
      return;
    }

  if (! (gimage = drawable_gimage (drawable)))
    return;

  /*  set undo blocks  */
  set_undo_tiles (drawable,
		  canvas_buf->x, canvas_buf->y,
		  canvas_buf->width, canvas_buf->height);

  if (mode == CONSTANT) 
  {
    /*  initialize any invalid canvas tiles  */
    set_canvas_tiles (canvas_buf->x, canvas_buf->y,
		      canvas_buf->width, canvas_buf->height);

    /* combine the brush mask and the canvas tiles */
    brush_to_canvas_tiles (paint_core, brush_mask, brush_opacity);

    /* set the alt source as the unaltered undo_tiles */ 
    alt = undo_tiles;

    /* initialize the maskPR from the canvas tiles */
    pixel_region_init (&maskPR, canvas_tiles,
		       canvas_buf->x, canvas_buf->y,
		       canvas_buf->width, canvas_buf->height, FALSE);
  }
  else   
  {
    /* The mask is just the brush mask */
    maskPR.bytes = 1;
    maskPR.x = 0; maskPR.y = 0;
    maskPR.w = canvas_buf->width;
    maskPR.h = canvas_buf->height;
    maskPR.rowstride = maskPR.bytes * brush_mask->width;
    maskPR.data = mask_buf_data (brush_mask);
  }
  
  /*  intialize canvas buf source pixel regions  */
  srcPR.bytes = canvas_buf->bytes;
  srcPR.x = 0; srcPR.y = 0;
  srcPR.w = canvas_buf->width;
  srcPR.h = canvas_buf->height;
  srcPR.rowstride = canvas_buf->width * canvas_buf->bytes;
  srcPR.data = temp_buf_data (canvas_buf);

  /*  apply the paint area to the gimage  */
  gimage_replace_image (gimage, drawable, &srcPR,
			FALSE, image_opacity,
			&maskPR, 
			canvas_buf->x, canvas_buf->y);

  /*  Update the undo extents  */
  paint_core->x1 = MINIMUM (paint_core->x1, canvas_buf->x);
  paint_core->y1 = MINIMUM (paint_core->y1, canvas_buf->y);
  paint_core->x2 = MAXIMUM (paint_core->x2, (canvas_buf->x + canvas_buf->width));
  paint_core->y2 = MAXIMUM (paint_core->y2, (canvas_buf->y + canvas_buf->height));

  /*  Update the gimage--it is important to call gdisplays_update_area
   *  instead of drawable_update because we don't want the drawable
   *  preview to be constantly invalidated
   */
  drawable_offsets (drawable, &offx, &offy);
  gdisplays_update_area (gimage, canvas_buf->x + offx, canvas_buf->y + offy,
			 canvas_buf->width, canvas_buf->height);
}

static void
canvas_tiles_to_canvas_buf(PaintCore *paint_core)
{
  PixelRegion srcPR, maskPR;

  /*  combine the canvas tiles and the canvas buf  */
  srcPR.bytes = canvas_buf->bytes;
  srcPR.x = 0; srcPR.y = 0;
  srcPR.w = canvas_buf->width;
  srcPR.h = canvas_buf->height;
  srcPR.rowstride = canvas_buf->width * canvas_buf->bytes;
  srcPR.data = temp_buf_data (canvas_buf);

  pixel_region_init (&maskPR, canvas_tiles,
		     canvas_buf->x, canvas_buf->y,
		     canvas_buf->width, canvas_buf->height, FALSE);

  /*  apply the canvas tiles to the canvas buf  */
  apply_mask_to_region (&srcPR, &maskPR, OPAQUE_OPACITY);
}

static void
brush_to_canvas_tiles (PaintCore *paint_core, 
		       MaskBuf   *brush_mask, 
		       int        brush_opacity)
{
  PixelRegion srcPR, maskPR;
  int x, y;
  int xoff, yoff;

  /*   combine the brush mask and the canvas tiles  */
  pixel_region_init (&srcPR, canvas_tiles,
		     canvas_buf->x, canvas_buf->y,
		     canvas_buf->width, canvas_buf->height, TRUE);

  x = (int) paint_core->curx - (brush_mask->width >> 1);
  y = (int) paint_core->cury - (brush_mask->height >> 1);
  xoff = (x < 0) ? -x : 0;
  yoff = (y < 0) ? -y : 0;

  maskPR.bytes = 1;
  maskPR.x = 0; maskPR.y = 0;
  maskPR.w = srcPR.w;
  maskPR.h = srcPR.h;
  maskPR.rowstride = maskPR.bytes * brush_mask->width;
  maskPR.data = mask_buf_data (brush_mask) + yoff * maskPR.rowstride + xoff * maskPR.bytes;

  /*  combine the mask to the canvas tiles  */
  combine_mask_and_region (&srcPR, &maskPR, brush_opacity);
}

static void
brush_to_canvas_buf (PaintCore *paint_core,
		     MaskBuf   *brush_mask,
		     int        brush_opacity)
{
  PixelRegion srcPR, maskPR;
  int x, y;
  int xoff, yoff;

  x = (int) paint_core->curx - (brush_mask->width >> 1);
  y = (int) paint_core->cury - (brush_mask->height >> 1);
  xoff = (x < 0) ? -x : 0;
  yoff = (y < 0) ? -y : 0;

  /*  combine the canvas buf and the brush mask to the canvas buf  */
  srcPR.bytes = canvas_buf->bytes;
  srcPR.x = 0; srcPR.y = 0;
  srcPR.w = canvas_buf->width;
  srcPR.h = canvas_buf->height;
  srcPR.rowstride = canvas_buf->width * canvas_buf->bytes;
  srcPR.data = temp_buf_data (canvas_buf);

  maskPR.bytes = 1;
  maskPR.x = 0; maskPR.y = 0;
  maskPR.w = srcPR.w;
  maskPR.h = srcPR.h;
  maskPR.rowstride = maskPR.bytes * brush_mask->width;
  maskPR.data = mask_buf_data (brush_mask) + yoff * maskPR.rowstride + xoff * maskPR.bytes;

  /*  apply the mask  */
  apply_mask_to_region (&srcPR, &maskPR, brush_opacity);
}

#if 0
static void
paint_to_canvas_tiles (PaintCore *paint_core, 
		       MaskBuf   *brush_mask, 
		       int        brush_opacity)
{
  PixelRegion srcPR, maskPR;
  int x, y;
  int xoff, yoff;

  /*   combine the brush mask and the canvas tiles  */
  pixel_region_init (&srcPR, canvas_tiles,
		     canvas_buf->x, canvas_buf->y,
		     canvas_buf->width, canvas_buf->height, TRUE);

  x = (int) paint_core->curx - (brush_mask->width >> 1);
  y = (int) paint_core->cury - (brush_mask->height >> 1);
  xoff = (x < 0) ? -x : 0;
  yoff = (y < 0) ? -y : 0;

  maskPR.bytes = 1;
  maskPR.x = 0; maskPR.y = 0;
  maskPR.w = srcPR.w;
  maskPR.h = srcPR.h;
  maskPR.rowstride = maskPR.bytes * brush_mask->width;
  maskPR.data = mask_buf_data (brush_mask) + yoff * maskPR.rowstride + xoff * maskPR.bytes;

  /*  combine the mask and canvas tiles  */
  combine_mask_and_region (&srcPR, &maskPR, brush_opacity);

  /*  combine the canvas tiles and the canvas buf  */
  srcPR.bytes = canvas_buf->bytes;
  srcPR.x = 0; srcPR.y = 0;
  srcPR.w = canvas_buf->width;
  srcPR.h = canvas_buf->height;
  srcPR.rowstride = canvas_buf->width * canvas_buf->bytes;
  srcPR.data = temp_buf_data (canvas_buf);

  pixel_region_init (&maskPR, canvas_tiles,
		     canvas_buf->x, canvas_buf->y,
		     canvas_buf->width, canvas_buf->height, FALSE);

  /*  apply the canvas tiles to the canvas buf  */
  apply_mask_to_region (&srcPR, &maskPR, OPAQUE_OPACITY);
}

static void
paint_to_canvas_buf (PaintCore *paint_core, 
		     MaskBuf   *brush_mask, 
		     int        brush_opacity)
{
  PixelRegion srcPR, maskPR;
  int x, y;
  int xoff, yoff;

  x = (int) paint_core->curx - (brush_mask->width >> 1);
  y = (int) paint_core->cury - (brush_mask->height >> 1);
  xoff = (x < 0) ? -x : 0;
  yoff = (y < 0) ? -y : 0;


  /*  combine the canvas buf and the brush mask to the canvas buf  */
  srcPR.bytes = canvas_buf->bytes;
  srcPR.x = 0; srcPR.y = 0;
  srcPR.w = canvas_buf->width;
  srcPR.h = canvas_buf->height;
  srcPR.rowstride = canvas_buf->width * canvas_buf->bytes;
  srcPR.data = temp_buf_data (canvas_buf);

  maskPR.bytes = 1;
  maskPR.x = 0; maskPR.y = 0;
  maskPR.w = srcPR.w;
  maskPR.h = srcPR.h;
  maskPR.rowstride = maskPR.bytes * brush_mask->width;
  maskPR.data = mask_buf_data (brush_mask) + yoff * maskPR.rowstride + xoff * maskPR.bytes;

  /*  apply the mask  */
  apply_mask_to_region (&srcPR, &maskPR, brush_opacity);
}
#endif

static void
set_undo_tiles (GimpDrawable *drawable, 
		int x, int y, int w, int h)
{
  int i, j;
  Tile *src_tile;
  Tile *dest_tile;

  if (undo_tiles == NULL) 
    {
      g_warning (_("set_undo_tiles: undo_tiles is null"));
      return;
    }

  for (i = y; i < (y + h); i += (TILE_HEIGHT - (i % TILE_HEIGHT)))
    {
      for (j = x; j < (x + w); j += (TILE_WIDTH - (j % TILE_WIDTH)))
	{
	  dest_tile = tile_manager_get_tile (undo_tiles, j, i, FALSE, FALSE);
	  if (tile_is_valid (dest_tile) == FALSE)
	    {
	      src_tile = tile_manager_get_tile (drawable_data (drawable), j, i, TRUE, FALSE);
	      tile_manager_map_tile (undo_tiles, j, i, src_tile);
	      tile_release (src_tile, FALSE);
	    }
	}
    }
}

static void
set_canvas_tiles (int x, int y, int w, int h)
{
  int i, j;
  Tile *tile;

  for (i = y; i < (y + h); i += (TILE_HEIGHT - (i % TILE_HEIGHT)))
    {
      for (j = x; j < (x + w); j += (TILE_WIDTH - (j % TILE_WIDTH)))
	{
	  tile = tile_manager_get_tile (canvas_tiles, j, i, FALSE, FALSE);
	  if (tile_is_valid (tile) == FALSE)
	    {
	      tile = tile_manager_get_tile (canvas_tiles, j, i, TRUE, TRUE);
	      memset (tile_data_pointer (tile, 0, 0), 0, 
		      tile_size (tile));
	      tile_release (tile, TRUE);
	    }
	}
    }
}


/*****************************************************/
/*  Paint buffers utility functions                  */
/*****************************************************/


static void
free_paint_buffers ()
{
  if (orig_buf)
    temp_buf_free (orig_buf);
  orig_buf = NULL;

  if (canvas_buf)
    temp_buf_free (canvas_buf);
  canvas_buf = NULL;
}


/**************************************************/
/*  Brush pipe utility functions                  */
/**************************************************/

void
paint_core_color_area_with_pixmap (PaintCore    *paint_core,
				   GImage       *dest,
				   GimpDrawable *drawable,
				   TempBuf      *area,
				   gdouble       scale,
				   int           mode)
{

  PixelRegion destPR;
  void *pr;
  guchar *d;
  int ulx, uly, offsetx, offsety, y;
  TempBuf *pixmap_mask;
  TempBuf *brush_mask;
  
  g_return_if_fail (GIMP_IS_BRUSH_PIXMAP (paint_core->brush));

  /*  scale the brushes  */
  pixmap_mask = 
    paint_core_scale_pixmap (gimp_brush_pixmap_pixmap (GIMP_BRUSH_PIXMAP (paint_core->brush)), scale);
  if (mode == SOFT)
    brush_mask = paint_core_scale_mask (paint_core->brush->mask, scale);
  else
    brush_mask = NULL;

  destPR.bytes = area->bytes;
  destPR.x = 0; destPR.y = 0;
  destPR.w = area->width;
  destPR.h = area->height;
  destPR.rowstride = destPR.bytes * area->width;
  destPR.data = temp_buf_data (area);
		
  pr = pixel_regions_register (1, &destPR);

  /* Calculate upper left corner of brush as in
   * paint_core_get_paint_area.  Ugly to have to do this here, too.
   */

  ulx = (int) paint_core->curx - (pixmap_mask->width >> 1);
  uly = (int) paint_core->cury - (pixmap_mask->height >> 1);

  offsetx = area->x - ulx;
  offsety = area->y - uly;

  for (; pr != NULL; pr = pixel_regions_process (pr))
    {
      d = destPR.data;
      for (y = 0; y < destPR.h; y++)
	{
	  paint_line_pixmap_mask (dest, drawable, pixmap_mask, brush_mask,
				  d, offsetx, y + offsety,
				  destPR.bytes, destPR.w, mode);
	  d += destPR.rowstride;
	}
    }
}

static void
paint_line_pixmap_mask (GImage          *dest,
			GimpDrawable    *drawable,
			TempBuf         *pixmap_mask,
			TempBuf         *brush_mask,
			guchar	        *d,
			int		 x,
			int              y,
			int              bytes,
			int              width,
			int              mode)
{
  guchar *b, *p;
  int x_index;
  gdouble alpha;
  gdouble factor = 0.00392156986;  /* 1.0/255.0 */
  gint i;
  guchar *mask;

  /*  Make sure x, y are positive  */
  while (x < 0)
    x += pixmap_mask->width;
  while (y < 0)
    y += pixmap_mask->height;

  /* Point to the approriate scanline */
  b = temp_buf_data (pixmap_mask) +
    (y % pixmap_mask->height) * pixmap_mask->width * pixmap_mask->bytes;
    
  if (mode == SOFT && brush_mask)
    {
      /* ditto, except for the brush mask, so we can pre-multiply the alpha value */
      mask = temp_buf_data (brush_mask) +
	(y % brush_mask->height) * brush_mask->width;
      for (i = 0; i < width; i++)
	{
	  /* attempt to avoid doing this calc twice in the loop */
	  x_index = ((i + x) % pixmap_mask->width);
	  p = b + x_index * pixmap_mask->bytes;
	  d[bytes-1] = mask[x_index];
	  
	  /* multiply alpha into the pixmap data */
	  /* maybe we could do this at tool creation or brush switch time? */
	  /* and compute it for the whole brush at once and cache it?  */
	  alpha = d[bytes-1] * factor;
	  if (alpha)
	    {
	     d[0] *= alpha;
	     d[1] *= alpha;
	     d[2] *= alpha;
	    }
	  /* printf("i: %i d->r: %i d->g: %i d->b: %i d->a: %i\n",i,(int)d[0], (int)d[1], (int)d[2], (int)d[3]); */
	  gimage_transform_color (dest, drawable, p, d, RGB);
	  d += bytes;
	}
    }
  else
    {
      for (i = 0; i < width; i++)
	{
	  /* attempt to avoid doing this calc twice in the loop */
	  x_index = ((i + x) % pixmap_mask->width);
	  p = b + x_index * pixmap_mask->bytes;
	  d[bytes-1] = 255;
	  
	  /* multiply alpha into the pixmap data */
	  /* maybe we could do this at tool creation or brush switch time? */
	  /* and compute it for the whole brush at once and cache it?  */
	  gimage_transform_color (dest, drawable, p, d, RGB);
	  d += bytes;
	}
    }
}
