// Current working directory.
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

// FIXME: Need file access, type etc. testing functions.

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <string>

#include "alloc.h"
#include "error.h"
#include "filename.h"
#include "libc.h"

using namespace std;
using reroot::alloc;
using reroot::filename;

namespace
{
	string working_directory;

	// Initialize working directory.
	void __attribute__ ((constructor))
	init_working_directory ()
	try
	{
		// Get value of PWD environment variable.
		char const env_var [] = "PWD";
		char *pwd = getenv (env_var);

		// Is the environment variable non-null?
		if (pwd && *pwd)
			working_directory = pwd;
		else
		{
			// Query libc.
			pwd = libc::getcwd (0, 0);
			working_directory = pwd;

			// Export PWD for children.
			setenv (env_var, pwd, 1);

			free (pwd);
		}
	}
	catch_all
}

// Ensure overrides can be called by C code.
extern "C"
{
	// Return absolute filename of current working directory.  Use given
	// buffer or allocate one as necessary.
	// FIXME: Check for permission to read/traverse working directory.
	char *
	getcwd (char *buffer, size_t const size)
	try
	{
		if (!filename::rerooting ())
			return libc::getcwd (buffer, size);

		// Get length of working directory name, including terminator.
		string::size_type const len = working_directory.length () + 1;

		// If given a size less than the length, fail.
		if (size && size < len)
		{
			errno = ERANGE;
			return 0;
		}

		// Were we given a buffer?
		if (buffer)
		{
			if (!size)
			{
				// We've been given a buffer with zero size?
				errno = EINVAL;
				return 0;
			}
		}
		else
		{
			// Allocate buffer of requested size, or as needed if no
			// size specified.
			if (size)
				buffer = alloc <char> (size);
			else
				buffer = alloc <char> (len);
		}

		// Fill & return buffer.
		memcpy (buffer, working_directory.c_str (), len);
		return buffer;
	}
	catch_all

	// Return absolute filename of current working directory.  Use given
	// buffer, which is assumed to be large enough.  This is a deprecated
	// function...
	// FIXME: Check for permission to read/traverse working directory.
	char *
	getwd (char *const buffer)
	try
	{
		if (!filename::rerooting ())
			return libc::getwd (buffer);

		// Make sure we have a buffer.
		if (!buffer)
		{
			errno = EINVAL;
			return 0;
		}

		// Get length of working directory name, including terminator.
		string::size_type const len = working_directory.length () + 1;

		// Buffer is required to be at least PATH_MAX sized, but
		// GNU/Linux filenames may be longer.
		if (len > PATH_MAX)
		{
			errno = ERANGE;
			return 0;
		}

		// Fill & return buffer.
		memcpy (buffer, working_directory.c_str (), len);
		return buffer;
	}
	catch_all

	// Return absolute filename of current working directory.
	// FIXME: Check for permission to read/traverse working directory.
	char *
	get_current_dir_name ()
	try
	{
		if (!filename::rerooting ())
			return libc::get_current_dir_name ();

		// Get length of working directory name, including terminator.
		string::size_type const len = working_directory.length () + 1;

		// Fill & return buffer.
		char *const buffer = alloc <char> (len);
		memcpy (buffer, working_directory.c_str (), len);
		return buffer;
	}
	catch_all

	int
	chdir (char const *const name)
	{
		return libc::chdir (name);
	}

	int
	fchdir (int const filedes)
	{
		return libc::fchdir (filedes);
	}
}
