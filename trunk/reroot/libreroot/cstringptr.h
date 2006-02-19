// Copyright (C) 2003-2006 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
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

// A class for safely storing C string pointers allocated with malloc.  This is
// very similar to std::auto_ptr but uses free instead of delete, and is
// intended for exception-safe storage of pointers returned by glibc functions.

#ifndef CSTRINGPTR_H
# define CSTRINGPTR_H

# include <cstdlib>
# include <string>

# include "libreroot.h"

namespace reroot
{
	class CStringPtr;
}

class shared reroot::CStringPtr
{
public:
	// C'tors & d'tor.
	CStringPtr (char *ptr = 0);
	CStringPtr (CStringPtr &ptr);
	~CStringPtr ();

	// Assignment operators.
	CStringPtr &operator = (CStringPtr &ptr);
	CStringPtr &operator = (char *ptr);

	// Dereference & conversion operators.
	char &operator * () const;
	operator char * () const;
	operator std::string () const;

	// For accessing the string.
	char *get () const;
	char *release ();
	void reset (char *ptr = 0);

private:
	// The actual pointer.
	char *pointer;
};

// Construct a CStringPtr that owns the addressed memory.
inline reroot::CStringPtr::CStringPtr (char *const ptr):
	pointer (ptr)
{
}

// Construct a CStringPtr that takes ownership from ptr.  Ptr is becomes a null
// pointer.
inline reroot::CStringPtr::CStringPtr (CStringPtr &ptr):
	pointer (ptr.release ())
{
}

// Free addressed memory.
inline reroot::CStringPtr::~CStringPtr ()
{
	if (pointer)
		std::free (pointer);
}

// Take ownership of addressed memory from ptr.  Ptr becomes a null pointer.
inline reroot::CStringPtr &reroot::CStringPtr::operator = (CStringPtr &ptr)
{
	reset (ptr.release ());
	return *this;
}

// Take ownership of addressed memory.
inline reroot::CStringPtr &reroot::CStringPtr::operator = (char *const ptr)
{
	reset (ptr);
	return *this;
}

// Dereference the C string pointer.  This will fail if the pointer is null.
inline char &reroot::CStringPtr::operator * () const
{
	return *pointer;
}

// Return the C string pointer.  Retain ownership.
inline reroot::CStringPtr::operator char * () const
{
	return get ();
}

// Return a C++ string.  Retain ownership.
inline reroot::CStringPtr::operator std::string () const
{
	return get ();
}

// Return the C string pointer.  Retain ownership.
inline char *reroot::CStringPtr::get () const
{
	return pointer;
}

// Return the C string pointer.  Lose ownership & become a null pointer.
inline char *reroot::CStringPtr::release ()
{
	char *const ptr = pointer;
	pointer = 0;
	return ptr;
}

// Take ownership of addressed memory.  Free existing pointer.
inline void reroot::CStringPtr::reset (char *const ptr)
{
	if (ptr != pointer)
	{
		if (pointer)
			std::free (pointer);
		pointer = ptr;
	}
}

#endif
