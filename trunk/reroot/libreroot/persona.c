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

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

#include "libc.h"

// True if root privileges are not being simulated.
static bool const reroot_limited;

// Simulated user & group IDs.  Simulate both real & effective IDs, but not file
// IDs.  File IDs only apply to SUID & SGID executables, which libreroot won't
// work with anyway.
static uid_t reroot_effective_uid = 0,
             reroot_effective_gid = 0,
             reroot_real_uid = 0,
             reroot_real_gid = 0;

// Initialize persona.  If the REROOT_LIMITED envirionment variable is set, do
// not simulate root privileges.
static void __attribute__ ((constructor))
reroot_persona_init ()
{
	*(bool *) &reroot_limited = getenv ("REROOT_LIMITED");
}

// Return real user ID.
uid_t
getuid ()
{
	return reroot_limited? libc_getuid () : reroot_real_uid;
}

// Return real group ID.
gid_t
getgid ()
{
	return reroot_limited? libc_getgid () : reroot_real_gid;
}

// Return effective user ID.
uid_t
geteuid ()
{
	return reroot_limited? libc_geteuid () : reroot_effective_uid;
}

// Return effective group ID.
gid_t
getegid ()
{
	return reroot_limited? libc_getegid () : reroot_effective_gid;
}

// Set effective user ID, & real also if privileged.
int
setuid (uid_t newuid)
{
	if (reroot_limited)
		return libc_setuid (newuid);

	if (!reroot_effective_uid)
	{
		reroot_effective_uid = reroot_real_uid = newuid;
		return 0;
	}

	// Can only change effective user ID to real user ID unless privileged.
	if (newuid == reroot_real_uid)
	{
		reroot_effective_uid = newuid;
		return 0;
	}

	errno = EPERM;
	return -1;
}

// Set effective group ID, & real also if privileged.
int
setgid (gid_t newgid)
{
	if (reroot_limited)
		return libc_setgid (newgid);

	if (!reroot_effective_uid)
	{
		reroot_effective_gid = reroot_real_gid = newgid;
		return 0;
	}

	// Can only change effective group ID to real group ID unless
	// privileged.
	if (newgid == reroot_real_gid)
	{
		reroot_effective_gid = newgid;
		return 0;
	}

	errno = EPERM;
	return -1;
}

// Set effective user ID.
int
seteuid (uid_t newuid)
{
	if (reroot_limited)
		return libc_seteuid (newuid);

	// Can only change effective user ID to real user ID unless privileged.
	if (!reroot_effective_uid || newuid == reroot_real_uid)
	{
		reroot_effective_uid = newuid;
		return 0;
	}

	errno = EPERM;
	return -1;
}

// Set effective group ID.
int
setegid (gid_t newgid)
{
	if (reroot_limited)
		return libc_setegid (newgid);

	// Can only change effective group ID to real group ID unless
	// privileged.
	if (!reroot_effective_uid || newgid == reroot_real_gid)
	{
		reroot_effective_gid = newgid;
		return 0;
	}

	errno = EPERM;
	return -1;
}
