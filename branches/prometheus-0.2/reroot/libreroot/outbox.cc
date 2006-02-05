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
#include <cstddef>
#include <mqueue.h>

#include "ipcerror.h"
#include "libreroot.h"
#include "outbox.h"
#include "packet.h"

using namespace reroot;
using namespace std;

// Send a packet to the message queue.
Outbox &Outbox::operator << (Packet const &packet)
{
	static char const error [] = "Cannot send to message queue: `";

	// How big is the packet?
	size_t const left = packet.header.size_left;
	size_t const size = left > Packet::body_size?
	                    Packet::size : Packet::header_size + left;

	do
		// Try to send packet.  Return if successful.
		if (mq_send (queue (), packet, size, 0) != -1)
			return *this;
	while (errno == EINTR);	// Retry if interrupted by a signal.

	// We only get this far if there was an error.
	throw IPCError (error + name () + '\'', errno);
}

// Send a message to the queue.
Outbox &Outbox::operator << (Message const &message)
{
	Outbox &outbox = *this;

	do
	{
		Packet packet;
		message >> packet;
		outbox << packet;
	}
	while (!message.is_complete ());

	return outbox;
}
