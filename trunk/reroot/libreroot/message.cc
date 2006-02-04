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

#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#include "ipcerror.h"
#include "message.h"
#include "packet.h"

using namespace reroot;
using namespace std;

namespace
{
	// Messages that are not yet fully assembled from packets shouldn't be
	// copied.
	char const incomplete_error [] = "Cannot copy incomplete message";
}

// Message c'tor.
Message::Message (Type const t, string const &txt):
	message_type (t),
	message_text (txt),
	buffer (0),
	sender_pid (0),
	id (0)
{
}

// Message copy c'tor.
Message::Message (Message const &message):
	message_type (message.message_type),
	message_text (message.message_text),
	buffer (message.buffer),
	sender_pid (message.sender_pid),
	id (message.id)
{
	if (buffer)
		IPCError::error (message, incomplete_error);
}

// Describe message to stream.
void Message::describe (ostream &os) const
{
	os << endl
	   << "== Message Details ==" << endl
	   << "Sender PID = " << sender () << endl
	   << "Type = " << type () << endl
	   << "Text = " << text () << endl;
}

// Get the next packet in message.
Message const &Message::operator >> (Packet &packet) const
{
	static char const error [] = "Cannot get next packet from message: "
	                             "Message is incomplete";

	// Initialize buffer & variables if necessary.
	if (!buffer)
		create_buffer ();
	else if (conversion != read_packets)
		IPCError::error (*this, packet, error);

	// Set packet header.
	packet.header.sender_pid = sender_pid;
	packet.header.message_id = id;
	packet.header.number = number++;
	packet.header.size_left = size_left;

	// How much data to copy into the packet.
	size_t const size = size_left > Packet::body_size?
	                    Packet::body_size : size_left;

	// Copy next block of data into the packet body.
	memcpy (packet.body, pos, size);
	pos += size;

	// If this was the last packet, free the buffer.
	if (size == size_left)
		reset_buffer ();

	return *this;
}

// Append packet to message.
Message &Message::operator << (Packet const &packet)
{
	// Error messages.
	static char const error [] = "Cannot append packet to message: ";
	static char const conversion_error [] = "Message is being converted to "
	                                        "packets";
	static char const wrong [] = "Packet has the wrong ";
	static char const sender_error [] = "sender";
	static char const id_error [] = "message ID";
	static char const size_error [] = "size remaining";
	static char const order_error [] = "sequence order number";

	// Are we starting a new message?
	if (!buffer)
	{
		// Initialize buffer & variables.
		sender_pid = packet.header.sender_pid;
		id = packet.header.message_id;
		number = 0;
		size_left = packet.header.size_left;
		pos = buffer = new char [size_left];
	}
	else
	{
		string error_message;

		// Check conversion direction.
		if (conversion != append_packets)
			error_message = conversion_error;

		// Check packet fits.
		else if (packet.header.sender_pid != sender_pid)
			error_message = string (wrong) + sender_error;
		else if (packet.header.message_id != id)
			error_message = string (wrong) + id_error;
		else if (packet.header.size_left != size_left)
			error_message = string (wrong) + size_error;

		// Was there an error?
		if (!error_message.empty ())
			IPCError::error (*this, packet, error + error_message);
	}

	// Check packet has the right sequence order number.
	if (packet.header.number != number++)
		IPCError::error (*this, packet,
		                 string (error) + wrong + order_error);

	// How much data to copy from the packet.
	size_t const size = size_left > Packet::body_size?
	                    Packet::body_size : size_left;

	// Copy next block of data from the packet body.
	pos = static_cast <char *> (mempcpy (pos, packet.body, size));
	size_left -= size;

	// If this was the last packet, parse & free the buffer.
	if (!size_left)
		parse_buffer ();

	return *this;
}

// Assignment operator.  Copy message if it is in a sane state.
Message const &Message::operator = (Message const &message)
{
	if (message.buffer)
		IPCError::error (message, incomplete_error);

	// Free any memory we're currently using.
	reset_buffer ();

	// Copy message.
	message_type = message.message_type;
	message_text = message.message_text;
	sender_pid = message.sender_pid;
	id = message.id;

	return *this;
}

// Create & fill a buffer for splitting into packets.  Initialize relevent
// variables.
void Message::create_buffer () const
{
	// Error message.
	static char const error [] = "Cannot split message into packets: "
	                             "Message is uninitialized";

	// Fail if message uninitialized.
	if (message_type == uninitialized)
		IPCError::error (*this, error);

	// Make sure ids are consistant.
	static pid_t const pid = getpid ();
	static Packet::MessageID current_id = 0;

	// Initialize variables.
	conversion = read_packets;
	sender_pid = pid;
	id = current_id++;
	number = 0;
	size_left = sizeof (message_type);

	// Size depends on message type.
	switch (message_type)
	{
	case terminate:
		break;

	default:
		size_left += message_text.length () + 1;
	}

	// Allocate a buffer large enough to hold the message data.
	char *p = pos = buffer = new char [size_left];

	// Copy message type to the buffer.
	p = static_cast <char *> (mempcpy (p, &message_type,
	                          sizeof (message_type)));

	// Copy message data to the buffer.
	switch (message_type)
	{
	case terminate:
		break;

	// FIXME: Cases with extra data then fall through to default (no break).

	default:
		strcpy (p, message_text.c_str ());
	}
}

// Read buffer into message data & free it.
void Message::parse_buffer ()
{
	char *p = buffer;

	// Copy message type from the buffer.
	memcpy (&message_type, p, sizeof (message_type));
	p += sizeof (message_type);

	// Copy message data from the buffer.
	switch (message_type)
	{
	case terminate:
		message_text.clear ();
		break;

	// FIXME: Cases with extra data then fall through to default (no break).

	default:
		message_text = p;
	}

	// Free buffer.
	reset_buffer ();
}

// For converting message type numbers into strings.
ostream &reroot::operator << (ostream &os, Message::Type const type)
{
	string t;

	switch (type)
	{
	case Message::uninitialized:
		t = "[uninitialized]";
		break;

	case Message::save_index:
		t = "save index";
		break;

	case Message::terminate:
		t = "terminate";
		break;

	default:
		t = "[unrecognized]";
	}

	os << t;
	return os;
}
