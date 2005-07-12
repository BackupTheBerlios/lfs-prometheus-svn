// Shared reroot implementation
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

#ifndef SHARED_IMPLEMENTATION_CC
# define SHARED_IMPLEMENTATION_CC

# include <cstring>

# include "packet.h"
# include "xmessage.h"

using namespace std;

// Construct xmessage with an error string & also the C error number.
reroot::xmessage::xmessage (string const &action, int const errnum):
	runtime_error (action + (errnum? string (": ") +
	                         strerror (errnum) : "")),
	error_number (errnum)
{}

namespace
{
	// Return true if packet metadata is equal.
	bool
	operator == (reroot::meta const &m1, reroot::meta const &m2)
	{
		return m1.type == m2.type &&
		       m1.body_size == m2.body_size &&
		       m1.packets_left == m2.packets_left &&
		       m1.packet_size == m2.packet_size;
	}

	// Return true if packet metadata is unequal.
	bool
	operator != (reroot::meta const &m1, reroot::meta const &m2)
	{
		return m1.type != m2.type ||
		       m1.body_size != m2.body_size ||
		       m1.packets_left != m2.packets_left ||
		       m1.packet_size != m2.packet_size;
	}
}

#endif
