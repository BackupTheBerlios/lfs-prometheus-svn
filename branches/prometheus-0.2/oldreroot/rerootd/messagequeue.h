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
		// C'tor & d'tor are protected to prevent this class from being
		// used directly, as it provides no useful functionality.
		explicit message_queue_base (char const queue);
		~message_queue_base ();

		// Descendants need access to queue.
		int get_qid () const;

	private:
		// Message queue data.
		key_t const key;
		int const qid;

		// Compiler generated copy c'tor & assignment operator could
		// cause problems (queue could be unallocated multiple or zero
		// times). It makes no sence to copy a System V message queue.
		message_queue_base (message_queue_base const &);
		message_queue_base const &operator =
			(message_queue_base const &);
};

// Deallocate message queue.  This shouldn't fail due to encapsulation, but if
// it does, we have no means to report it!  (Destructors shouldn't throw
// exceptions.)  Then again, there's nothing we can do about it if this fails
// anyway.
inline
reroot::message_queue_base::~message_queue_base ()
{
	msgctl (qid, IPC_RMID, 0);
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

		// So daemon can message itself in response to signals.
		void send_self (message_type const type) const;

		// For receiving packets.
		inbox const &operator >> (packet &pkt) const;
};

// Construct message queue.
inline
reroot::inbox::inbox ():
	message_queue_base ('i')
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
	message_queue_base ('o')
{}

// Full-blown message queue.  (Actually two System V queues: one in, one out).
class reroot::message_queue:
	public inbox,
	public outbox
{};

// Return the message queue, creating it if necessary.
inline reroot::message_queue const &
reroot::get_message_queue ()
{
	static message_queue const queue;
	return queue;
}

#endif
