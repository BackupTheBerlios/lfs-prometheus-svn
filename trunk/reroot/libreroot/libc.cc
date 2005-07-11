// Pointers to overridden libc functions initialization
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

// Make sure we get definitions rather than declarations of libc pointers.
#define LIBC_CC

#include <cerrno>
#include <cstdlib>
#include <dlfcn.h>
#include <error.h>

#include "libc.h"

using namespace std;

namespace
{
	// Error messages.
	char const symbol_error [] = "libreroot: Cannot load symbol: %s: %s",
	           alloc_error [] = "libreroot: Cannot allocate memory";

	// Wrapper for dlopen.
	void *
	load_symbol (char const *const name)
	{
		// Return symbol address if found.
		void *const ptr = dlsym (RTLD_NEXT, name);
		if (ptr)
			return ptr;

		// If we get this far, something's gone wrong.
		error (1, 0, symbol_error, name, dlerror ());
		return 0;	// Never get here but prevent gcc warning.
	}

	// Helper to simplify tedious casting.
	template <typename type>
	inline void
	dl (type &ptr, char const *const name)
	{
		ptr = type (load_symbol (name));
	}

	// Get addresses of libc functions we override.
	void __attribute__ ((constructor))
	init_pointers ()
	{
		// Process persona.
		dl (libc::geteuid, "geteuid");
		dl (libc::getuid, "getuid");
		dl (libc::getegid, "getegid");
		dl (libc::getgid, "getgid");
		dl (libc::seteuid, "seteuid");
		dl (libc::setuid, "setuid");
		dl (libc::setegid, "setegid");
		dl (libc::setgid, "setgid");
		dl (libc::setreuid, "setreuid");
		dl (libc::setregid, "setregid");

		// Current working directory.
		dl (libc::getcwd, "getcwd");
		dl (libc::getwd, "getwd");
		dl (libc::get_current_dir_name, "get_current_dir_name");
		dl (libc::chdir, "chdir");
		dl (libc::fchdir, "fchdir");
	}
}

// Wrapper for malloc.
void * __attribute__ ((malloc))
reroot::do_alloc (size_t const size)
{
	void *ptr = malloc (size);

	if (ptr)
		return ptr;

	error (1, errno, alloc_error);
	return 0;	// Never get here but prevent gcc warning.
}

// Wrapper for realloc.
void * __attribute__ ((malloc))
reroot::do_realloc (void *ptr, size_t const newsize)
{
	ptr = realloc (ptr, newsize);

	if (ptr)
		return ptr;

	error (1, errno, alloc_error);
	return 0;	// Never get here but prevent gcc warning.
}
