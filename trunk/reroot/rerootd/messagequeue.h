// rerootd message queue declarations
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

#ifndef MESSAGEQUEUE_H
# define MESSAGEQUEUE_H

# include <string>
# include <sys/ipc.h>
# include <sys/msg.h>

# include "packet.h"
# include "xmessage.h"

// Base functionality for managing a System V message queue.
class message_queue_base
{
	public:
		// For daemonizing safely.
		void disown ();

	protected:
		// C'tors & d'tor are protected to prevent this class from being
		// used directly, as it provides no useful functionality.
		message_queue_base (std::string const &false_root,
			char const queue);
		~message_queue_base ();

		// Descendants need access to queue.
		int get_queue_id () const;

	private:
		// Compiler generated copy c'tor & assignment operator could
		// cause problems (queue could be unallocated multiple or zero
		// times). It makes no sence to copy a System V message queue.
		message_queue_base (message_queue_base const &);
		message_queue_base const &operator =
			(message_queue_base const &);

		// Message queue data.
		bool own;
		key_t const key;
		int const queue_id;

		// Error messages.
		static std::string const no_key,
		                         no_queue;
};

// Prevent message queue from being unallocated by the destructor.  Used after
// daemon forks to prevent parent destroying the queue when it exits, which
// would cause the child to fail.
inline void
message_queue_base::disown ()
{
	own = false;
}

// Deallocate message queue.  This shouldn't fail due to encapsulation, but if
// it does, we have no means to report it!  (Destructors shouldn't throw
// exceptions.)  Then again, there's nothing we can do about it if this fails
// anyway.
inline
message_queue_base::~message_queue_base ()
{
	if (own)
		msgctl (queue_id, IPC_RMID, 0);
}

// Return queue identifier.
inline int
message_queue_base::get_queue_id () const
{
	return queue_id;
}

// Message queue for receiving messages.
class inbox:
	public message_queue_base
{
	public:
		explicit inbox (std::string const &false_root);

		// For receiving packets.
		inbox const &operator >> (packet &pkt) const;

	private:
		// Error message.
		static std::string const no_receive;
};

// Construct message queue.
inline
inbox::inbox (std::string const &false_root):
	message_queue_base (false_root, 'i')
{}

// Message queue for sending messages.
class outbox:
	public message_queue_base
{
	public:
		explicit outbox (std::string const &false_root);

		// For sending packets.
		outbox const &operator << (packet const &pkt) const;

	private:
		// Error messages.
		static std::string const no_send,
		                         bad_pid;
};

// Construct message queue.
inline
outbox::outbox (std::string const &false_root):
	message_queue_base (false_root, 'o')
{}

// Full-blown message queue.  (Actually two System V queues: one in, one out).
class message_queue:
	public inbox,
	public outbox
{
	public:
		explicit message_queue (std::string const &false_root);

		// Resolve ambiguity if disown is called.
		void disown ();
};

// Construct message queues.
inline
message_queue::message_queue (std::string const &false_root):
	inbox (false_root),
	outbox (false_root)
{}

// Prevent message queue from being unallocated by the destructor.  Used after
// daemon forks to prevent parent destroying the queue when it exits, which
// would cause the child to fail.  This wraps the base classes' versions to
// resolve the ambiguous call.
inline void
message_queue::disown ()
{
	inbox::disown ();
	outbox::disown ();
}

#endif
