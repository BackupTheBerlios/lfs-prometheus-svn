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

#include <cerrno>
#include <mqueue.h>

#include "inbox.h"
#include "ipcerror.h"
#include "libreroot.h"
#include "packet.h"

using namespace reroot;
using namespace std;

// Receive a packet from the message queue.
Inbox &Inbox::operator >> (Packet &packet)
{
	static char const error [] = "Cannot receive from message queue: `";

	do
		// Try to receive packet.  Return if successful.
		if (mq_receive (queue (), packet, sizeof (packet), 0) != -1)
			return *this;
	while (errno == EINTR);	// Retry if interrupted by a signal.

	// We only get this far if there was an error.
	throw IPCError (error + name () + '\'', errno);
}

// Receive a message from the queue.
Inbox &Inbox::operator >> (Message &message)
{
	Inbox &inbox = *this;

	do
	{
		Packet packet;
		inbox >> packet;
		message << packet;
	}
	while (!message.is_complete ());

	return inbox;
}
