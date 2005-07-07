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
#include <unistd.h>

#include "error.h"
#include "filename.h"
#include "stringlist.h"
#include "stringtok.h"

using namespace std;

// Reroot parameters.
string const reroot::filename::false_root;
reroot::string_list const reroot::filename::exclude_path;

// Reset filename.
void
reroot::filename::clear ()
{
	absolute_name.clear ();
	original_name.clear ();
	rerooted_name.clear ();
	status = unknown;
}

// Return the absolute filename.
string const &
reroot::filename::get_absolute () const
{
	if (absolute_name.empty ())
	{
		// Can we remove the false root from rerooted?
		if (rerooted_name.length ())
			rerooted_to_absolute ();

		// Or can we make the original absolute?
		else if (original_name.length ())
			make_absolute (absolute_name, original_name);
	}
	return absolute_name;
}

// Return the original filename.
string const &
reroot::filename::get_original () const
{
	// If necessary use absolute filename.
	if (original_name.empty ())
		original_name = get_absolute ();

	return original_name;
}

// Return the rerooted filename.
string const &
reroot::filename::get_rerooted () const
{
	// Reroot absolute if necessary & possible.
	if (rerooted_name.empty ())
		if (get_absolute ().length ())
			absolute_to_rerooted ();

	return rerooted_name;
}

// Return true if the file is exluded, i.e. the absolute filename is under a
// directory in the exclude path.
bool
reroot::filename::is_excluded () const
{
	// Have we already worked it out?
	if (status == excluded)
		return true;
	if (status == rerooted)
		return false;

	// If no rerooting is happening, can assume excluded.  Otherwise check
	// if absolute filename is in exclude path.
	if (!rerooting () || check_exclude_path (get_absolute ()))
	{
		// File is excluded.
		status = excluded;
		return true;
	}

	// No excluded directories found in filename.
	status = rerooted;
	return false;
}

// Initialize false_root from the REROOT_FALSE_ROOT environment variable.
void
reroot::filename::init_false_root ()
{
	// Get value of environment variable.
	char const env_var [] = "REROOT_FALSE_ROOT";
	char const *const fr = getenv (env_var);

	// Is the environment variable non-null?
	if (fr && *fr)
	{
		// Get writeable reference to false_root & set it.
		string &dir = const_cast <string &> (false_root);
		make_absolute (dir, fr);

		// If false_root is `/', clear it.
		if (dir == "/")
			dir.clear ();

		// Save absolute false root directory back to environment incase
		// any children change their working directories.
		setenv (env_var, false_root.c_str (), 1);
	}
}

// Initialize exclude_path from the REROOT_EXCLUDE_PATH environment variable.
void
reroot::filename::init_exclude_path ()
{
	// Get writeable reference to exclude_path.
	string_list &list = const_cast <string_list &> (exclude_path);

	// Get value of environment variable.
	char const env_var [] = "REROOT_EXCLUDE_PATH";
	char const *const ep = getenv (env_var);

	// Is the environment variable non-null?
	if (ep && *ep)
	{
		char path_delim = ':';

		// Parse variable.
		string directory;
		string_tok tokenizer (ep, path_delim);
		while (tokenizer >> directory)
		{
			// Get absolute filename & add to list.
			make_absolute (directory, directory);
			list.push_back (directory);
		}

		// Save absolute path back to environment incase any children
		// change their working directories.  Allow for redundant
		// leading delim.
		list.get_string (directory, path_delim);
		setenv (env_var, directory.c_str () + 1, 1);
	}

	// Insert false root directory to avoid multiple rerootings (e.g. if
	// glibc calls back into libreroot).  By default exclude all directories
	// if false_root not specified.
	list.push_front (rerooting ()? false_root : "/");
}

// Initialize false_root & exclude_path from environment variables.
void __attribute__ ((constructor))
reroot::filename::init_reroot ()
try
{
	init_false_root ();
	init_exclude_path ();
}
catch (exception const &x)
{
	error (x);
}

// Return true if the absolute filename is under a directory in the exclude
// path.
bool
reroot::filename::check_exclude_path (string const &absolute)
{
	typedef string_list::const_iterator iterator;

	iterator const end = exclude_path.end ();
	for (iterator xdir = exclude_path.begin (); xdir != end; ++xdir)
		if (absolute.find (*xdir) == 0)
			return true;

	return false;
}

// Add current working directory to string list.
void
reroot::filename::add_working_directory (string_list &list)
{
	// Get current working directory.
	char *const directory = getcwd (0, 0);

	// Add individual filename components to list.
	string component;
	string_tok tokenizer (directory, directory_delim);
	free (directory);
	while (tokenizer >> component)
		list.push_back (component);
}

// Convert original filename to an absolute filename.  Don't resolve symbolic
// links or check if the file exists.
void
reroot::filename::make_absolute (string &absolute, string const &original)
{
	string_list list;

	// Add working directory if relative filename.
	if (original [0] != directory_delim)
		add_working_directory (list);

	// Parse original filename via list.
	string component;
	string_tok tokenizer (original, directory_delim);
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

// Convert the absolute filename into a filename under the false root directory.
// If the absolute name is in an excluded directory, the rerooted filename is
// the same as the absolute.
void
reroot::filename::absolute_to_rerooted () const
{
	// If in excluded directory, don't waste time.
	if (is_excluded ())
	{
		rerooted_name = absolute_name;
		return;
	}

	// Reroot filename.
	rerooted_name.clear ();
	rerooted_name.reserve (false_root.length () + absolute_name.length ());
	rerooted_name += false_root;
	rerooted_name += absolute_name;
}

// Convert the rerooted filename into a the name the file would have were it
// really installed.
void
reroot::filename::rerooted_to_absolute () const
{
	if (rerooting () && rerooted_name.find (false_root) == 0)
		absolute_name = rerooted_name.substr (false_root.length ());
	else
		absolute_name = rerooted_name;
}
