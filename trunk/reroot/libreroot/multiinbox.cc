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

#include <algorithm>
#include <sys/types.h>

#include "multiinbox.h"
#include "packet.h"

using namespace reroot;
using namespace std;

namespace
{
	// To save typing.
	typedef MultiInbox::MessageMap::iterator Iterator;
	typedef MultiInbox::MessageMap::value_type Value;

	// Functor for identifying messages from a specific sender.
	class FromSender
	{
	public:
		explicit FromSender (pid_t s);
		bool operator () (Value const &value) const;

	private:
		pid_t const sender;
	};

	// FromSender c'tor.
	inline FromSender::FromSender (pid_t const s):
		sender (s)
	{
	}

	// Return true if message is from sender.
	inline bool FromSender::operator () (Value const &value) const
	{
		return value.second.get_sender () == sender;
	}

	// Return true if the message associated with value is complete.
	inline bool is_complete (Value const &value)
	{
		return value.second.is_complete ();
	}
}

// Message keys must be less-than comparable.
bool MultiInbox::MessageKey::operator < (MessageKey const &mk) const
{
	bool retval;

	if (sender < mk.sender)
		retval = true;
	else if (sender == mk.sender)
		retval = id < mk.id;
	else
		retval = false;

	return retval;
}

// Drop any partially received messages from sender.
void MultiInbox::drop_messages_from (pid_t sender)
{
	Iterator i = find_if (messages.begin (), messages.end (),
	                      FromSender (sender));
	Iterator previous = i;
	while (i != messages.end ())
	{
		i = find_if (i, messages.end (), FromSender (sender));
		messages.erase (previous);
		previous = i;
	}
}

// Receive a message from the queue.
MultiInbox &MultiInbox::operator >> (Message &message)
{
	Inbox &inbox = *this;
	Iterator i;

	do
	{
		// Receive a packet from the queue.
		Packet packet;
		inbox >> packet;

		// Append the packet to its message.  Create a new message if
		// necessary.
		messages [MessageKey (packet.header.sender, packet.header.id)]
			<< packet;

		// Search for a complete message.
		i = find_if (messages.begin (), messages.end (), is_complete);
	}
	while (i == messages.end ());	// Go until we find one.

	// Receive message & remove it from map.
	message = i->second;
	messages.erase (i);

	return *this;
}
