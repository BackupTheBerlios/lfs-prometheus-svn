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
		// For getting reroot parameters.
		static bool rerooting ();
		static std::string const &get_false_root ();

		// C'tor.  Compiler generated copy c'tor is sufficient.
		filename (std::string const &name = "");

		// For resetting filename.
		void clear ();

		// For setting filename.
		void set_absolute (std::string const &name);
		void set_original (std::string const &name);
		void set_rerooted (std::string const &name);

		// For getting filename.
		std::string const &get_absolute () const;
		std::string const &get_original () const;
		std::string const &get_rerooted () const;

		// About filename.
		bool is_excluded () const;
		bool is_rerooted () const;

		// For assigning filename.  Compiler generated copy assignment
		// operator is sufficient.
		filename const &operator = (std::string const &name);

	private:
		// Possible filename statuses.
		enum status_type
		{
			unknown,
			excluded,
			rerooted
		};

		// Reroot parameters.
		static char const directory_delim = '/';
		static std::string const false_root;
		static string_list const exclude_path;

		// For initializing reroot parameters.
		static void init_false_root ();
		static void init_exclude_path ();
		static void init_reroot ();

		// Helper functions.
		static bool check_exclude_path (std::string const &absolute);
		static void add_working_directory (string_list &list);
		static void make_absolute (std::string &absolute,
			std::string const &original);

		// Filename data.
		mutable std::string absolute_name,
		                    original_name,
		                    rerooted_name;
		mutable status_type status;

		// For rerooting filenames.
		void absolute_to_rerooted () const;
		void rerooted_to_absolute () const;
};

// Return true if filenames are to be rerooted.
inline bool
reroot::filename::rerooting ()
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

// C'tor.  Set the original filename.
inline
reroot::filename::filename (std::string const &name):
	original_name (name),
	status (unknown)
{}

// Set the absolute filename.  Clear other names.
inline void
reroot::filename::set_absolute (std::string const &name)
{
	clear ();
	absolute_name = name;
}

// Set the original filename.  Clear other names.
inline void
reroot::filename::set_original (std::string const &name)
{
	clear ();
	original_name = name;
}

// Set the rerooted filename.  Clear other names.
inline void
reroot::filename::set_rerooted (std::string const &name)
{
	clear ();
	rerooted_name = name;
}

// Return true if filename is rerooted.
inline bool
reroot::filename::is_rerooted () const
{
	return !is_excluded ();
}

// Set the original filename.
inline reroot::filename const &
reroot::filename::operator = (std::string const &name)
{
	set_original (name);
	return *this;
}

#endif
