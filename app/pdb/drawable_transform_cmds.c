/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-2003 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "config.h"


#include <glib-object.h>

#include "libgimpmath/gimpmath.h"

#include "pdb-types.h"
#include "procedural_db.h"

#include "config/gimpcoreconfig.h"
#include "core/gimp-transform-utils.h"
#include "core/gimp.h"
#include "core/gimpdrawable-transform.h"
#include "core/gimpdrawable.h"
#include "core/gimpimage.h"
#include "core/gimpprogress.h"
#include "gimp-intl.h"

static ProcRecord drawable_transform_flip_proc;
static ProcRecord drawable_transform_flip_free_proc;
static ProcRecord drawable_transform_perspective_proc;
static ProcRecord drawable_transform_rotate_proc;
static ProcRecord drawable_transform_scale_proc;
static ProcRecord drawable_transform_shear_proc;
static ProcRecord drawable_transform_2d_proc;

void
register_drawable_transform_procs (Gimp *gimp)
{
  procedural_db_register (gimp, &drawable_transform_flip_proc);
  procedural_db_register (gimp, &drawable_transform_flip_free_proc);
  procedural_db_register (gimp, &drawable_transform_perspective_proc);
  procedural_db_register (gimp, &drawable_transform_rotate_proc);
  procedural_db_register (gimp, &drawable_transform_scale_proc);
  procedural_db_register (gimp, &drawable_transform_shear_proc);
  procedural_db_register (gimp, &drawable_transform_2d_proc);
}

static Argument *
drawable_transform_flip_invoker (Gimp         *gimp,
                                 GimpContext  *context,
                                 GimpProgress *progress,
                                 Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gint32 flip_type;
  gboolean center = FALSE;
  gdouble axis = 0;
  gint32 transform_direction = 0;
  gint32 interpolation = 0;
  gboolean supersample = FALSE;
  gint32 recursion_level = 0;
  gboolean clip_result = FALSE;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  flip_type = args[1].value.pdb_int;
  if (flip_type < GIMP_ORIENTATION_HORIZONTAL || flip_type > GIMP_ORIENTATION_VERTICAL)
    success = FALSE;

  center = args[2].value.pdb_int ? TRUE : FALSE;

  axis = args[3].value.pdb_float;

  transform_direction = args[4].value.pdb_int;
  if (transform_direction < GIMP_TRANSFORM_FORWARD || transform_direction > GIMP_TRANSFORM_BACKWARD)
    success = FALSE;

  interpolation = args[5].value.pdb_int;
  if (interpolation < GIMP_INTERPOLATION_NONE || interpolation > GIMP_INTERPOLATION_CUBIC)
    success = FALSE;

  supersample = args[6].value.pdb_int ? TRUE : FALSE;

  recursion_level = args[7].value.pdb_int;
  if (recursion_level <= 0)
    success = FALSE;

  clip_result = args[8].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        success = gimp_drawable_transform_flip (drawable,
                                                context,
                                                transform_direction,
                                                center, axis,
                                                clip_result);
    }

  return_args = procedural_db_return_args (&drawable_transform_flip_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (drawable));

  return return_args;
}

static ProcArg drawable_transform_flip_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_INT32,
    "flip_type",
    "Type of flip: GIMP_ORIENTATION_HORIZONTAL (0) or GIMP_ORIENTATION_VERTICAL (1)"
  },
  {
    GIMP_PDB_INT32,
    "center",
    "Whether to automatically position the axis in the image center"
  },
  {
    GIMP_PDB_FLOAT,
    "axis",
    "coord. of flip axis"
  },
  {
    GIMP_PDB_INT32,
    "transform_direction",
    "Direction of Transformation: { GIMP_TRANSFORM_FORWARD (0), GIMP_TRANSFORM_BACKWARD (1) }"
  },
  {
    GIMP_PDB_INT32,
    "interpolation",
    "Type of interpolation: { GIMP_INTERPOLATION_NONE (0), GIMP_INTERPOLATION_LINEAR (1), GIMP_INTERPOLATION_CUBIC (2) }"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Whether to perform supersample"
  },
  {
    GIMP_PDB_INT32,
    "recursion_level",
    "Level of recursion (3 is a nice default)"
  },
  {
    GIMP_PDB_INT32,
    "clip_result",
    "Whether to clip results"
  }
};

