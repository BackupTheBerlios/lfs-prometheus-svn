// filename
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
#include <string>

#include "filename.h"
#include "stringlist.h"
#include "stringtok.h"

using namespace std;

namespace
{
	char const directory_delim = '/';

	string const false_root;
	reroot::string_list const exclude_path;

	// Add current working directory to string list.
	void
	add_cwd (reroot::string_list &list)
	{
		// Get current working directory.
		// FIXME: Use filesystem interface functions.
		string const directory = getenv ("PWD")?: "/home/gareth";

		// Add individual filename components to list.
		string component;
		reroot::string_tok tokenizer (directory, directory_delim);
		while (tokenizer >> component)
			list.push_back (component);
	}

	// Convert original filename to an absolute filename.  Don't resolve
	// symbolic links or check if the file exists.
	void
	make_absolute (string &absolute, string const &original)
	{
		reroot::string_list list;

		// Add working directory if relative filename.
		if (original [0] != directory_delim)
			add_cwd (list);

		// Parse original filename via list.
		string component;
		reroot::string_tok tokenizer (original, directory_delim);
		while (tokenizer >> component)
		{
			// `..' => Up one level.
			if (component == "..")
			{
				if (!list.empty ())
					list.pop_back ();
			}

			// `.' => Skipped.
			else if (component != ".")
				list.push_back (component);
		}

		// Get absolute filename from list.
		list.get_string (absolute, directory_delim);

		// If we `..'-ed past /, restore it.
		if (absolute.empty ())
			absolute = "/";
	}

	// Initialize false_root from the REROOT_FALSE_ROOT environment
	// variable.
	void
	init_false_root ()
	{
		// Get value of environment variable.
		char const env_var [] = "REROOT_FALSE_ROOT";
		char const *const fr = getenv (env_var);

		// Is the environment variable non-null?
		if (fr && *fr)
		{
			// Save absolute false root directory back to
			// environment incase any children change their working
			// directories.
			make_absolute (const_cast <string &> (false_root), fr);
			setenv (env_var, false_root.c_str (), 1);
		}
	}

	// Initialize exclude_path from the REROOT_EXCLUDE_PATH environment
	// variable.
	void
	init_exclude_path ()
	{
		// Get writeable reference to exclude_path.
		reroot::string_list &list =
			const_cast <reroot::string_list &> (exclude_path);

		// Get value of environment variable.
		char const env_var [] = "REROOT_EXCLUDE_PATH";
		char const *const ep = getenv (env_var);

		// Is the environment variable non-null?
		if (ep && *ep)
		{
			char path_delim = ':';

			// Parse variable.
			string directory;
			reroot::string_tok tokenizer (ep, path_delim);
			while (tokenizer >> directory)
			{
				// Get absolute filename & add to list.
				make_absolute (directory, directory);
				list.push_back (directory);
			}

			// Save absolute path back to environment incase any
			// children change their working directories.  Allow for
			// redundant leading delim.
			list.get_string (directory, path_delim);
			setenv (env_var, directory.c_str () + 1, 1);
		}

		// Insert false root directory to avoid multiple rerootings
		// (e.g. if glibc calls back into libreroot).  By default
		// exclude all directories if false_root not specified.
		list.push_front (false_root.length ()? false_root : "/");
	}

	// Initialize false_root & exclude_path from environment variables.
	void __attribute__ ((constructor))
	init_reroot ()
	{
		init_false_root ();
		init_exclude_path ();
	}

	// Remove the false root directory from the beginning of a rerooted
	// filename, to yield the absolute filename it would have were it really
	// installed.
	void
	remove_false_root (string &absolute, string const &rerooted)
	{
		string::size_type const length = false_root.length ();
		if (length && rerooted.find (false_root) == 0)
			absolute = rerooted.substr (length);
		else
			absolute = rerooted;
	}

	// Convert the absolute filename into a filename under the false root
	// directory.  If the requested name is in an excluded directory, the
	// rerooted filename is the same as the absolute.
	void
	add_false_root (string &rerooted, string const &absolute)
	{
		// If no false root specified, don't waste time.
		if (false_root.empty ())
		{
			rerooted = absolute;
			return;
		}

		// Check for excluded directories.
		typedef reroot::string_list::const_iterator iterator;
		iterator const end = exclude_path.end ();
		for (iterator xdir = exclude_path.begin (); xdir != end; ++xdir)
			if (absolute.find (*xdir) == 0)
			{
				rerooted = absolute;
				return;
			}

		// Reroot filename.
		rerooted.clear ();
		rerooted.reserve (false_root.length () + absolute.length ());
		rerooted += false_root;
		rerooted += absolute;
	}
}

// Set the absolute filename.  Clear other names.
void
reroot::filename::set_absolute (string const &name)
{
	absolute = name;
	original.clear ();
	rerooted.clear ();
}

// Set the original filename.  Clear other names.
void
reroot::filename::set_original (string const &name)
{
	original = name;
	absolute.clear ();
	rerooted.clear ();
}

// Set the rerooted filename.  Clear other names.
void
reroot::filename::set_rerooted (string const &name)
{
	rerooted = name;
	absolute.clear ();
	original.clear ();
}

// Return the absolute filename.
string const &
reroot::filename::get_absolute () const
{
	if (absolute.empty ())
	{
		// Can we remove the false root from the rerooted?
		if (rerooted.length ())
			remove_false_root (absolute, rerooted);

		// Or can we make the original absolute?
		else if (original.length ())
			make_absolute (absolute, original);
	}
	return absolute;
}

// Return the original filename.
string const &
reroot::filename::get_original () const
{
	if (original.empty ())
	{
		// Can we use the absolute?
		if (absolute.length ())
			original = absolute;

		// Or remove the false root from the rerooted?
		else if (rerooted.length ())
			remove_false_root (original, rerooted);
	}
	return original;
}

// Return the rerooted filename.
string const &
reroot::filename::get_rerooted () const
{
	if (rerooted.empty ())
	{
		// Can & should we generate the absolute?
		if (absolute.empty () && original.length ())
			get_absolute ();

		// Can we reroot absolute?
		if (absolute.length ())
			add_false_root (rerooted, absolute);
	}
	return rerooted;
}
