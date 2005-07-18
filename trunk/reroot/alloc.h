// malloc & realloc wrapper function declarations
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

#ifndef ALLOC_H
# define ALLOC_H

# include <cstdlib>

namespace reroot
{
	// Wrappers for C-style memory allocation.
	void *do_alloc (size_t const size) __attribute__ ((malloc));
	void *do_realloc (void *const ptr, size_t const newsize)
		__attribute__ ((malloc));

	// Helper to simplify tedious casting with alloc.
	template <typename type>
	inline type * __attribute__ ((malloc))
	alloc (size_t const size)
	{
		return reinterpret_cast <type *> (do_alloc (size));
	}

	// Helper to simplify tedious casting with realloc.
	template <typename type>
	inline type * __attribute__ ((malloc))
	realloc (type *const ptr, size_t const newsize)
	{
		return reinterpret_cast <type *> (do_realloc (ptr, newsize));
	}
}

#endif