static ProcArg drawable_transform_flip_outargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The flipped drawable"
  }
};

static ProcRecord drawable_transform_flip_proc =
{
  "gimp_drawable_transform_flip",
  "Flip the specified drawable either vertically or horizontally.",
  "This tool flips the specified drawable if no selection exists. If a selection exists, the portion of the drawable which lies under the selection is cut from the drawable and made into a floating selection which is then flipped. If center is set to true, the flip is around the image center. Otherwise, the coordinate of the axis needs to be specified. The return value is the ID of the flipped drawable. If there was no selection, this will be equal to the drawable ID supplied as input. Otherwise, this will be the newly created and flipped drawable.",
  "",
  "",
  "",
  NULL,
  GIMP_INTERNAL,
  9,
  drawable_transform_flip_inargs,
  1,
  drawable_transform_flip_outargs,
  { { drawable_transform_flip_invoker } }
};

static Argument *
drawable_transform_flip_free_invoker (Gimp         *gimp,
                                      GimpContext  *context,
                                      GimpProgress *progress,
                                      Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gdouble x1 = 0;
  gdouble y1 = 0;
  gdouble x2 = 0;
  gdouble y2 = 0;
  gint32 transform_direction = 0;
  gint32 interpolation = 0;
  gboolean supersample = FALSE;
  gint32 recursion_level = 0;
  gboolean clip_result = FALSE;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  x1 = args[1].value.pdb_float;

  y1 = args[2].value.pdb_float;

  x2 = args[3].value.pdb_float;

  y2 = args[4].value.pdb_float;

  transform_direction = args[5].value.pdb_int;
  if (transform_direction < GIMP_TRANSFORM_FORWARD || transform_direction > GIMP_TRANSFORM_BACKWARD)
    success = FALSE;

  interpolation = args[6].value.pdb_int;
  if (interpolation < GIMP_INTERPOLATION_NONE || interpolation > GIMP_INTERPOLATION_CUBIC)
    success = FALSE;

  supersample = args[7].value.pdb_int ? TRUE : FALSE;

  recursion_level = args[8].value.pdb_int;
  if (recursion_level <= 0)
    success = FALSE;

  clip_result = args[9].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          gint x, y, width, height;

          if (gimp_drawable_mask_intersect (drawable, &x, &y, &width, &height))
            {
              gdouble      angle;
              gdouble      dx, dy;
              GimpMatrix3  matrix;

              angle = atan2  ((y2 - y1),  (x2 - x1));
              dx = x  - x1;
              dy = (x1 + ( (gdouble) (y2 - y1) / (x2 - x1) ) * x ) - y1;

              gimp_matrix3_identity  (&matrix);
              gimp_matrix3_translate (&matrix, dx, dy);
              gimp_matrix3_rotate    (&matrix, -angle);
              gimp_matrix3_scale     (&matrix, 1.0, -1.0);
              gimp_matrix3_rotate    (&matrix, angle);
              gimp_matrix3_translate (&matrix, -dx, -dy);

              if (progress)
                gimp_progress_start (progress, _("Flip..."), FALSE);

              /* Transform the selection */
              success = gimp_drawable_transform_affine (drawable, context,
                                                        &matrix,
                                                        transform_direction,
                                                        interpolation,
                                                        supersample,
                                                        recursion_level,
                                                        clip_result,
                                                        progress);
              if (progress)
                gimp_progress_end (progress);
            }
        }
    }

  return_args = procedural_db_return_args (&drawable_transform_flip_free_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (drawable));

  return return_args;
}

