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

// A message queue that allows messages to be sent.

#ifndef OUTBOX_H
# define OUTBOX_H

# include <string>

# include "libreroot.h"
# include "message.h"
# include "messagequeue.h"
# include "packet.h"

namespace reroot
{
	class Outbox;
}

class shared reroot::Outbox:
	public MessageQueue
{
public:
	// C'tors.
	Outbox (std::string const &prefix, std::string const &suffix = "outbox",
	        UnlinkFlag uflag = on_close);
	Outbox (std::string const &name, OpenFlag oflag,
	        UnlinkFlag uflag = never);

	// For sending messages.
	Outbox &operator << (Packet const &packet);
	Outbox &operator << (Message const &message);
};

// Create an outbox by generating a name from the given prefix & suffix.
inline reroot::Outbox::Outbox (std::string const &prefix,
                               std::string const &suffix,
                               UnlinkFlag const uflag):
	MessageQueue (prefix, suffix, write, uflag)
{
}

// Create or open an outbox with the given name.
inline reroot::Outbox::Outbox (std::string const &name,
                               OpenFlag const oflag,
                               UnlinkFlag const uflag):
	MessageQueue (name, oflag, write, uflag)
{
}

#endif
