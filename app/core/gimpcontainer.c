/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1997 Spencer Kimball and Peter Mattis
 *
 * gimpcontainer.c
 * Copyright (C) 2001 Michael Natterer <mitch@gimp.org>
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

#include <string.h>

#include <glib-object.h>

#include "core-types.h"

#include "gimp.h"
#include "gimpcontainer.h"
#include "gimpmarshal.h"

#include "config/gimpconfig.h"
#include "config/gimpconfig-deserialize.h"
#include "config/gimpconfigwriter.h"
#include "config/gimpscanner.h"


/* #define DEBUG_CONTAINER */

#ifdef DEBUG_CONTAINER
#define D(stmnt) stmnt
#else
#define D(stmnt)
#endif


typedef struct _GimpContainerHandler
{
  gchar     *signame;
  GCallback  callback;
  gpointer   callback_data;

  GQuark     quark;  /*  used to attach the signal id's of child signals  */
} GimpContainerHandler;


enum
{
  ADD,
  REMOVE,
  REORDER,
  FREEZE,
  THAW,
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_CHILDREN_TYPE,
  PROP_POLICY
};


/*  local function prototypes  */

static void   gimp_container_class_init          (GimpContainerClass  *klass);
static void   gimp_container_init                (GimpContainer       *container);
static void   gimp_container_config_iface_init   (GimpConfigInterface *config_iface);

static void       gimp_container_dispose         (GObject            *object);

static void       gimp_container_set_property    (GObject            *object,
                                                  guint               property_id,
                                                  const GValue       *value,
                                                  GParamSpec         *pspec);
static void       gimp_container_get_property    (GObject            *object,
                                                  guint               property_id,
                                                  GValue             *value,
                                                  GParamSpec         *pspec);

static gint64     gimp_container_get_memsize     (GimpObject         *object,
                                                  gint64             *gui_size);

static gboolean   gimp_container_serialize       (GimpConfig         *config,
                                                  GimpConfigWriter   *writer,
                                                  gpointer            data);
static gboolean   gimp_container_deserialize     (GimpConfig         *config,
                                                  GScanner           *scanner,
                                                  gint                nest_level,
                                                  gpointer            data);

static void   gimp_container_disconnect_callback (GimpObject         *object,
						  gpointer            data);


static guint container_signals[LAST_SIGNAL] = { 0, };

static GimpObjectClass *parent_class = NULL;


GType
gimp_container_get_type (void)
{
  static GType container_type = 0;

  if (! container_type)
    {
      static const GTypeInfo container_info =
      {
        sizeof (GimpContainerClass),
	(GBaseInitFunc) NULL,
	(GBaseFinalizeFunc) NULL,
	(GClassInitFunc) gimp_container_class_init,
	NULL,		/* class_finalize */
	NULL,		/* class_data     */
	sizeof (GimpContainer),
	0,              /* n_preallocs    */
	(GInstanceInitFunc) gimp_container_init,
      };
      static const GInterfaceInfo config_iface_info =
      {
        (GInterfaceInitFunc) gimp_container_config_iface_init,
        NULL,           /* iface_finalize */
        NULL            /* iface_data     */
      };

      container_type = g_type_register_static (GIMP_TYPE_OBJECT,
					       "GimpContainer",
                                               &container_info, 0);

      g_type_add_interface_static (container_type, GIMP_TYPE_CONFIG,
                                   &config_iface_info);
    }

  return container_type;
}

