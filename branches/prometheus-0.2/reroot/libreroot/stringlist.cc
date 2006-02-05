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

#include <algorithm>
#include <string>

#include "stringlist.h"
#include "tokenizer.h"

using namespace reroot;
using namespace std;

namespace
{
	// Functor for adding lengths of strings with deliminators.
	class TotalLength
	{
	public:
		TotalLength (string::size_type len = 0);
		void operator () (string const &str);
		operator string::size_type () const;

	private:
		string::size_type length;
	};

	// TotalLength c'tor.
	inline TotalLength::TotalLength (string::size_type const len):
		length (len)
	{
	}

	// Add the length of string with single char deliminator.
	inline void TotalLength::operator () (string const &str)
	{
		length += str.length () + 1;
	}

	// Return the length of string so far.
	inline TotalLength::operator string::size_type () const
	{
		return length;
	}

	// Functor for concatenating strings with deliminators.
	class ConcatenateString
	{
	public:
		ConcatenateString (string &s, char delim);
		void operator () (string const &s);

	private:
		string &str;
		char const deliminator;
	};

	// ConcatenateString c'tor.
	inline ConcatenateString::ConcatenateString (string &s,
	                                             char const delim):
		str (s),
		deliminator (delim)
	{
	}

	// Concatenate string with deliminator to total.
	inline void ConcatenateString::operator () (string const &s)
	{
		str += deliminator;
		str += s;
	}
}

// Split the string & append the components to the list.
void StringList::append_string (string const &str, char const delim)
{
	Tokenizer tokenizer (str, delim);
	string token;
	while (tokenizer >> token)
		push_back (token);
}

// Append components of a filename to the list, processing `.' & `..'.
void StringList::append_filename (string const &str, char const delim)
{
	Tokenizer tokenizer (str, delim);
	string token;
	while (tokenizer >> token)
	{
		// `..' => Up one level.
		if (token == "..")
		{
			if (!empty ())
				pop_back ();
		}

		// `.' => Skipped.
		else if (token != ".")
			push_back (token);
	}
}

// Concatenate the strings in the list with single char deliminators, including
// prefix.
void StringList::get_filename (string &str, char const delim) const
{
	str.clear ();
	str.reserve (for_each (begin (), end (), TotalLength ()));
	for_each (begin (), end (), ConcatenateString (str, delim));
}
