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

#include <stdio.h>
#include <string.h> /* strcmp, memcmp */

#include <glib-object.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpcolor/gimpcolor.h"

#include "core/core-types.h"

#include "base/tile.h"
#include "base/tile-manager.h"
#include "base/tile-manager-private.h"

#include "config/gimpcoreconfig.h"

#include "core/gimp.h"
#include "core/gimpchannel.h"
#include "core/gimpcontainer.h"
#include "core/gimpdrawable.h"
#include "core/gimpimage.h"
#include "core/gimpimage-guides.h"
#include "core/gimplayer.h"
#include "core/gimplayer-floating-sel.h"
#include "core/gimplayermask.h"
#include "core/gimpparasitelist.h"
#include "core/gimpunit.h"

#include "vectors/gimpbezierstroke.h"
#include "vectors/gimpvectors.h"

#include "xcf-private.h"
#include "xcf-load.h"
#include "xcf-read.h"
#include "xcf-seek.h"

#include "gimp-intl.h"


typedef struct _XcfPathPoint PathPoint;

struct _XcfPathPoint
{
  guint32 type;
  gdouble x;
  gdouble y;
};


static gboolean        xcf_load_image_props     (XcfInfo     *info,
                                                 GimpImage   *gimage);
static gboolean        xcf_load_layer_props     (XcfInfo     *info,
                                                 GimpImage   *gimage,
                                                 GimpLayer   *layer,
                                                 gboolean    *apply_mask,
                                                 gboolean    *edit_mask,
                                                 gboolean    *show_mask);
static gboolean        xcf_load_channel_props   (XcfInfo     *info,
                                                 GimpImage   *gimage,
                                                 GimpChannel *channel);
static gboolean        xcf_load_prop            (XcfInfo     *info,
                                                 PropType    *prop_type,
                                                 guint32     *prop_size);
static GimpLayer     * xcf_load_layer           (XcfInfo     *info,
                                                 GimpImage   *gimage);
static GimpChannel   * xcf_load_channel         (XcfInfo     *info,
                                                 GimpImage   *gimage);
static GimpLayerMask * xcf_load_layer_mask      (XcfInfo     *info,
                                                 GimpImage   *gimage);
static gboolean        xcf_load_hierarchy       (XcfInfo     *info,
                                                 TileManager *tiles);
static gboolean        xcf_load_level           (XcfInfo     *info,
                                                 TileManager *tiles);
static gboolean        xcf_load_tile            (XcfInfo     *info,
                                                 Tile        *tile);
static gboolean        xcf_load_tile_rle        (XcfInfo     *info,
                                                 Tile        *tile,
                                                 gint         data_length);
static GimpParasite  * xcf_load_parasite        (XcfInfo     *info);
static gboolean        xcf_load_old_paths       (XcfInfo     *info,
                                                 GimpImage   *gimage);
static gboolean        xcf_load_old_path        (XcfInfo     *info,
                                                 GimpImage   *gimage);
static PathPoint     * xcf_load_old_path_point1 (XcfInfo    *info,
                                                 GimpCoords *coords);
static PathPoint     * xcf_load_old_path_point  (XcfInfo    *info,
                                                 GimpCoords *coords);


#ifdef SWAP_FROM_FILE
static gboolean        xcf_swap_func          (gint         fd,
					       Tile        *tile,
					       gint         cmd,
					       gpointer     user_data);
#endif


GimpImage *
xcf_load_image (Gimp    *gimp,
		XcfInfo *info)
{
  GimpImage   *gimage;
  GimpLayer   *layer;
  GimpChannel *channel;
  guint32      saved_pos;
  guint32      offset;
  gint         width;
  gint         height;
  gint         image_type;
  gint         num_successful_elements = 0;

  /* read in the image width, height and type */
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &width, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &height, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &image_type, 1);

  gimage = gimp_create_image (gimp,
			      width, height,
			      image_type,
			      FALSE);

  /* read the image properties */
  if (!xcf_load_image_props (info, gimage))
    goto hard_error;

  while (TRUE)
    {
      /* read in the offset of the next layer */
      info->cp += xcf_read_int32 (info->fp, &offset, 1);

      /* if the offset is 0 then we are at the end
       *  of the layer list.
       */
      if (offset == 0)
	break;

      /* save the current position as it is where the
       *  next layer offset is stored.
       */
      saved_pos = info->cp;

      /* seek to the layer offset */
      if (! xcf_seek_pos (info, offset, NULL))
        goto error;

      /* read in the layer */
      layer = xcf_load_layer (info, gimage);
      if (!layer)
	goto error;

      num_successful_elements++;

      /* add the layer to the image if its not the floating selection */
      if (layer != info->floating_sel)
	gimp_image_add_layer (gimage, layer,
			      gimp_container_num_children (gimage->layers));

      /* restore the saved position so we'll be ready to
       *  read the next offset.
       */
      if (! xcf_seek_pos (info, saved_pos, NULL))
        goto error;
    }

  while (TRUE)
    {
      /* read in the offset of the next channel */
      info->cp += xcf_read_int32 (info->fp, &offset, 1);

      /* if the offset is 0 then we are at the end
       *  of the channel list.
       */
      if (offset == 0)
	break;

      /* save the current position as it is where the
       *  next channel offset is stored.
       */
      saved_pos = info->cp;

      /* seek to the channel offset */
      if (! xcf_seek_pos (info, offset, NULL))
        goto error;

      /* read in the layer */
      channel = xcf_load_channel (info, gimage);
      if (!channel)
	goto error;

      num_successful_elements++;

      /* add the channel to the image if its not the selection */
      if (channel != gimage->selection_mask)
	gimp_image_add_channel (gimage, channel, -1);

      /* restore the saved position so we'll be ready to
       *  read the next offset.
       */
      if (! xcf_seek_pos (info, saved_pos, NULL))
        goto error;
    }

  if (info->active_layer)
    gimp_image_set_active_layer (gimage, info->active_layer);

  if (info->active_channel)
    gimp_image_set_active_channel (gimage, info->active_channel);

  gimp_image_set_filename (gimage, info->filename);

  return gimage;

 error:
  if (num_successful_elements == 0)
    goto hard_error;

  g_message ("XCF: This file is corrupt!  I have loaded as much\n"
	     "of it as I can, but it is incomplete.");

  return gimage;

 hard_error:  
  g_message ("XCF: This file is corrupt!  I could not even\n"
	     "salvage any partial image data from it.");

  g_object_unref (gimage);

  return NULL;
}