static void
gimp_container_class_init (GimpContainerClass *klass)
{
  GObjectClass    *object_class;
  GimpObjectClass *gimp_object_class;

  object_class      = G_OBJECT_CLASS (klass);
  gimp_object_class = GIMP_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  container_signals[ADD] =
    g_signal_new ("add",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GimpContainerClass, add),
		  NULL, NULL,
		  gimp_marshal_VOID__OBJECT,
		  G_TYPE_NONE, 1,
		  GIMP_TYPE_OBJECT);

  container_signals[REMOVE] =
    g_signal_new ("remove",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GimpContainerClass, remove),
		  NULL, NULL,
		  gimp_marshal_VOID__OBJECT,
		  G_TYPE_NONE, 1,
		  GIMP_TYPE_OBJECT);

  container_signals[REORDER] =
    g_signal_new ("reorder",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GimpContainerClass, reorder),
		  NULL, NULL,
		  gimp_marshal_VOID__OBJECT_INT,
		  G_TYPE_NONE, 2,
		  GIMP_TYPE_OBJECT,
		  G_TYPE_INT);

  container_signals[FREEZE] =
    g_signal_new ("freeze",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GimpContainerClass, freeze),
		  NULL, NULL,
		  gimp_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  container_signals[THAW] =
    g_signal_new ("thaw",
		  G_TYPE_FROM_CLASS (klass),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GimpContainerClass, thaw),
		  NULL, NULL,
		  gimp_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  object_class->dispose          = gimp_container_dispose;
  object_class->set_property     = gimp_container_set_property;
  object_class->get_property     = gimp_container_get_property;

  gimp_object_class->get_memsize = gimp_container_get_memsize;

  klass->add                     = NULL;
  klass->remove                  = NULL;
  klass->reorder                 = NULL;
  klass->freeze                  = NULL;
  klass->thaw                    = NULL;

  klass->clear                   = NULL;
  klass->have                    = NULL;
  klass->foreach                 = NULL;
  klass->get_child_by_name       = NULL;
  klass->get_child_by_index      = NULL;
  klass->get_child_index         = NULL;

  g_object_class_install_property (object_class,
				   PROP_CHILDREN_TYPE,
				   g_param_spec_pointer ("children_type",
                                                         NULL, NULL,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (object_class,
				   PROP_POLICY,
				   g_param_spec_enum ("policy",
                                                      NULL, NULL,
                                                      GIMP_TYPE_CONTAINER_POLICY,
                                                      GIMP_CONTAINER_POLICY_STRONG,
                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void
gimp_container_init (GimpContainer *container)
{
  container->children_type = G_TYPE_NONE;
  container->policy        = GIMP_CONTAINER_POLICY_STRONG;
  container->num_children  = 0;

  container->handlers      = NULL;
  container->freeze_count  = 0;
}

static void
gimp_container_config_iface_init (GimpConfigInterface *config_iface)
{
  config_iface->serialize   = gimp_container_serialize;
  config_iface->deserialize = gimp_container_deserialize;
}

static void
gimp_container_dispose (GObject *object)
{
  GimpContainer *container = GIMP_CONTAINER (object);

  while (container->handlers)
    gimp_container_remove_handler (container,
                                   ((GimpContainerHandler *)
                                    container->handlers->data)->quark);

  if (container->children_type != G_TYPE_NONE)
    {
      g_type_class_unref (g_type_class_peek (container->children_type));
      container->children_type = G_TYPE_NONE;
    }

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
gimp_container_set_property (GObject      *object,
                             guint         property_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  GimpContainer *container = GIMP_CONTAINER (object);

  switch (property_id)
    {
    case PROP_CHILDREN_TYPE:
      container->children_type = (GType) g_value_get_pointer (value);
      g_type_class_ref (container->children_type);
      break;
    case PROP_POLICY:
      container->policy = g_value_get_enum (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static void
gimp_container_get_property (GObject    *object,
                             guint       property_id,
                             GValue     *value,
                             GParamSpec *pspec)
{
  GimpContainer *container = GIMP_CONTAINER (object);

  switch (property_id)
    {
    case PROP_CHILDREN_TYPE:
      g_value_set_pointer (value, (gpointer) container->children_type);
      break;
    case PROP_POLICY:
      g_value_set_enum (value, container->policy);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
    }
}

static gint64
gimp_container_get_memsize (GimpObject *object,
                            gint64     *gui_size)
{
  GimpContainer *container = GIMP_CONTAINER (object);
  gint64         memsize   = 0;
  GList         *list;

  for (list = container->handlers; list; list = g_list_next (list))
    {
      GimpContainerHandler *handler = list->data;

      memsize += (sizeof (GList) +
                  sizeof (GimpContainerHandler) +
                  strlen (handler->signame) + 1);
    }

  return memsize + GIMP_OBJECT_CLASS (parent_class)->get_memsize (object,
                                                                  gui_size);
}

typedef struct
{
  GimpConfigWriter *writer;
  gpointer          data;
  gboolean          success;
} SerializeData;

static void
gimp_container_serialize_foreach (GObject       *object,
                                  SerializeData *serialize_data)
{
  GimpConfigInterface *config_iface;
  const gchar         *name;

  config_iface = GIMP_CONFIG_GET_INTERFACE (object);

  if (! config_iface)
    serialize_data->success = FALSE;

  if (! serialize_data->success)
    return;

  gimp_config_writer_open (serialize_data->writer,
			   g_type_name (G_TYPE_FROM_INSTANCE (object)));

  name = gimp_object_get_name (GIMP_OBJECT (object));

  if (name)
    gimp_config_writer_string (serialize_data->writer, name);
  else
    gimp_config_writer_print (serialize_data->writer, "NULL", 4);

  serialize_data->success = config_iface->serialize (GIMP_CONFIG (object),
                                                     serialize_data->writer,
                                                     serialize_data->data);
  gimp_config_writer_close (serialize_data->writer);
}

static gboolean
gimp_container_serialize (GimpConfig       *config,
                          GimpConfigWriter *writer,
                          gpointer          data)
{
  GimpContainer *container = GIMP_CONTAINER (config);
  SerializeData  serialize_data;

  serialize_data.writer  = writer;
  serialize_data.data    = data;
  serialize_data.success = TRUE;

  gimp_container_foreach (container,
                          (GFunc) gimp_container_serialize_foreach,
                          &serialize_data);

  return serialize_data.success;
}

static gboolean
gimp_container_deserialize (GimpConfig *config,
                            GScanner   *scanner,
                            gint        nest_level,
                            gpointer    data)
{
  GimpContainer *container = GIMP_CONTAINER (config);
  GTokenType     token;

  token = G_TOKEN_LEFT_PAREN;

  while (g_scanner_peek_next_token (scanner) == token)
    {
      token = g_scanner_get_next_token (scanner);

      switch (token)
        {
        case G_TOKEN_LEFT_PAREN:
          token = G_TOKEN_IDENTIFIER;
          break;

        case G_TOKEN_IDENTIFIER:
          {
            GimpObject *child;
            GType       type;
            gchar      *name = NULL;

            type = g_type_from_name (scanner->value.v_identifier);

            if (! type)
              {
                g_scanner_error (scanner,
                                 "unable to determine type of '%s'",
                                 scanner->value.v_identifier);
                return FALSE;
              }

            if (! g_type_is_a (type, container->children_type))
              {
                g_scanner_error (scanner,
                                 "'%s' is not a subclass of '%s'",
                                 scanner->value.v_identifier,
                                 g_type_name (container->children_type));
                return FALSE;
              }

            if (! g_type_is_a (type, GIMP_TYPE_CONFIG))
              {
                g_scanner_error (scanner,
                                 "'%s' does not implement GimpConfigInterface",
                                 scanner->value.v_identifier);
                return FALSE;
              }

            if (! gimp_scanner_parse_string (scanner, &name))
              {
                token = G_TOKEN_STRING;
                break;
              }

            if (! name)
              name = g_strdup ("");

            child = gimp_container_get_child_by_name (container, name);

            if (! child)
              {
                if (GIMP_IS_GIMP (data))
                  {
                    child = g_object_new (type,
                                          "name", name,
                                          "gimp", data, NULL);
                  }
                else
                  {
                    child = g_object_new (type,
                                          "name", name, NULL);
                 }

                gimp_container_add (container, child);

                if (container->policy == GIMP_CONTAINER_POLICY_STRONG)
                  g_object_unref (child);
              }

            g_free (name);

            if (! GIMP_CONFIG_GET_INTERFACE (child)->deserialize (GIMP_CONFIG (child),
                                                                  scanner,
                                                                  nest_level + 1,
                                                                  FALSE))
              {
                /*  warning should be already set by child  */
                return FALSE;
              }
          }
          token = G_TOKEN_RIGHT_PAREN;
          break;

        case G_TOKEN_RIGHT_PAREN:
          token = G_TOKEN_LEFT_PAREN;
          break;

        default: /* do nothing */
          break;
        }
    }

  return gimp_config_deserialize_return (scanner, token, nest_level);
}

static void
gimp_container_disconnect_callback (GimpObject *object,
				    gpointer    data)
{
  GimpContainer *container = GIMP_CONTAINER (data);

  gimp_container_remove (container, object);
}

GType
gimp_container_children_type (const GimpContainer *container)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), G_TYPE_NONE);

  return container->children_type;
}

GimpContainerPolicy
gimp_container_policy (const GimpContainer *container)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), 0);

  return container->policy;
}

gint
gimp_container_num_children (const GimpContainer *container)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), 0);

  return container->num_children;
}

