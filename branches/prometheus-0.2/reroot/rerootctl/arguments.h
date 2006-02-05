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

// A class that interprets a command line and stores the results.

#ifndef ARGUMENTS_H
# define ARGUMENTS_H

# include <string>

# include "action.h"
# include "actions.h"

namespace reroot
{
	class Arguments;
}

class reroot::Arguments
{
public:
	// C'tor requires argument array.
	Arguments (int argc, char *argv []);

	// For accessing parameters.
	std::string const &message_queue () const;
	void set_message_queue (std::string const &q);
	bool unlink_queue () const;
	void set_unlink_queue (bool u);
	Actions const &actions () const;
	void append_action (Action const &action);

private:
	// Data.
	std::string queue;
	bool unlink;
	Actions action_queue;
};

// Return the name of the message queue.
inline std::string const &reroot::Arguments::message_queue () const
{
	return queue;
}

// Set the name of the index file.
inline void reroot::Arguments::set_message_queue (std::string const &q)
{
	queue = q;
}

// Return true if the queue is to be unlinked.
inline bool reroot::Arguments::unlink_queue () const
{
	return unlink;
}

// Set whether the queue is to be unlinked.
inline void reroot::Arguments::set_unlink_queue (bool u)
{
	unlink = u;
}

// Return a vector of actions to perform.
inline reroot::Actions const &reroot::Arguments::actions () const
{
	return action_queue;
}

// Append an action.
inline void reroot::Arguments::append_action (Action const &action)
{
	action_queue.push_back (action);
}

#endif
