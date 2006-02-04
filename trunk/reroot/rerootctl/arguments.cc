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

#include <argp.h>
#include <cstdlib>

#include "arguments.h"
#include "message.h"

using namespace reroot;
using namespace std;

// Values used by --version & --help handlers.  These must be global.
char const *argp_program_version = "rerootctl 0.00";
char const *argp_program_bug_address =
	"Gareth Jones <gareth_jones@users.berlios.de>";

namespace
{
	// --help documentation.
	char const args_doc [] = "QUEUE";
	char const doc [] =
		"Send messages to the reroot deamon.\vThe reroot daemon "
		"responds to messages sent to it via a POSIX message queue.  "
		"QUEUE is the name of the message queue to use.  Messages are "
		"sent in the order they occur on the command line.  If no "
		"messages are specified, none are sent, but the message queue "
		"is still opened.  This can be used to test if the queue "
		"exists.";

	// Recognized options.
	argp_option const options [] =
	{
		{
			0,
			0,
			0,
			0,
			"The following control messages may be sent:",
			0
		},
		{
			"save-index",
			'i',
			"FILE",
			0,
			"Save index to FILE",
			0
		},
		{
			"quit",
			'q',
			0,
			0,
			"Tell rerootd to exit",
			0
		},
		{
			0,
			0,
			0,
			0,
			"If given, this must be the only option:",
			0
		},
		{
			"unlink",
			'u',
			0,
			0,
			"Unlink the message queue",
			0
		},
		{
			0, 0, 0, 0, 0, 0
		}
	};

	// Argument parser.
	error_t argument_parser (int key, char arg [], argp_state *state)
	{
		// Error messages.
		static char const q_dupe [] =
			"Message queue specified multiple times";
		static char const no_queue [] =
			"No message queue specified";
		static char const unlink [] =
			"--unlink must be the only option on the commandline";

		// Get arguments object.
		Arguments &args = *static_cast <Arguments *> (state->input);

		// Recognize argument.
		switch (key)
		{
		case 'i':	// --save-index FILE.
			// FIXME: Canonicalize filename.
			args.append_action (Message (Message::save_index, arg));
			break;

		case 'q':	// --quit.
			args.append_action (Message (Message::terminate));
			break;

		case 'u':	// --unlink.
			args.set_unlink_queue (true);
			break;

		case ARGP_KEY_ARG:	// Message queue name.
			if (!args.message_queue ().empty ())
				argp_failure (state, EXIT_FAILURE, 0, q_dupe);
			args.set_message_queue (arg);
			break;

		case ARGP_KEY_SUCCESS:
			// Must have a queue name.
			if (args.message_queue ().empty ())
				argp_failure (state, EXIT_FAILURE, 0, no_queue);

			// Cannot unlink queue & also send messages.
			if (args.unlink_queue () && args.actions ().size ())
				argp_failure (state, EXIT_FAILURE, 0, unlink);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
		}

		return 0;
	}
}

// Arguments c'tor.  Parse command line.
Arguments::Arguments (int argc, char *argv []):
	unlink (false)
{
	argp const parser =
	{
		options,
		argument_parser,
		args_doc,
		doc,
		0,
		0,
		0
	};

	argp_parse (&parser, argc, argv, 0, 0, this);
}
