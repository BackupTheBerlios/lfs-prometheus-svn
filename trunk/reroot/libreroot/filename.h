// filename declaration
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

#ifndef FILENAME_H
# define FILENAME_H

# include <string>

namespace reroot
{
	class filename;
}

// A filename.  Filenames can come in three forms:
// 1) the original filename, which may be relative and/or contain `.', `..' or
//    multiple `/'s;
// 2) the absolute filename as it would be installed on the system;
// 3) the rerooted filename, including the false root directory.
// This class eases storage of these three names, & (lazily) converts between
// them.
class reroot::filename
{
	public:
		filename (std::string const &name = "");

		void set_absolute (std::string const &name);
		void set_original (std::string const &name);
		void set_rerooted (std::string const &name);

		std::string const &get_absolute () const;
		std::string const &get_original () const;
		std::string const &get_rerooted () const;

		filename const &operator = (std::string const &name);

	private:
		mutable std::string absolute,
		                    original,
		                    rerooted;
};

// C'tor.  Set the original filename.
inline
reroot::filename::filename (std::string const &name):
	original (name)
{}

// Set the original filename.
inline reroot::filename const &
reroot::filename::operator = (std::string const &name)
{
	set_original (name);
	return *this;
}

#endif
