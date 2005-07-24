// File metadata
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

#include <fstream>
#include <stdexcept>

#include "file.h"
#include "rerootd.h"

namespace
{
	std::ifstream &operator >> (std::ifstream &is, reroot::file &/*file*/) {return is;} // FIXME.

	std::ofstream &operator << (std::ofstream &os, reroot::file const &/*file*/) {return os;} // FIXME.

	std::ifstream &operator >> (std::ifstream &is, reroot::file_db &/*db*/) {return is;} // FIXME.

	std::ofstream &operator << (std::ofstream &os, reroot::file_db const &/*db*/) {return os;} // FIXME.
}

// If index file exists, read it into file database.
void
reroot::read_index (file_db &db)
{
	std::ifstream is (index_file.c_str ());
	if (is)
		is >> db;
}

// Write file database to index file.
void
reroot::write_index (file_db const &db)
{
	// Error message.
	static char const file_error [] = "Cannot open index file: ",
	                  write_error [] = "Error writing to index file: ";

	// Try to open & write file.
	std::ofstream os (index_file.c_str ());
	if (os)
		os << db;
	else
		throw std::runtime_error (file_error + index_file);

	// Check write succeeded.
	if (!os)
		throw std::runtime_error (write_error + index_file);
}

void reroot::cleanup (file_db &/*db*/) {} // FIXME.
