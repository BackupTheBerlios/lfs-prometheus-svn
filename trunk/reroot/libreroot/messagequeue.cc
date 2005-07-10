// Message queue
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

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#include "filename.h"
#include "libc.h"
#include "message.h"
#include "messagequeue.h"
#include "xmessage.h"

using namespace std;

// Error messages are constructed here to avoid having to allocate them in
// possible low memory situations.
namespace reroot
{
	string const message_queue_base::no_key = "Cannot get IPC key",
	             message_queue_base::no_queue = "Cannot get message queue",
	             inbox::no_receive = "Cannot receive packet",
	             outbox::no_send = "Cannot send packet";
}

// Construct IPC key from the false root directory & use it to obtain a System
// V IPC message queue.  Fail if queue doesn't exist (rerootd should have
// created it).
reroot::message_queue_base::message_queue_base (char const queue):
	pid (getpid ()),
	key (ftok (filename::rerooting ()?
	           filename::get_false_root ().c_str () : "/", queue)),
	qid (key == -1?: msgget (key, 0))
{
	if (key == -1)
		throw xmessage (no_key, errno);

	if (qid == -1)
		throw xmessage (no_queue, errno);
}

// Receive a packet addressed to this PID.
reroot::inbox const &
reroot::inbox::operator >> (packet &pkt) const
{
	// Receive packet.
	if (msgrcv (get_qid (), &pkt, packet_data_size, get_pid (), 0) == -1)
		throw xmessage (no_receive, errno);

	return *this;
}

// Send a packet addressed from this PID.
reroot::outbox const &
reroot::outbox::operator << (packet const &pkt) const
{
	// Check size is reasonable.
	unsigned const pkt_size = pkt.header.packet_size;
	if (pkt_size > packet_data_size)
		throw xmessage (no_send, E2BIG);

	// Set sender PID.
	pkt.pid = get_pid ();

	// Send packet.
	if (msgsnd (get_qid (), &pkt, pkt_size, 0))
		throw xmessage (no_send, errno);

	return *this;
}

namespace
{
	// Error message.
	string const bad_packet = "Unexpected packet when constructing message";

	// Split message into packets & send.
	void
	send (reroot::outbox const &out, reroot::message &msg)
	{
		// Get message parameters.
		reroot::meta header = msg.header;

		// What's left to send?
		unsigned size_left = header.body_size;
		header.packets_left = size_left / packet_body_size +
		                      size_left % packet_body_size? 1 : 0;

		// Send packets.  This is optimized to avoid copying large
		// blocks of data.  The original message data will be trashed.
		reroot::packet *pkt =
			reinterpret_cast <reroot::packet *> (&msg);
		while (--header.packets_left)
		{
			// Calculate packet body size.
			unsigned const bs =
				std::min (size_left, packet_body_size);
			size_left -= bs;

			// Initialize packet header.
			header.packet_size = packet_meta_size + bs;
			pkt->header = header;

			// Send packet.
			out << *pkt;

			// Increment packet pointer.
			pkt = reinterpret_cast <reroot::packet *>
			      (reinterpret_cast <char *> (pkt) +
			       packet_body_size);
		}
	}

	// Return message assembled from received packets.
	reroot::message *
	receive (reroot::inbox const &in)
	{
		// Allocate memory & get first packet.
		reroot::message *msg = reroot::alloc <reroot::message>
		                       (sizeof (reroot::packet));
		in >> *reinterpret_cast <reroot::packet *> (msg);

		// Get packet parameters.
		reroot::meta header = msg->header;

		// Do we have more packets?
		if (header.packets_left)
		{
			// Reallocate memory.
			msg = reroot::realloc (msg, sizeof (reroot::message) +
			                            header.body_size);

			// Get remaining packets.
			void *ptr =
				reinterpret_cast <reroot::packet *> (msg) + 1;
			while (header.packets_left--)
			{
				// Get next packet.
				reroot::packet pkt;
				in >> pkt;

				// Check packet.
				header.packet_size = pkt.header.packet_size;
				if (pkt.header != header)
					throw reroot::xmessage (bad_packet, 0);

				// Append packet body to message.
				ptr = mempcpy (ptr, pkt.body,
					header.packet_size - packet_meta_size);
			}
		}

		// Return complete message.
		return msg;
	}
}