static ProcArg drawable_transform_flip_free_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_FLOAT,
    "x1",
    "horz. coord. of one end of axis"
  },
  {
    GIMP_PDB_FLOAT,
    "y1",
    "vert. coord. of one end of axis"
  },
  {
    GIMP_PDB_FLOAT,
    "x2",
    "horz. coord. of other end of axis"
  },
  {
    GIMP_PDB_FLOAT,
    "y2",
    "vert. coord. of other end of axis"
  },
  {
    GIMP_PDB_INT32,
    "transform_direction",
    "Direction of Transformation: { GIMP_TRANSFORM_FORWARD (0), GIMP_TRANSFORM_BACKWARD (1) }"
  },
  {
    GIMP_PDB_INT32,
    "interpolation",
    "Type of interpolation: { GIMP_INTERPOLATION_NONE (0), GIMP_INTERPOLATION_LINEAR (1), GIMP_INTERPOLATION_CUBIC (2) }"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Whether to perform supersample"
  },
  {
    GIMP_PDB_INT32,
    "recursion_level",
    "Level of recursion (3 is a nice default)"
  },
  {
    GIMP_PDB_INT32,
    "clip_result",
    "Whether to clip results"
  }
};

static ProcArg drawable_transform_flip_free_outargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The flipped drawable"
  }
};

static ProcRecord drawable_transform_flip_free_proc =
{
  "gimp_drawable_transform_flip_free",
  "Flip the specified drawable around a given line.",
  "This tool flips the specified drawable if no selection exists. If a selection exists, the portion of the drawable which lies under the selection is cut from the drawable and made into a floating selection which is then flipped. The axis to flip around is specified by specifying two points from that line. The return value is the ID of the flipped drawable. If there was no selection, this will be equal to the drawable ID supplied as input. Otherwise, this will be the newly created and flipped drawable. The clip results parameter specifies wheter current selection will affect the transform.",
  "",
  "",
  "",
  NULL,
  GIMP_INTERNAL,
  10,
  drawable_transform_flip_free_inargs,
  1,
  drawable_transform_flip_free_outargs,
  { { drawable_transform_flip_free_invoker } }
};

static Argument *
drawable_transform_perspective_invoker (Gimp         *gimp,
                                        GimpContext  *context,
                                        GimpProgress *progress,
                                        Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gint32 transform_direction = 0;
  gint32 interpolation = 0;
  gboolean supersample = FALSE;
  gint32 recursion_level = 0;
  gboolean clip_result = FALSE;
  gdouble trans_info[8];

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  trans_info[X0] = args[1].value.pdb_float;

  trans_info[Y0] = args[2].value.pdb_float;

  trans_info[X1] = args[3].value.pdb_float;

  trans_info[Y1] = args[4].value.pdb_float;

  trans_info[X2] = args[5].value.pdb_float;

  trans_info[Y2] = args[6].value.pdb_float;

  trans_info[X3] = args[7].value.pdb_float;

  trans_info[Y3] = args[8].value.pdb_float;

  transform_direction = args[9].value.pdb_int;
  if (transform_direction < GIMP_TRANSFORM_FORWARD || transform_direction > GIMP_TRANSFORM_BACKWARD)
    success = FALSE;

  interpolation = args[10].value.pdb_int;
  if (interpolation < GIMP_INTERPOLATION_NONE || interpolation > GIMP_INTERPOLATION_CUBIC)
    success = FALSE;

  supersample = args[11].value.pdb_int ? TRUE : FALSE;

  recursion_level = args[12].value.pdb_int;
  if (recursion_level <= 0)
    success = FALSE;

  clip_result = args[13].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          gint x, y, width, height;

          if (gimp_drawable_mask_intersect (drawable, &x, &y, &width, &height))
            {
              GimpMatrix3  matrix;

              /* Assemble the transformation matrix */
              gimp_transform_matrix_perspective (x, y, x + width, y + height,
                                                 trans_info[X0], trans_info[Y0],
                                                 trans_info[X1], trans_info[Y1],
                                                 trans_info[X2], trans_info[Y2],
                                                 trans_info[X3], trans_info[Y3],
                                                 &matrix);

              if (progress)
                gimp_progress_start (progress, _("Perspective..."), FALSE);

              /* Perspective the selection */
              success = gimp_drawable_transform_affine (drawable, context,
                                                        &matrix,
                                                        transform_direction,
                                                        interpolation, supersample,
                                                        recursion_level,
                                                        clip_result,
                                                        progress);

              if (progress)
                gimp_progress_end (progress);
            }
        }
    }

  return_args = procedural_db_return_args (&drawable_transform_perspective_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (drawable));

  return return_args;
}

