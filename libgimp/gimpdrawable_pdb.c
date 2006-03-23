/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2003 Peter Mattis and Spencer Kimball
 *
 * gimpdrawable_pdb.c
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/* NOTE: This file is autogenerated by pdbgen.pl */

#include "config.h"

#include <string.h>

#include "gimp.h"

/**
 * gimp_drawable_delete:
 * @drawable_ID: The drawable to delete.
 *
 * Delete a drawable.
 *
 * This procedure deletes the specified drawable. This must not be done
 * if the image containing this drawable was already deleted or if the
 * drawable was already removed from the image. The only case in which
 * this procedure is useful is if you want to get rid of a drawable
 * which has not yet been added to an image.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_delete (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-delete",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_is_layer:
 * @drawable_ID: The drawable.
 *
 * Returns whether the drawable is a layer.
 *
 * This procedure returns non-zero if the specified drawable is a
 * layer.
 *
 * Returns: Non-zero if the drawable is a layer.
 */
gboolean
gimp_drawable_is_layer (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean layer = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-is-layer",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    layer = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return layer;
}

/**
 * gimp_drawable_is_layer_mask:
 * @drawable_ID: The drawable.
 *
 * Returns whether the drawable is a layer mask.
 *
 * This procedure returns non-zero if the specified drawable is a layer
 * mask.
 *
 * Returns: Non-zero if the drawable is a layer mask.
 */
gboolean
gimp_drawable_is_layer_mask (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean layer_mask = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-is-layer-mask",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    layer_mask = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return layer_mask;
}

/**
 * gimp_drawable_is_channel:
 * @drawable_ID: The drawable.
 *
 * Returns whether the drawable is a channel.
 *
 * This procedure returns non-zero if the specified drawable is a
 * channel.
 *
 * Returns: Non-zero if the drawable is a channel.
 */
gboolean
gimp_drawable_is_channel (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean channel = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-is-channel",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    channel = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return channel;
}

/**
 * gimp_drawable_type:
 * @drawable_ID: The drawable.
 *
 * Returns the drawable's type.
 *
 * This procedure returns the drawable's type.
 *
 * Returns: The drawable's type.
 */
