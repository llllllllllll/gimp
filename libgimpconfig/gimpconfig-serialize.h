/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * Object properties serialization routines
 * Copyright (C) 2001-2002  Sven Neumann <sven@gimp.org>
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

#ifndef __GIMP_CONFIG_SERIALIZE_H__
#define __GIMP_CONFIG_SERIALIZE_H__


gboolean  gimp_config_serialize_properties         (GObject      *object,
                                                    gint          fd,
                                                    gint          indent_level);
gboolean  gimp_config_serialize_changed_properties (GObject      *new,
                                                    GObject      *old,
                                                    gint          fd,
                                                    gint          indent_level);
gboolean  gimp_config_serialize_unknown_tokens     (GObject      *object,
                                                    gint          fd,
                                                    gint          indent_level);
gboolean  gimp_config_serialize_value              (const GValue *value,
                                                    GString      *str,
                                                    gboolean      escaped);

#endif /* __GIMP_CONFIG_SERIALIZE_H__ */
