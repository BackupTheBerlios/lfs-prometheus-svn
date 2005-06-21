// libreroot filename parsing
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

#include <stdlib.h>	// free.
#include <string.h>	// strcmp, strdupa & strtok_r.
#include <unistd.h>	// get_current_dir_name.

#include "absolute.h"
#include "memory.h"

static char const reroot_directory_delim [] = "/";

// Add current working directory to stack.
static void
reroot_push_cwd (reroot_stack *const restrict stack)
{
	// Get current working directory.
	char *const directory = get_current_dir_name ();

	// Add individual components to stack.
	char *ptr;
	char *restrict element =
		strtok_r (directory, reroot_directory_delim, &ptr);
	while (element)
	{
		reroot_stack_push (stack, element);
		element = strtok_r (0, reroot_directory_delim, &ptr);
	}

	free (directory);
}

// Return absolute filename, which must later be freed.  Don't resolve symbolic
// links or check if the file exists.
char *
reroot_absolute_filename (char const *const restrict name)
{
	// Initialize stack.
	reroot_stack stack;
	reroot_stack_init (&stack);

	// Add working directory if relative filename.
	if (*name != '/')
		reroot_push_cwd (&stack);

	// Parse given filename via stack.
	char *ptr;
	char *const scratch = strdupa (name);
	char *restrict element =
		strtok_r (scratch, reroot_directory_delim, &ptr);
	while (element)
	{
		if (!strcmp (element, ".."))		// `..' => Up one level.
			reroot_stack_pop (&stack);
		else if (strcmp (element, "."))		// `.'  => Skipped.
			reroot_stack_push (&stack, element);
		element = strtok_r (0, reroot_directory_delim, &ptr);
	}

	// Get absolute filename from stack.
	char *const restrict abs_name =
		reroot_stack_string (&stack, reroot_directory_delim, true);

	// Clean up & return.
	reroot_stack_empty (&stack);
	return abs_name;
}
