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

// A string tokenizer based on std::istringstream.

#ifndef TOKENIZER_H
# define TOKENIZER_H

# include <sstream>
# include <string>

# include "libreroot.h"

namespace reroot
{
	class Tokenizer;
}

class shared reroot::Tokenizer:
	private std::istringstream
{
public:
	// C'tor.
	explicit Tokenizer (std::string const &str, char d = '/');

	// For accessing the deliminator.
	char deliminator () const;
	void set_deliminator (char d);

	// For getting the next token.
	Tokenizer &operator >> (std::string &token);

	// Are there any more tokens?
	operator bool () const;
	bool operator ! () const;

private:
	char delim;
};

// Tokenizer c'tor.
inline reroot::Tokenizer::Tokenizer (std::string const &str, char const d):
	std::istringstream (str),
	delim (d)
{
}

// Return the deliminator.
inline char reroot::Tokenizer::deliminator () const
{
	return delim;
}

// Set the deliminator.
inline void reroot::Tokenizer::set_deliminator (char const d)
{
	delim = d;
}

// Return false if last token was read.
inline reroot::Tokenizer::operator bool () const
{
	return !fail ();
}

// Return true if last token was read.
inline bool reroot::Tokenizer::operator ! () const
{
	return fail ();
}

#endif