static ProcArg drawable_transform_perspective_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_FLOAT,
    "x0",
    "The new x coordinate of upper-left corner of original bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "y0",
    "The new y coordinate of upper-left corner of original bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "x1",
    "The new x coordinate of upper-right corner of original bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "y1",
    "The new y coordinate of upper-right corner of original bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "x2",
    "The new x coordinate of lower-left corner of original bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "y2",
    "The new y coordinate of lower-left corner of original bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "x3",
    "The new x coordinate of lower-right corner of original bounding box"
  },
  {
    GIMP_PDB_FLOAT,
    "y3",
    "The new y coordinate of lower-right corner of original bounding box"
  },
  {
    GIMP_PDB_INT32,
    "transform_direction",
    "Direction of Transformation: { GIMP_TRANSFORM_FORWARD (0), GIMP_TRANSFORM_BACKWARD (1) }"
  },
  {
    GIMP_PDB_INT32,
    "interpolation",
    "Type of interpolation: { GIMP_INTERPOLATION_NONE (0), GIMP_INTERPOLATION_LINEAR (1), GIMP_INTERPOLATION_CUBIC (2) }"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Whether to perform supersample"
  },
  {
    GIMP_PDB_INT32,
    "recursion_level",
    "Level of recursion (3 is a nice default)"
  },
  {
    GIMP_PDB_INT32,
    "clip_result",
    "Whether to clip results"
  }
};

static ProcArg drawable_transform_perspective_outargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The newly mapped drawable"
  }
};

static ProcRecord drawable_transform_perspective_proc =
{
  "gimp_drawable_transform_perspective",
  "Perform a possibly non-affine transformation on the specified drawable, with extra parameters.",
  "This tool performs a possibly non-affine transformation on the specified drawable by allowing the corners of the original bounding box to be arbitrarily remapped to any values. The specified drawable is remapped if no selection exists. However, if a selection exists, the portion of the drawable which lies under the selection is cut from the drawable and made into a floating selection which is then remapped as specified. The return value is the ID of the remapped drawable. If there was no selection, this will be equal to the drawable ID supplied as input. Otherwise, this will be the newly created and remapped drawable. The 4 coordinates specify the new locations of each corner of the original bounding box. By specifying these values, any affine transformation (rotation, scaling, translation) can be affected. Additionally, these values can be specified such that the resulting transformed drawable will appear to have been projected via a perspective transform.",
  "João S. O. Bueno Calligaris",
  "João S. O. Bueno Calligaris",
  "2004",
  NULL,
  GIMP_INTERNAL,
  14,
  drawable_transform_perspective_inargs,
  1,
  drawable_transform_perspective_outargs,
  { { drawable_transform_perspective_invoker } }
};

static Argument *
drawable_transform_rotate_invoker (Gimp         *gimp,
                                   GimpContext  *context,
                                   GimpProgress *progress,
                                   Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gdouble angle;
  gint32 cx;
  gint32 cy;
  gint32 transform_direction = 0;
  gint32 interpolation = 0;
  gboolean supersample = FALSE;
  gint32 recursion_level = 0;
  gboolean clip_result = FALSE;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  angle = args[1].value.pdb_float;

  cx = args[2].value.pdb_int;

  cy = args[3].value.pdb_int;

  transform_direction = args[4].value.pdb_int;
  if (transform_direction < GIMP_TRANSFORM_FORWARD || transform_direction > GIMP_TRANSFORM_BACKWARD)
    success = FALSE;

  interpolation = args[5].value.pdb_int;
  if (interpolation < GIMP_INTERPOLATION_NONE || interpolation > GIMP_INTERPOLATION_CUBIC)
    success = FALSE;

  supersample = args[6].value.pdb_int ? TRUE : FALSE;

  recursion_level = args[7].value.pdb_int;
  if (recursion_level <= 0)
    success = FALSE;

  clip_result = args[8].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpMatrix3  matrix;

          /* Assemble the transformation matrix */
          gimp_transform_matrix_rotate_center (cx, cy, angle, &matrix);

          if (progress)
            gimp_progress_start (progress, _("Rotating..."), FALSE);

          /* Rotate the selection */
          success = gimp_drawable_transform_affine (drawable, context,
                                                    &matrix, transform_direction,
                                                    interpolation, supersample,
                                                    recursion_level,
                                                    clip_result, progress);

          if (progress)
            gimp_progress_end (progress);
        }
    }

  return_args = procedural_db_return_args (&drawable_transform_rotate_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (drawable));

  return return_args;
}

