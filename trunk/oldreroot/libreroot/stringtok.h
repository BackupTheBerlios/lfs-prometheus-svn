// string_tok declaration
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

#ifndef STRINGTOK_H
# define STRINGTOK_H

# include <string>

namespace reroot
{
	class string_tok;
}

// A wrapper for the GNU C strtok_r function.  Usage is deliberately similar
// to std::istringstream.  FIXME: Can we use istringstream instead? Yes with
// getline(stream, string, delim)
class reroot::string_tok
{
	public:
		// C'tor & d'tor.
		string_tok (std::string const &str, char const del);
		~string_tok ();

		// For getting tokens.
		string_tok &operator >> (std::string &component);
		operator bool () const;

	private:
		// Data.
		bool okay;
		char delim [2];
		char *buf,
		     *pos;

		// Copying makes no sense.
		string_tok (string_tok const &);
		string_tok const &operator = (string_tok const &);
};

// D'tor.  Delete buffer.
inline
reroot::string_tok::~string_tok ()
{
	delete [] buf;
}

// Return status of tokenizer.
inline
reroot::string_tok::operator bool () const
{
	return okay;
}

#endif
