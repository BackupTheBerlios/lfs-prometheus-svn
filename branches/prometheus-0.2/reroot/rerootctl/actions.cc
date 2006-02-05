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

#include "action.h"
#include "actions.h"
#include "inbox.h"
#include "outbox.h"

using namespace reroot;
using namespace std;

namespace
{
	// Functor for running actions with a given inbox & outbox.
	class RunAction
	{
	public:
		RunAction (Inbox &i, Outbox &o);
		void operator () (Action const &action);

	private:
		Inbox &inbox;
		Outbox &outbox;
	};

	// RunAction c'tor.
	inline RunAction::RunAction (Inbox &i, Outbox &o):
		inbox (i),
		outbox (o)
	{
	}

	// Run the action.
	inline void RunAction::operator () (Action const &action)
	{
		action.run (inbox, outbox);
	}
}

// Run the actions in order, given the name of the daemon's message queue.
void Actions::run (string const &outbox_name) const
{
	// Create message queues.
	Outbox outbox (outbox_name, Outbox::open);	// The daemon's queue.
	Inbox inbox ("rerootctl");			// For replies.

	// Run the actions.
	for_each (begin (), end (), RunAction (inbox, outbox));
}