static ProcArg drawable_transform_rotate_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_FLOAT,
    "angle",
    "The angle of rotation (radians)"
  },
  {
    GIMP_PDB_INT32,
    "cx",
    "The hor. coordinate of the center of rotation"
  },
  {
    GIMP_PDB_INT32,
    "cy",
    "The vert. coordinate of the center of rotation"
  },
  {
    GIMP_PDB_INT32,
    "transform_direction",
    "Direction of Transformation: { GIMP_TRANSFORM_FORWARD (0), GIMP_TRANSFORM_BACKWARD (1) }"
  },
  {
    GIMP_PDB_INT32,
    "interpolation",
    "Type of interpolation: { GIMP_INTERPOLATION_NONE (0), GIMP_INTERPOLATION_LINEAR (1), GIMP_INTERPOLATION_CUBIC (2) }"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Whether to perform supersample"
  },
  {
    GIMP_PDB_INT32,
    "recursion_level",
    "Level of recursion (3 is a nice default)"
  },
  {
    GIMP_PDB_INT32,
    "clip_result",
    "Whether to clip results"
  }
};

static ProcArg drawable_transform_rotate_outargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The rotated drawable"
  }
};

static ProcRecord drawable_transform_rotate_proc =
{
  "gimp_drawable_transform_rotate",
  "Rotate the specified drawable about given coordinates through the specified angle.",
  "This tool rotates the specified drawable if no selection exists. If a selection exists, the portion of the drawable which lies under the selection is cut from the drawable and made into a floating selection which is then rotated by the specified amount. The return value is the ID of the rotated drawable. If there was no selection, this will be equal to the drawable ID supplied as input. Otherwise, this will be the newly created and rotated drawable.",
  "João S. O. Bueno Calligaris",
  "João S. O. Bueno Calligaris",
  "2004",
  NULL,
  GIMP_INTERNAL,
  9,
  drawable_transform_rotate_inargs,
  1,
  drawable_transform_rotate_outargs,
  { { drawable_transform_rotate_invoker } }
};

static Argument *
drawable_transform_scale_invoker (Gimp         *gimp,
                                  GimpContext  *context,
                                  GimpProgress *progress,
                                  Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gint32 transform_direction = 0;
  gint32 interpolation = 0;
  gboolean supersample = FALSE;
  gint32 recursion_level = 0;
  gboolean clip_result = FALSE;
  gdouble trans_info[4];

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  trans_info[X0] = args[1].value.pdb_float;

  trans_info[Y0] = args[2].value.pdb_float;

  trans_info[X1] = args[3].value.pdb_float;

  trans_info[Y1] = args[4].value.pdb_float;

  transform_direction = args[5].value.pdb_int;
  if (transform_direction < GIMP_TRANSFORM_FORWARD || transform_direction > GIMP_TRANSFORM_BACKWARD)
    success = FALSE;

  interpolation = args[6].value.pdb_int;
  if (interpolation < GIMP_INTERPOLATION_NONE || interpolation > GIMP_INTERPOLATION_CUBIC)
    success = FALSE;

  supersample = args[7].value.pdb_int ? TRUE : FALSE;

  recursion_level = args[8].value.pdb_int;
  if (recursion_level <= 0)
    success = FALSE;

  clip_result = args[9].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = (gimp_item_is_attached (GIMP_ITEM (drawable)) &&
                 trans_info[X0] < trans_info[X1] &&
                 trans_info[Y0] < trans_info[X1]);

      if (success)
        {
          gint x, y, width, height;

          if (gimp_drawable_mask_intersect (drawable, &x, &y, &width, &height))
            {
              GimpMatrix3  matrix;

              /* Assemble the transformation matrix */
              gimp_transform_matrix_scale (x, y, x + width, y + height,
                                           trans_info[X0], trans_info[Y0],
                                           trans_info[X1], trans_info[Y1],
                                           &matrix);

              if (progress)
                gimp_progress_start (progress, _("Scaling..."), FALSE);

              /* Scale the selection */
              success = gimp_drawable_transform_affine (drawable, context,
                                                        &matrix,
                                                        transform_direction,
                                                        interpolation, supersample,
                                                        recursion_level,
                                                        clip_result, progress);

              if (progress)
                gimp_progress_end (progress);
            }
        }
    }

  return_args = procedural_db_return_args (&drawable_transform_scale_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (drawable));

  return return_args;
}

