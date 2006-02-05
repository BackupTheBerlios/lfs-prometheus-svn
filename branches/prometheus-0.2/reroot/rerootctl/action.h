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

// A class for storing a message, and whether to wait for a reply.

#ifndef ACTION_H
# define ACTION_H

# include "inbox.h"
# include "message.h"
# include "outbox.h"

namespace reroot
{
	class Action;
}

class reroot::Action
{
public:
	// C'tor.
	Action (Message const &msg, bool r = false);

	// For sending the message & processing for a reply if necessary.
	void run (Inbox &inbox, Outbox &outbox) const;

private:
	// Data.
	Message message;
	bool reply;
};

// Action c'tor.  Contrust message.
inline reroot::Action::Action (Message const &msg, bool const r):
	message (msg),
	reply (r)
{
}

#endif
