// libreroot initialization
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
#define LIBREROOT_C

#include <dlfcn.h>	// dlerror & dlsym.
#include <error.h>	// error.

#include "libreroot.h"

// Wrapper for dlopen.
static void *
reroot_dlsym (char const *const restrict name)
{
	// Return symbol address if found.
	void *const restrict ptr = dlsym (RTLD_NEXT, name);
	if (ptr)
		return ptr;

	// If we get this far, something's gone wrong.
	error (1, 0, "libreroot: Cannot load symbol: %s: %s", name, dlerror ());
	return 0;	// Never get here but prevent gcc warning.
}

// Get addresses of libc functions we override.
static void __attribute__ ((constructor))
reroot_overrides_init ()
{
	libc_getuid = reroot_dlsym ("getuid");
	libc_getgid = reroot_dlsym ("getgid");
	libc_geteuid = reroot_dlsym ("geteuid");
	libc_getegid = reroot_dlsym ("getegid");
	libc_fopen = reroot_dlsym ("fopen");
}
