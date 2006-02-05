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

// Functions for converting between absolute and relative filenames.

#ifndef ABSOLUTE_H
# define ABSOLUTE_H

# include <string>
# include "libreroot.h"

namespace reroot
{
	// For getting absolute filenames.
	shared void absolute (std::string &filename);
	shared void absolute (std::string &abs, std::string const &orig);

	// For getting relative filenames.
	shared void relative (std::string &filename);
	shared void relative (std::string &rel, std::string const &orig);
	shared void relative (std::string &rel, std::string const &orig,
		std::string const &dir);
}

// Convert filename to an absolute filename.  Don't resolve symbolic links or
// check if the file exists.
inline void reroot::absolute (std::string &filename)
{
	absolute (filename, filename);
}

// Convert filename to a filename relative to the current working directory.
// Don't resolve symbolic links or check if the file exists.
inline void reroot::relative (std::string &filename)
{
	relative (filename, filename);
}

#endif