GimpImageType
gimp_drawable_type (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  GimpImageType type = 0;

  return_vals = gimp_run_procedure ("gimp-drawable-type",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    type = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return type;
}

/**
 * gimp_drawable_type_with_alpha:
 * @drawable_ID: The drawable.
 *
 * Returns the drawable's type with alpha.
 *
 * This procedure returns the drawable's type as if had an alpha
 * channel. If the type is currently Gray, for instance, the returned
 * type would be GrayA. If the drawable already has an alpha channel,
 * the drawable's type is simply returned.
 *
 * Returns: The drawable's type with alpha.
 */
GimpImageType
gimp_drawable_type_with_alpha (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  GimpImageType type_with_alpha = 0;

  return_vals = gimp_run_procedure ("gimp-drawable-type-with-alpha",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    type_with_alpha = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return type_with_alpha;
}

/**
 * gimp_drawable_has_alpha:
 * @drawable_ID: The drawable.
 *
 * Returns non-zero if the drawable has an alpha channel.
 *
 * This procedure returns whether the specified drawable has an alpha
 * channel. This can only be true for layers, and the associated type
 * will be one of: { RGBA , GRAYA, INDEXEDA }.
 *
 * Returns: Does the drawable have an alpha channel?
 */
gboolean
gimp_drawable_has_alpha (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean has_alpha = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-has-alpha",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    has_alpha = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return has_alpha;
}

/**
 * gimp_drawable_is_rgb:
 * @drawable_ID: The drawable.
 *
 * Returns whether the drawable is an RGB type.
 *
 * This procedure returns non-zero if the specified drawable is of type
 * { RGB, RGBA }.
 *
 * Returns: non-zero if the drawable is an RGB type.
 */
gboolean
gimp_drawable_is_rgb (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean is_rgb = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-is-rgb",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    is_rgb = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return is_rgb;
}

/**
 * gimp_drawable_is_gray:
 * @drawable_ID: The drawable.
 *
 * Returns whether the drawable is a grayscale type.
 *
 * This procedure returns non-zero if the specified drawable is of type
 * { Gray, GrayA }.
 *
 * Returns: non-zero if the drawable is a grayscale type.
 */
gboolean
gimp_drawable_is_gray (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean is_gray = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-is-gray",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    is_gray = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return is_gray;
}

/**
 * gimp_drawable_is_indexed:
 * @drawable_ID: The drawable.
 *
 * Returns whether the drawable is an indexed type.
 *
 * This procedure returns non-zero if the specified drawable is of type
 * { Indexed, IndexedA }.
 *
 * Returns: non-zero if the drawable is an indexed type.
 */
gboolean
gimp_drawable_is_indexed (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean is_indexed = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-is-indexed",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    is_indexed = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return is_indexed;
}

/**
 * gimp_drawable_bpp:
 * @drawable_ID: The drawable.
 *
 * Returns the bytes per pixel.
 *
 * This procedure returns the number of bytes per pixel (or the number
 * of channels) for the specified drawable.
 *
 * Returns: Bytes per pixel.
 */
gint
gimp_drawable_bpp (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint bpp = 0;

  return_vals = gimp_run_procedure ("gimp-drawable-bpp",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    bpp = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return bpp;
}

/**
 * gimp_drawable_width:
 * @drawable_ID: The drawable.
 *
 * Returns the width of the drawable.
 *
 * This procedure returns the specified drawable's width in pixels.
 *
 * Returns: Width of drawable.
 */
gint
gimp_drawable_width (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint width = 0;

  return_vals = gimp_run_procedure ("gimp-drawable-width",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    width = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return width;
}

/**
 * gimp_drawable_height:
 * @drawable_ID: The drawable.
 *
 * Returns the height of the drawable.
 *
 * This procedure returns the specified drawable's height in pixels.
 *
 * Returns: Height of drawable.
 */
gint
gimp_drawable_height (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint height = 0;

  return_vals = gimp_run_procedure ("gimp-drawable-height",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    height = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return height;
}

/**
 * gimp_drawable_offsets:
 * @drawable_ID: The drawable.
 * @offset_x: x offset of drawable.
 * @offset_y: y offset of drawable.
 *
 * Returns the offsets for the drawable.
 *
 * This procedure returns the specified drawable's offsets. This only
 * makes sense if the drawable is a layer since channels are anchored.
 * The offsets of a channel will be returned as 0.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_offsets (gint32  drawable_ID,
		       gint   *offset_x,
		       gint   *offset_y)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-offsets",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  *offset_x = 0;
  *offset_y = 0;

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  if (success)
    {
      *offset_x = return_vals[1].data.d_int32;
      *offset_y = return_vals[2].data.d_int32;
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_get_image:
 * @drawable_ID: The drawable.
 *
 * Returns the drawable's image.
 *
 * This procedure returns the drawable's image.
 *
 * Returns: The drawable's image.
 */
gint32
gimp_drawable_get_image (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint32 image_ID = -1;

  return_vals = gimp_run_procedure ("gimp-drawable-get-image",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    image_ID = return_vals[1].data.d_image;

  gimp_destroy_params (return_vals, nreturn_vals);

  return image_ID;
}

/**
 * gimp_drawable_set_image:
 * @drawable_ID: The drawable.
 * @image_ID: The image.
 *
 * This procedure is deprecated!
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_set_image (gint32 drawable_ID,
			 gint32 image_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-set-image",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_IMAGE, image_ID,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_get_name:
 * @drawable_ID: The drawable.
 *
 * Get the name of the specified drawable.
 *
 * This procedure returns the specified drawable's name.
 *
 * Returns: The drawable name.
 */
gchar *
gimp_drawable_get_name (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gchar *name = NULL;

  return_vals = gimp_run_procedure ("gimp-drawable-get-name",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    name = g_strdup (return_vals[1].data.d_string);

  gimp_destroy_params (return_vals, nreturn_vals);

  return name;
}

/**
 * gimp_drawable_set_name:
 * @drawable_ID: The drawable.
 * @name: The new drawable name.
 *
 * Set the name of the specified drawable.
 *
 * This procedure sets the specified drawable's name.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_set_name (gint32       drawable_ID,
			const gchar *name)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-set-name",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_STRING, name,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_get_visible:
 * @drawable_ID: The drawable.
 *
 * Get the visibility of the specified drawable.
 *
 * This procedure returns the specified drawable's visibility.
 *
 * Returns: The drawable visibility.
 */
gboolean
gimp_drawable_get_visible (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean visible = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-get-visible",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    visible = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return visible;
}

/**
 * gimp_drawable_set_visible:
 * @drawable_ID: The drawable.
 * @visible: The new drawable visibility.
 *
 * Set the visibility of the specified drawable.
 *
 * This procedure sets the specified drawable's visibility.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_set_visible (gint32   drawable_ID,
			   gboolean visible)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-set-visible",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, visible,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_get_linked:
 * @drawable_ID: The drawable.
 *
 * Get the linked state of the specified drawable.
 *
 * This procedure returns the specified drawable's linked state.
 *
 * Returns: The drawable linked state (for moves).
 */
gboolean
gimp_drawable_get_linked (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean linked = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-get-linked",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    linked = return_vals[1].data.d_int32;

  gimp_destroy_params (return_vals, nreturn_vals);

  return linked;
}

/**
 * gimp_drawable_set_linked:
 * @drawable_ID: The drawable.
 * @linked: The new drawable linked state.
 *
 * Set the linked state of the specified drawable.
 *
 * This procedure sets the specified drawable's linked state.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_set_linked (gint32   drawable_ID,
			  gboolean linked)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-set-linked",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, linked,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_get_tattoo:
 * @drawable_ID: The drawable.
 *
 * Get the tattoo of the specified drawable.
 *
 * This procedure returns the specified drawable's tattoo. A tattoo is
 * a unique and permanent identifier attached to a drawable that can be
 * used to uniquely identify a drawable within an image even between
 * sessions.
 *
 * Returns: The drawable tattoo.
 */
gint
gimp_drawable_get_tattoo (gint32 drawable_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gint tattoo = 0;

  return_vals = gimp_run_procedure ("gimp-drawable-get-tattoo",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    tattoo = return_vals[1].data.d_tattoo;

  gimp_destroy_params (return_vals, nreturn_vals);

  return tattoo;
}

/**
 * gimp_drawable_set_tattoo:
 * @drawable_ID: The drawable.
 * @tattoo: The new drawable tattoo.
 *
 * Set the tattoo of the specified drawable.
 *
 * This procedure sets the specified drawable's tattoo. A tattoo is a
 * unique and permanent identifier attached to a drawable that can be
 * used to uniquely identify a drawable within an image even between
 * sessions.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_set_tattoo (gint32 drawable_ID,
			  gint   tattoo)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-set-tattoo",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, tattoo,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_mask_bounds:
 * @drawable_ID: The drawable.
 * @x1: x coordinate of the upper left corner of selection bounds.
 * @y1: y coordinate of the upper left corner of selection bounds.
 * @x2: x coordinate of the lower right corner of selection bounds.
 * @y2: y coordinate of the lower right corner of selection bounds.
 *
 * Find the bounding box of the current selection in relation to the
 * specified drawable.
 *
 * This procedure returns whether there is a selection. If there is
 * one, the upper left and lower righthand corners of its bounding box
 * are returned. These coordinates are specified relative to the
 * drawable's origin, and bounded by the drawable's extents. Please
 * note that the pixel specified by the lower righthand coordinate of
 * the bounding box is not part of the selection. The selection ends at
 * the upper left corner of this pixel. This means the width of the
 * selection can be calculated as (x2 - x1), its height as (y2 - y1).
 * Note that the returned boolean does NOT correspond with the returned
 * region being empty or not, it always returns whether the selection
 * is non_empty. See gimp_drawable_mask_intersect() for a boolean
 * return value which is more useful in most cases.
 *
 * Returns: TRUE if there is a selection.
 */
gboolean
gimp_drawable_mask_bounds (gint32  drawable_ID,
			   gint   *x1,
			   gint   *y1,
			   gint   *x2,
			   gint   *y2)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean non_empty = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-mask-bounds",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      non_empty = return_vals[1].data.d_int32;
      *x1 = return_vals[2].data.d_int32;
      *y1 = return_vals[3].data.d_int32;
      *x2 = return_vals[4].data.d_int32;
      *y2 = return_vals[5].data.d_int32;
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return non_empty;
}

/**
 * gimp_drawable_mask_intersect:
 * @drawable_ID: The drawable.
 * @x: x coordinate of the upper left corner of the intersection.
 * @y: y coordinate of the upper left corner of the intersection.
 * @width: width of the intersection.
 * @height: height of the intersection.
 *
 * Find the bounding box of the current selection in relation to the
 * specified drawable.
 *
 * This procedure returns whether there is an intersection between the
 * drawable and the selection. Unlike gimp_drawable_mask_bounds(), the
 * intersection's bounds are returned as x, y, width, height. If there
 * is no selection this function returns TRUE and the returned bounds
 * are the extents of the whole drawable.
 *
 * Returns: TRUE if the returned area is not empty.
 *
 * Since: GIMP 2.2
 */
gboolean
gimp_drawable_mask_intersect (gint32  drawable_ID,
			      gint   *x,
			      gint   *y,
			      gint   *width,
			      gint   *height)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean non_empty = FALSE;

  return_vals = gimp_run_procedure ("gimp-drawable-mask-intersect",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_END);

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      non_empty = return_vals[1].data.d_int32;
      *x = return_vals[2].data.d_int32;
      *y = return_vals[3].data.d_int32;
      *width = return_vals[4].data.d_int32;
      *height = return_vals[5].data.d_int32;
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return non_empty;
}

/**
 * gimp_drawable_merge_shadow:
 * @drawable_ID: The drawable.
 * @undo: Push merge to undo stack?
 *
 * Merge the shadow buffer with the specified drawable.
 *
 * This procedure combines the contents of the image's shadow buffer
 * (for temporary processing) with the specified drawable. The \"undo\"
 * parameter specifies whether to add an undo step for the operation.
 * Requesting no undo is useful for such applications as 'auto-apply'.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_merge_shadow (gint32   drawable_ID,
			    gboolean undo)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-merge-shadow",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, undo,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_update:
 * @drawable_ID: The drawable.
 * @x: x coordinate of upper left corner of update region.
 * @y: y coordinate of upper left corner of update region.
 * @width: Width of update region.
 * @height: Height of update region.
 *
 * Update the specified region of the drawable.
 *
 * This procedure updates the specified region of the drawable. The (x,
 * y) coordinate pair is relative to the drawable's origin, not to the
 * image origin. Therefore, the entire drawable can be updated using
 * (0, 0, width, height).
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_update (gint32 drawable_ID,
		      gint   x,
		      gint   y,
		      gint   width,
		      gint   height)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-update",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, x,
				    GIMP_PDB_INT32, y,
				    GIMP_PDB_INT32, width,
				    GIMP_PDB_INT32, height,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_get_pixel:
 * @drawable_ID: The drawable.
 * @x_coord: The x coordinate.
 * @y_coord: The y coordinate.
 * @num_channels: The number of channels for the pixel.
 *
 * Gets the value of the pixel at the specified coordinates.
 *
 * This procedure gets the pixel value at the specified coordinates.
 * The 'num_channels' argument must always be equal to the
 * bytes-per-pixel value for the specified drawable.
 *
 * Returns: The pixel value.
 */
guint8 *
gimp_drawable_get_pixel (gint32  drawable_ID,
			 gint    x_coord,
			 gint    y_coord,
			 gint   *num_channels)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  guint8 *pixel = NULL;

  return_vals = gimp_run_procedure ("gimp-drawable-get-pixel",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, x_coord,
				    GIMP_PDB_INT32, y_coord,
				    GIMP_PDB_END);

  *num_channels = 0;

  if (return_vals[0].data.d_status == GIMP_PDB_SUCCESS)
    {
      *num_channels = return_vals[1].data.d_int32;
      pixel = g_new (guint8, *num_channels);
      memcpy (pixel, return_vals[2].data.d_int8array,
	      *num_channels * sizeof (guint8));
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return pixel;
}

/**
 * gimp_drawable_set_pixel:
 * @drawable_ID: The drawable.
 * @x_coord: The x coordinate.
 * @y_coord: The y coordinate.
 * @num_channels: The number of channels for the pixel.
 * @pixel: The pixel value.
 *
 * Sets the value of the pixel at the specified coordinates.
 *
 * This procedure sets the pixel value at the specified coordinates.
 * The 'num_channels' argument must always be equal to the
 * bytes-per-pixel value for the specified drawable. Note that this
 * function is not undoable, you should use it only on drawables you
 * just created yourself.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_set_pixel (gint32        drawable_ID,
			 gint          x_coord,
			 gint          y_coord,
			 gint          num_channels,
			 const guint8 *pixel)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-set-pixel",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, x_coord,
				    GIMP_PDB_INT32, y_coord,
				    GIMP_PDB_INT32, num_channels,
				    GIMP_PDB_INT8ARRAY, pixel,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_fill:
 * @drawable_ID: The drawable.
 * @fill_type: The type of fill.
 *
 * Fill the drawable with the specified fill mode.
 *
 * This procedure fills the drawable with the fill mode. If the fill
 * mode is foreground the current foreground color is used. If the fill
 * mode is background, the current background color is used. If the
 * fill type is white, then white is used. Transparent fill only
 * affects layers with an alpha channel, in which case the alpha
 * channel is set to transparent. If the drawable has no alpha channel,
 * it is filled to white. No fill leaves the drawable's contents
 * undefined. This procedure is unlike the bucket fill tool because it
 * fills regardless of a selection
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_fill (gint32       drawable_ID,
		    GimpFillType fill_type)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-fill",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, fill_type,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_offset:
 * @drawable_ID: The drawable to offset.
 * @wrap_around: wrap image around or fill vacated regions.
 * @fill_type: fill vacated regions of drawable with background or transparent.
 * @offset_x: offset by this amount in X direction.
 * @offset_y: offset by this amount in Y direction.
 *
 * Offset the drawable by the specified amounts in the X and Y
 * directions
 *
 * This procedure offsets the specified drawable by the amounts
 * specified by 'offset_x' and 'offset_y'. If 'wrap_around' is set to
 * TRUE, then portions of the drawable which are offset out of bounds
 * are wrapped around. Alternatively, the undefined regions of the
 * drawable can be filled with transparency or the background color, as
 * specified by the 'fill_type' parameter.
 *
 * Returns: TRUE on success.
 */
gboolean
gimp_drawable_offset (gint32         drawable_ID,
		      gboolean       wrap_around,
		      GimpOffsetType fill_type,
		      gint           offset_x,
		      gint           offset_y)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-offset",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, wrap_around,
				    GIMP_PDB_INT32, fill_type,
				    GIMP_PDB_INT32, offset_x,
				    GIMP_PDB_INT32, offset_y,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * _gimp_drawable_thumbnail:
 * @drawable_ID: The drawable.
 * @width: The requested thumbnail width.
 * @height: The requested thumbnail height.
 * @actual_width: The previews width.
 * @actual_height: The previews height.
 * @bpp: The previews bpp.
 * @thumbnail_data_count: The number of bytes in thumbnail data.
 * @thumbnail_data: The thumbnail data.
 *
 * Get a thumbnail of a drawable.
 *
 * This function gets data from which a thumbnail of a drawable preview
 * can be created. Maximum x or y dimension is 512 pixels. The pixels
 * are returned in RGB[A] or GRAY[A] format. The bpp return value gives
 * the number of bytes in the image.
 *
 * Returns: TRUE on success.
 */
gboolean
_gimp_drawable_thumbnail (gint32   drawable_ID,
			  gint     width,
			  gint     height,
			  gint    *actual_width,
			  gint    *actual_height,
			  gint    *bpp,
			  gint    *thumbnail_data_count,
			  guint8 **thumbnail_data)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-thumbnail",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, width,
				    GIMP_PDB_INT32, height,
				    GIMP_PDB_END);

  *actual_width = 0;
  *actual_height = 0;
  *bpp = 0;
  *thumbnail_data_count = 0;
  *thumbnail_data = NULL;

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  if (success)
    {
      *actual_width = return_vals[1].data.d_int32;
      *actual_height = return_vals[2].data.d_int32;
      *bpp = return_vals[3].data.d_int32;
      *thumbnail_data_count = return_vals[4].data.d_int32;
      *thumbnail_data = g_new (guint8, *thumbnail_data_count);
      memcpy (*thumbnail_data, return_vals[5].data.d_int8array,
	      *thumbnail_data_count * sizeof (guint8));
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * _gimp_drawable_sub_thumbnail:
 * @drawable_ID: The drawable.
 * @src_x: The x coordinate of the area.
 * @src_y: The y coordinate of the area.
 * @src_width: The width of the area.
 * @src_height: The height of the area.
 * @dest_width: The thumbnail width.
 * @dest_height: The thumbnail height.
 * @width: The previews width.
 * @height: The previews height.
 * @bpp: The previews bpp.
 * @thumbnail_data_count: The number of bytes in thumbnail data.
 * @thumbnail_data: The thumbnail data.
 *
 * Get a thumbnail of a sub-area of a drawable drawable.
 *
 * This function gets data from which a thumbnail of a drawable preview
 * can be created. Maximum x or y dimension is 512 pixels. The pixels
 * are returned in RGB[A] or GRAY[A] format. The bpp return value gives
 * the number of bytes in the image.
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.2
 */
gboolean
_gimp_drawable_sub_thumbnail (gint32   drawable_ID,
			      gint     src_x,
			      gint     src_y,
			      gint     src_width,
			      gint     src_height,
			      gint     dest_width,
			      gint     dest_height,
			      gint    *width,
			      gint    *height,
			      gint    *bpp,
			      gint    *thumbnail_data_count,
			      guint8 **thumbnail_data)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-sub-thumbnail",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, src_x,
				    GIMP_PDB_INT32, src_y,
				    GIMP_PDB_INT32, src_width,
				    GIMP_PDB_INT32, src_height,
				    GIMP_PDB_INT32, dest_width,
				    GIMP_PDB_INT32, dest_height,
				    GIMP_PDB_END);

  *width = 0;
  *height = 0;
  *bpp = 0;
  *thumbnail_data_count = 0;
  *thumbnail_data = NULL;

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  if (success)
    {
      *width = return_vals[1].data.d_int32;
      *height = return_vals[2].data.d_int32;
      *bpp = return_vals[3].data.d_int32;
      *thumbnail_data_count = return_vals[4].data.d_int32;
      *thumbnail_data = g_new (guint8, *thumbnail_data_count);
      memcpy (*thumbnail_data, return_vals[5].data.d_int8array,
	      *thumbnail_data_count * sizeof (guint8));
    }

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}

/**
 * gimp_drawable_foreground_extract:
 * @drawable_ID: The drawable.
 * @mode: The algorithm to use.
 * @mask_ID: Tri-Map.
 *
 * Extract the foreground of a drawable using a given trimap.
 *
 * Image Segmentation by Uniform Color Clustering, see
 * http://www.inf.fu-berlin.de/inst/pubs/tr-b-05-07.pdf
 *
 * Returns: TRUE on success.
 *
 * Since: GIMP 2.4
 */
gboolean
gimp_drawable_foreground_extract (gint32                    drawable_ID,
				  GimpForegroundExtractMode mode,
				  gint32                    mask_ID)
{
  GimpParam *return_vals;
  gint nreturn_vals;
  gboolean success = TRUE;

  return_vals = gimp_run_procedure ("gimp-drawable-foreground-extract",
				    &nreturn_vals,
				    GIMP_PDB_DRAWABLE, drawable_ID,
				    GIMP_PDB_INT32, mode,
				    GIMP_PDB_DRAWABLE, mask_ID,
				    GIMP_PDB_END);

  success = return_vals[0].data.d_status == GIMP_PDB_SUCCESS;

  gimp_destroy_params (return_vals, nreturn_vals);

  return success;
}
