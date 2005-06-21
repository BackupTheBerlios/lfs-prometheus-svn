// rerootd filename parsing
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

#include <cstdlib>
#include <cstring>
#include <list>
#include <string>
#include <unistd.h>

#include "absolute.h"

// FIXME: Use a C++ method instead of strtok.  istringstream with `/' as
// whitespace?

// We're using C functions, some of which may be in the std namespace, some of
// which are global.
using namespace std;

namespace
{
	char const directory_delim [] = "/";

	// For storing filename elements.
	typedef list <string> string_stack;
	typedef string_stack::const_iterator iterator;

	// Create a full filename by concatenating the elements in stack.
	void
	get_filename (string_stack const &stack, string &filename)
	{
		iterator const end = stack.end ();
		for (iterator i = stack.begin (); i != end; ++i)
			filename += directory_delim + *i;
	}

	// Add current working directory to stack.
	void
	add_cwd (string_stack &stack)
	{
		// Get current working directory.
		char *const directory = get_current_dir_name ();

		// Add individual components to stack.
		char *ptr,
		     *element = strtok_r (directory, directory_delim, &ptr);
		while (element)
		{
			stack.push_back (element);
			element = strtok_r (0, directory_delim, &ptr);
		}

		free (directory);
	}
}

// Make filename absolute.  Don't resolve symbolic links or check if the file
// exists.
void
absolute_filename (string &name)
{
	string_stack stack;
	char *const scratch = strdupa (name.c_str ());

	// Add working directory if relative filename.
	if (*scratch != '/')
		add_cwd (stack);

	// Parse given filename via stack.
	char *ptr,
	     *element = strtok_r (scratch, directory_delim, &ptr);
	while (element)
	{
		if (!strcmp (element, ".."))		// `..' => Up one level.
		{
			if (!stack.empty ())
				stack.pop_back ();
		}
		else if (strcmp (element, "."))		// `.' => Skipped.
			stack.push_back (element);
		element = strtok_r (0, directory_delim, &ptr);
	}

	// Get absolute filename from stack.
	name.clear ();
	get_filename (stack, name);
}
