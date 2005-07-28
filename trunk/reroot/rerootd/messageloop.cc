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
#include <cstdlib>
#include <error.h>
#include <exception>
#include <stdexcept>

#include "file.h"
#include "message.h"
#include "messagequeue.h"
#include "rerootd.h"

using namespace std;

namespace
{
	// Respond to a request for file metadata.
	void
	send_meta (reroot::outbox const &queue,
	           reroot::file_db const &db,
	           reroot::message const &request)
	{
		// Error message.
		static char const not_found [] = "File is not in database: ";

		// Get entry for file identified in request.
		string const name = &request.get_body <char> ();
		reroot::file_db::const_iterator const entry = db.find (name);

		if (entry == db.end ())
			throw runtime_error (not_found + name);

		// Prepare reply.
		reroot::message reply (sizeof (reroot::file_meta));
		reply.set_pid (request.get_pid ());
		reply.set_type (reroot::metadata);
		reply.get_body <reroot::file_meta> () =
			entry->second.get_metadata ();

		// Send reply.
		queue << reply;
	}
}

// Termination signal handler.  Call exit so that the message queue is properly
// destroyed.  The default handlers would leave the queue allocated.
void __attribute__ ((noreturn))
reroot::signal_exit (int const)
{
	exit (0);
}

// Signal handler.  Convert signals to System V IPC messages to self.  USR1
// cleanup & save, USR2 becomes dump database.
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
	case SIGUSR1:
		type = reroot::cleanup_and_save;
		break;

	case SIGUSR2:
		type = reroot::dump_db;
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
void __attribute__ ((noreturn))
reroot::message_loop ()
{
	// Error message.
	static char const bad_type [] = "Received message has invalid type";

	// The file metadata database.
	file_db db;

	// The message loop.
	message_queue const &queue = get_message_queue ();
	message msg;
	while (true)
	{
		// Get message.
		queue >> msg;

		// Choose appropriate handler.
		switch (msg.get_type ())
		{
		case cleanup_and_save:
			// Write an index file suitable for use in package
			// management.  Prepare the database for archiving the
			// false root directory.
			write_index (db, false);
			break;

		case dump_db:
			// Dump database to the index file unmodified.  This is
			// mainly used for debugging.
			write_index (db, true);
			break;

		case get_metadata:
			// Reply to request for file metadata.
			send_meta (queue, db, msg);
			break;

		default:
			throw runtime_error (bad_type);
		}
	}
}
