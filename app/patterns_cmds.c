/* The GIMP -- an image manipulation program
 * Copyright (C) 1995-1999 Spencer Kimball and Peter Mattis
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

/* NOTE: This file is autogenerated by pdbgen.pl. */

#include "procedural_db.h"

#include <string.h>

#include "gimpcontext.h"
#include "patterns.h"

static ProcRecord patterns_get_pattern_proc;
static ProcRecord patterns_set_pattern_proc;
static ProcRecord patterns_list_proc;
static ProcRecord patterns_get_pattern_data_proc;

void
register_patterns_procs (void)
{
  procedural_db_register (&patterns_get_pattern_proc);
  procedural_db_register (&patterns_set_pattern_proc);
  procedural_db_register (&patterns_list_proc);
  procedural_db_register (&patterns_get_pattern_data_proc);
}

static Argument *
patterns_get_pattern_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  GPattern *pattern;

  success = (pattern = gimp_context_get_pattern (NULL)) != NULL;

  return_args = procedural_db_return_args (&patterns_get_pattern_proc, success);

  if (success)
    {
      return_args[1].value.pdb_pointer = g_strdup (pattern->name);
      return_args[2].value.pdb_int = pattern->mask->width;
      return_args[3].value.pdb_int = pattern->mask->height;
    }

  return return_args;
}

static ProcArg patterns_get_pattern_outargs[] =
{
  {
    PDB_STRING,
    "name",
    "The pattern name"
  },
  {
    PDB_INT32,
    "width",
    "The pattern width"
  },
  {
    PDB_INT32,
    "height",
    "The pattern height"
  }
};

static ProcRecord patterns_get_pattern_proc =
{
  "gimp_patterns_get_pattern",
  "Retrieve information about the currently active pattern.",
  "This procedure retrieves information about the currently active pattern. This includes the pattern name, and the pattern extents (width and height). All clone and bucket-fill operations with patterns will use this pattern to control the application of paint to the image.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  0,
  NULL,
  3,
  patterns_get_pattern_outargs,
  { { patterns_get_pattern_invoker } }
};

static Argument *
patterns_set_pattern_invoker (Argument *args)
{
  gboolean success = TRUE;
  gchar *name;
  GPattern *pattern;
  GSList *list;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  if (success)
    {
      success = FALSE;
    
      for (list = pattern_list; list; list = g_slist_next (list))
	{
	  pattern = (GPattern *) list->data;
    
	  if (!strcmp (pattern->name, name))
	    {
	      gimp_context_set_pattern (NULL, pattern);
	      success = TRUE;
	      break;
	    }
	}
    }

  return procedural_db_return_args (&patterns_set_pattern_proc, success);
}

static ProcArg patterns_set_pattern_inargs[] =
{
  {
    PDB_STRING,
    "name",
    "The pattern name"
  }
};

static ProcRecord patterns_set_pattern_proc =
{
  "gimp_patterns_set_pattern",
  "Set the specified pattern as the active pattern.",
  "This procedure allows the active pattern mask to be set by specifying its name. The name is simply a string which corresponds to one of the names of the installed patterns. If there is no matching pattern found, this procedure will return an error. Otherwise, the specified pattern becomes active and will be used in all subsequent paint operations.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  1,
  patterns_set_pattern_inargs,
  0,
  NULL,
  { { patterns_set_pattern_invoker } }
};

static Argument *
patterns_list_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar **patterns;
  GSList *list = NULL;
  int i = 0;

  patterns = g_new (char *, num_patterns);

  success = (list = pattern_list) != NULL;

  while (list)
    {
      patterns[i++] = g_strdup (((GPattern *) list->data)->name);
      list = list->next;
    }

  return_args = procedural_db_return_args (&patterns_list_proc, success);

  if (success)
    {
      return_args[1].value.pdb_int = num_patterns;
      return_args[2].value.pdb_pointer = patterns;
    }

  return return_args;
}

static ProcArg patterns_list_outargs[] =
{
  {
    PDB_INT32,
    "num_patterns",
    "The number of patterns in the pattern list"
  },
  {
    PDB_STRINGARRAY,
    "pattern_list",
    "The list of pattern names"
  }
};

static ProcRecord patterns_list_proc =
{
  "gimp_patterns_list",
  "Retrieve a complete listing of the available patterns.",
  "This procedure returns a complete listing of available GIMP patterns. Each name returned can be used as input to the 'gimp_patterns_set_pattern'.",
  "Spencer Kimball & Peter Mattis",
  "Spencer Kimball & Peter Mattis",
  "1995-1996",
  PDB_INTERNAL,
  0,
  NULL,
  2,
  patterns_list_outargs,
  { { patterns_list_invoker } }
};

static Argument *
patterns_get_pattern_data_invoker (Argument *args)
{
  gboolean success = TRUE;
  Argument *return_args;
  gchar *name;
  gint32 length = 0;
  guint8 *mask_data = NULL;
  GPattern *pattern = NULL;

  name = (gchar *) args[0].value.pdb_pointer;
  if (name == NULL)
    success = FALSE;

  if (success)
    {
      if (strlen (name))
	{
	  GSList *list;
    
	  success = FALSE;
    
	  for (list = pattern_list; list; list = g_slist_next (list))
	    {
	      pattern = (GPattern *) list->data;
    
	      if (!strcmp (pattern->name, name))
		{
		  success = TRUE;
		  break;
		}
	    }
	}
      else
	success = (pattern = gimp_context_get_pattern (NULL)) != NULL;
    
      if (success)
	{
	  length = pattern->mask->height * pattern->mask->width *
		   pattern->mask->bytes;
	  mask_data = g_new (guint8, length);
	  g_memmove (mask_data, temp_buf_data (pattern->mask), length);
	}
    }

  return_args = procedural_db_return_args (&patterns_get_pattern_data_proc, success);

  if (success)
    {
      return_args[1].value.pdb_pointer = g_strdup (pattern->name);
      return_args[2].value.pdb_int = pattern->mask->width;
      return_args[3].value.pdb_int = pattern->mask->height;
      return_args[4].value.pdb_int = pattern->mask->bytes;
      return_args[5].value.pdb_int = length;
      return_args[6].value.pdb_pointer = mask_data;
    }

  return return_args;
}

static ProcArg patterns_get_pattern_data_inargs[] =
{
  {
    PDB_STRING,
    "name",
    "the pattern name (\"\" means current active pattern)"
  }
};

static ProcArg patterns_get_pattern_data_outargs[] =
{
  {
    PDB_STRING,
    "name",
    "The pattern name"
  },
  {
    PDB_INT32,
    "width",
    "The pattern width"
  },
  {
    PDB_INT32,
    "height",
    "The pattern height"
  },
  {
    PDB_INT32,
    "mask_bpp",
    "Pattern bytes per pixel"
  },
  {
    PDB_INT32,
    "length",
    "Length of pattern mask data"
  },
  {
    PDB_INT8ARRAY,
    "mask_data",
    "The pattern mask data"
  }
};

static ProcRecord patterns_get_pattern_data_proc =
{
  "gimp_patterns_get_pattern_data",
  "Retrieve information about the currently active pattern (including data).",
  "This procedure retrieves information about the currently active pattern. This includes the pattern name, and the pattern extents (width and height). It also returns the pattern data.",
  "Andy Thomas",
  "Andy Thomas",
  "1998",
  PDB_INTERNAL,
  1,
  patterns_get_pattern_data_inargs,
  6,
  patterns_get_pattern_data_outargs,
  { { patterns_get_pattern_data_invoker } }
};