static gboolean
xcf_load_image_props (XcfInfo   *info,
		      GimpImage *gimage)
{
  PropType prop_type;
  guint32  prop_size;

  while (TRUE)
    {
      if (!xcf_load_prop (info, &prop_type, &prop_size))
	return FALSE;

      switch (prop_type)
	{
	case PROP_END:
	  return TRUE;

	case PROP_COLORMAP:
	  if (info->file_version == 0) 
	    {
	      gint i;

	      g_message (_("XCF warning: version 0 of XCF file format\n"
			   "did not save indexed colormaps correctly.\n"
			   "Substituting grayscale map."));
	      info->cp += 
                xcf_read_int32 (info->fp, (guint32 *) &gimage->num_cols, 1);
	      gimage->cmap = g_new (guchar, gimage->num_cols * 3);
	      if (!xcf_seek_pos (info, info->cp + gimage->num_cols, NULL))
	        return FALSE;

	      for (i = 0; i<gimage->num_cols; i++) 
		{
		  gimage->cmap[i*3+0] = i;
		  gimage->cmap[i*3+1] = i;
		  gimage->cmap[i*3+2] = i;
		}
	    }
	  else 
	    {
	      info->cp += 
                xcf_read_int32 (info->fp, (guint32 *) &gimage->num_cols, 1);
	      gimage->cmap = g_new (guchar, gimage->num_cols * 3);
	      info->cp += 
                xcf_read_int8 (info->fp, 
                               (guint8 *) gimage->cmap, gimage->num_cols * 3);
	    }
	  break;

	case PROP_COMPRESSION:
	  {
	    guint8 compression;

	    info->cp += xcf_read_int8 (info->fp, (guint8 *) &compression, 1);

	    if ((compression != COMPRESS_NONE) &&
		(compression != COMPRESS_RLE) &&
		(compression != COMPRESS_ZLIB) &&
		(compression != COMPRESS_FRACTAL))
	      {
		g_message ("unknown compression type: %d", (int) compression);
		return FALSE;
	      }

	    info->compression = compression;
	  }
	  break;

	case PROP_GUIDES:
	  {
	    gint32 position;
	    gint8  orientation;
	    gint   i, nguides;

	    nguides = prop_size / (4 + 1);
	    for (i = 0; i < nguides; i++)
	      {
		info->cp += xcf_read_int32 (info->fp, (guint32 *) &position, 1);
		info->cp += xcf_read_int8 (info->fp, (guint8 *) &orientation, 1);

                /*  skip -1 guides from old XCFs  */
                if (position < 0)
                  continue;

		switch (orientation)
		  {
		  case XCF_ORIENTATION_HORIZONTAL:
		    gimp_image_add_hguide (gimage, position, FALSE);
		    break;

		  case XCF_ORIENTATION_VERTICAL:
		    gimp_image_add_vguide (gimage, position, FALSE);
		    break;

		  default:
		    g_message ("guide orientation out of range in XCF file");
		    continue;
		  }
	      }

	    /*  this is silly as the order of guides doesn't really matter,
	     *  but it restores the list to it's original order, which
	     *  cannot be wrong  --Mitch
	     */
	    gimage->guides = g_list_reverse (gimage->guides);
	  }
	  break;

	case PROP_RESOLUTION:
	  {
	    gfloat xres, yres;
	    
	    info->cp += xcf_read_float (info->fp, &xres, 1);
	    info->cp += xcf_read_float (info->fp, &yres, 1);
	    if (xres < GIMP_MIN_RESOLUTION || xres > GIMP_MAX_RESOLUTION ||
		yres < GIMP_MIN_RESOLUTION || yres > GIMP_MAX_RESOLUTION)
	      {
		g_message ("Warning, resolution out of range in XCF file");
		xres = gimage->gimp->config->default_xresolution;
		yres = gimage->gimp->config->default_yresolution;
	      }
	    gimage->xresolution = xres;
	    gimage->yresolution = yres;
	  }
	  break;

	case PROP_TATTOO:
	  {
	    info->cp += xcf_read_int32 (info->fp, &gimage->tattoo_state, 1);
	  }
	  break;

	case PROP_PARASITES:
	  {
	    glong         base = info->cp;
	    GimpParasite *p;

	    while (info->cp - base < prop_size)
	      {
		p = xcf_load_parasite (info);
		gimp_image_parasite_attach (gimage, p);
		gimp_parasite_free (p);
	      }
	    if (info->cp - base != prop_size)
	      g_message ("Error detected while loading an image's parasites");
	  }
	  break;

	case PROP_UNIT:
	  {
	    guint32 unit;
	    
	    info->cp += xcf_read_int32 (info->fp, &unit, 1);
	    
	    if ((unit <= GIMP_UNIT_PIXEL) ||
		(unit >= _gimp_unit_get_number_of_built_in_units (gimage->gimp)))
	      {
		g_message ("Warning, unit out of range in XCF file, "
                           "falling back to inches");
		unit = GIMP_UNIT_INCH;
	      }
	    
	    gimage->unit = unit;
	  }
	  break;

	case PROP_PATHS:
          xcf_load_old_paths (info, gimage);
	  break;

	case PROP_USER_UNIT:
	  {
	    gchar    *unit_strings[5];
	    float     factor;
	    guint32   digits;
	    GimpUnit  unit;
	    gint      num_units;
	    gint      i;

	    info->cp += xcf_read_float (info->fp, &factor, 1);
	    info->cp += xcf_read_int32 (info->fp, &digits, 1);
	    info->cp += xcf_read_string (info->fp, unit_strings, 5);

	    for (i = 0; i < 5; i++)
	      if (unit_strings[i] == NULL)
		unit_strings[i] = g_strdup ("");

	    num_units = _gimp_unit_get_number_of_units (gimage->gimp);

	    for (unit = _gimp_unit_get_number_of_built_in_units (gimage->gimp);
		 unit < num_units; unit++)
	      {
		/* if the factor and the identifier match some unit
		 * in unitrc, use the unitrc unit
		 */
		if ((ABS (_gimp_unit_get_factor (gimage->gimp,
						 unit) - factor) < 1e-5) &&
		    (strcmp (unit_strings[0],
			     _gimp_unit_get_identifier (gimage->gimp,
							unit)) == 0))
		  {
		    break;
		  }
	      }
	    
	    /* no match */
	    if (unit == num_units)
	      unit = _gimp_unit_new (gimage->gimp,
				     unit_strings[0],
				     factor,
				     digits,
				     unit_strings[1],
				     unit_strings[2],
				     unit_strings[3],
				     unit_strings[4]);

	    gimage->unit = unit;

	    for (i = 0; i < 5; i++)
	      g_free (unit_strings[i]);
	  }
	 break;

	default:
	  g_message ("unexpected/unknown image property: %d (skipping)", 
                     prop_type);

	  {
	    guint8 buf[16];
	    guint  amount;

	    while (prop_size > 0)
	      {
		amount = MIN (16, prop_size);
		info->cp += xcf_read_int8 (info->fp, buf, amount);
		prop_size -= MIN (16, amount);
	      }
	  }
	  break;
	}
    }

  return FALSE;
}

