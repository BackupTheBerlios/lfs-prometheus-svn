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

#include <cerrno>
#include <mqueue.h>
#include <string>
#include <sstream>
#include <unistd.h>

#include "ipcerror.h"
#include "messagequeue.h"
#include "packet.h"

using namespace reroot;
using namespace std;

namespace
{
	// Generate a message queue name given a prefix & suffix.
	inline string generate_name (string const &prefix, string const &suffix)
	{
		ostringstream os;
		os << '/' << prefix << '-' << getpid () << '-' << suffix;
		return os.str ();
	}

	// Helper for creating or opening POSIX message queues.  Provides error
	// handling & retries if interrupted by a signal.  Will unlink the queue
	// to prevent further opening if requested.
	mqd_t open_queue (string const &name,
	                  int const flags,
	                  bool const unlink)
	{
		static char const error [] = "Cannot open message queue: `";

		// Set message queue attributes.
		mq_attr attrib;
		attrib.mq_maxmsg = 10;
		attrib.mq_msgsize = sizeof (Packet);

		do
		{
			// Try to open a queue.  Return if successful.
			mqd_t const queue =
				mq_open (name.c_str (), flags, 0600, &attrib);
			if (queue != -1)
			{
				// Unlinking the queue will prevent it being
				// opened in future.  It will be deallocated
				// when its last descriptor is closed.  This
				// doesn't make much sense if we just created
				// the queue.
				if (unlink)
					MessageQueue::unlink_queue (name);

				return queue;
			}
		}
		while (errno == EINTR);	// Retry if interrupted by a signal.

		// We only get this far if there was an error.
		throw IPCError (error + name + '\'', errno);
	}
}

// Unlink the named message queue.  If check is true & unlinking fails, throw an
// exception.
void MessageQueue::unlink_queue (string const &name, bool const check)
{
	static char const error [] = "Cannot unlink message queue: `";

	if (mq_unlink (name.c_str ()) && check)
		throw IPCError (error + name + '\'', errno);
}

// Create a message queue by generating a name from the given prefix & suffix.
MessageQueue::MessageQueue (string const &prefix,
                            string const &suffix,
                            AccessFlag const aflag,
                            UnlinkFlag const uflag):
	name (generate_name (prefix, suffix)),
	mqd (open_queue (name, create | aflag, uflag == on_open)),
	unlink (uflag == on_close)
{
}

// Create or open a message queue with the given name.
MessageQueue::MessageQueue (string const &n,
                            OpenFlag const oflag,
                            AccessFlag const aflag,
                            UnlinkFlag const uflag):
	name (n),
	mqd (open_queue (name, oflag | aflag, uflag == on_open)),
	unlink (uflag == on_close)
{
}

// Close the message queue & unlink it if required.
MessageQueue::~MessageQueue ()
{
	mq_close (queue ());

	if (unlink)
		unlink_queue (name);
}
