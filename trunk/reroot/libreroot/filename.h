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
# include "stringlist.h"

namespace reroot
{
	bool rerooted ();
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
		// C'tor.  Compiler generated copy c'tor is sufficient.
		filename (std::string const &name = "");

		// For setting filename.
		void set_absolute (std::string const &name);
		void set_original (std::string const &name);
		void set_rerooted (std::string const &name);

		// For getting filename.
		std::string const &get_absolute () const;
		std::string const &get_original () const;
		std::string const &get_rerooted () const;

		// For assigning filename.  Compiler generated copy assignment
		// operator is sufficient.
		filename const &operator = (std::string const &name);

		// For getting reroot parameters.
		static bool is_rerooted ();
		static std::string const &get_false_root ();
		static string_list const &get_exclude_path ();

	private:
		// Filename data.
		mutable std::string absolute,
		                    original,
		                    rerooted;

		// Reroot parameters.
		static char const directory_delim = '/';
		static std::string const false_root;
		static string_list const exclude_path;

		// For manipulating filenames.
		static void add_working_directory (string_list &list);
		static void make_absolute (std::string &absolute,
			std::string const &original);
		static void add_false_root (std::string &rerooted,
			std::string const &absolute);
		static void remove_false_root (std::string &absolute,
			std::string const &rerooted);

		// For initializing reroot parameters.
		static void init_false_root ();
		static void init_exclude_path ();
		static void init_reroot ();
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

// Return true if filenames are to be rerooted.
inline bool
reroot::filename::is_rerooted ()
{
	static bool const rerooted = false_root.length ();
	return rerooted;
}

// Return the absolute filename of the false root directory.
inline std::string const &
reroot::filename::get_false_root ()
{
	return false_root;
}

// Return the exclude path list as absolute filenames.
inline reroot::string_list const &
reroot::filename::get_exclude_path ()
{
	return exclude_path;
}

#endif
