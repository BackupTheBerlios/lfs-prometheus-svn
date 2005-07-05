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
	// Simulated user & group IDs.  Simulate both real & effective IDs, but
	// not file IDs.  File IDs only apply to SUID & SGID executables, which
	// libreroot won't work with anyway.
	uid_t effective_uid = 0,
	      real_uid = 0;
	gid_t effective_gid = 0,
	      real_gid = 0;

	// Return true if root privileges are not being simulated.
	inline bool
	is_limited ()
	{
		static bool const limited = getenv ("REROOT_LIMITED");
		return limited;
	}
}

// Ensure overrides can be called by C code.
extern "C"
{
	// Return effective user ID.
	uid_t
	geteuid ()
	{
		return is_limited ()? libc::geteuid () : effective_uid;
	}

	// Return real user ID.
	uid_t
	getuid ()
	{
		return is_limited ()? libc::getuid () : real_uid;
	}

	// Return effective group ID.
	gid_t
	getegid ()
	{
		return is_limited ()? libc::getegid () : effective_gid;
	}

	// Return real group ID.
	gid_t
	getgid ()
	{
		return is_limited ()? libc::getgid () : real_gid;
	}

	// Set effective user ID.
	int
	seteuid (uid_t const newuid)
	{
		if (is_limited ())
			return libc::seteuid (newuid);

		if (newuid == effective_uid)
			return 0;

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
		if (is_limited ())
			return libc::setuid (newuid);

		if (!effective_uid)
		{
			effective_uid = real_uid = newuid;
			return 0;
		}

		// Can only change effective user ID to real user ID unless
		// privileged.
		if (newuid == real_uid || newuid == effective_uid)
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
		if (is_limited ())
			return libc::setegid (newgid);

		if (newgid == effective_gid)
			return 0;

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
		if (is_limited ())
			return libc::setgid (newgid);

		if (!effective_uid)
		{
			effective_gid = real_gid = newgid;
			return 0;
		}

		// Can only change effective group ID to real group ID unless
		// privileged.
		if (newgid == real_gid || newgid == effective_gid)
		{
			effective_gid = newgid;
			return 0;
		}

		errno = EPERM;
		return -1;
	}

	// For privileged processes set real & effective user IDs.  Unprivileged
	// processes can only swap real & effective.  Values of -1 indicate not
	// to change.
	int
	setreuid (uid_t const ruid, uid_t const euid)
	{
		if (is_limited ())
			return libc::setreuid (ruid, euid);

		// Check we're allowed to do this.
		if (effective_uid && ((ruid != uid_t (-1) && ruid != real_uid &&
		    ruid != effective_uid) || (euid != uid_t (-1) &&
		    euid != real_uid && euid != effective_uid)))
		{
			errno = EPERM;
			return -1;
		}

		if (ruid != uid_t (-1))
			real_uid = ruid;

		if (euid != uid_t (-1))
			effective_uid = euid;

		return 0;
	}

	// For privileged processes set real & effective group IDs.
	// Unprivileged processes can only swap real & effective.  Values of -1
	// indicate not to change.
	int
	setregid (gid_t const rgid, gid_t const egid)
	{
		if (is_limited ())
			return libc::setregid (rgid, egid);

		// Check we're allowed to do this.
		if (effective_uid && ((rgid != gid_t (-1) && rgid != real_gid &&
		    rgid != effective_gid) || (egid != gid_t (-1) &&
		    egid != real_gid && egid != effective_gid)))
		{
			errno = EPERM;
			return -1;
		}

		if (rgid != gid_t (-1))
			real_gid = rgid;

		if (egid != gid_t (-1))
			effective_gid = egid;

		return 0;
	}
}
