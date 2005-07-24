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

#include <csignal>
#include <stdexcept>

#include "file.h"
#include "message.h"
#include "messagequeue.h"
#include "rerootd.h"

namespace
{
	// Stores number of signal most recently caught by signal handler.
	int volatile signal_number = 0;

	// Signal handler implementation.  Convert signals to System V IPC
	// messages to self.  HUP becomes save & exit, USR1 becomes cleanup
	// database.
	void
	handle_signal (int const signum, reroot::message_queue const &queue)
	{
		reroot::message_type type;

		// Identify signal type, & hence message type to send.
		switch (signum)
		{
		case SIGHUP:
			type = reroot::save_and_exit;
			break;

		case SIGUSR1:
			type = reroot::cleanup_db;
			break;

		default:
			throw std::invalid_argument ("handle_signal: Bad signal"
			                             " number");
		}

		// Send message to self.
		queue.send_self (type);
	}
}

// Signal handler.  Store values of caught signals for processing in message
// loop.
void
reroot::signal_handler (int const signum)
{
	signal_number = signum;
}

// Message loop - wait for a message, deal with it (maybe reply), loop.
void __attribute__ ((noreturn))
reroot::message_loop (message_queue const &queue)
{
	file_db db;
	message msg;

	// The message loop.
	while (true)
	{
		// Have we received any signals?
		if (int const signum = signal_number)
		{
			signal_number = 0;	// Could lose new signals here.
			handle_signal (signum, queue);
		}

		// Get message.
		queue >> msg;

		// Choose appropriate handler.
		switch (msg.get_type ())
		{
		case save_and_exit:
			break;

		case cleanup_db:
			break;
		}
	}
}
