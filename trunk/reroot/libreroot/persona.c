// libreroot user & group handling
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

#include <stdbool.h>
#include <stdlib.h>	// getenv.

#include "reroot.h"

// True if root privileges are not being simulated.
static bool const reroot_limited;

// Initialize persona.  If the REROOT_LIMITED envirionment variable is set, do
// not simulate root privileges.
static void __attribute__ ((constructor))
reroot_persona_init ()
{
	*(bool *) &reroot_limited = getenv ("REROOT_LIMITED");
}

// Return `real' user ID.
uid_t
getuid ()
{
	return reroot_limited? libc_getuid () : 0;
}

// Return `real' group ID.
gid_t
getgid ()
{
	return reroot_limited? libc_getgid () : 0;
}

// Return `effective' user ID.
uid_t
geteuid ()
{
	return reroot_limited? libc_geteuid () : 0;
}

// Return `effective' group ID.
gid_t
getegid ()
{
	return reroot_limited? libc_getegid () : 0;
}
