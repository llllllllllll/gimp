/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * GimpImageCommentEditor
 * Copyright (C) 2007  Sven Neumann <sven@gimp.org>
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

#include <gtk/gtk.h>

#include "libgimpbase/gimpbase.h"
#include "libgimpwidgets/gimpwidgets.h"

#include "widgets-types.h"

#include "core/gimpimage.h"

#include "gimpimagecommenteditor.h"


#define GIMP_IMAGE_COMMENT_PARASITE "gimp-comment"


static void  gimp_image_comment_editor_update         (GimpImageParasiteView  *view);

static void  gimp_image_comment_editor_buffer_changed (GtkTextBuffer          *buffer,
                                                       GimpImageCommentEditor *editor);


G_DEFINE_TYPE (GimpImageCommentEditor,
               gimp_image_comment_editor, GIMP_TYPE_IMAGE_PARASITE_VIEW)

static void
gimp_image_comment_editor_class_init (GimpImageCommentEditorClass *klass)
{
  GimpImageParasiteViewClass *view_class;

  view_class = GIMP_IMAGE_PARASITE_VIEW_CLASS (klass);

  view_class->update = gimp_image_comment_editor_update;
}

static void
gimp_image_comment_editor_init (GimpImageCommentEditor *editor)
{
  GtkWidget *scrolled_window;
  GtkWidget *text_view;

  scrolled_window = gtk_scrolled_window_new (NULL, NULL);

  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 2);
  gtk_container_add (GTK_CONTAINER (editor), scrolled_window);
  gtk_widget_show (scrolled_window);

  text_view = gtk_text_view_new ();

  gtk_text_view_set_editable (GTK_TEXT_VIEW (text_view), TRUE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD);

  gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (text_view), 6);
  gtk_text_view_set_left_margin (GTK_TEXT_VIEW (text_view), 6);
  gtk_text_view_set_right_margin (GTK_TEXT_VIEW (text_view), 6);

  gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
  gtk_widget_show (text_view);

  editor->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text_view));

  g_signal_connect (editor->buffer, "changed",
                    G_CALLBACK (gimp_image_comment_editor_buffer_changed),
                    editor);
}


/*  public functions  */

GtkWidget *
gimp_image_comment_editor_new (GimpImage *image)
{
  g_return_val_if_fail (GIMP_IS_IMAGE (image), NULL);

  return g_object_new (GIMP_TYPE_IMAGE_COMMENT_EDITOR,
                       "image",    image,
                       "parasite", GIMP_IMAGE_COMMENT_PARASITE,
                       NULL);
}


/*  private functions  */

static void
gimp_image_comment_editor_update (GimpImageParasiteView *view)
{
  GimpImageCommentEditor *editor = GIMP_IMAGE_COMMENT_EDITOR (view);
  const GimpParasite     *parasite;

  if (editor->recursing)
    return;

  g_signal_handlers_block_by_func (editor->buffer,
                                   gimp_image_comment_editor_buffer_changed,
                                   editor);

  parasite = gimp_image_parasite_view_get_parasite (view);

  if (parasite)
    {
      gchar *text = g_strndup (gimp_parasite_data (parasite),
                               gimp_parasite_data_size (parasite));

      if (! g_utf8_validate (text, -1, NULL))
        {
          gchar *tmp = gimp_any_to_utf8 (text, -1, NULL);

          g_free (text);
          text = tmp;
        }

      gtk_text_buffer_set_text (editor->buffer, text, -1);
      g_free (text);
    }
  else
    {
      gtk_text_buffer_set_text (editor->buffer, "", 0);
    }

  g_signal_handlers_unblock_by_func (editor->buffer,
                                     gimp_image_comment_editor_buffer_changed,
                                     editor);
}

static void
gimp_image_comment_editor_buffer_changed (GtkTextBuffer          *buffer,
                                          GimpImageCommentEditor *editor)
{
  GimpImage   *image;
  gchar       *text;
  gint         len;
  GtkTextIter  start;
  GtkTextIter  end;

  image =
    gimp_image_parasite_view_get_image (GIMP_IMAGE_PARASITE_VIEW (editor));

  gtk_text_buffer_get_bounds (buffer, &start, &end);

  text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

  len = text ? strlen (text) : 0;

  editor->recursing = TRUE;

  if (len > 0)
    {
      GimpParasite *parasite;

      parasite = gimp_parasite_new (GIMP_IMAGE_COMMENT_PARASITE,
                                    GIMP_PARASITE_PERSISTENT,
                                    len + 1, text);

      gimp_image_parasite_attach (image, parasite);
      gimp_parasite_free (parasite);
    }
  else
    {
      gimp_image_parasite_detach (image, GIMP_IMAGE_COMMENT_PARASITE);
    }

  editor->recursing = FALSE;

  g_free (text);
}