static gboolean
xcf_load_layer_props (XcfInfo   *info,
		      GimpImage *gimage,
		      GimpLayer *layer,
                      gboolean  *apply_mask,
                      gboolean  *edit_mask,
                      gboolean  *show_mask)
{
  PropType prop_type;
  guint32  prop_size;

  while (TRUE)
    {
      if (!xcf_load_prop (info, &prop_type, &prop_size))
	return FALSE;

      switch (prop_type)
	{
	case PROP_END:
	  return TRUE;
	case PROP_ACTIVE_LAYER:
	  info->active_layer = layer;
	  break;
	case PROP_FLOATING_SELECTION:
	  info->floating_sel = layer;
	  info->cp += 
            xcf_read_int32 (info->fp, 
                            (guint32 *) &info->floating_sel_offset, 1);
	  break;
	case PROP_OPACITY:
          {
            guint32 opacity;

            info->cp += xcf_read_int32 (info->fp, &opacity, 1);
            layer->opacity = CLAMP ((gdouble) opacity / 255.0,
                                    GIMP_OPACITY_TRANSPARENT,
                                    GIMP_OPACITY_OPAQUE);
          }
	  break;
	case PROP_VISIBLE:
	  {
	    gboolean visible;

	    info->cp += xcf_read_int32 (info->fp, (guint32 *) &visible, 1);
	    gimp_drawable_set_visible (GIMP_DRAWABLE (layer),
				       visible ? TRUE : FALSE, FALSE);
	  }
	  break;
	case PROP_LINKED:
          {
            gboolean linked;

            info->cp += xcf_read_int32 (info->fp, (guint32 *) &linked, 1);
            gimp_item_set_linked (GIMP_ITEM (layer),
                                  linked ? TRUE : FALSE, FALSE);
          }
	  break;
	case PROP_PRESERVE_TRANSPARENCY:
	  info->cp += 
            xcf_read_int32 (info->fp, (guint32 *) &layer->preserve_trans, 1);
	  break;
	case PROP_APPLY_MASK:
	  info->cp += xcf_read_int32 (info->fp, (guint32 *) apply_mask, 1);
	  break;
	case PROP_EDIT_MASK:
	  info->cp += xcf_read_int32 (info->fp, (guint32 *) edit_mask, 1);
	  break;
	case PROP_SHOW_MASK:
	  info->cp += xcf_read_int32 (info->fp, (guint32 *) show_mask, 1);
	  break;
	case PROP_OFFSETS:
	  info->cp += 
            xcf_read_int32 (info->fp, 
                            (guint32 *) &GIMP_ITEM (layer)->offset_x, 1);
	  info->cp += 
            xcf_read_int32 (info->fp, 
                            (guint32 *) &GIMP_ITEM (layer)->offset_y, 1);
	  break;
	case PROP_MODE:
	  info->cp += xcf_read_int32 (info->fp, (guint32 *) &layer->mode, 1);
	  break;
	case PROP_TATTOO:
	  info->cp += xcf_read_int32 (info->fp,
				      (guint32 *) &GIMP_ITEM (layer)->tattoo,
				      1);
	  break;
	 case PROP_PARASITES:
           {
             glong         base = info->cp;
             GimpParasite *p;

             while (info->cp - base < prop_size)
               {
                 p = xcf_load_parasite (info);
                 gimp_item_parasite_attach (GIMP_ITEM (layer), p);
                 gimp_parasite_free (p);
               }
             if (info->cp - base != prop_size)
               g_message ("Error detected while loading a layer's parasites");
           }
	 break;
	default:
	  g_message ("unexpected/unknown layer property: %d (skipping)", 
                     prop_type);

	  {
	    guint8 buf[16];
	    guint  amount;

	    while (prop_size > 0)
	      {
		amount = MIN (16, prop_size);
		info->cp += xcf_read_int8 (info->fp, buf, amount);
		prop_size -= MIN (16, amount);
	      }
	  }
	  break;
	}
    }

  return FALSE;
}

