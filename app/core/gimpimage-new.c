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

#include "config.h"

#include <glib-object.h>

#include "libgimpconfig/gimpconfig.h"

#include "core-types.h"

#include "gimp.h"
#include "gimpimage.h"
#include "gimpimage-new.h"
#include "gimptemplate.h"

#include "gimp-intl.h"


GimpTemplate *
gimp_image_new_get_last_template (Gimp      *gimp,
                                  GimpImage *gimage)
{
  GimpTemplate *template;

  g_return_val_if_fail (GIMP_IS_GIMP (gimp), NULL);
  g_return_val_if_fail (gimage == NULL || GIMP_IS_IMAGE (gimage), NULL);

  template = gimp_template_new ("image new values");

  if (gimage)
    gimp_template_set_from_image (template, gimage);
  else
    gimp_config_sync (G_OBJECT (gimp->image_new_last_template),
                      G_OBJECT (template), 0);

  return template;
}

void
gimp_image_new_set_last_template (Gimp         *gimp,
                                  GimpTemplate *template)
{
  g_return_if_fail (GIMP_IS_GIMP (gimp));
  g_return_if_fail (GIMP_IS_TEMPLATE (template));

  gimp_config_sync (G_OBJECT (template),
                    G_OBJECT (gimp->image_new_last_template), 0);
}
