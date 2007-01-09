/* GIMP - The GNU Image Manipulation Program
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

/* NOTE: This file is auto-generated by pdbgen.pl. */

#include "config.h"


#include <glib-object.h>

#include "pdb-types.h"
#include "gimppdb.h"
#include "gimpprocedure.h"
#include "core/gimpparamspecs.h"

#include "core/gimp.h"

#include "internal_procs.h"


static GValueArray *
fonts_popup_invoker (GimpProcedure     *procedure,
                     Gimp              *gimp,
                     GimpContext       *context,
                     GimpProgress      *progress,
                     const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *font_callback;
  const gchar *popup_title;
  const gchar *initial_font;

  font_callback = g_value_get_string (&args->values[0]);
  popup_title = g_value_get_string (&args->values[1]);
  initial_font = g_value_get_string (&args->values[2]);

  if (success)
    {
      if (gimp->no_interface ||
          ! gimp_pdb_lookup_procedure (gimp->pdb, font_callback) ||
          ! gimp_pdb_dialog_new (gimp, context, progress, gimp->fonts,
                                 popup_title, font_callback, initial_font,
                                 NULL))
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
fonts_close_popup_invoker (GimpProcedure     *procedure,
                           Gimp              *gimp,
                           GimpContext       *context,
                           GimpProgress      *progress,
                           const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *font_callback;

  font_callback = g_value_get_string (&args->values[0]);

  if (success)
    {
      if (gimp->no_interface ||
          ! gimp_pdb_lookup_procedure (gimp->pdb, font_callback) ||
          ! gimp_pdb_dialog_close (gimp, gimp->fonts, font_callback))
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

static GValueArray *
fonts_set_popup_invoker (GimpProcedure     *procedure,
                         Gimp              *gimp,
                         GimpContext       *context,
                         GimpProgress      *progress,
                         const GValueArray *args)
{
  gboolean success = TRUE;
  const gchar *font_callback;
  const gchar *font_name;

  font_callback = g_value_get_string (&args->values[0]);
  font_name = g_value_get_string (&args->values[1]);

  if (success)
    {
      if (gimp->no_interface ||
          ! gimp_pdb_lookup_procedure (gimp->pdb, font_callback) ||
          ! gimp_pdb_dialog_set (gimp, gimp->fonts, font_callback, font_name,
                                 NULL))
        success = FALSE;
    }

  return gimp_procedure_get_return_values (procedure, success);
}

void
register_font_select_procs (GimpPDB *pdb)
{
  GimpProcedure *procedure;

  /*
   * gimp-fonts-popup
   */
  procedure = gimp_procedure_new (fonts_popup_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-fonts-popup");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-fonts-popup",
                                     "Invokes the Gimp font selection.",
                                     "This procedure opens the font selection dialog.",
                                     "Sven Neumann <sven@gimp.org>",
                                     "Sven Neumann",
                                     "2003",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("font-callback",
                                                       "font callback",
                                                       "The callback PDB proc to call when font selection is made",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("popup-title",
                                                       "popup title",
                                                       "Title of the font selection dialog",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("initial-font",
                                                       "initial font",
                                                       "The name of the font to set as the first selected",
                                                       FALSE, TRUE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-fonts-close-popup
   */
  procedure = gimp_procedure_new (fonts_close_popup_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-fonts-close-popup");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-fonts-close-popup",
                                     "Close the font selection dialog.",
                                     "This procedure closes an opened font selection dialog.",
                                     "Sven Neumann <sven@gimp.org>",
                                     "Sven Neumann",
                                     "2003",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("font-callback",
                                                       "font callback",
                                                       "The name of the callback registered for this pop-up",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);

  /*
   * gimp-fonts-set-popup
   */
  procedure = gimp_procedure_new (fonts_set_popup_invoker);
  gimp_object_set_static_name (GIMP_OBJECT (procedure), "gimp-fonts-set-popup");
  gimp_procedure_set_static_strings (procedure,
                                     "gimp-fonts-set-popup",
                                     "Sets the current font in a font selection dialog.",
                                     "Sets the current font in a font selection dialog.",
                                     "Sven Neumann <sven@gimp.org>",
                                     "Sven Neumann",
                                     "2003",
                                     NULL);
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("font-callback",
                                                       "font callback",
                                                       "The name of the callback registered for this pop-up",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_procedure_add_argument (procedure,
                               gimp_param_spec_string ("font-name",
                                                       "font name",
                                                       "The name of the font to set as selected",
                                                       FALSE, FALSE,
                                                       NULL,
                                                       GIMP_PARAM_READWRITE));
  gimp_pdb_register_procedure (pdb, procedure);
  g_object_unref (procedure);
}