static gboolean
xcf_load_channel_props (XcfInfo     *info,
			GimpImage   *gimage,
			GimpChannel *channel)
{
  PropType prop_type;
  guint32  prop_size;

  while (TRUE)
    {
      if (!xcf_load_prop (info, &prop_type, &prop_size))
	return FALSE;

      switch (prop_type)
	{
	case PROP_END:
	  return TRUE;
	case PROP_ACTIVE_CHANNEL:
	  info->active_channel = channel;
	  break;
	case PROP_SELECTION:
	  g_object_unref (gimage->selection_mask);
	  gimage->selection_mask = channel;
	  channel->boundary_known = FALSE;
	  channel->bounds_known   = FALSE;
	  break;
	case PROP_OPACITY:
	  {
	    guint32 opacity;

	    info->cp += xcf_read_int32 (info->fp, &opacity, 1);
	    channel->color.a = opacity / 255.0;
	  }
	  break;
	case PROP_VISIBLE:
	  {
	    gboolean visible;

	    info->cp += xcf_read_int32 (info->fp, (guint32 *) &visible, 1);
	    gimp_drawable_set_visible (GIMP_DRAWABLE (channel),
				       visible ? TRUE : FALSE, FALSE);
	  }
	  break;
	case PROP_LINKED:
          {
            gboolean linked;

            info->cp += xcf_read_int32 (info->fp, (guint32 *) &linked, 1);
            gimp_item_set_linked (GIMP_ITEM (channel),
                                  linked ? TRUE : FALSE, FALSE);
          }
	  break;
	case PROP_SHOW_MASKED:
	  info->cp += 
            xcf_read_int32 (info->fp, (guint32 *) &channel->show_masked, 1);
	  break;
	case PROP_COLOR:
	  {
	    guchar col[3];

	    info->cp += xcf_read_int8 (info->fp, (guint8 *) col, 3);

	    gimp_rgb_set_uchar (&channel->color, col[0], col[1], col[2]);
				
	  }
	  break;
	case PROP_TATTOO:
	  info->cp += 
            xcf_read_int32 (info->fp, &GIMP_ITEM (channel)->tattoo, 1);
	  break;
	 case PROP_PARASITES:
           {
             glong         base = info->cp;
             GimpParasite *p;

             while ((info->cp - base) < prop_size)
               {
                 p = xcf_load_parasite (info);
                 gimp_item_parasite_attach (GIMP_ITEM (channel), p);
                 gimp_parasite_free (p);
               }
             if (info->cp - base != prop_size)
               g_message("Error detected while loading a channel's parasites");
           }
           break;
	default:
	  g_message ("unexpected/unknown channel property: %d (skipping)", 
                     prop_type);

	  {
	    guint8 buf[16];
	    guint amount;

	    while (prop_size > 0)
	      {
		amount = MIN (16, prop_size);
		info->cp += xcf_read_int8 (info->fp, buf, amount);
		prop_size -= MIN (16, amount);
	      }
	  }
	  break;
	}
    }

  return FALSE;
}

static gboolean
xcf_load_prop (XcfInfo  *info,
	       PropType *prop_type,
	       guint32  *prop_size)
{
  info->cp += xcf_read_int32 (info->fp, (guint32 *) prop_type, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) prop_size, 1);
  return TRUE;
}

