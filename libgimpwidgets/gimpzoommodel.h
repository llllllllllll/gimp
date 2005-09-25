/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * gimpzoommodel.h
 * Copyright (C) 2005  David Odin <dindinx@gimp.org> 
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

#ifndef __GIMP_ZOOM_MODEL_H__
#define __GIMP_ZOOM_MODEL_H__

G_BEGIN_DECLS


#define GIMP_TYPE_ZOOM_MODEL            (gimp_zoom_model_get_type ())
#define GIMP_ZOOM_MODEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GIMP_TYPE_ZOOM_MODEL, GimpZoomModel))
#define GIMP_ZOOM_MODEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GIMP_TYPE_ZOOM_MODEL, GimpZoomModelClass))
#define GIMP_IS_ZOOM_MODEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GIMP_TYPE_ZOOM_MODEL))
#define GIMP_IS_ZOOM_MODEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GIMP_TYPE_ZOOM_MODEL))
#define GIMP_ZOOM_MODEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GIMP_TYPE_ZOOM_MODEL, GimpZoomModel))


typedef struct _GimpZoomModelClass  GimpZoomModelClass;

struct _GimpZoomModel
{
  GObject  parent_instance;
};

struct _GimpZoomModelClass
{
  GObjectClass  parent_class;

  /* Padding for future expansion */
  void (* _gimp_reserved1) (void);
  void (* _gimp_reserved2) (void);
  void (* _gimp_reserved3) (void);
  void (* _gimp_reserved4) (void);
};


GType          gimp_zoom_model_get_type     (void) G_GNUC_CONST;

GimpZoomModel *gimp_zoom_model_new          (void);
void           gimp_zoom_model_set_range    (GimpZoomModel      *model,
                                             gdouble             min,
                                             gdouble             max);
gdouble        gimp_zoom_model_get_factor   (GimpZoomModel      *model);

GtkWidget     *gimp_zoom_widget_new         (GimpZoomModel      *model,
                                             GimpZoomWidgetType  type);

void           gimp_zoom_model_get_fraction (gdouble             zoom_factor,
                                             gint               *numerator,
                                             gint               *denominator);
gdouble        gimp_zoom_model_zoom_step    (GimpZoomType        zoom_type,
                                             gdouble             scale);

G_END_DECLS

#endif /* __GIMP_ZOOM_MODEL_H__ */
