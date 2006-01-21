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

// A message sent between rerootd and rerootd.  It may be split into multiple
// packets because POSIX IPC has a maximum message size.

#ifndef MESSAGE_H
# define MESSAGE_H

# include <cstdlib>
# include <iostream>
# include <string>
# include <sys/types.h>

# include "libreroot.h"
# include "packet.h"

namespace reroot
{
	class Message;
}

class shared reroot::Message
{
public:
	// Types of messages recognized by reroot.
	enum Type
	{
		uninitialized,

		// Daemon control.
		save_index,
		terminate
	};

	// C'tors & d'tor.
	explicit Message (Type t = uninitialized, std::string const &txt = "");
	Message (Message const &message);
	~Message ();

	// Describe message to stream.
	void describe (std::ostream &os) const;

	// For accessing the message type.
	Type get_type () const;
	void set_type (Type t);

	// For accessing the message text.
	std::string const &get_text () const;
	void set_text (std::string const &txt);

	// For accessing the message sender.
	pid_t get_sender () const;

	// Return true if a message has been completely converted to packets, or
	// all the packets in a message have been assembled.
	bool is_complete () const;

	// Cancel the current message <=> packet conversion.
	void reset_buffer () const;

	// For converting between messages & packets.
	Message const &operator >> (Packet &packet) const;
	Message &operator << (Packet const &packet);

	// Assignment operator.
	Message const &operator = (Message const &message);

private:
	// Is the message being converted to or from packets?
	enum Conversion
	{
		read_packets,
		append_packets
	};

	// Message data.
	Type type;
	std::string text;

	// Used when converting between messages & packets.
	mutable Conversion conversion;
	mutable char *buffer, *pos;
	mutable pid_t sender;
	mutable Packet::MessageID id;
	mutable unsigned number;
	mutable std::size_t size_left;

	// Create & fill a buffer for splitting into packets.
	void create_buffer () const;

	// Read buffer into message data & free it.
	void parse_buffer ();
};

// D'tor.  Free any memory used in message <=> packet conversion.
inline reroot::Message::~Message ()
{
	delete [] buffer;
}

// Return the message type.
inline reroot::Message::Type reroot::Message::get_type () const
{
	return type;
}

// Set the message type.
inline void reroot::Message::set_type (Type const t)
{
	type = t;
}

// Return the message text.
inline std::string const &reroot::Message::get_text () const
{
	return text;
}

// Set the message text.
inline void reroot::Message::set_text (std::string const &txt)
{
	text = txt;
}

// Return the PID of the message sender.
inline pid_t reroot::Message::get_sender () const
{
	return sender;
}

// Return true if a message has been completely converted to packets, or all the
// packets in a message have been assembled.
inline bool reroot::Message::is_complete () const
{
	return !buffer;
}

// Cancel the current message <=> packet conversion.
inline void reroot::Message::reset_buffer () const
{
	delete [] buffer;
	buffer = 0;
}

namespace reroot
{
	// For converting message type numbers into strings.
	shared std::ostream &operator << (std::ostream &os, Message::Type type);
}

#endif