static ProcArg drawable_transform_scale_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_FLOAT,
    "x0",
    "The new x coordinate of upper-left corner of newly scaled region"
  },
  {
    GIMP_PDB_FLOAT,
    "y0",
    "The new y coordinate of upper-left corner of newly scaled region"
  },
  {
    GIMP_PDB_FLOAT,
    "x1",
    "The new x coordinate of lower-right corner of newly scaled region"
  },
  {
    GIMP_PDB_FLOAT,
    "y1",
    "The new y coordinate of lower-right corner of newly scaled region"
  },
  {
    GIMP_PDB_INT32,
    "transform_direction",
    "Direction of Transformation: { GIMP_TRANSFORM_FORWARD (0), GIMP_TRANSFORM_BACKWARD (1) }"
  },
  {
    GIMP_PDB_INT32,
    "interpolation",
    "Type of interpolation: { GIMP_INTERPOLATION_NONE (0), GIMP_INTERPOLATION_LINEAR (1), GIMP_INTERPOLATION_CUBIC (2) }"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Whether to perform supersample"
  },
  {
    GIMP_PDB_INT32,
    "recursion_level",
    "Level of recursion (3 is a nice default)"
  },
  {
    GIMP_PDB_INT32,
    "clip_result",
    "Whether to clip results"
  }
};

static ProcArg drawable_transform_scale_outargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The scaled drawable"
  }
};

static ProcRecord drawable_transform_scale_proc =
{
  "gimp_drawable_transform_scale",
  "Scale the specified drawable with extra parameters",
  "This tool scales the specified drawable if no selection exists. If a selection exists, the portion of the drawable which lies under the selection is cut from the drawable and made into a floating selection which is then scaled by the specified amount. The return value is the ID of the scaled drawable. If there was no selection, this will be equal to the drawable ID supplied as input. Otherwise, this will be the newly created and scaled drawable.",
  "João S. O. Bueno Calligaris",
  "João S. O. Bueno Calligaris",
  "2004",
  NULL,
  GIMP_INTERNAL,
  10,
  drawable_transform_scale_inargs,
  1,
  drawable_transform_scale_outargs,
  { { drawable_transform_scale_invoker } }
};

static Argument *
drawable_transform_shear_invoker (Gimp         *gimp,
                                  GimpContext  *context,
                                  GimpProgress *progress,
                                  Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gint32 shear_type;
  gdouble magnitude;
  gint32 transform_direction = 0;
  gint32 interpolation = 0;
  gboolean supersample = FALSE;
  gint32 recursion_level = 0;
  gboolean clip_result = FALSE;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  shear_type = args[1].value.pdb_int;
  if (shear_type < GIMP_ORIENTATION_HORIZONTAL || shear_type > GIMP_ORIENTATION_VERTICAL)
    success = FALSE;

  magnitude = args[2].value.pdb_float;

  transform_direction = args[3].value.pdb_int;
  if (transform_direction < GIMP_TRANSFORM_FORWARD || transform_direction > GIMP_TRANSFORM_BACKWARD)
    success = FALSE;

  interpolation = args[4].value.pdb_int;
  if (interpolation < GIMP_INTERPOLATION_NONE || interpolation > GIMP_INTERPOLATION_CUBIC)
    success = FALSE;

  supersample = args[5].value.pdb_int ? TRUE : FALSE;

  recursion_level = args[6].value.pdb_int;
  if (recursion_level <= 0)
    success = FALSE;

  clip_result = args[7].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          gint x, y, width, height;

          if (gimp_drawable_mask_intersect (drawable, &x, &y, &width, &height))
            {
              GimpMatrix3  matrix;

              /* Assemble the transformation matrix */
              gimp_transform_matrix_shear (x, y, x + width, y + height,
                                           shear_type, magnitude,
                                           &matrix);

              if (progress)
                gimp_progress_start (progress, _("Shearing..."), FALSE);

              /* Shear the selection */
              success = gimp_drawable_transform_affine (drawable, context,
                                                        &matrix,
                                                        transform_direction,
                                                        interpolation, supersample,
                                                        recursion_level,
                                                        clip_result, progress);

              if (progress)
                gimp_progress_end (progress);
            }
        }
    }

  return_args = procedural_db_return_args (&drawable_transform_shear_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (drawable));

  return return_args;
}

