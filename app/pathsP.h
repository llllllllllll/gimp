/* The GIMP -- an image manipulation program
 * Copyright (C) 1999 Andy Thomas alt@picnic.demon.co.uk
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
#ifndef  __PATHSP_H__
#define  __PATHSP_H__

/* Cutdown representation of the bezier curve description */
/* Will be used to hopefully store in XCF format...
 */

typedef struct {
  gint type;
  gint x;
  gint y;
} BZPOINT, *BZPOINTP;

typedef struct {
  GSList       * bezier_details;
  gboolean     closed;
  gint         state;
  gint         locked;  /* Only bottom bit used */
  GString      * name;
} BZPATH, *BZPATHP;

typedef struct {
  GimpImage * gimage;
  GDisplay  * gdisp; /* This is a hack.. Needed so we can get back to 
		      * the same display that these curves were added
		      * too. That way when an entry in the paths dialog
		      * is clicked the bezier tool can be targeted at 
		      * correct display. Note this display could have been
		      * deleted (ie different view), but gdisplays_check_valid()
		      * function will take care of that.. In this case we just
		      * pick a display that the gimage is rendered in.
		      */
  GSList    * bz_paths;  /* list of BZPATHP */
  guint       sig_id;
  gint        last_selected_row;
} PATHIMAGELIST, *PATHIMAGELISTP, PathsList;

BZPOINTP    bzpoint_new(gint,gint,gint);
BZPATHP     bzpath_new(GSList *,gint,gint,gint,gchar *);
PathsList * pathsList_new(GimpImage *,gint,GSList *);

#endif  /*  __PATHSP_H__  */
