/*
 * This is a plug-in for the GIMP.
 *
 * Generates clickable image maps.
 *
 * Copyright (C) 1998-2000 Maurits Rijk  lpeek.mrijk@consunet.nl
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
 *
 */

#ifndef _IMAP_DEFAULT_DIALOG_H
#define _IMAP_DEFAULT_DIALOG_H

#include "gtk/gtk.h"

typedef struct {
   GtkWidget	*dialog;
   GtkWidget	*ok;
   GtkWidget	*apply;
   GtkWidget	*cancel;
   GtkWidget	*help;
   void		(*ok_cb)(gpointer);
   gpointer	ok_cb_data;
   void		(*apply_cb)(gpointer);
   gpointer	apply_cb_data;
   void		(*cancel_cb)(gpointer);
   gpointer	cancel_cb_data;
} DefaultDialog_t;

DefaultDialog_t *make_default_dialog(const gchar *title);
void default_dialog_set_ok_cb(DefaultDialog_t *dialog, void (*ok_cb)(gpointer),
			      gpointer ok_cb_data);
void default_dialog_set_apply_cb(DefaultDialog_t *dialog, 
				 void (*apply_cb)(gpointer),
				 gpointer apply_cb_data);
void default_dialog_set_cancel_cb(DefaultDialog_t *dialog, 
				  void (*ok_cb)(gpointer),
				  gpointer ok_cb_data);
void default_dialog_show(DefaultDialog_t *dialog);
void default_dialog_hide_cancel_button(DefaultDialog_t *dialog);
void default_dialog_hide_apply_button(DefaultDialog_t *dialog);
void default_dialog_hide_help_button(DefaultDialog_t *dialog);
void default_dialog_set_title(DefaultDialog_t *dialog, const gchar *title);
void default_dialog_set_ok_sensitivity(DefaultDialog_t *dialog, 
				       gint sensitive);
void default_dialog_set_label(DefaultDialog_t *dialog, gchar *text);

#endif /* _IMAP_DEFAULT_DIALOG_H */
