/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1999 Spencer Kimball and Peter Mattis
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

#include "procedural_db.h"

#include "color_balance.h"
#include "curves.h"
#include "desaturate.h"
#include "drawable.h"
#include "equalize.h"
#include "gimage.h"
#include "gimpdrawable.h"
#include "gimpimage.h"
#include "gimplut.h"
#include "histogram_tool.h"
#include "hue_saturation.h"
#include "invert.h"
#include "lut_funcs.h"
#include "threshold.h"

static ProcRecord brightness_contrast_proc;
static ProcRecord levels_proc;
static ProcRecord posterize_proc;
static ProcRecord desaturate_proc;
static ProcRecord equalize_proc;
static ProcRecord invert_proc;
static ProcRecord curves_spline_proc;
static ProcRecord curves_explicit_proc;
static ProcRecord color_balance_proc;
static ProcRecord histogram_proc;
static ProcRecord hue_saturation_proc;
static ProcRecord threshold_proc;

void
register_color_procs (void)
{
  procedural_db_register (&brightness_contrast_proc);
  procedural_db_register (&levels_proc);
  procedural_db_register (&posterize_proc);
  procedural_db_register (&desaturate_proc);
  procedural_db_register (&equalize_proc);
  procedural_db_register (&invert_proc);
  procedural_db_register (&curves_spline_proc);
  procedural_db_register (&curves_explicit_proc);
  procedural_db_register (&color_balance_proc);
  procedural_db_register (&histogram_proc);
  procedural_db_register (&hue_saturation_proc);
  procedural_db_register (&threshold_proc);
}

static Argument *
brightness_contrast_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 brightness;
  gint32 contrast;
  GimpImage *gimage;
  GimpLut *lut;
  PixelRegion srcPR, destPR;
  int x1, y1, x2, y2;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  brightness = args[1].value.pdb_int;
  if (brightness < -127 || brightness > 127)
    success = FALSE;

  contrast = args[2].value.pdb_int;
  if (contrast < -127 || contrast > 127)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_indexed (drawable))
	success = FALSE;
      else
	{
	  gimage = gimp_drawable_gimage (drawable);
    
	  lut = brightness_contrast_lut_new (brightness / 255.0,
					     contrast / 127.0,
					     gimp_drawable_bytes (drawable));
    
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
    
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  pixel_regions_process_parallel ((p_func) gimp_lut_process, lut, 2,
					  &srcPR, &destPR);
    
	  gimp_lut_free (lut);
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&brightness_contrast_proc, success);
}

static ProcArg brightness_contrast_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "brightness",
    "Brightness adjustment: (-127 <= brightness <= 127)"
  },
  {
    PDB_INT32,
    "contrast",
    "Contrast adjustment: (-127 <= contrast <= 127)"
  }
};

static ProcRecord brightness_contrast_proc =
{
  "gimp_brightness_contrast",
  "Modify brightness/contrast in the specified drawable.",
  "This procedures allows the brightness and contrast of the specified drawable to be modified. Both 'brightness' and 'contrast' parameters are defined between -127 and 127.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1997",
  PDB_INTERNAL,
  3,
  brightness_contrast_inargs,
  0,
  NULL,
  { { brightness_contrast_invoker } }
};