static ProcArg drawable_transform_shear_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_INT32,
    "shear_type",
    "Type of shear: GIMP_ORIENTATION_HORIZONTAL (0) or GIMP_ORIENTATION_VERTICAL (1)"
  },
  {
    GIMP_PDB_FLOAT,
    "magnitude",
    "The magnitude of the shear"
  },
  {
    GIMP_PDB_INT32,
    "transform_direction",
    "Direction of Transformation: { GIMP_TRANSFORM_FORWARD (0), GIMP_TRANSFORM_BACKWARD (1) }"
  },
  {
    GIMP_PDB_INT32,
    "interpolation",
    "Type of interpolation: { GIMP_INTERPOLATION_NONE (0), GIMP_INTERPOLATION_LINEAR (1), GIMP_INTERPOLATION_CUBIC (2) }"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Whether to perform supersample"
  },
  {
    GIMP_PDB_INT32,
    "recursion_level",
    "Level of recursion (3 is a nice default)"
  },
  {
    GIMP_PDB_INT32,
    "clip_result",
    "Whether to clip results"
  }
};

static ProcArg drawable_transform_shear_outargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The sheared drawable"
  }
};

static ProcRecord drawable_transform_shear_proc =
{
  "gimp_drawable_transform_shear",
  "Shear the specified drawable about its center by the specified magnitude, with extra parameters.",
  "This tool shears the specified drawable if no selection exists. If a selection exists, the portion of the drawable which lies under the selection is cut from the drawable and made into a floating selection which is then sheard by the specified amount. The return value is the ID of the sheard drawable. If there was no selection, this will be equal to the drawable ID supplied as input. Otherwise, this will be the newly created and sheard drawable. The shear type parameter indicates whether the shear will be applied horizontally or vertically. The magnitude can be either positive or negative and indicates the extent (in pixels) to shear by.",
  "João S. O. Bueno Calligaris",
  "João S. O. Bueno Calligaris",
  "2004",
  NULL,
  GIMP_INTERNAL,
  8,
  drawable_transform_shear_inargs,
  1,
  drawable_transform_shear_outargs,
  { { drawable_transform_shear_invoker } }
};

static Argument *
drawable_transform_2d_invoker (Gimp         *gimp,
                               GimpContext  *context,
                               GimpProgress *progress,
                               Argument     *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gdouble source_x;
  gdouble source_y;
  gdouble scale_x;
  gdouble scale_y;
  gdouble angle;
  gdouble dest_x;
  gdouble dest_y;
  gint32 transform_direction = 0;
  gint32 interpolation = 0;
  gboolean supersample = FALSE;
  gint32 recursion_level = 0;
  gboolean clip_result = FALSE;

  drawable = (GimpDrawable *) gimp_item_get_by_ID (gimp, args[0].value.pdb_int);
  if (! (GIMP_IS_DRAWABLE (drawable) && ! gimp_item_is_removed (GIMP_ITEM (drawable))))
    success = FALSE;

  source_x = args[1].value.pdb_float;

  source_y = args[2].value.pdb_float;

  scale_x = args[3].value.pdb_float;

  scale_y = args[4].value.pdb_float;

  angle = args[5].value.pdb_float;

  dest_x = args[6].value.pdb_float;

  dest_y = args[7].value.pdb_float;

  transform_direction = args[8].value.pdb_int;
  if (transform_direction < GIMP_TRANSFORM_FORWARD || transform_direction > GIMP_TRANSFORM_BACKWARD)
    success = FALSE;

  interpolation = args[9].value.pdb_int;
  if (interpolation < GIMP_INTERPOLATION_NONE || interpolation > GIMP_INTERPOLATION_CUBIC)
    success = FALSE;

  supersample = args[10].value.pdb_int ? TRUE : FALSE;

  recursion_level = args[11].value.pdb_int;
  if (recursion_level <= 0)
    success = FALSE;

  clip_result = args[12].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      success = gimp_item_is_attached (GIMP_ITEM (drawable));

      if (success)
        {
          GimpMatrix3  matrix;

          /* Assemble the transformation matrix */
          gimp_matrix3_identity  (&matrix);
          gimp_matrix3_translate (&matrix, -source_x, -source_y);
          gimp_matrix3_scale     (&matrix, scale_x, scale_y);
          gimp_matrix3_rotate    (&matrix, angle);
          gimp_matrix3_translate (&matrix, dest_x, dest_y);

          if (progress)
            gimp_progress_start (progress, _("2D Transform..."), FALSE);

          /* Transform the selection */
          success = gimp_drawable_transform_affine (drawable, context,
                                                    &matrix, transform_direction,
                                                    interpolation, supersample,
                                                    recursion_level,
                                                    clip_result, progress);

          if (progress)
            gimp_progress_end (progress);
        }
    }

  return_args = procedural_db_return_args (&drawable_transform_2d_proc, success);

  if (success)
    return_args[1].value.pdb_int = gimp_item_get_ID (GIMP_ITEM (drawable));

  return return_args;
}

