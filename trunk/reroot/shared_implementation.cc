// Shared reroot implementation
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

#ifndef SHARED_IMPLEMENTATION_CC
# define SHARED_IMPLEMENTATION_CC

# include <algorithm>
# include <cstdlib>
# include <cstring>
# include <new>

# include "alloc.h"
# include "message.h"
# include "packet.h"
# include "xmessage.h"

using namespace std;

// Wrapper for malloc.
void * __attribute__ ((malloc))
reroot::do_alloc (size_t const size)
{
	void *ptr = std::malloc (size);

	if (ptr)
		return ptr;

	throw bad_alloc ();
}

// Wrapper for realloc.
void * __attribute__ ((malloc))
reroot::do_realloc (void *ptr, size_t const newsize)
{
	ptr = std::realloc (ptr, newsize);

	if (ptr)
		return ptr;

	throw bad_alloc ();
}

// Construct xmessage with an error string & also the C error number.
reroot::xmessage::xmessage (string const &action, int const errnum):
	runtime_error (action + (errnum? string (": ") +
	                         strerror (errnum) : "")),
	error_number (errnum)
{}

namespace
{
	// Error message.
	string const bad_packet = "Unexpected packet when constructing message";

	// Return true if packet metadata is unequal.
	bool
	operator != (reroot::meta const &m1, reroot::meta const &m2)
	{
		return m1.type != m2.type ||
		       m1.body_size != m2.body_size ||
		       m1.packets_left != m2.packets_left ||
		       m1.packet_size != m2.packet_size;
	}
}

// Receive a message by assembling received packets.
reroot::inbox const &
reroot::operator >> (inbox const &in, message &msg)
{
	// Reserve memory for first packet & receive it.
	msg.data = realloc (msg.data, sizeof (packet));
	in >> *reinterpret_cast <packet *> (msg.data);

	// Get packet parameters.
	long const pid = msg.data->pid;
	meta header = msg.data->header;

	// Do we have more packets?
	if (header.packets_left)
	{
		// Reallocate memory.
		msg.data = realloc (msg.data,
		                    sizeof (message_data) + header.body_size);

		// Get address to append body data to.
		void *ptr = reinterpret_cast <packet *> (msg.data) + 1;

		// Get remaining packets.
		while (header.packets_left--)
		{
			// Get next packet in message.
			packet pkt;
			pkt.pid = pid;
			in >> pkt;

			// Check packet.
			header.packet_size = pkt.header.packet_size;
			if (pkt.header != header)
				throw xmessage (bad_packet, 0);

			// Append packet body to message.
			ptr = mempcpy (ptr, pkt.body,
			               header.packet_size - packet_meta_size);
		}
	}

	return in;
}

// Send a message by splitting into packets to send.  Message data may be
// trashed after sending.
reroot::outbox const &
reroot::operator << (outbox const &out, message &msg)
{
	// Get message parameters.
	meta header = msg.data->header;

	// What's left to send?
	unsigned size_left = header.body_size;
	header.packets_left = size_left / packet_body_size +
	                      size_left % packet_body_size? 1 : 0;

	// Send packets.  This is optimized to avoid copying large blocks of
	// data.  The original message data will be trashed if multiple packets
	// are sent.
	packet *pkt = reinterpret_cast <packet *> (&msg);
	while (header.packets_left--)
	{
		// Calculate packet body size.
		unsigned const bs = min (size_left, packet_body_size);
		size_left -= bs;

		// Intialize packet header.
		header.packet_size = packet_meta_size + bs;
		pkt->header = header;

		// Send packet.
		out << *pkt;

		// Increment packet pointer so that the next unsent body section
		// can be turned into another packet & sent.
		if (header.packets_left)
			pkt = reinterpret_cast <packet *>
			      (reinterpret_cast <char *> (pkt) +
			       packet_body_size);
	}

	return out;
}

// Allocate message data.
reroot::message::message (message_type const type, unsigned const size):
	data (alloc <message_data> (sizeof (message_data) + size))
{
	data->pid = 0;
	data->header.type = type;
	data->header.body_size = size;
}

#endif
