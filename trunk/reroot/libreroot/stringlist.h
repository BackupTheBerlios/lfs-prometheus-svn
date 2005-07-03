// string_list declaration
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

#ifndef STRINGLIST_H
# define STRINGLIST_H

# include <list>
# include <string>

namespace reroot
{
	typedef std::list <std::string> string_list_base;
	class string_list;
}

// A list of strings.  Once complete, the list can be converted to a single
// string with the list items separated by a specified deliminator, which also
// prefixes the string.  This is mainly intended for parsing filenames and
// PATH-like environment variables.
class reroot::string_list:
	private std::list <std::string>
{
	public:
		string_list ();

		void push_front (std::string const &string);
		void push_back (std::string const &string);
		void pop_front ();
		void pop_back ();

		void get_string (std::string &string, char const delim) const;

		using string_list_base::begin;
		using string_list_base::const_iterator;
		using string_list_base::empty;
		using string_list_base::end;

	private:
		std::string::size_type length;
};

// C'tor.  Make sure length starts off zero.
inline
reroot::string_list::string_list ():
	length (0)
{}

// Insert string at start of list.
inline void
reroot::string_list::push_front (std::string const &string)
{
	string_list_base::push_front (string);
	length += string.length () + 1;
}

// Append string to end of list.
inline void
reroot::string_list::push_back (std::string const &string)
{
	string_list_base::push_back (string);
	length += string.length () + 1;
}

// Remove first string.
inline void
reroot::string_list::pop_front ()
{
	length -= front ().length () + 1;
	string_list_base::pop_front ();
}

// Remove last string.
inline void
reroot::string_list::pop_back ()
{
	length -= back ().length () + 1;
	string_list_base::pop_back ();
}

#endif
