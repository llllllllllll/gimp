/* The GIMP -- an image manipulation program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * Datafiles module copyight (C) 1996 Federico Mena Quintero
 * federico@nuclecu.unam.mx
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

#ifndef __GIMP_DATAFILES_H__
#define __GIMP_DATAFILES_H__

#include <time.h>


typedef enum
{
  MODE_EXECUTABLE = 1 << 0,
  TYPE_DIRECTORY  = 1 << 1
} GimpDataFileFlags;


gboolean   gimp_datafiles_check_extension  (const gchar            *filename,
					    const gchar            *extension);

void       gimp_datafiles_read_directories (const gchar            *path_str,
					    GimpDataFileFlags       flags,
					    GimpDataFileLoaderFunc  loader_func,
					    gpointer                loader_data);

/* Return the current datafiles access, modification
 *  or change time. The current datafile is the one for
 *  which the "DataFileLoaderFunc" function has been called
 *  on.
 */
time_t   gimp_datafile_atime (void);
time_t   gimp_datafile_mtime (void);
time_t   gimp_datafile_ctime (void);


#endif  /*  __GIMP_DATAFILES_H__ */
