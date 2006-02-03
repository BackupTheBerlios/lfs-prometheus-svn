// Copyright (C) 2003-2006 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
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
#include <unistd.h>

#include "absolute.h"
#include "stringlist.h"
#include "tokenizer.h"

using namespace reroot;
using namespace std;

// Convert original filename to an absolute filename.  Don't resolve symbolic
// links or check if the file exists.
void reroot::absolute (string &abs, string const &orig)
{
	StringList list;

	// Add working directory if filename is relative.
	if (orig [0] != '/')
	{
		char *const cwd = getcwd (0, 0);
		list.append_string (cwd);
		free (cwd);
	}

	// Parse original filename.
	if (orig.length ())
		list.append_filename (orig);

	// Get absolute filename from list.
	list.get_filename (abs);

	// If we `..'-ed past /, restore it.
	if (abs.empty ())
		abs = '/';
}

void reroot::relative (string &/*rel*/, string const &/*orig*/)
{
}
