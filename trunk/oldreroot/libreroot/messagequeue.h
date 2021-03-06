// Message queue declarations
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

#ifndef MESSAGEQUEUE_H
# define MESSAGEQUEUE_H

# include <sys/ipc.h>
# include <sys/msg.h>
# include <sys/types.h>

# include "packet.h"

namespace reroot
{
	// Message queues.
	class message_queue_base;
	class inbox;
	class outbox;
	class message_queue;

	// For obtaining a message queue.
	message_queue const &get_message_queue ();
}

// Base functionality for managing a System V message queue.
class reroot::message_queue_base
{
	protected:
		// C'tor is protected to prevent this class from being used
		// directly, as it provides no useful functionality.
		explicit message_queue_base (char const queue);

		// Descendants need access to IDs.
		pid_t get_pid () const;
		int get_qid () const;

	private:
		// Message queue data.
		pid_t const pid;
		key_t const key;
		int const qid;
};

// Return PID.
inline pid_t
reroot::message_queue_base::get_pid () const
{
	return pid;
}

// Return queue identifier.
inline int
reroot::message_queue_base::get_qid () const
{
	return qid;
}

// Message queue for receiving messages.
class reroot::inbox:
	public message_queue_base
{
	public:
		inbox ();

		// For receiving packets.
		inbox const &operator >> (packet &pkt) const;
};

// Construct message queue.
inline
reroot::inbox::inbox ():
	message_queue_base ('o')
{}

// Message queue for sending messages.
class reroot::outbox:
	public message_queue_base
{
	public:
		outbox ();

		// For sending packets.
		outbox const &operator << (packet const &pkt) const;
};

// Construct message queue.
inline
reroot::outbox::outbox ():
	message_queue_base ('i')
{}

// Full-blown message queue.  (Actually two System V queues: one in, one out).
class reroot::message_queue:
	public inbox,
	public outbox
{};

// Return a message queue for communicating with the reroot daemon.  Create the
// queue if necessary.
inline reroot::message_queue const &
reroot::get_message_queue ()
{
	static message_queue const queue;
	return queue;
}

#endif
