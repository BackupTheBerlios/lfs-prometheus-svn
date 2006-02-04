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
	CStringPtr (CStringPtr const &ptr);
	~CStringPtr ();

	// Memory control.
	void free ();
	void disown () const;

	// Assingment operators.
	CStringPtr const &operator = (char *ptr);
	CStringPtr const &operator = (CStringPtr const &ptr);

	// For accessing the string.
	operator char * () const;
	char &operator * () const;
	operator std::string () const;

private:
	// Data.
	char *pointer;
	mutable bool own;
};

// Construct a CStringPtr that owns the addressed memory.
inline reroot::CStringPtr::CStringPtr (char *const ptr):
	pointer (ptr),
	own (true)
{
}

// Construct a CStringPtr that takes ownership from ptr.
inline reroot::CStringPtr::CStringPtr (CStringPtr const &ptr):
	pointer (ptr.pointer),
	own (ptr.own)
{
	ptr.disown ();
}

// Free addressed memory if non-null & we own it.
inline reroot::CStringPtr::~CStringPtr ()
{
	if (pointer && own)
		std::free (pointer);
}

// Free addressed memory if non-null & we own it.
inline void reroot::CStringPtr::free ()
{
	if (pointer && own)
	{
		std::free (pointer);
		pointer = 0;
	}
}

// Relinquish ownership of addressed memory.
inline void reroot::CStringPtr::disown () const
{
	own = false;
}

// Take ownership of addressed memory.
inline reroot::CStringPtr const &reroot::CStringPtr::operator =
	(char *const ptr)
{
	free ();
	pointer = ptr;
	own = true;
	return *this;
}

// Take ownership of addressed memory from ptr.
inline reroot::CStringPtr const &reroot::CStringPtr::operator =
	(CStringPtr const &ptr)
{
	free ();
	pointer = ptr.pointer;
	own = ptr.own;
	ptr.disown ();
	return *this;
}

// Return the C string pointer.
inline reroot::CStringPtr::operator char * () const
{
	return pointer;
}

// Dereference the C string pointer.
inline char &reroot::CStringPtr::operator * () const
{
	return *pointer;
}

// Return a C++ string.
inline reroot::CStringPtr::operator std::string () const
{
	return pointer;
}

#endif
