/* parasite.h
 * Copyright (C) 1998 Jay Cox <jaycox@earthlink.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _PARASITE_H_
#define _PARASITE_H_

#include <glib.h>
#include <stdio.h>
#include <libgimp/parasiteF.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define PARASITE_PERSISTENT 1

#define PARASITE_ATTACH_PARENT (0x80 << 8)
#define PARASITE_PARENT_PERSISTENT (PARASITE_PERSISTENT << 8)

#define PARASITE_ATTACH_GRANDPARENT (0x80 << 16)
#define PARASITE_GRANDPARENT_PERSISTENT (PARASITE_PERSISTENT << 16)

Parasite   *parasite_new      (const char *name, guint32 flags,
			       guint32 size, const void *data);
void        parasite_free     (Parasite *parasite);

Parasite   *parasite_copy     (const Parasite *parasite);

int         parasite_compare  (const Parasite *a, const Parasite *b);

int         parasite_is_type       (const Parasite *parasite,
				    const char *name);
int         parasite_is_persistent (const Parasite *p);
int         parasite_has_flag      (const Parasite *p, gulong flag);
gulong      parasite_flags         (const Parasite *p);
const char *parasite_name          (const Parasite *p);
void       *parasite_data          (const Parasite *p);
long        parasite_data_size     (const Parasite *p);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _PARASITE_H_ */
