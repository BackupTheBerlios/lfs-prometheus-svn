// libreroot pointers to overridden libc functions
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

# include <stdio.h>
# include <sys/types.h>
# include <unistd.h>

// Make sure pointers are external declarations except in libc.c.
# ifdef LIBC_C
#  define EXTERN
# else
#  define EXTERN extern
# endif

// Process persona.
EXTERN uid_t (*libc_getuid) (void);
EXTERN gid_t (*libc_getgid) (void);
EXTERN uid_t (*libc_geteuid) (void);
EXTERN gid_t (*libc_getegid) (void);
EXTERN int (*libc_setuid) (uid_t);
EXTERN int (*libc_setgid) (gid_t);
EXTERN int (*libc_seteuid) (uid_t);
EXTERN int (*libc_setegid) (gid_t);

// Filesystem.

// Opening files.
EXTERN FILE *(*libc_fopen) (char const *, char const *);

#endif