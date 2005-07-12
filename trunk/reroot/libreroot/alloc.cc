// malloc & realloc wrapper functions
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
#include <error.h>

#include "alloc.h"

namespace
{
	// Error message.
	char const alloc_error [] = "libreroot: Cannot allocate memory";
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
