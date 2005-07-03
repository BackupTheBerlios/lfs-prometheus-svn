// Pointers to overridden libc functions
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

#ifndef LIBC_H
# define LIBC_H

# include <sys/types.h>
# include <unistd.h>

// Make sure pointers are external declarations except in libc.cc.
# ifdef LIBC_CC
#  define EXTERN
# else
#  define EXTERN extern
# endif

namespace libc
{
	// Pointer to C function types.
	extern "C"
	{
		// Process persona.
		typedef uid_t (*getuid_t) ();
		typedef gid_t (*getgid_t) ();
		typedef int (*setuid_t) (uid_t);
		typedef int (*setgid_t) (gid_t);
		typedef int (*setreuid_t) (uid_t, uid_t);
		typedef int (*setregid_t) (gid_t, gid_t);
	}

	// Process persona.
	EXTERN getuid_t geteuid;
	EXTERN getuid_t getuid;
	EXTERN getgid_t getegid;
	EXTERN getgid_t getgid;
	EXTERN setuid_t seteuid;
	EXTERN setuid_t setuid;
	EXTERN setgid_t setegid;
	EXTERN setgid_t setgid;
	EXTERN setreuid_t setreuid;
	EXTERN setregid_t setregid;
}

# undef EXTERN
#endif
