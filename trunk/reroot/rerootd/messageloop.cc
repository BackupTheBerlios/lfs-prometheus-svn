// Message handling loop
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

#include <cerrno>
#include <csignal>
#include <error.h>
#include <exception>
#include <stdexcept>

#include "file.h"
#include "message.h"
#include "messagequeue.h"
#include "rerootd.h"

using namespace std;

// Signal handler.  Convert signals to System V IPC messages to self.  HUP
// becomes save & exit, USR1 becomes cleanup database.
void
reroot::signal_handler (int const signum)
try
{
	// Error message.
	static char const bad_signal [] = "signal_handler: Bad signal number";

	// Identify signal type, & hence message type to send.
	reroot::message_type type;
	switch (signum)
	{
	case SIGHUP:
		type = reroot::save_and_exit;
		break;

	case SIGUSR1:
		type = reroot::cleanup_db;
		break;

	default:
		throw invalid_argument (bad_signal);
	}

	// Send message to self.
	reroot::get_message_queue ().send_self (type);
}

// Prevent unhandled exceptions resulting in abortion, which would not
// deallocate the System V IPC message queue.  Apart from wasting memory until
// manually unallocated, libreroot processes would hang waiting for replies
// rather than aborting.  FIXME: stderr may be closed.  Need some other way to
// report errors.
catch (exception const &x)	// All standard & reroot exceptions.
{
	error (1, 0, "Caught exception: %s", x.what ());
}
catch (...)			// Should never get here!
{
	error (1, errno, "Caught unrecognized exception, "
	                 "possible error may follow");
}

// Message loop - wait for a message, deal with it (maybe reply), loop.
void
reroot::message_loop ()
{
	// Initialize file database.
	file_db db;
	read_index (db);

	// The message loop.
	message msg;
	while (true)
	{
		// Get message.
		get_message_queue () >> msg;

		// Choose appropriate handler.
		switch (msg.get_type ())
		{
		case save_and_exit:
			// Write index file & exit.
			write_index (db);
			return;

		case cleanup_db:
			// Cleanup file database.
			cleanup (db);
			break;

		case metadata:		// FIXME.
			break;

		case get_metadata:	// FIXME.
			break;
		}
	}
}