static GimpLayer *
xcf_load_layer (XcfInfo   *info,
		GimpImage *gimage)
{
  GimpLayer     *layer;
  GimpLayerMask *layer_mask;
  guint32        hierarchy_offset;
  guint32        layer_mask_offset;
  gboolean       apply_mask;
  gboolean       edit_mask;
  gboolean       show_mask;
  gint           width;
  gint           height;
  gint           type;
  gint           add_floating_sel;
  gchar         *name;

  /* check and see if this is the drawable the floating selection
   *  is attached to. if it is then we'll do the attachment at
   *  the end of this function.
   */
  add_floating_sel = (info->cp == info->floating_sel_offset);

  /* read in the layer width, height, type and name */
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &width, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &height, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &type, 1);
  info->cp += xcf_read_string (info->fp, &name, 1);

  /* create a new layer */
  layer = gimp_layer_new (gimage, width, height, 
                          type, name, 255, GIMP_NORMAL_MODE);
  g_free (name);
  if (!layer)
    return NULL;

  /* read in the layer properties */
  if (!xcf_load_layer_props (info, gimage, layer,
                             &apply_mask, &edit_mask, &show_mask))
    goto error;

  /* read the hierarchy and layer mask offsets */
  info->cp += xcf_read_int32 (info->fp, &hierarchy_offset, 1);
  info->cp += xcf_read_int32 (info->fp, &layer_mask_offset, 1);

  /* read in the hierarchy */
  if (!xcf_seek_pos (info, hierarchy_offset, NULL))
    goto error;

  if (!xcf_load_hierarchy (info, GIMP_DRAWABLE(layer)->tiles))
    goto error;

  /* read in the layer mask */
  if (layer_mask_offset != 0)
    {
      if (! xcf_seek_pos (info, layer_mask_offset, NULL))
        goto error;

      layer_mask = xcf_load_layer_mask (info, gimage);
      if (!layer_mask)
	goto error;

      /* set the offsets of the layer_mask */
      GIMP_ITEM (layer_mask)->offset_x = GIMP_ITEM (layer)->offset_x;
      GIMP_ITEM (layer_mask)->offset_y = GIMP_ITEM (layer)->offset_y;

      gimp_layer_add_mask (layer, layer_mask, FALSE);
      g_object_unref (layer_mask);

      layer->mask->apply_mask = apply_mask;
      layer->mask->edit_mask  = edit_mask;
      layer->mask->show_mask  = show_mask;
    }

  /* attach the floating selection... */
  if (add_floating_sel)
    {
      GimpLayer *floating_sel;

      floating_sel = info->floating_sel;
      floating_sel_attach (floating_sel, GIMP_DRAWABLE (layer));
    }

  return layer;

 error:
  g_object_unref (layer);
  return NULL;
}

static GimpChannel *
xcf_load_channel (XcfInfo   *info,
		  GimpImage *gimage)
{
  GimpChannel *channel;
  guint32      hierarchy_offset;
  gint         width;
  gint         height;
  gboolean     add_floating_sel;
  gboolean     is_qmask = FALSE;
  gchar       *name;
  GimpRGB      color = { 0.0, 0.0, 0.0, GIMP_OPACITY_OPAQUE };

  /* check and see if this is the drawable the floating selection
   *  is attached to. if it is then we'll do the attachment at
   *  the end of this function.
   */
  add_floating_sel = (info->cp == info->floating_sel_offset);

  /* read in the layer width, height and name */
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &width, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &height, 1);
  info->cp += xcf_read_string (info->fp, &name, 1);

  if (name)
    is_qmask = (strcmp (name, "Qmask") == 0);

  /* create a new channel */
  channel = gimp_channel_new (gimage, width, height, name, &color);
  g_free (name);
  if (!channel)
    return NULL;

  /* read in the channel properties */
  if (!xcf_load_channel_props (info, gimage, channel))
    goto error;

  /* read the hierarchy and layer mask offsets */
  info->cp += xcf_read_int32 (info->fp, &hierarchy_offset, 1);

  /* read in the hierarchy */
  if (!xcf_seek_pos (info, hierarchy_offset, NULL))
    goto error;
    
  if (!xcf_load_hierarchy (info, GIMP_DRAWABLE (channel)->tiles))
    goto error;

  /* attach the floating selection... */
  if (add_floating_sel)
    {
      GimpLayer *floating_sel;

      floating_sel = info->floating_sel;
      floating_sel_attach (floating_sel, GIMP_DRAWABLE (channel));
    }

  if (is_qmask)
    gimage->qmask_state = TRUE;

  return channel;

 error:
  g_object_unref (channel);
  return NULL;
}

static GimpLayerMask *
xcf_load_layer_mask (XcfInfo   *info,
		     GimpImage *gimage)
{
  GimpLayerMask *layer_mask;
  guint32        hierarchy_offset;
  gint           width;
  gint           height;
  gint           add_floating_sel;
  gchar         *name;
  GimpRGB        color = { 0.0, 0.0, 0.0, GIMP_OPACITY_OPAQUE };

  /* check and see if this is the drawable the floating selection
   *  is attached to. if it is then we'll do the attachment at
   *  the end of this function.
   */
  add_floating_sel = (info->cp == info->floating_sel_offset);

  /* read in the layer width, height and name */
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &width, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &height, 1);
  info->cp += xcf_read_string (info->fp, &name, 1);

  /* create a new layer mask */
  layer_mask = gimp_layer_mask_new (gimage, width, height, name, &color);
  g_free (name);
  if (!layer_mask)
    return NULL;

  /* read in the layer_mask properties */
  if (!xcf_load_channel_props (info, gimage, GIMP_CHANNEL (layer_mask)))
    goto error;

  /* read the hierarchy and layer mask offsets */
  info->cp += xcf_read_int32 (info->fp, &hierarchy_offset, 1);

  /* read in the hierarchy */
  if (! xcf_seek_pos (info, hierarchy_offset, NULL))
    goto error;

  if (!xcf_load_hierarchy (info, GIMP_DRAWABLE (layer_mask)->tiles))
    goto error;

  /* attach the floating selection... */
  if (add_floating_sel)
    {
      GimpLayer *floating_sel;

      floating_sel = info->floating_sel;
      floating_sel_attach (floating_sel, GIMP_DRAWABLE (layer_mask));
    }

  return layer_mask;

 error:
  g_object_unref (layer_mask);
  return NULL;
}