static Argument *
levels_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 channel;
  gint32 low_inputv;
  gint32 high_inputv;
  gdouble gammav;
  gint32 low_outputv;
  gint32 high_outputv;
  PixelRegion srcPR, destPR;
  int x1, y1, x2, y2;
  GimpLut *lut;
  int i;
  int low_input[5];
  int high_input[5];
  double gamma[5];
  int low_output[5];
  int high_output[5];

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  channel = args[1].value.pdb_int;
  if (channel < VALUE_LUT || channel > BLUE_LUT)
    success = FALSE;

  low_inputv = args[2].value.pdb_int;
  if (low_inputv < 0 || low_inputv > 255)
    success = FALSE;

  high_inputv = args[3].value.pdb_int;
  if (high_inputv < 0 || high_inputv > 255)
    success = FALSE;

  gammav = args[4].value.pdb_float;
  if (gammav < 0.1 || gammav > 10.0)
    success = FALSE;

  low_outputv = args[5].value.pdb_int;
  if (low_outputv < 0 || low_outputv > 255)
    success = FALSE;

  high_outputv = args[6].value.pdb_int;
  if (high_outputv < 0 || high_outputv > 255)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_indexed (drawable) ||
	  (gimp_drawable_gray (drawable) && channel != GRAY_LUT))
	success = FALSE;
      else
	{
	  for (i = 0; i < 5; i++)
	    {
	      low_input[i] = 0;
	      high_input[i] = 255;
	      low_output[i] = 0;
	      high_output[i] = 255;
	      gamma[i] = 1.0;
	    }
    
	  low_input[channel] = low_inputv;
	  high_input[channel] = high_inputv;
	  gamma[channel] = gammav;
	  low_output[channel] = low_outputv;
	  high_output[channel] = high_outputv;
    
	  /* setup the lut */
	  lut = levels_lut_new (gamma, low_input, high_input,
				low_output, high_output,
				gimp_drawable_bytes (drawable));
    
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
    
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  pixel_regions_process_parallel ((p_func) gimp_lut_process, lut, 2,
					  &srcPR, &destPR);
    
	  gimp_lut_free(lut);
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&levels_proc, success);
}

static ProcArg levels_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "channel",
    "The channel to modify: { VALUE_LUT (0), RED_LUT (1), GREEN_LUT (2), BLUE_LUT (3) }"
  },
  {
    PDB_INT32,
    "low_input",
    "Intensity of lowest input: (0 <= low_input <= 255)"
  },
  {
    PDB_INT32,
    "high_input",
    "Intensity of highest input: (0 <= high_input <= 255)"
  },
  {
    PDB_FLOAT,
    "gamma",
    "Gamma correction factor: (0.1 <= gamma <= 10)"
  },
  {
    PDB_INT32,
    "low_output",
    "Intensity of lowest output: (0 <= low_output <= 255)"
  },
  {
    PDB_INT32,
    "high_output",
    "Intensity of highest output: (0 <= high_output <= 255)"
  }
};

static ProcRecord levels_proc =
{
  "gimp_levels",
  "Modifies intensity levels in the specified drawable.",
  "This tool allows intensity levels in the specified drawable to be remapped according to a set of parameters. The low/high input levels specify an initial mapping from the source intensities. The gamma value determines how intensities between the low and high input intensities are interpolated. A gamma value of 1.0 results in a linear interpolation. Higher gamma values result in more high-level intensities. Lower gamma values result in more low-level intensities. The low/high output levels constrain the final intensity mapping--that is, no final intensity will be lower than the low output level and no final intensity will be higher than the high output level. This tool is only valid on RGB color and grayscale images. It will not operate on indexed drawables.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  7,
  levels_inargs,
  0,
  NULL,
  { { levels_invoker } }
};

static Argument *
posterize_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 levels;
  GimpImage *gimage;
  GimpLut *lut;
  PixelRegion srcPR, destPR;
  int x1, y1, x2, y2;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  levels = args[1].value.pdb_int;
  if (levels < 2 || levels > 255)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_indexed (drawable))
	success = FALSE;
      else
	{
	  gimage = gimp_drawable_gimage (drawable);
    
	  lut = posterize_lut_new (levels, gimp_drawable_bytes (drawable)); 
    
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
    
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  pixel_regions_process_parallel ((p_func) gimp_lut_process, lut, 2,
					  &srcPR, &destPR);
    
	  gimp_lut_free (lut);
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&posterize_proc, success);
}

static ProcArg posterize_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "levels",
    "Levels of posterization: (2 <= levels <= 255)"
  }
};

static ProcRecord posterize_proc =
{
  "gimp_posterize",
  "Posterize the specified drawable.",
  "This procedures reduces the number of shades allows in each intensity channel to the specified 'levels' parameter.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1997",
  PDB_INTERNAL,
  2,
  posterize_inargs,
  0,
  NULL,
  { { posterize_invoker } }
};

static Argument *
desaturate_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_color (drawable))
	desaturate (drawable);
      else
	success = FALSE;
    }

  return procedural_db_return_args (&desaturate_proc, success);
}

static ProcArg desaturate_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  }
};