gboolean
gimp_container_add (GimpContainer *container,
		    GimpObject    *object)
{
  GimpContainerHandler *handler;
  GList                *list;
  gulong                handler_id;

  g_return_val_if_fail (GIMP_IS_CONTAINER (container), FALSE);
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (object,
						    container->children_type),
			FALSE);

  if (gimp_container_have (container, object))
    {
      g_warning ("%s(): container %p already contains object %p",
		 G_GNUC_FUNCTION, container, object);
      return FALSE;
    }

  for (list = container->handlers; list; list = g_list_next (list))
    {
      handler = (GimpContainerHandler *) list->data;

      handler_id = g_signal_connect (object,
				     handler->signame,
				     handler->callback,
				     handler->callback_data);

      g_object_set_qdata (G_OBJECT (object), handler->quark,
			  GUINT_TO_POINTER (handler_id));
    }

  switch (container->policy)
    {
    case GIMP_CONTAINER_POLICY_STRONG:
      g_object_ref (object);
      break;

    case GIMP_CONTAINER_POLICY_WEAK:
      g_signal_connect (object, "disconnect",
			G_CALLBACK (gimp_container_disconnect_callback),
			container);
      break;
    }

  container->num_children++;

  g_signal_emit (container, container_signals[ADD], 0, object);

  return TRUE;
}

