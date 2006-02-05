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

#include <unistd.h>

#include "absolute.h"
#include "cstringptr.h"
#include "stringlist.h"
#include "tokenizer.h"

using namespace reroot;
using namespace std;

namespace
{
	// Convert original filename to an absolute filename & store it in list.
	void absolute_list (StringList &abs, string const &orig)
	{
		// Add working directory if filename is relative.
		if (orig [0] != '/')
			abs.append_string (CStringPtr (getcwd (0, 0)));

		// Append original filename.
		if (!orig.empty ())
			abs.append_filename (orig);
	}
}

// Convert original filename to an absolute filename.  Don't resolve symbolic
// links or check if the file exists.
void reroot::absolute (string &abs, string const &orig)
{
	// Add absolute filename to list.
	StringList list;
	absolute_list (list, orig);

	// Get absolute filename from list.
	list.get_filename (abs);

	// If we `..'-ed past /, restore it.
	if (abs.empty ())
		abs = '/';
}

// Convert original filename to a filename relative to the current working
// directory.  Don't resolve symbolic links or check if the file exists.
void reroot::relative (string &rel, string const &orig)
{
	relative (rel, orig, CStringPtr (getcwd (0, 0)));
}

// Convert original filename to a filename relative to dir.  Don't resolve
// symbolic links or check if the file exists.
void reroot::relative (string &rel, string const &orig, string const &dir)
{
	// Get absolute filename in list.
	StringList filename;
	absolute_list (filename, orig);

	{
		// Get the reference directory's absolute name in a list, & an
		// iterator pointing to the start.
		StringList const directory;
		absolute_list (const_cast <StringList &> (directory), dir);
		StringList::const_iterator i = directory.begin ();

		// Remove elements of the filename that are shared with the
		// reference directory.
		for (; i != directory.end (); ++i)
		{
			if (filename.empty () || *i != *filename.begin ())
				break;

			filename.pop_front ();
		}

		// Prepend the filename with `..' for each different element of
		// the reference directory name.
		for (; i != directory.end (); ++i)
			filename.push_front ("..");
	}

	// Get relative filename from list.
	filename.get_string (rel);

	// If filename is the directory name, the relative filename will be
	// empty.
	if (rel.empty ())
		rel = '.';
}