static ProcArg drawable_transform_2d_inargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The affected drawable"
  },
  {
    GIMP_PDB_FLOAT,
    "source_x",
    "X coordinate of the transformation center"
  },
  {
    GIMP_PDB_FLOAT,
    "source_y",
    "Y coordinate of the transformation center"
  },
  {
    GIMP_PDB_FLOAT,
    "scale_x",
    "Amount to scale in x direction"
  },
  {
    GIMP_PDB_FLOAT,
    "scale_y",
    "Amount to scale in y direction"
  },
  {
    GIMP_PDB_FLOAT,
    "angle",
    "The angle of rotation (radians)"
  },
  {
    GIMP_PDB_FLOAT,
    "dest_x",
    "X coordinate of where the center goes"
  },
  {
    GIMP_PDB_FLOAT,
    "dest_y",
    "Y coordinate of where the center goes"
  },
  {
    GIMP_PDB_INT32,
    "transform_direction",
    "Direction of Transformation: { GIMP_TRANSFORM_FORWARD (0), GIMP_TRANSFORM_BACKWARD (1) }"
  },
  {
    GIMP_PDB_INT32,
    "interpolation",
    "Type of interpolation: { GIMP_INTERPOLATION_NONE (0), GIMP_INTERPOLATION_LINEAR (1), GIMP_INTERPOLATION_CUBIC (2) }"
  },
  {
    GIMP_PDB_INT32,
    "supersample",
    "Whether to perform supersample"
  },
  {
    GIMP_PDB_INT32,
    "recursion_level",
    "Level of recursion (3 is a nice default)"
  },
  {
    GIMP_PDB_INT32,
    "clip_result",
    "Whether to clip results"
  }
};

static ProcArg drawable_transform_2d_outargs[] =
{
  {
    GIMP_PDB_DRAWABLE,
    "drawable",
    "The transformed drawable"
  }
};

static ProcRecord drawable_transform_2d_proc =
{
  "gimp_drawable_transform_2d",
  "Transform the specified drawable in 2d, with extra parameters.",
  "This tool transforms the specified drawable if no selection exists. If a selection exists, the portion of the drawable which lies under the selection is cut from the drawable and made into a floating selection which is then transformed. The transformation is done by scaling the image by the x and y scale factors about the point (source_x, source_y), then rotating around the same point, then translating that point to the new position (dest_x, dest_y). The return value is the ID of the rotated drawable. If there was no selection, this will be equal to the drawable ID supplied as input. Otherwise, this will be the newly created and transformed drawable.",
  "João S. O. Bueno Calligaris",
  "João S. O. Bueno Calligaris",
  "2004",
  NULL,
  GIMP_INTERNAL,
  13,
  drawable_transform_2d_inargs,
  1,
  drawable_transform_2d_outargs,
  { { drawable_transform_2d_invoker } }
};