static gboolean
xcf_load_hierarchy (XcfInfo     *info,
		    TileManager *tiles)
{
  guint32 saved_pos;
  guint32 offset;
  guint32 junk;
  gint    width;
  gint    height;
  gint    bpp;

  info->cp += xcf_read_int32 (info->fp, (guint32 *) &width, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &height, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &bpp, 1);

  /* make sure the values in the file correspond to the values
   *  calculated when the TileManager was created.
   */
  if (width  != tile_manager_width (tiles)  ||
      height != tile_manager_height (tiles) ||
      bpp    != tile_manager_bpp (tiles))
    return FALSE;

  /* load in the levels...we make sure that the number of levels
   *  calculated when the TileManager was created is the same
   *  as the number of levels found in the file.
   */

  info->cp += xcf_read_int32 (info->fp, &offset, 1); /* top level */

  /* discard offsets for layers below first, if any.
   */
  do 
    {
      info->cp += xcf_read_int32 (info->fp, &junk, 1);
    }
  while (junk != 0);

  /* save the current position as it is where the
   *  next level offset is stored.
   */
  saved_pos = info->cp;
  
  /* seek to the level offset */
  if (!xcf_seek_pos (info, offset, NULL))
    return FALSE;
  
  /* read in the level */
  if (!xcf_load_level (info, tiles))
    return FALSE;
      
  /* restore the saved position so we'll be ready to
   *  read the next offset.
   */
  if (!xcf_seek_pos (info, saved_pos, NULL))
    return FALSE;

  return TRUE;
}


static gboolean
xcf_load_level (XcfInfo     *info,
		TileManager *tiles)
{
  guint32 saved_pos;
  guint32 offset, offset2;
  guint ntiles;
  gint width;
  gint height;
  gint i;
  gint fail;
  Tile *previous;
  Tile *tile;

  info->cp += xcf_read_int32 (info->fp, (guint32 *) &width, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &height, 1);

  if (width  != tile_manager_width  (tiles) ||
      height != tile_manager_height (tiles))
    return FALSE;

  /* read in the first tile offset.
   *  if it is '0', then this tile level is empty
   *  and we can simply return.
   */
  info->cp += xcf_read_int32 (info->fp, &offset, 1);
  if (offset == 0)
    return TRUE;

  /* Initialise the reference for the in-memory tile-compression
   */
  previous = NULL;

  ntiles = tiles->ntile_rows * tiles->ntile_cols;
  for (i = 0; i < ntiles; i++)
    {
      fail = FALSE;

      if (offset == 0)
	{
	  g_message ("not enough tiles found in level");
	  return FALSE;
	}

      /* save the current position as it is where the
       *  next tile offset is stored.
       */
      saved_pos = info->cp;

      /* read in the offset of the next tile so we can calculate the amount
	 of data needed for this tile*/
      info->cp += xcf_read_int32 (info->fp, &offset2, 1);
      
      /* if the offset is 0 then we need to read in the maximum possible
	 allowing for negative compression */
      if (offset2 == 0)
	offset2 = offset + TILE_WIDTH * TILE_WIDTH * 4 * 1.5; 
                                        /* 1.5 is probably more
					   than we need to allow */

      /* seek to the tile offset */
      if (! xcf_seek_pos (info, offset, NULL))
        return FALSE;

      /* get the tile from the tile manager */
      tile = tile_manager_get (tiles, i, TRUE, TRUE);

      /* read in the tile */
      switch (info->compression)
	{
	case COMPRESS_NONE:
	  if (!xcf_load_tile (info, tile))
	    fail = TRUE;
	  break;
	case COMPRESS_RLE:
	  if (!xcf_load_tile_rle (info, tile, offset2 - offset))
	    fail = TRUE;
	  break;
	case COMPRESS_ZLIB:
	  g_error ("xcf: zlib compression unimplemented");
	  fail = TRUE;
	  break;
	case COMPRESS_FRACTAL:
	  g_error ("xcf: fractal compression unimplemented");
	  fail = TRUE;
	  break;
	}

      if (fail) 
	{
	  tile_release (tile, TRUE);
	  return FALSE;
	}
	    
      /* To potentially save memory, we compare the
       *  newly-fetched tile against the last one, and
       *  if they're the same we copy-on-write mirror one against
       *  the other.
       */
      if (previous != NULL) 
	{
	  tile_lock (previous);
	  if (tile_ewidth (tile) == tile_ewidth (previous) &&
	      tile_eheight (tile) == tile_eheight (previous) &&
	      tile_bpp (tile) == tile_bpp (previous) &&
	      memcmp (tile_data_pointer (tile, 0, 0), 
		      tile_data_pointer (previous, 0, 0),
		      tile_size (tile)) == 0)
	    tile_manager_map (tiles, i, previous);
	  tile_release (previous, FALSE);
	}
      tile_release (tile, TRUE);
      previous = tile_manager_get (tiles, i, FALSE, FALSE);

      /* restore the saved position so we'll be ready to
       *  read the next offset.
       */
      if (!xcf_seek_pos (info, saved_pos, NULL))
        return FALSE;

      /* read in the offset of the next tile */
      info->cp += xcf_read_int32 (info->fp, &offset, 1);
    }

  if (offset != 0)
    {
      g_message ("encountered garbage after reading level: %d", offset);
      return FALSE;
    }

  return TRUE;
}

static gboolean
xcf_load_tile (XcfInfo *info,
	       Tile    *tile)
{
#ifdef SWAP_FROM_FILE

  if (!info->swap_num)
    {
      info->ref_count = g_new (int, 1);
      info->swap_num = tile_swap_add (info->filename, 
				      xcf_swap_func, 
				      info->ref_count);
    }

  tile->swap_num = info->swap_num;
  tile->swap_offset = info->cp;
  *info->ref_count += 1;

#else

  info->cp += xcf_read_int8 (info->fp, tile_data_pointer(tile, 0, 0), 
			     tile_size (tile));

#endif

  return TRUE;
}

