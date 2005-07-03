// rerootd initialization
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

#include <argp.h>
#include <cstdio>
#include <error.h>
#include <string>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>

#include "absolute.h"
#include "messagequeue.h"
#include "rerootd.h"
#include "xmessage.h"

// We're using C functions, some of which may be in the std namespace, some of
// which are global.
using namespace std;

// Values used by default --version & --help handlers.
char const *argp_program_version = "rerootd 0.00";
char const *argp_program_bug_address =
	"Gareth Jones <gareth_jones@users.berlios.de>";

// False root directory.
string const false_root;

namespace
{
	// For storing arguments after parsing.
	struct arguments
	{
		arguments (): background (true) {}
		bool background;
		string false_root,
		       index_file;
	};

	// --help documentation.
	char const args_doc [] = "[ROOT]";
	char const doc [] =
		"The reroot daemon maintains file metadata for processes that "
		"have been rerooted via libreroot.  The cached metadata are "
		"typically permissions and ownership (such as setuid root) that"
		" cannot be committed to the filesystem due to a lack of "
		"privileges.\vIf specified, ROOT is the directory to use as a "
		"false root directory, otherwise the current working directory "
		"is used.  Cached metadata are saved to an index file, whose "
		"name may be specified as an absolute filename, or relative to "
		"ROOT.";

	// Commandline argument parser for use by argp.
	error_t
	argument_parser (int key, char arg [], argp_state *state)
	{
		arguments &args = *static_cast <arguments *> (state->input);
		switch (key)
		{
			case 'f':
				args.background = false;
				break;

			case 'i':
				if (!args.index_file.empty ())
					argp_failure (state, 1, 0, "index file "
					              "specified twice");
				args.index_file = arg;
				break;

			case ARGP_KEY_ARG:
				if (!args.false_root.empty ())
					argp_failure (state, 1, 0, "false root "
					              "specified twice");
				args.false_root = arg;
				break;

			default:
				return ARGP_ERR_UNKNOWN;
		}
		return 0;
	}

	// Use argp to parse commandline.
	void parse_commandline (arguments &args, int const argc, char *argv [])
	{
		// Recognized options.
		argp_option options [] =
		{
			{
				"foreground",
				'f',
				0,
				0,
				"Do not background the daemon",
				0
			},
			{
				"index-file",
				'i',
				"FILE",
				0,
				"Specify filename for index (default = "
					"`index')",
				0
			},
			{0, 0, 0, 0, 0, 0}
		};

		// The parser.
		argp parser =
		{
			options,
			argument_parser,
			args_doc,
			doc,
			0,
			0,
			0
		};

		// Parse the command line.
		argp_parse (&parser, argc, argv, 0, 0, &args);
	}

	// Fork child daemon in background & exit.
	void
	daemonize (message_queue &queue)
	{
		// Get maximum possible number of open file descriptors.
		rlimit lim;
		getrlimit (RLIMIT_NOFILE, &lim);

		switch (pid_t pid = fork ())
		{
			case 0:
				// This is the child.  Safely close all streams
				// & their file descriptors.
				// FIXME: C++ streams?
				fcloseall ();

				// Close any remaining file descriptors.
				// FIXME: There's gotta be a better way...
				for (unsigned i = 0; i < lim.rlim_cur; ++i)
					close (i);

				// Create new session for daemon.
				setsid ();
				break;

			case -1:
				error (1, errno, "Cannot fork child process");

			default:
				// This is the parent.  Report child's PID &
				// exit.  Do not deallocate message queue.
				queue.disown ();
				printf ("%i\n", pid);
				exit (0);
		}
	}
}

int
main (int const argc, char *argv [])
try
{
	// Parse arguments.
	arguments args;
	parse_commandline (args, argc, argv);

	// Set false_root constant.
	absolute_filename (args.false_root);
	const_cast <string &> (false_root) = args.false_root;

	// Create message System V IPC queues.  Static to ensure it is destroyed
	// on exit.
	static message_queue queue (false_root);

	// If necessary background daemon.
	if (args.background)
		daemonize (queue);
	else
	{
		// Run in foreground.  Report PID.
		printf ("%i\n", getpid ());
		fflush (stdout);
	}

	return 0;
}
catch (...)
{
	// Prevent unhandled exceptions resulting in abortion, which would not
	// deallocate the System V IPC message queue.  Apart from wasting memory
	// until manually unallocated, libreroot processes would hang waiting
	// for replies rather than aborting.
	// FIXME: Some kind of error reporting/logging?
	return 1;
}