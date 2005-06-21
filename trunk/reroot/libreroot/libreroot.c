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

#include <dlfcn.h>
// Use: libc_funcname = dlsym (RTLD_NEXT, "funcname");
// Returns 0 if error, use dlerror () to return error string.  No need to free.

#include "libreroot.h"
#include "reroot.h"

// Initialize libreroot.  This function is called automatically when libreroot
// is loaded.
static void __attribute__ ((constructor))
reroot_start ()
{
	// Initialize global variables based on environment variables.
	reroot_env_init ();
}

// Destroy libreroot.  This function is called automatically when libreroot is
// unloaded.
static void __attribute__ ((destructor))
reroot_finish ()
{
	// Deallocate global variables.
	reroot_env_destroy ();
}