static ProcRecord desaturate_proc =
{
  "gimp_desaturate",
  "Desaturate the contents of the specified drawable.",
  "This procedure desaturates the contents of the specified drawable. This procedure only works on drawables of type RGB color.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  desaturate_inargs,
  0,
  NULL,
  { { desaturate_invoker } }
};

static Argument *
equalize_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gboolean mask_only;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  mask_only = args[1].value.pdb_int ? TRUE : FALSE;

  if (success)
    {
      if (! gimp_drawable_indexed (drawable))
	equalize (drawable_gimage (drawable), drawable, mask_only);
      else
	success = FALSE;
    }

  return procedural_db_return_args (&equalize_proc, success);
}

static ProcArg equalize_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "mask_only",
    "Equalization option"
  }
};

static ProcRecord equalize_proc =
{
  "gimp_equalize",
  "Equalize the contents of the specified drawable.",
  "This procedure equalizes the contents of the specified drawable. Each intensity channel is equalizeed independently. The equalized intensity is given as inten' = (255 - inten). Indexed color drawables are not valid for this operation. The 'mask_only' option specifies whether to adjust only the area of the image within the selection bounds, or the entire image based on the histogram of the selected area. If there is no selection, the entire image is adjusted based on the histogram for the entire image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  2,
  equalize_inargs,
  0,
  NULL,
  { { equalize_invoker } }
};

static Argument *
invert_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  if (success)
    {
      if (! gimp_drawable_indexed (drawable))
	invert (drawable);
      else
	success = FALSE;
    }

  return procedural_db_return_args (&invert_proc, success);
}

static ProcArg invert_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  }
};

static ProcRecord invert_proc =
{
  "gimp_invert",
  "Invert the contents of the specified drawable.",
  "This procedure inverts the contents of the specified drawable. Each intensity channel is inverted independently. The inverted intensity is given as inten' = (255 - inten). Indexed color drawables are not valid for this operation.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  invert_inargs,
  0,
  NULL,
  { { invert_invoker } }
};

static Argument *
curves_spline_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 channel;
  gint32 num_points;
  gint8 *control_pts;
  CurvesDialog cd;
  int x1, y1, x2, y2;
  int i, j;
  PixelRegion srcPR, destPR;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  channel = args[1].value.pdb_int;
  if (channel < VALUE_LUT || channel > ALPHA_LUT)
    success = FALSE;

  num_points = args[2].value.pdb_int;
  if (num_points <= 3 || num_points > 32)
    success = FALSE;

  control_pts = (gint8 *) args[3].value.pdb_pointer;

  if (success)
    {
      if (gimp_drawable_indexed (drawable) || (num_points & 1) ||
	  (gimp_drawable_gray (drawable) && channel != GRAY_LUT))
	success = FALSE;
      else
	{
	  cd.lut = gimp_lut_new ();
    
	  for (i = 0; i < 5; i++)
	    for (j = 0; j < 256; j++)
	      cd.curve[i][j] = j;
    
	  for (i = 0; i < 5; i++)
	    for (j = 0; j < 17; j++)
	      {
		cd.points[i][j][0] = -1;
		cd.points[i][j][1] = -1;
	      }
    
	  cd.drawable = drawable;
	  cd.channel = channel;
	  cd.color = gimp_drawable_color (drawable);
	  cd.curve_type = SMOOTH;
    
	  for (j = 0; j < num_points / 2; j++)
	    {
	      cd.points[cd.channel][j][0] = control_pts[j * 2];
	      cd.points[cd.channel][j][1] = control_pts[j * 2 + 1];
	    }
	  curves_calculate_curve (&cd);
    
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
    
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  pixel_regions_process_parallel ((p_func) gimp_lut_process, cd.lut, 2,
					  &srcPR, &destPR);
    
	  gimp_lut_free (cd.lut);
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&curves_spline_proc, success);
}

static ProcArg curves_spline_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "channel",
    "The channel to modify: { VALUE_LUT (0), RED_LUT (1), GREEN_LUT (2), BLUE_LUT (3), ALPHA_LUT (4) }"
  },
  {
    PDB_INT32,
    "num_points",
    "The number of values in the control point array (3 < num_points <= 32)"
  },
  {
    PDB_INT8ARRAY,
    "control_pts",
    "The spline control points: { cp1.x, cp1.y, cp2.x, cp2.y, ... }"
  }
};

