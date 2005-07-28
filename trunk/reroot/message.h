// Shared message handling declarations
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

#ifndef MESSAGE_H
# define MESSAGE_H

# include <cstdlib>

# include "alloc.h"
# include "packet.h"

// Some header files specific to libreroot or rerootd are needed.
# if defined LIBREROOT
#  include "libreroot/messagequeue.h"
# elif defined REROOTD
#  include "rerootd/messagequeue.h"
# else
#  error "Neither LIBREROOT nor REROOTD defined"
# endif

namespace reroot
{
	class message;

	inbox const &operator >> (inbox const &in, message &msg);
	outbox const &operator << (outbox const &out, message &msg);
}

// A wrapper around reroot::message_data to handle memory allocation.
class reroot::message
{
	public:
		// C'tor & d'tor.
		explicit message (unsigned const size = packet_body_size);
		~message ();

		// For accessing the sender/receiver PID.
		long get_pid () const;
		void set_pid (long const pid);

		// For accessing the message type.
		message_type get_type () const;
		void set_type (message_type const type);

		// For resizing the message body.
		unsigned get_size () const;
		void set_size (unsigned const size);

		// For accessing the message body.
		template <typename type> type const &get_body () const;
		template <typename type> type &get_body ();

	private:
		// The actual message data.
		message_data *data;

	// For working with message queues.  FIXME: reroot:: specification
	// unnecessary according to standard & GCC 4?  GCC 3 needs it currently.
	friend inbox const &reroot::operator >> (inbox const &in, message &msg);
	friend outbox const &reroot::operator << (outbox const &out,
		message &msg);
};

// Free message data.
inline
reroot::message::~message ()
{
	std::free (data);
}

// Return sender/receiver PID.
inline long
reroot::message::get_pid () const
{
	return data->pid;
}

// Set sender/receiver PID.
inline void
reroot::message::set_pid (long const pid)
{
	data->pid = pid;
}

// Return message type.
inline reroot::message_type
reroot::message::get_type () const
{
	return data->header.type;
}

// Set message type.
inline void
reroot::message::set_type (message_type const type)
{
	data->header.type = type;
}

// Return message body size.
inline unsigned
reroot::message::get_size () const
{
	return data->header.body_size;
}

// Resize message body.
inline void
reroot::message::set_size (unsigned const size)
{
	// Realloc is guaranteed to do nothing if the new size is the same as
	// the old.
	data = realloc (data, sizeof (message_data) + size);
	data->header.body_size = size;
}

// Return message body read-only.
template <typename type>
inline type const &
reroot::message::get_body () const
{
	return *reinterpret_cast <type const *> (&data->body);
}

// Return message body for writing.
template <typename type>
inline type &
reroot::message::get_body ()
{
	return *reinterpret_cast <type *> (&data->body);
}

#endif
