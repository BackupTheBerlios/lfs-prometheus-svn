// User & group handling
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

#include <cerrno>
#include <cstdlib>

#include "libc.h"

using namespace std;

namespace
{
	// True if root privileges are not being simulated.
	bool limited;

	// Simulated user & group IDs.  Simulate both real & effective IDs, but
	// not file IDs.  File IDs only apply to SUID & SGID executables, which
	// libreroot won't work with anyway.
	uid_t effective_uid = 0,
	      real_uid = 0;
	gid_t effective_gid = 0,
	      real_gid = 0;

	// Initialize persona.  If the REROOT_LIMITED envirionment variable is
	// set, do not simulate root privileges.
	static void __attribute__ ((constructor))
	init_persona ()
	{
		limited = getenv ("REROOT_LIMITED");
	}
}

// Ensure overrides can be called by C code.
extern "C"
{
	// Return effective user ID.
	uid_t
	geteuid ()
	{
		return limited? libc::geteuid () : effective_uid;
	}

	// Return real user ID.
	uid_t
	getuid ()
	{
		return limited? libc::getuid () : real_uid;
	}

	// Return effective group ID.
	gid_t
	getegid ()
	{
		return limited? libc::getegid () : effective_gid;
	}

	// Return real group ID.
	gid_t
	getgid ()
	{
		return limited? libc::getgid () : real_gid;
	}

	// Set effective user ID.
	int
	seteuid (uid_t const newuid)
	{
		if (limited)
			return libc::seteuid (newuid);

		// Can only change effective user ID to real user ID unless
		// privileged.
		if (!effective_uid || newuid == real_uid)
		{
			effective_uid = newuid;
			return 0;
		}

		errno = EPERM;
		return -1;
	}

	// Set effective user ID, & real also if privileged.
	int
	setuid (uid_t const newuid)
	{
		if (limited)
			return libc::setuid (newuid);

		if (!effective_uid)
		{
			effective_uid = real_uid = newuid;
			return 0;
		}

		// Can only change effective user ID to real user ID unless
		// privileged.
		if (newuid == real_uid)
		{
			effective_uid = newuid;
			return 0;
		}

		errno = EPERM;
		return -1;
	}

	// Set effective group ID.
	int
	setegid (gid_t const newgid)
	{
		if (limited)
			return libc::setegid (newgid);

		// Can only change effective group ID to real group ID unless
		// privileged.
		if (!effective_uid || newgid == real_gid)
		{
			effective_gid = newgid;
			return 0;
		}

		errno = EPERM;
		return -1;
	}

	// Set effective group ID, & real also if privileged.
	int
	setgid (gid_t const newgid)
	{
		if (limited)
			return libc::setgid (newgid);

		if (!effective_uid)
		{
			effective_gid = real_gid = newgid;
			return 0;
		}

		// Can only change effective group ID to real group ID unless
		// privileged.
		if (newgid == real_gid)
		{
			effective_gid = newgid;
			return 0;
		}

		errno = EPERM;
		return -1;
	}
}