static gboolean
xcf_load_tile_rle (XcfInfo *info,
		   Tile    *tile,
		   int     data_length)
{
  guchar *data;
  guchar val;
  gint size;
  gint count;
  gint length;
  gint bpp;
  gint i, j;
  gint nmemb_read_successfully;
  guchar *xcfdata, *xcfodata, *xcfdatalimit;
  
  data = tile_data_pointer (tile, 0, 0);
  bpp = tile_bpp (tile);
 
  xcfdata = xcfodata = g_malloc (data_length);

  /* we have to use fread instead of xcf_read_* because we may be
     reading past the end of the file here */
  nmemb_read_successfully = fread ((gchar *) xcfdata, sizeof (gchar),
				   data_length, info->fp);
  info->cp += nmemb_read_successfully;

  xcfdatalimit = &xcfodata[nmemb_read_successfully - 1];
  
  for (i = 0; i < bpp; i++)
    {
      data = (guchar *) tile_data_pointer (tile, 0, 0) + i;
      size = tile_ewidth (tile) * tile_eheight (tile);
      count = 0;

      while (size > 0)
	{
	  if (xcfdata > xcfdatalimit)
	    {
	      goto bogus_rle;
	    }

	  val = *xcfdata++;

	  length = val;
	  if (length >= 128)
	    {
	      length = 255 - (length - 1);
	      if (length == 128)
		{
		  if (xcfdata >= xcfdatalimit)
		    {
		      goto bogus_rle;
		    }

		  length = (*xcfdata << 8) + xcfdata[1];
		  xcfdata += 2;
		}

	      count += length;
	      size -= length;

	      if (size < 0)
		{
		  goto bogus_rle;
		}

	      if (&xcfdata[length-1] > xcfdatalimit)
		{
		  goto bogus_rle;
		}

	      while (length-- > 0)
		{
		  *data = *xcfdata++;
		  data += bpp;
		}
	    }
	  else
	    {
	      length += 1;
	      if (length == 128)
		{
		  if (xcfdata >= xcfdatalimit)
		    {
		      goto bogus_rle;
		    }

		  length = (*xcfdata << 8) + xcfdata[1];
		  xcfdata += 2;
		}

	      count += length;
	      size -= length;

              if (size < 0)
		{
		  goto bogus_rle;
		}

	      if (xcfdata > xcfdatalimit)
		{
		  goto bogus_rle;
		}

	      val = *xcfdata++;

	      for (j = 0; j < length; j++)
		{
		  *data = val;
		  data += bpp;
		}
	    }
	}
    }
  g_free (xcfodata);
  return TRUE;

 bogus_rle:
  if (xcfodata)
    g_free (xcfodata);
  return FALSE;
}

static GimpParasite *
xcf_load_parasite (XcfInfo *info)
{
  GimpParasite *p;

  p = g_new (GimpParasite, 1);
  info->cp += xcf_read_string (info->fp, &p->name, 1);
  info->cp += xcf_read_int32 (info->fp, &p->flags, 1);
  info->cp += xcf_read_int32 (info->fp, &p->size, 1);
  p->data = g_new (gchar, p->size);
  info->cp += xcf_read_int8 (info->fp, p->data, p->size);

  return p;
}

static gboolean
xcf_load_old_paths (XcfInfo   *info,
                    GimpImage *gimage)
{
  guint32      num_paths;
  guint32      last_selected_row;
  GimpVectors *active_vectors;

  info->cp += xcf_read_int32 (info->fp, &last_selected_row, 1);
  info->cp += xcf_read_int32 (info->fp, &num_paths, 1);

  g_print ("num_paths: %d  selected_row: %d\n", num_paths, last_selected_row);

  while (num_paths-- > 0)
    xcf_load_old_path (info, gimage);

  active_vectors = (GimpVectors *)
    gimp_container_get_child_by_index (gimage->vectors, last_selected_row);

  if (active_vectors)
    gimp_image_set_active_vectors (gimage, active_vectors);

  return TRUE;
}

