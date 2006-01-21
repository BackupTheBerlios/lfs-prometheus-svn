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

#include <iostream>

#include "action.h"
#include "message.h"

using namespace reroot;
using namespace std;

// Send the message associated with the action, & process the reply if
// necessary.
void Action::run (Inbox &inbox, Outbox &outbox) const
{
	// Send message.
	outbox << message;

	if (reply)
	{
		// Get reply.
		Message message;
		inbox >> message;

		// Process reply.
		switch (Message::Type const type = message.get_type ())
		{
		// Unexpected message types.
		case Message::uninitialized:
		case Message::terminate:
			cout << "Received unexpected " << type << " message"
			     << endl;
			break;

		// Unrecognized message types.
		default:
			cout << "Received message with unrecognized type"
			     << endl;
		}
	}
}
