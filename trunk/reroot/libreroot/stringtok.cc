// string_tok
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

#include <cstring>
#include "stringtok.h"

using namespace std;

// C'tor.  Initialize variables & allocate buffer.
reroot::string_tok::string_tok (string const &str, char const del):
	okay (true),
	pos (0)
{
	string::size_type const length = str.length () + 1;
	buf = new char [length];
	memcpy (buf, str.c_str (), length);

	delim [0] = del;
	delim [1] = 0;
}

// Read next token into component.
reroot::string_tok &
reroot::string_tok::operator >> (string &component)
{
	char const *const comp = strtok_r (pos? 0 : buf, delim, &pos);
	if (comp)
		component = comp;
	else
		okay = false;		// No more tokens.

	return *this;
}