static gboolean
xcf_load_old_path (XcfInfo   *info,
                   GimpImage *gimage)
{
  gchar       *name;
  guint32      locked;
  guint8       state;
  guint32      closed;
  guint32      num_points;
  guint32      version; /* changed from num_paths */
  GimpTattoo   tattoo = 0;
  GSList      *pts_list = NULL;
  GSList      *free_list = NULL;
  GimpVectors *vectors;
  GimpCoords  *coords;
  GimpCoords  *curr_coord;
  gint         num_coords;

  info->cp += xcf_read_string (info->fp, &name, 1);
  info->cp += xcf_read_int32  (info->fp, &locked, 1);
  info->cp += xcf_read_int8   (info->fp, &state, 1);
  info->cp += xcf_read_int32  (info->fp, &closed, 1);
  info->cp += xcf_read_int32  (info->fp, &num_points, 1);
  info->cp += xcf_read_int32  (info->fp, &version, 1);

  g_print ("num_points: %d  closed: %d\n", num_points, closed);

  vectors = gimp_vectors_new (gimage, name);

  GIMP_ITEM (vectors)->linked = locked;

  coords     = g_new0 (GimpCoords, num_points + 1);
  num_coords = num_points;

  curr_coord = coords;

  if (version == 1)
    {
      while (num_points-- > 0)
        {
          PathPoint *bpt;
          /* Read in a path */
          bpt = xcf_load_old_path_point1 (info, curr_coord++);
          pts_list = g_slist_append (pts_list, bpt);
        }
    }
  else if (version == 2)
    {
      guint32 dummy;

      /* Had extra type field and points are stored as doubles */
      info->cp += xcf_read_int32 (info->fp, (guint32 *) &dummy, 1);

      while (num_points-- > 0)
        {
          PathPoint *bpt;
          /* Read in a path */
          bpt = xcf_load_old_path_point (info, curr_coord++);
          pts_list = g_slist_append (pts_list, bpt);
        }
    }
  else if (version == 3)
    {
      guint32 dummy;

      /* Has extra tatto field */
      info->cp += xcf_read_int32 (info->fp, (guint32 *) &dummy, 1);
      info->cp += xcf_read_int32 (info->fp, (guint32 *) &tattoo, 1);

      while (num_points-- > 0)
	{
	  PathPoint *bpt;
	  /* Read in a path */
	  bpt = xcf_load_old_path_point (info, curr_coord++);
	  pts_list = g_slist_append (pts_list, bpt);
	}
    }
  else
    {
      g_warning ("Unknown path type. Possibly corrupt XCF file");
    }

  g_print ("\n");
  
  if (tattoo)
    GIMP_ITEM (vectors)->tattoo = tattoo;

  curr_coord = coords;

  free_list = pts_list;

  while (num_coords > 0)
    {
      GimpStroke *stroke;
      GimpCoords *next_stroke;
      gint        num_stroke_coords;

      for (next_stroke = curr_coord;
           num_coords > 0 && pts_list;
           next_stroke++, num_coords--, pts_list = g_slist_next (pts_list))
        {
          PathPoint *bpt = pts_list->data;

          if (next_stroke != curr_coord && bpt->type == 3)
            break;
        }

      num_stroke_coords = next_stroke - curr_coord;

      if (num_coords == 0 && ! closed)
        num_stroke_coords++;

      {
        GimpCoords temp_coords;
        gint       i;

        temp_coords = curr_coord[num_stroke_coords - 1];

        for (i = num_stroke_coords - 1; i >= 0; i--)
          curr_coord[i] = curr_coord[i - 1];

        if (num_coords > 0 || closed)
          curr_coord[0] = temp_coords;
        else
          curr_coord[0] = curr_coord[1];
      }

      stroke = gimp_bezier_stroke_new_from_coords (curr_coord,
                                                   num_stroke_coords,
                                                   num_coords > 0 || closed);
      gimp_vectors_stroke_add (vectors, stroke);
      g_object_unref (stroke);

      curr_coord = next_stroke;
    }

  g_free (coords);
  g_slist_foreach (free_list, (GFunc) g_free, NULL);
  g_slist_free (free_list);

  gimp_image_add_vectors (gimage, vectors,
                          gimp_container_num_children (gimage->vectors));

  return TRUE;
}

static PathPoint* 
xcf_load_old_path_point1 (XcfInfo    *info,
                          GimpCoords *coords)
{
  PathPoint *ptr;
  guint32    type;
  gint32     x;
  gint32     y;

  info->cp += xcf_read_int32 (info->fp, &type, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &x, 1);
  info->cp += xcf_read_int32 (info->fp, (guint32 *) &y, 1);

  ptr = g_new0 (PathPoint, 1);

  ptr->type = type;
  ptr->x    = x;
  ptr->y    = y;

  coords->x        = x;
  coords->y        = y;
  coords->pressure = 1.0;
  coords->xtilt    = 0.5;
  coords->ytilt    = 0.5;
  coords->wheel    = 0.5;

  return ptr;
}

static PathPoint * 
xcf_load_old_path_point (XcfInfo    *info,
                         GimpCoords *coords)
{
  PathPoint *ptr;
  guint32    type;
  gfloat     x;
  gfloat     y;
 
  info->cp += xcf_read_int32 (info->fp, &type, 1);
  info->cp += xcf_read_float (info->fp, &x, 1);
  info->cp += xcf_read_float (info->fp, &y, 1);

  g_print ("path point type: %d   (at %f, %f)\n", type, x, y);
 
  ptr = g_new0 (PathPoint, 1);

  ptr->type = type;
  ptr->x    = x;
  ptr->y    = y;

  coords->x        = x;
  coords->y        = y;
  coords->pressure = 1.0;
  coords->xtilt    = 0.5;
  coords->ytilt    = 0.5;
  coords->wheel    = 0.5;

  return ptr;
}


#ifdef SWAP_FROM_FILE

static gboolean
xcf_swap_func (gint      fd,
	       Tile     *tile,
	       gint      cmd,
	       gpointer  user_data)
{
  gint bytes;
  gint err;
  gint nleft;
  gint *ref_count;

  switch (cmd)
    {
    case SWAP_IN:
      lseek (fd, tile->swap_offset, SEEK_SET);

      bytes = tile_size (tile);
      tile_alloc (tile);

      nleft = bytes;
      while (nleft > 0)
	{
	  do {
	    err = read (fd, tile->data + bytes - nleft, nleft);
	  } while ((err == -1) && ((errno == EAGAIN) || (errno == EINTR)));

	  if (err <= 0)
	    {
	      g_message ("unable to read tile data from xcf file: "
                         "%d ( %d ) bytes read", err, nleft);
	      return FALSE;
	    }

	  nleft -= err;
	}
      break;
    case SWAP_OUT:
    case SWAP_DELETE:
    case SWAP_COMPRESS:
      ref_count = user_data;
      *ref_count -= 1;
      if (*ref_count == 0)
	{
	  tile_swap_remove (tile->swap_num);
	  g_free (ref_count);
	}

      tile->swap_num = 1;
      tile->swap_offset = -1;

      return TRUE;
    }

  return FALSE;
}

#endif