static ProcRecord curves_spline_proc =
{
  "gimp_curves_spline",
  "Modifies the intensity curve(s) for specified drawable.",
  "Modifies the intensity mapping for one channel in the specified drawable. The drawable must be either grayscale or RGB, and the channel can be either an intensity component, or the value. The 'control_pts' parameter is an array of integers which define a set of control points which describe a Catmull Rom spline which yields the final intensity curve. Use the 'gimp_curves_explicit' function to explicitly modify intensity levels.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  4,
  curves_spline_inargs,
  0,
  NULL,
  { { curves_spline_invoker } }
};

static Argument *
curves_explicit_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 channel;
  gint32 num_bytes;
  guint8 *curve;
  CurvesDialog cd;
  int x1, y1, x2, y2;
  int i, j;
  PixelRegion srcPR, destPR;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  channel = args[1].value.pdb_int;
  if (channel < VALUE_LUT || channel > ALPHA_LUT)
    success = FALSE;

  num_bytes = args[2].value.pdb_int;
  if (num_bytes <= 0)
    success = FALSE;

  curve = (guint8 *) args[3].value.pdb_pointer;

  if (success)
    {
      if (gimp_drawable_indexed (drawable) || (num_bytes != 256) ||
	  (gimp_drawable_gray (drawable) && channel != GRAY_LUT))
	success = FALSE;
      else 
	{
	  for (i = 0; i < 5; i++)
	    for (j = 0; j < 256; j++)
	      cd.curve[i][j] = j;
    
	  cd.drawable = drawable;
	  cd.channel = channel;
	  cd.color = gimp_drawable_color (drawable);
    
	  for (j = 0; j < 256; j++)
	    cd.curve[cd.channel][j] = curve[j];
    
	  cd.lut = gimp_lut_new ();
	  gimp_lut_setup (cd.lut, (GimpLutFunc) curves_lut_func,
			  (void *) &cd, gimp_drawable_bytes(drawable));
	  
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  pixel_regions_process_parallel ((p_func) gimp_lut_process, cd.lut, 2,
					  &srcPR, &destPR);
    
	  gimp_lut_free (cd.lut);
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&curves_explicit_proc, success);
}

static ProcArg curves_explicit_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "channel",
    "The channel to modify: { VALUE_LUT (0), RED_LUT (1), GREEN_LUT (2), BLUE_LUT (3), ALPHA_LUT (4) }"
  },
  {
    PDB_INT32,
    "num_bytes",
    "The number of bytes in the new curve (always 256)"
  },
  {
    PDB_INT8ARRAY,
    "curve",
    "The explicit curve"
  }
};

static ProcRecord curves_explicit_proc =
{
  "gimp_curves_explicit",
  "Modifies the intensity curve(s) for specified drawable.",
  "Modifies the intensity mapping for one channel in the specified drawable. The drawable must be either grayscale or RGB, and the channel can be either an intensity component, or the value. The 'curve' parameter is an array of bytes which explicitly defines how each pixel value in the drawable will be modified. Use the 'gimp_curves_spline' function to modify intensity levels with Catmull Rom splines.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  4,
  curves_explicit_inargs,
  0,
  NULL,
  { { curves_explicit_invoker } }
};

