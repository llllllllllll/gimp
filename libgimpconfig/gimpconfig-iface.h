/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1997 Spencer Kimball and Peter Mattis
 *
 * Config file serialization and deserialization interface
 * Copyright (C) 2001  Sven Neumann <sven@gimp.org>
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

#ifndef __GIMP_CONFIG_H__
#define __GIMP_CONFIG_H__


#define GIMP_TYPE_CONFIG_INTERFACE     (gimp_config_interface_get_type ())
#define GIMP_GET_CONFIG_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GIMP_TYPE_CONFIG_INTERFACE, GimpConfigInterface))

typedef struct _GimpConfigInterface GimpConfigInterface;

struct _GimpConfigInterface
{
  GTypeInterface base_iface;

  void     (* serialize)   (GObject  *object,
                            gint      fd);
  gboolean (* deserialize) (GObject  *object,
                            GScanner *scanner,
                            gboolean  store_unknown);
};

typedef void  (*GimpConfigForeachFunc) (const gchar *key,
                                        const gchar *value,
                                        gpointer     user_data);


GType         gimp_config_interface_get_type    (void) G_GNUC_CONST;

gboolean      gimp_config_serialize             (GObject     *object,
                                                 const gchar *filename);
gboolean      gimp_config_deserialize           (GObject     *object,
                                                 const gchar *filename,
                                                 gboolean     store_unknown);

void          gimp_config_add_unknown_token     (GObject     *object,
                                                 const gchar *key,
                                                 const gchar *value);
const gchar * gimp_config_lookup_unknown_token  (GObject     *object,
                                                 const gchar *key);
void          gimp_config_foreach_unknown_token (GObject     *object,
                                                 GimpConfigForeachFunc  func,
                                                 gpointer     user_data);


#endif  /* __GIMP_CONFIG_H__ */
