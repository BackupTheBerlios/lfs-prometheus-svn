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

#include <cerrno>
#include <cstdlib>
#include <csignal>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messagequeue.h"
#include "packet.h"
#include "rerootd.h"
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

// Construct IPC key from the false root directory & use it to create a System V
// IPC message queue.  Fail if queue is already in use.
reroot::message_queue_base::message_queue_base (string const &false_root,
                                                char const queue):
	own (true),
	key (ftok (false_root.c_str (), queue)),
	qid (key == -1?: msgget (key, IPC_CREAT | IPC_EXCL | 0600))
{
	if (key == -1)
		throw xmessage (no_key, errno);

	if (qid == -1)
		throw xmessage (no_queue, errno);
}

// Receive a packet from PID specified therein (zero means get packet from any
// PID).
reroot::inbox const &
reroot::inbox::operator >> (packet &pkt) const
{
	// Receive packet.
	if (msgrcv (get_qid (), &pkt, packet_data_size, pkt.pid, 0) == -1)
		throw xmessage (no_receive, errno);

	return *this;
}

// Send a packet to the PID specified therein.
reroot::outbox const &
reroot::outbox::operator << (packet const &pkt) const
{
	// Check size is reasonable.
	unsigned const pkt_size = pkt.header.packet_size;
	if (pkt_size > packet_data_size)
		throw xmessage (no_send, E2BIG);

	// Send packet.
	if (msgsnd (get_qid (), &pkt, pkt_size, 0))
		throw xmessage (no_send, errno);

	return *this;
}

// Signal handler.  Convert signals to System V IPC messages to self.  HUP
// becomes save & exit, USR1 becomes cleanup database.
// FIXME: Error reporting.
void
reroot::signal_handler (int const signum)
{
	message_type type;

	// Identify signal type, & hence message type.
	switch (signum)
	{
	case SIGHUP:
		type = save_and_exit;
		break;

	case SIGUSR1:
		type = cleanup_db;
		break;

	default:
		abort ();
	}

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

	// Get System V IPC key.
	key_t const key = ftok (false_root.c_str (), 'i');
	if (key == -1)
		abort ();

	// Get System V IPC message queue ID.
	int const qid = msgget (key, 0);
	if (qid == -1)
		abort ();

	// Send message.
	if (msgsnd (qid, &pkt, pkt.header.packet_size, 0))
		abort ();
}
