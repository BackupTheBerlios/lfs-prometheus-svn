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

// A message queue that allows messages to be received.

#ifndef INBOX_H
# define INBOX_H

# include <string>

# include "libreroot.h"
# include "message.h"
# include "messagequeue.h"
# include "packet.h"

namespace reroot
{
	class Inbox;
}

class shared reroot::Inbox:
	public MessageQueue
{
public:
	// C'tors.
	Inbox (std::string const &prefix, std::string const &suffix = "inbox",
		UnlinkFlag uflag = on_close);
	Inbox (std::string const &name, OpenFlag oflag,
		UnlinkFlag uflag = never);

	// For receiving messages.
	Inbox &operator >> (Packet &packet);
	Inbox &operator >> (Message &message);
};

// Create an inbox by generating a name from the given prefix & suffix.
inline reroot::Inbox::Inbox (std::string const &prefix,
                             std::string const &suffix,
                             UnlinkFlag const uflag):
	MessageQueue (prefix, suffix, read, uflag)
{
}

// Create or open an inbox with the given name.
inline reroot::Inbox::Inbox (std::string const &name,
                             OpenFlag const oflag,
                             UnlinkFlag const uflag):
	MessageQueue (name, oflag, read, uflag)
{
}

#endif