static Argument *
color_balance_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 transfer_mode;
  gboolean preserve_lum;
  gdouble cyan_red;
  gdouble magenta_green;
  gdouble yellow_blue;
  ColorBalanceDialog cbd;
  int i;
  void *pr;
  PixelRegion srcPR, destPR;
  int x1, y1, x2, y2;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  transfer_mode = args[1].value.pdb_int;
  if (transfer_mode < SHADOWS || transfer_mode > HIGHLIGHTS)
    success = FALSE;

  preserve_lum = args[2].value.pdb_int ? TRUE : FALSE;

  cyan_red = args[3].value.pdb_float;
  if (cyan_red < -100.0 || cyan_red > 100.0)
    success = FALSE;

  magenta_green = args[4].value.pdb_float;
  if (magenta_green < -100.0 || magenta_green > 100.0)
    success = FALSE;

  yellow_blue = args[5].value.pdb_float;
  if (yellow_blue < -100.0 || yellow_blue > 100.0)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_indexed (drawable))
	success = FALSE;
      else
	{
	  for (i = 0; i < 3; i++)
	    {
	      cbd.cyan_red[i] = 0.0;
	      cbd.magenta_green[i] = 0.0;
	      cbd.yellow_blue[i] = 0.0;
	    }
    
	  cbd.preserve_luminosity = preserve_lum;
	  cbd.cyan_red[transfer_mode] = cyan_red;
	  cbd.magenta_green[transfer_mode] = magenta_green;
	  cbd.yellow_blue[transfer_mode] = yellow_blue;
    
	  color_balance_create_lookup_tables (&cbd);
    
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
    
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  for (pr = pixel_regions_register (2, &srcPR, &destPR); pr;
	       pr = pixel_regions_process (pr))
	    color_balance (&srcPR, &destPR, (void *) &cbd);
    
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&color_balance_proc, success);
}

static ProcArg color_balance_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "transfer_mode",
    "Transfer mode: { SHADOWS (0), MIDTONES (1), HIGHLIGHTS (2) }"
  },
  {
    PDB_INT32,
    "preserve_lum",
    "Preserve luminosity values at each pixel"
  },
  {
    PDB_FLOAT,
    "cyan_red",
    "Cyan-Red color balance: (-100 <= cyan_red <= 100)"
  },
  {
    PDB_FLOAT,
    "magenta_green",
    "Magenta-Green color balance: (-100 <= magenta_green <= 100)"
  },
  {
    PDB_FLOAT,
    "yellow_blue",
    "Yellow-Blue color balance: (-100 <= yellow_blue <= 100)"
  }
};

static ProcRecord color_balance_proc =
{
  "gimp_color_balance",
  "Modify the color balance of the specified drawable.",
  "Modify the color balance of the specified drawable. There are three axis which can be modified: cyan-red, magenta-green, and yellow-blue. Negative values increase the amount of the former, positive values increase the amount of the latter. Color balance can be controlled with the 'transfer_mode' setting, which allows shadows, midtones, and highlights in an image to be affected differently. The 'preserve_lum' parameter, if non-zero, ensures that the luminosity of each pixel remains fixed.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1997",
  PDB_INTERNAL,
  6,
  color_balance_inargs,
  0,
  NULL,
  { { color_balance_invoker } }
};

static Argument *
histogram_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GimpDrawable *drawable;
  gint32 channel;
  gint32 start_range;
  gint32 end_range;
  PixelRegion srcPR, maskPR;
  int x1, y1, x2, y2;
  HistogramToolDialog htd;
  int off_x, off_y;
  gboolean no_mask;
  Channel *mask;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  channel = args[1].value.pdb_int;
  if (channel < VALUE_LUT || channel > ALPHA_LUT)
    success = FALSE;

  start_range = args[2].value.pdb_int;
  if (start_range < 0 || start_range >= 256)
    success = FALSE;

  end_range = args[3].value.pdb_int;
  if (end_range < 0 || end_range >= 256)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_indexed (drawable) ||
	  (gimp_drawable_gray (drawable) && channel != GRAY_LUT))
	success = FALSE;
      else
	{
	  htd.shell = NULL;
	  htd.channel = channel;
	  htd.drawable = drawable;
	  htd.color = gimp_drawable_color (drawable);
	  htd.histogram = histogram_widget_new (HISTOGRAM_WIDTH,
						HISTOGRAM_HEIGHT);
    
	  gtk_signal_connect (GTK_OBJECT (htd.histogram), "rangechanged",
			      (GtkSignalFunc) histogram_tool_histogram_range,
			      (void*) &htd);
    
	  /* The information collection should occur only within selection bounds */
	  no_mask = (gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2) == FALSE);
	  gimp_drawable_offsets (drawable, &off_x, &off_y);
    
	  /* Configure the src from the drawable data */
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
    
	  /*  Configure the mask from the gimage's selection mask */
	  mask = gimage_get_mask (drawable_gimage (GIMP_DRAWABLE (drawable)));
	  pixel_region_init (&maskPR, gimp_drawable_data (GIMP_DRAWABLE(mask)),
			     x1 + off_x, y1 + off_y, (x2 - x1), (y2 - y1), FALSE);
    
	  /* Apply the image transformation to the pixels */
	  htd.hist = gimp_histogram_new ();
	  if (no_mask)
	    gimp_histogram_calculate (htd.hist, &srcPR, NULL);
	  else
	    gimp_histogram_calculate (htd.hist, &srcPR, &maskPR);
    
	  /* Calculate the statistics */
	  histogram_tool_histogram_range (htd.histogram, start_range, end_range,
					  &htd);
	}
    }

  return_args = procedural_db_return_args (&histogram_proc, success);

  if (success)
    {
      return_args[1].value.pdb_float = htd.mean;
      return_args[2].value.pdb_float = htd.std_dev;
      return_args[3].value.pdb_float = htd.median;
      return_args[4].value.pdb_float = htd.pixels;
      return_args[5].value.pdb_float = htd.count;
      return_args[6].value.pdb_float = htd.percentile;
    }

  return return_args;
}