gboolean
gimp_container_remove (GimpContainer *container,
		       GimpObject    *object)
{
  GimpContainerHandler *handler;
  GList                *list;
  gulong                handler_id;

  g_return_val_if_fail (GIMP_IS_CONTAINER (container), FALSE);
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (object,
						    container->children_type),
			FALSE);

  if (! gimp_container_have (container, object))
    {
      g_warning ("%s(): container %p does not contain object %p",
		 G_GNUC_FUNCTION, container, object);
      return FALSE;
    }

  for (list = container->handlers; list; list = g_list_next (list))
    {
      handler = (GimpContainerHandler *) list->data;

      handler_id = GPOINTER_TO_UINT (g_object_get_qdata (G_OBJECT (object),
                                                         handler->quark));

      if (handler_id)
	{
	  g_signal_handler_disconnect (object, handler_id);

	  g_object_set_qdata (G_OBJECT (object), handler->quark, NULL);
	}
    }

  container->num_children--;

  g_signal_emit (container, container_signals[REMOVE], 0,
		 object);

  switch (container->policy)
    {
    case GIMP_CONTAINER_POLICY_STRONG:
      g_object_unref (object);
      break;

    case GIMP_CONTAINER_POLICY_WEAK:
      g_signal_handlers_disconnect_by_func (object,
                                            gimp_container_disconnect_callback,
                                            container);
      break;
    }

  return TRUE;
}

