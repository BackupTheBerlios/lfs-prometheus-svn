// rerootd message queue
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
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "messagequeue.h"
#include "packet.h"
#include "xmessage.h"

// FIXME: Check error code options for msg* functions.  May have ideas?

// We're using C functions, some of which may be in the std namespace, some of
// which are global.
using namespace std;

// Error messages are constructed here to avoid having to allocate them in
// possible low memory situations.
string const message_queue_base::no_key = "Cannot get IPC key",
             message_queue_base::no_queue = "Cannot create message queue",
             inbox::no_receive = "Cannot receive packet",
             outbox::no_send = "Cannot send packet",
             outbox::bad_pid = ": Invalid destination PID";

// Construct IPC key from the false root directory & use it to construct a
// message queue.  Fail if queue is already in use.
message_queue_base::message_queue_base (string const &false_root,
                                        char const queue):
	own (true),
	key (ftok (false_root.c_str (), queue)),
	queue_id (key == -1?: msgget (key, IPC_CREAT | IPC_EXCL | 0600))
{
	if (key == -1)
		throw xmessage (no_key, errno);

	if (queue_id == -1)
		throw xmessage (no_queue, errno);
}

// Receive a packet from PID specified therein (zero means get packet from any
// PID).
inbox const &
inbox::operator >> (packet &pkt) const
{
	// Receive packet.
	if (msgrcv (get_queue_id (), &pkt, packet_data_size, pkt.pid, 0) == -1)
		throw xmessage (no_receive, errno);

	return *this;
}

// Send a packet.
outbox const &
outbox::operator << (packet const &pkt) const
{
	// Check destination PID is valid.  FIXME: msgsnd checks this?
	if (pkt.pid < 1)
		throw xmessage (no_send + bad_pid, 0);

	// Check size is reasonable.
	// FIXME: Too small?
	unsigned const pkt_size = pkt.packet_size;
	if (pkt_size > packet_data_size)
		throw xmessage (no_send, E2BIG);

	// Send packet.
	if (msgsnd (get_queue_id (), &pkt, pkt_size, 0))
		throw xmessage (no_send, errno);

	return *this;
}
