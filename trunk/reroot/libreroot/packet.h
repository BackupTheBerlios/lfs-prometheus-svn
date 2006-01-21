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

// POSIX IPC defines a finite maximum size for messages.  Since messages between
// reroot.so and rerootd may contain filenames of unspecified maximum length,
// they may be split into an arbitrary number of packets for sending.

#ifndef PACKET_H
# define PACKET_H

# include <cstddef>
# include <iostream>
# include <sys/types.h>

namespace reroot
{
	struct Packet;
}

struct reroot::Packet
{
	// The maximum size of a packet in bytes.
	static std::size_t const size = 8192;

	// Packets from the same message share the same message identifier.
	typedef unsigned MessageID;

	// Header containing information about a packet or the message of which
	// it's part.
	struct Header
	{
		// These identify the message of which the packet is part.
		pid_t sender;
		MessageID id;

		// The packets in a message are numbered in order, with the
		// first being zero.
		unsigned number;

		// The size of the remaining message body, including this & any
		// following packets, but not any packets sent previously.  If
		// this is greater than body_size (defined below), further
		// packets are assumed to be needed in order to complete the
		// message.
		std::size_t size_left;
	};

	// Used to figure out the size of a packet's body.  This is not as
	// simple as subtracting the size of the header because there may be
	// padding for alignment purposes.
	struct Empty
	{
		Header header;
		char body [];
	};

	// A few useful size constants.
	static std::size_t const header_size = sizeof (Empty);
	static std::size_t const body_size = size - header_size;

	// The actual packet data.
	Header header;
	char body [body_size];

	// Describe packet to stream.
	void describe (std::ostream &os) const;

	// POSIX message queue functions expect char rather than packet
	// pointers.  This is a non-trivial conversion so provide implicit cast
	// operators.
	operator char const * () const;
	operator char * ();
};

// Return a pointer to the packet's raw bytes.
inline reroot::Packet::operator char const * () const
{
	return reinterpret_cast <char const *> (this);
}

// Return a pointer to the packet's raw bytes.
inline reroot::Packet::operator char * ()
{
	return reinterpret_cast <char *> (this);
}

#endif