static ProcArg histogram_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "channel",
    "The channel to modify: { VALUE_LUT (0), RED_LUT (1), GREEN_LUT (2), BLUE_LUT (3), ALPHA_LUT (4) }"
  },
  {
    PDB_INT32,
    "start_range",
    "Start of the intensity measurement range"
  },
  {
    PDB_INT32,
    "end_range",
    "End of the intensity measurement range"
  }
};

static ProcArg histogram_outargs[] =
{
  {
    PDB_FLOAT,
    "mean",
    "Mean itensity value"
  },
  {
    PDB_FLOAT,
    "std_dev",
    "Standard deviation of intensity values"
  },
  {
    PDB_FLOAT,
    "median",
    "Median itensity value"
  },
  {
    PDB_FLOAT,
    "pixels",
    "Alpha-weighted pixel count for entire image"
  },
  {
    PDB_FLOAT,
    "count",
    "Alpha-weighted pixel count for range"
  },
  {
    PDB_FLOAT,
    "percentile",
    "Percentile that range falls under"
  }
};

static ProcRecord histogram_proc =
{
  "gimp_histogram",
  "Returns information on the intensity histogram for the specified drawable.",
  "This tool makes it possible to gather information about the intensity histogram of a drawable. A channel to examine is first specified. This can be either value, red, green, or blue, depending on whether the drawable is of type color or grayscale. The drawable may not be indexed. Second, a range of intensities are specified. The gimp_histogram function returns statistics based on the pixels in the drawable that fall under this range of values. Mean, standard deviation, median, number of pixels, and percentile are all returned. Additionally, the total count of pixels in the image is returned. Counts of pixels are weighted by any associated alpha values and by the current selection mask. That is, pixels that lie outside an active selection mask will not be counted. Similarly, pixels with transparent alpha values will not be counted.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  4,
  histogram_inargs,
  6,
  histogram_outargs,
  { { histogram_invoker } }
};

static Argument *
hue_saturation_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 hue_range;
  gdouble hue_offset;
  gdouble lightness;
  gdouble saturation;
  HueSaturationDialog hsd;
  int i;
  void *pr;
  PixelRegion srcPR, destPR;
  int x1, y1, x2, y2;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  hue_range = args[1].value.pdb_int;
  if (hue_range < ALL_HUES || hue_range > MAGENTA_HUES)
    success = FALSE;

  hue_offset = args[2].value.pdb_float;
  if (hue_offset < -180.0 || hue_offset > 180.0)
    success = FALSE;

  lightness = args[3].value.pdb_float;
  if (lightness < -100.0 || lightness > 100.0)
    success = FALSE;

  saturation = args[4].value.pdb_float;
  if (saturation < -100.0 || saturation > 100.0)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_indexed (drawable))
	success = FALSE;
      else
	{
	  for (i = 0; i < 7; i++)
	    {
	      hsd.hue[i] = 0.0;
	      hsd.lightness[i] = 0.0;
	      hsd.saturation[i] = 0.0;
	    }
    
	  hsd.hue[hue_range] = hue_offset;
	  hsd.lightness[hue_range] = lightness;
	  hsd.saturation[hue_range] = saturation;
    
	  /* Calculate the transfer arrays */
	  hue_saturation_calculate_transfers (&hsd);
    
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
    
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  for (pr = pixel_regions_register (2, &srcPR, &destPR); pr;
	       pr = pixel_regions_process (pr))
	    hue_saturation (&srcPR, &destPR, (void *) &hsd);
    
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&hue_saturation_proc, success);
}

