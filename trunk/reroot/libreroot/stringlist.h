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

// A list of strings.  Once complete, the list can be converted to a single
// string with the list items separated by a specified deliminator, which may
// also prefix the string.  This is mainly intended for parsing filenames and
// PATH-like environment variables.

#ifndef STRINGLIST_H
# define STRINGLIST_H

# include <list>
# include <string>

# include "libreroot"

namespace reroot
{
	typedef std::list <std::string> StringListBase;
	class StringList;
}

class reroot::StringList:
	private StringListBase
{
public:
	// List iterator.
	typedef StringListBase::const_iterator Iterator;

	// C'tor.
	StringList ();

	// List access.
	Iterator begin () const;
	Iterator end () const;
	void push_front (std::string const &string);
	void push_back (std::string const &string);
	void pop_front ();
	void pop_back ();
	void clear ();
	using StringListBase::empty;
	using StringListBase::size;

	// For getting the whole string.
	std::string::size_type length (bool prefix = true) const;
	void get_string (std::string &str, char delim = '/', bool prefix = true)
		const;
};

#endif
