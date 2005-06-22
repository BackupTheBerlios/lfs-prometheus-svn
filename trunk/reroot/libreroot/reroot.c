// libreroot support
// Copyright (C) 2003-2005 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
// Gareth Jones <gareth_jones@users.berlios.de>
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation; either version 2 of the License, or (at your option) any later
// version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place, Suite 330, Boston, MA  02111-1307  USA

#include <stdlib.h>	// free, getenv & setenv.
#include <string.h>	// memcpy, mempcpy, strdupa, strlen & strtok_r.

#include "absolute.h"
#include "memory.h"
#include "reroot.h"

// The false root directory.
static char const *const restrict reroot_false_root;
static unsigned const reroot_false_root_length;

// Files in excluded directories will not be rerooted.
static reroot_stack const reroot_exclude_path;

// Initialize reroot_false_root from the REROOT_FALSE_ROOT environment variable.
static void
reroot_false_root_init ()
{
	// Get value of environment variable.
	char const env_var [] = "REROOT_FALSE_ROOT";
	char const *const restrict false_root = getenv (env_var);

	// Is the environment variable non-null?
	if (false_root && *false_root)
	{
		// Set constants.
		*(char const **) &reroot_false_root =
			reroot_absolute_filename (false_root);
		*(unsigned *) &reroot_false_root_length =
			strlen (reroot_false_root);

		// Save back to environment incase any children change their
		// working directories.
		setenv (env_var, reroot_false_root, 1);
	}
	else
	{
		// By default don't reroot.
		*(char const **) &reroot_false_root = 0;
		*(unsigned *) &reroot_false_root_length = 0;
	}
}

// Initialize reroot_exclude_path from the REROOT_EXCLUDE_PATH environment
// variable.
static void
reroot_exclude_path_init ()
{
	// Initialize directory stack.
	reroot_stack *const restrict stack =
		(reroot_stack *) &reroot_exclude_path;
	reroot_stack_init (stack);

	// Get value of environment variable.
	char const env_var [] = "REROOT_EXCLUDE_PATH";
	char const *const restrict exclude_path = getenv (env_var);

	// Is the environment variable non-null?
	if (exclude_path && *exclude_path)
	{
		// Path deliminator.
		char const path_delim [] = ":";

		// Parse path variable.
		char *ptr;
		char *const path = strdupa (exclude_path);
		char *restrict directory = strtok_r (path, path_delim, &ptr);
		while (directory)
		{
			// Get absolute filename & add to stack.
			char *const restrict abs_dir =
				reroot_absolute_filename (directory);
			reroot_stack_push (stack, abs_dir);
			free (abs_dir);

			// Next directory.
			directory = strtok_r (0, path_delim, &ptr);
		}

		// Save back to environment incase any children change their
		// working directories.
		char *const restrict new_exclude_path =
			reroot_stack_string (stack, path_delim, false);
		setenv (env_var, new_exclude_path, 1);
		free (new_exclude_path);
	}

	// Insert false root directory to avoid multiple rerootings (e.g. if
	// glibc calls back into libreroot).  By default exclude /.
	if (reroot_false_root_length)
		reroot_stack_insert (stack, reroot_false_root);
	else
		reroot_stack_insert (stack, "/");
}

// Initialize reroot_false_root & reroot_exclude_path from environment
// variables.
void
reroot_env_init ()
{
	reroot_false_root_init ();
	reroot_exclude_path_init ();
}

// Free memory used by reroot_false_root & reroot_exclude_path.
void
reroot_env_destroy ()
{
	if (reroot_false_root)
		free ((char *) reroot_false_root);
	reroot_stack_empty ((reroot_stack *) &reroot_exclude_path);
}

// Convert the requested filename into a filename under the false root
// directory.  If the requested name is in an excluded directory, the returned
// pointer is equal to the argument, otherwise the returned pointer addresses
// the rerooted filename, and must be freed.
char *
reroot (char *const restrict requested)
{
	// If no false root specified, don't waste time.
	if (!reroot_false_root_length)
		return requested;

	// Get absolute filename.
	char *const restrict filename = reroot_absolute_filename (requested);

	// Check for excluded directories.
	reroot_stack_item const *restrict xdir = reroot_exclude_path.start;
	while (xdir)
	{
		// If absolute filename is in an excluded directory...
		if (strstr (filename, xdir->string) == filename)
		{
			free (filename);
			return requested;
		}

		// Next excluded directory.
		xdir = xdir->next;
	}

	// Reroot filename.
	unsigned const filename_length = strlen (filename);
	char *const restrict actual =
		reroot_alloc (reroot_false_root_length + filename_length + 1);
	memcpy (mempcpy (actual, reroot_false_root, reroot_false_root_length),
	        filename, filename_length + 1);

	free (filename);
	return actual;
}