gboolean
gimp_container_insert (GimpContainer *container,
		       GimpObject    *object,
		       gint           index)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), FALSE);
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (object,
						    container->children_type),
			FALSE);

  g_return_val_if_fail (index >= -1 &&
			index <= container->num_children, FALSE);

  if (gimp_container_have (container, object))
    {
      g_warning ("%s(): container %p already contains object %p",
		 G_GNUC_FUNCTION, container, object);
      return FALSE;
    }

  if (gimp_container_add (container, object))
    {
      return gimp_container_reorder (container, object, index);
    }

  return FALSE;
}

gboolean
gimp_container_reorder (GimpContainer *container,
			GimpObject    *object,
			gint           new_index)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), FALSE);
  g_return_val_if_fail (object != NULL, FALSE);
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (object,
						    container->children_type),
			FALSE);

  g_return_val_if_fail (new_index >= -1 &&
			new_index < container->num_children, FALSE);

  if (! gimp_container_have (container, object))
    {
      g_warning ("%s(): container %p does not contain object %p",
		 G_GNUC_FUNCTION, container, object);
      return FALSE;
    }

  if (container->num_children == 1)
    return TRUE;

  g_signal_emit (container, container_signals[REORDER], 0,
		 object, new_index);

  return TRUE;
}

void
gimp_container_freeze (GimpContainer *container)
{
  g_return_if_fail (GIMP_IS_CONTAINER (container));

  container->freeze_count++;

  if (container->freeze_count == 1)
    g_signal_emit (container, container_signals[FREEZE], 0);
}

void
gimp_container_thaw (GimpContainer *container)
{
  g_return_if_fail (GIMP_IS_CONTAINER (container));

  if (container->freeze_count > 0)
    container->freeze_count--;

  if (container->freeze_count == 0)
    g_signal_emit (container, container_signals[THAW], 0);
}

gboolean
gimp_container_frozen (GimpContainer *container)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), FALSE);

  return (container->freeze_count > 0) ? TRUE : FALSE;
}

void
gimp_container_clear (GimpContainer *container)
{
  g_return_if_fail (GIMP_IS_CONTAINER (container));

  if (container->num_children > 0)
    {
      gimp_container_freeze (container);
      GIMP_CONTAINER_GET_CLASS (container)->clear (container);
      gimp_container_thaw (container);
    }
}

gboolean
gimp_container_have (const GimpContainer *container,
		     GimpObject          *object)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), FALSE);

  if (container->num_children < 1)
    return FALSE;

  return GIMP_CONTAINER_GET_CLASS (container)->have (container, object);
}

void
gimp_container_foreach (const GimpContainer *container,
			GFunc                func,
			gpointer             user_data)
{
  g_return_if_fail (GIMP_IS_CONTAINER (container));
  g_return_if_fail (func != NULL);

  if (container->num_children > 0)
    GIMP_CONTAINER_GET_CLASS (container)->foreach (container, func, user_data);
}

GimpObject *
gimp_container_get_child_by_name (const GimpContainer *container,
				  const gchar         *name)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), NULL);

  if (!name)
    return NULL;

  return GIMP_CONTAINER_GET_CLASS (container)->get_child_by_name (container,
								  name);
}

GimpObject *
gimp_container_get_child_by_index (const GimpContainer *container,
				   gint                 index)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), NULL);

  if (index < 0 || index >= container->num_children)
    return NULL;

  return GIMP_CONTAINER_GET_CLASS (container)->get_child_by_index (container,
								   index);
}

