// Message queue
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
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messagequeue.h"
#include "packet.h"
#include "rerootd.h"
#include "xmessage.h"

using namespace std;

// Construct IPC key from the false root directory & use it to create a System V
// IPC message queue.  Fail if queue is already in use.
reroot::message_queue_base::message_queue_base (char const queue):
	key (ftok (false_root.c_str (), queue)),
	qid (key == -1?: msgget (key, IPC_CREAT | IPC_EXCL | 0600))
{
	// Error messages.
	static char const no_key [] = "Cannot get IPC key",
	                  no_queue [] = "Cannot get message queue";

	if (key == -1)
		throw xmessage (no_key, errno);

	if (qid == -1)
		throw xmessage (no_queue, errno);
}

// Send an empty message of the given type to self.  This is used to queue
// received signals along with messages.
void
reroot::inbox::send_self (message_type const type) const
{
	// Error message.
	static char const no_send [] = "Cannot send packet to self";

	// Create message to send.
	packet const pkt =
	{
		1,
		{
			type,
			0,
			0,
			packet_meta_size
		},
		""
	};

	// Send message.
	if (msgsnd (get_qid (), &pkt, pkt.header.packet_size, 0))
		throw xmessage (no_send, errno);
}

// Receive a packet from PID specified therein (zero means get packet from any
// PID).
reroot::inbox const &
reroot::inbox::operator >> (packet &pkt) const
{
	// Error message.
	static char const no_receive [] = "Cannot receive packet";

	// Receive packet.  Retry if interrupted by a signal.
	do
		// msgrcv returns -1 on failure.
		if (msgrcv (get_qid (), &pkt, packet_data_size, pkt.pid, 0) + 1)
			return *this;
	while (errno == EINTR);

	// Only get here if an error occurs.
	throw xmessage (no_receive, errno);
}

// Send a packet to the PID specified therein.
reroot::outbox const &
reroot::outbox::operator << (packet const &pkt) const
{
	// Error message.
	static char const no_send [] = "Cannot send packet";

	// Check size is reasonable.
	unsigned const pkt_size = pkt.header.packet_size;
	if (pkt_size > packet_data_size)
		throw xmessage (no_send, E2BIG);

	// Send packet.  Retry if interrupted by a signal.
	do
		// msgsnd returns 0 on success.
		if (!msgsnd (get_qid (), &pkt, pkt_size, 0))
			return *this;
	while (errno == EINTR);

	// Only get here if an error occurs.
	throw xmessage (no_send, errno);
}
