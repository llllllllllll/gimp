/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-2000 Peter Mattis and Spencer Kimball
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

#ifndef __GIMP_SIGNAL_H__
#define __GIMP_SIGNAL_H__

#include <signal.h>

G_BEGIN_DECLS

/* For information look into the C source or the html documentation */


/* GimpSignalHandlerFunc is a reference to a (signal handler) function
 * that takes a signal ID and returns void.
 * signal(2) returns such references; so does gimp_signal_private.
 */
typedef void (* GimpSignalHandlerFunc) (gint signum);

GimpSignalHandlerFunc  gimp_signal_private (gint                   signum,
                                            GimpSignalHandlerFunc  handler,
                                            gint                   flags);


G_END_DECLS

#endif /* __GIMP_SIGNAL_H__ */
