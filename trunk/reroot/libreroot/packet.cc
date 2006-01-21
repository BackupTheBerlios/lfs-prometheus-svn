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

#include "packet.h"

using namespace reroot;
using namespace std;

// Describe packet to stream.
void Packet::describe (ostream &os) const
{
	os << endl
	   << "== Packet Details ==" << endl
	   << "Sender = " << header.sender << endl
	   << "Message ID = " << header.id << endl
	   << "Packet number = " << header.number << endl
	   << "Size left = " << header.size_left << endl;
}
