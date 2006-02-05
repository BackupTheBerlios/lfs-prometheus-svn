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

#include "ipcerror.h"
#include "message.h"
#include "messageloop.h"
#include "multiinbox.h"

using namespace reroot;
using namespace std;

// The message handling loop.
void reroot::message_loop (Arguments const &args)
{
	// Error messages.
	static char const unexpected [] = "Received unexpected message";
	static char const unrecognized [] = "Received unrecognized message";

	// Create a message queue for receiving messages, & report its name to
	// stdout.
	MultiInbox inbox ("rerootd");
	cout << inbox.name () << endl;

	// The loop.
	while (true)
	{
		// Receive message.
		Message message;
		inbox >> message;

		// Log message to stdout if --verbose specified.
		if (args.verbose ())
			message.describe (cout);

		// Interpret message.
		switch (message.type ())
		{
		case Message::save_index:
			// FIXME: Save the index file.
			cout << "Save index to " << message.text () << endl;
			break;

		case Message::terminate:
			return;

		// Unexpected message types.
		case Message::uninitialized:
			IPCError::error (message, unexpected);

		// Unrecognized message types.
		default:
			IPCError::error (message, unrecognized);
		}
	}
}
