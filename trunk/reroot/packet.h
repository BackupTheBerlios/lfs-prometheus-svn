// Shared reroot communication data structure declarations
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

#ifndef PACKET_H
# define PACKET_H

// Size of packet header, minus the PID, in bytes.
# define packet_meta_size (sizeof (reroot::message_data) - sizeof (long))

// Maximum packet size, minus the PID, in bytes.
# define packet_data_size (sizeof (reroot::packet) - sizeof (long))

// Maximum packet body size in bytes.  See below.
# define packet_body_size (512 - sizeof (reroot::message_data))

namespace reroot
{
	// Message types.
	enum message_type
	{
		def
	};

	// Base message passing structures.
	struct meta;
	struct message_data;
	struct packet;
}

// Metadata for packets & messages.
struct reroot::meta
{
	message_type type;		// Type of message represented in body.
	unsigned body_size,		// Size of entire message body.
	         packets_left,		// Number of packets left in message.
	         packet_size;		// Size of packet, minus PID.
};

// Messages are the same as packets (for efficiency reasons - small messages can
// be sent as single packets), but with unlimited size.
struct reroot::message_data
{
	// PID of the sender or receiver, whichever *isn't* the daemon.
	long pid;

	meta header;
	char body [];			// Attached data.
};

// Messages are split into packets of known maximum size.
struct reroot::packet
{
	// PID of the sender or receiver, whichever *isn't* the daemon.
	mutable long pid;

	meta header;
	char body [packet_body_size];	// Attached data.
};

#endif
