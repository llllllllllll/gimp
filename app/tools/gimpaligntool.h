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

#ifndef __GIMP_ALIGN_TOOL_H__
#define __GIMP_ALIGN_TOOL_H__


#include "gimpdrawtool.h"


#define GIMP_TYPE_ALIGN_TOOL            (gimp_align_tool_get_type ())
#define GIMP_ALIGN_TOOL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_ALIGN_TOOL, GimpAlignTool))
#define GIMP_ALIGN_TOOL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_ALIGN_TOOL, GimpAlignToolClass))
#define GIMP_IS_ALIGN_TOOL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_ALIGN_TOOL))
#define GIMP_IS_ALIGN_TOOL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_ALIGN_TOOL))
#define GIMP_ALIGN_TOOL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_ALIGN_TOOL, GimpAlignToolClass))

#define ALIGN_TOOL_NUM_BUTTONS 6

typedef struct _GimpAlignTool      GimpAlignTool;
typedef struct _GimpAlignToolClass GimpAlignToolClass;

struct _GimpAlignTool
{
  GimpDrawTool         parent_instance;

  GtkWidget           *controls;
  GtkWidget           *button[ALIGN_TOOL_NUM_BUTTONS];

  GimpItem            *target_item;
  GimpItem            *reference_item;

  gboolean             select_reference;

  GimpAlignmentType    target_horz_align_type;
  GimpAlignmentType    target_vert_align_type;
  GimpAlignmentType    ref_horz_align_type;
  GimpAlignmentType    ref_vert_align_type;

  gdouble              horz_offset;
  gdouble              vert_offset;

  GtkObject           *horz_offset_adjustment;
  GtkObject           *vert_offset_adjustment;

};

struct _GimpAlignToolClass
{
  GimpDrawToolClass parent_class;
};


void    gimp_align_tool_register (GimpToolRegisterCallback  callback,
                                 gpointer                  data);

GType   gimp_align_tool_get_type (void) G_GNUC_CONST;


#endif  /*  __GIMP_ALIGN_TOOL_H__  */
