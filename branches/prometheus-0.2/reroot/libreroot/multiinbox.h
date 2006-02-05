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

// A message queue that allows messages to be received from multiple sources
// concurrently.  Packets forming parts of different messages may be received
// interleaved.

#ifndef MULTIINBOX_H
# define MULTIINBOX_H

# include <map>
# include <string>
# include <sys/types.h>

# include "inbox.h"
# include "libreroot.h"
# include "message.h"
# include "packet.h"

namespace reroot
{
	class MultiInbox;
}

class shared reroot::MultiInbox:
	public Inbox
{
public:
	// For identifying the message a packet belongs to.
	struct shared MessageKey
	{
		typedef Packet::MessageID MessageID;

		pid_t sender;
		MessageID id;

		MessageKey (pid_t s, MessageID i);
		bool operator < (MessageKey const &mk) const;
	};

	// For storing partially received messages.
	typedef std::map <MessageKey, Message> MessageMap;

	// C'tors.
	MultiInbox (std::string const &prefix,
		std::string const &suffix = "inbox",
		UnlinkFlag uflag = on_close);
	MultiInbox (std::string const &name, OpenFlag oflag,
		UnlinkFlag uflag = never);

	// Drop any partially received messages from sender.
	void drop_messages_from (pid_t sender);

	// For receiving messages.
	MultiInbox &operator >> (Message &message);

private:
	// Partially received messages.
	MessageMap messages;
};

// Construct message key.
inline reroot::MultiInbox::MessageKey::MessageKey (pid_t const s,
                                                   MessageID const i):
	sender (s),
	id (i)
{
}

// Create an inbox by generating a name from the given prefix & suffix.
inline reroot::MultiInbox::MultiInbox (std::string const &prefix,
                                       std::string const &suffix,
                                       UnlinkFlag const uflag):
	Inbox (prefix, suffix, uflag)
{
}

// Create or open an inbox with the given name.
inline reroot::MultiInbox::MultiInbox (std::string const &name,
                                       OpenFlag const oflag,
                                       UnlinkFlag const uflag):
	Inbox (name, oflag, uflag)
{
}

#endif