static ProcArg hue_saturation_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "hue_range",
    "Range of affected hues: { ALL_HUES (0), RED_HUES (1), YELLOW_HUES (2), GREEN_HUES (3), CYAN_HUES (4), BLUE_HUES (5), MAGENTA_HUES (6) }"
  },
  {
    PDB_FLOAT,
    "hue_offset",
    "Hue offset in degrees: (-180 <= hue_offset <= 180)"
  },
  {
    PDB_FLOAT,
    "lightness",
    "lightness modification: (-100 <= lightness <= 100)"
  },
  {
    PDB_FLOAT,
    "saturation",
    "saturation modification: (-100 <= saturation <= 100)"
  }
};

static ProcRecord hue_saturation_proc =
{
  "gimp_hue_saturation",
  "Modify hue, lightness, and saturation in the specified drawable.",
  "This procedures allows the hue, lightness, and saturation in the specified drawable to be modified. The 'hue_range' parameter provides the capability to limit range of affected hues.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1997",
  PDB_INTERNAL,
  5,
  hue_saturation_inargs,
  0,
  NULL,
  { { hue_saturation_invoker } }
};

static Argument *
threshold_invoker (Argument *args)
{
  gboolean success = TRUE;
  GimpDrawable *drawable;
  gint32 low_threshold;
  gint32 high_threshold;
  ThresholdDialog td;
  int x1, y1, x2, y2;
  PixelRegion srcPR, destPR;

  drawable = gimp_drawable_get_ID (args[0].value.pdb_int);
  if (drawable == NULL)
    success = FALSE;

  low_threshold = args[1].value.pdb_int;
  if (low_threshold < 0 || low_threshold > 255)
    success = FALSE;

  high_threshold = args[2].value.pdb_int;
  if (high_threshold < 0 || high_threshold > 255)
    success = FALSE;

  if (success)
    {
      if (gimp_drawable_indexed (drawable) || (low_threshold >= high_threshold))
	success = FALSE;
      else
	{
	  td.color = gimp_drawable_color (drawable);
	  td.low_threshold = low_threshold;
	  td.high_threshold = high_threshold;
    
	  /* The application should occur only within selection bounds */
	  gimp_drawable_mask_bounds (drawable, &x1, &y1, &x2, &y2);
    
	  pixel_region_init (&srcPR, gimp_drawable_data (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), FALSE);
	  pixel_region_init (&destPR, gimp_drawable_shadow (drawable),
			     x1, y1, (x2 - x1), (y2 - y1), TRUE);
    
	  pixel_regions_process_parallel ((p_func) threshold_2, (void*) &td, 2,
					  &srcPR, &destPR);
    
	  gimp_drawable_merge_shadow (drawable, TRUE);
	  drawable_update (drawable, x1, y1, (x2 - x1), (y2 - y1));
	}
    }

  return procedural_db_return_args (&threshold_proc, success);
}

static ProcArg threshold_inargs[] =
{
  {
    PDB_DRAWABLE,
    "drawable",
    "The drawable"
  },
  {
    PDB_INT32,
    "low_threshold",
    "The low threshold value: 0 <= low_threshold <= 255"
  },
  {
    PDB_INT32,
    "high_threshold",
    "The high threshold value: 0 <= high_threshold <= 255"
  }
};

static ProcRecord threshold_proc =
{
  "gimp_threshold",
  "Threshold the specified drawable.",
  "This procedures generates a threshold map of the specified drawable. All pixels between the values of 'low_threshold' and 'high_threshold' are replaced with white, and all other pixels with black.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1997",
  PDB_INTERNAL,
  3,
  threshold_inargs,
  0,
  NULL,
  { { threshold_invoker } }
};