gint
gimp_container_get_child_index (const GimpContainer *container,
				const GimpObject    *object)
{
  g_return_val_if_fail (GIMP_IS_CONTAINER (container), -1);
  g_return_val_if_fail (object != NULL, -1);
  g_return_val_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (object,
						    container->children_type),
			-1);

  return GIMP_CONTAINER_GET_CLASS (container)->get_child_index (container,
								object);
}

static void
gimp_container_get_name_array_foreach_func (GimpObject   *object,
                                            gchar      ***iter)
{
  gchar **array = *iter;

  *array = g_strdup (gimp_object_get_name (object));
  (*iter)++;
}

gchar **
gimp_container_get_name_array (const GimpContainer *container,
                               gint                *length)
{
  gchar **names;
  gchar **iter;

  g_return_val_if_fail (GIMP_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (length != NULL, NULL);

  *length = gimp_container_num_children (container);
  if (*length == 0)
    return NULL;

  names = iter = g_new (gchar *, *length);

  gimp_container_foreach (container,
                          (GFunc) gimp_container_get_name_array_foreach_func,
                          &iter);

  return names;
}

static void
gimp_container_add_handler_foreach_func (GimpObject           *object,
					 GimpContainerHandler *handler)
{
  gulong handler_id;

  handler_id = g_signal_connect (object,
				 handler->signame,
				 handler->callback,
				 handler->callback_data);

  g_object_set_qdata (G_OBJECT (object), handler->quark,
		      GUINT_TO_POINTER (handler_id));
}

GQuark
gimp_container_add_handler (GimpContainer *container,
			    const gchar   *signame,
			    GCallback      callback,
			    gpointer       callback_data)
{
  GimpContainerHandler *handler;
  gchar                *key;

  static gint           handler_id = 0;

  g_return_val_if_fail (GIMP_IS_CONTAINER (container), 0);
  g_return_val_if_fail (signame != NULL, 0);
  g_return_val_if_fail (g_signal_lookup (signame, container->children_type), 0);
  g_return_val_if_fail (callback != NULL, 0);

  handler = g_new0 (GimpContainerHandler, 1);

  /*  create a unique key for this handler  */
  key = g_strdup_printf ("%s-%d", signame, handler_id++);

  handler->signame       = g_strdup (signame);
  handler->callback      = callback;
  handler->callback_data = callback_data;
  handler->quark         = g_quark_from_string (key);

  D (g_print ("%s: key = %s, id = %d\n", G_GNUC_FUNCTION, key, handler->quark));

  g_free (key);

  container->handlers = g_list_prepend (container->handlers, handler);

  gimp_container_foreach (container,
			  (GFunc) gimp_container_add_handler_foreach_func,
			  handler);

  return handler->quark;
}

static void
gimp_container_remove_handler_foreach_func (GimpObject           *object,
					    GimpContainerHandler *handler)
{
  gulong handler_id;

  handler_id = GPOINTER_TO_UINT (g_object_get_qdata (G_OBJECT (object),
                                                     handler->quark));

  if (handler_id)
    {
      g_signal_handler_disconnect (object, handler_id);

      g_object_set_qdata (G_OBJECT (object), handler->quark, NULL);
    }
}

void
gimp_container_remove_handler (GimpContainer *container,
			       GQuark         id)
{
  GimpContainerHandler *handler;
  GList                *list;

  g_return_if_fail (GIMP_IS_CONTAINER (container));
  g_return_if_fail (id != 0);

  for (list = container->handlers; list; list = g_list_next (list))
    {
      handler = (GimpContainerHandler *) list->data;

      if (handler->quark == id)
	break;
    }

  if (! list)
    {
      g_warning ("%s: tried to remove handler which unknown id %d",
                 G_STRLOC, id);
      return;
    }

  D (g_print ("%s: id = %d\n", G_GNUC_FUNCTION, handler->quark));

  gimp_container_foreach (container,
			  (GFunc) gimp_container_remove_handler_foreach_func,
			  handler);

  container->handlers = g_list_remove (container->handlers, handler);

  g_free (handler->signame);
  g_free (handler);
}
