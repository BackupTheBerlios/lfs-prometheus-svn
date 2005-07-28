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
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <error.h>
#include <exception>
#include <string>
#include <sys/resource.h>
#include <unistd.h>

#include "messagequeue.h"
#include "rerootd.h"

using namespace std;

// Values used by default --version & --help handlers.
char const *argp_program_version = "rerootd 0.00";
char const *argp_program_bug_address =
	"Gareth Jones <gareth_jones@users.berlios.de>";

// False root directory & index file name.
string const reroot::false_root,
             reroot::index_file = "index";

namespace
{
	// For storing arguments after parsing.
	struct arguments
	{
		bool background;
		string false_root,
		       index_file;

		arguments (): background (true) {}
	};

	// --help documentation.
	char const args_doc [] = "ROOT";
	char const doc [] =
		"The reroot daemon maintains file metadata for processes that "
		"have been rerooted via libreroot.  The cached metadata are "
		"typically permissions and ownership (such as setuid root) that"
		" cannot be committed to the filesystem due to a lack of "
		"privileges.\vROOT is the directory to use as a false root "
		"directory.  Cached metadata are saved to an index file.";

	// Commandline argument parser for use by argp.
	error_t
	argument_parser (int key, char arg [], argp_state *state)
	{
		// Error messages.
		static char const index_dupe [] = "Index file specified twice",
		                  root_dupe [] = "False root specified twice";

		// Get argument structure.
		arguments &args = *static_cast <arguments *> (state->input);

		// Recognize argument.
		switch (key)
		{
		case 'f':
			args.background = false;
			break;

		case 'i':
			if (args.index_file.length ())
				argp_failure (state, 1, 0, index_dupe);

			args.index_file = arg;
			break;

		case ARGP_KEY_ARG:
			if (args.false_root.length ())
				argp_failure (state, 1, 0, root_dupe);

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
	daemonize ()
	{
		// Error message.
		static char const fork_error [] = "Cannot fork child process";

		// Get maximum possible number of open file descriptors.
		rlimit lim;
		getrlimit (RLIMIT_NOFILE, &lim);

		switch (pid_t pid = fork ())
		{
		case 0:
			// This is the child.  Safely close all streams & their
			// file descriptors.
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
			error (1, errno, fork_error);

		default:
			// This is the parent.  Report child's PID & exit.
			printf ("%i\n", pid);
			exit (0);
		}
	}
}

int
main (int const argc, char *argv [])
try
{
	// Error messages.
	static char const no_root [] = "False root directory not specified",
	                  abs_fail [] = "Cannot get absolute filename of false"
	                                " root directory";

	// Parse arguments.
	arguments args;
	parse_commandline (args, argc, argv);

	// Set false_root constant.
	{
		if (args.false_root.empty ())
			error (1, errno, no_root);

		char *false_root =
			canonicalize_file_name (args.false_root.c_str ());
		if (false_root)
		{
			const_cast <string &> (reroot::false_root) = false_root;
			free (false_root);
		}
		else
			error (1, errno, abs_fail);
	}

	// Set index_file constant if specified.
	if (args.index_file.length ())
		const_cast <string &> (reroot::index_file) = args.index_file;

	// If necessary background daemon.
	if (args.background)
		daemonize ();
	else
	{
		// Run in foreground.  Report PID.
		printf ("%i\n", getpid ());
		fflush (stdout);
	}

	// Register signal handlers for manipulating the file database.
	{
		// Initialize signal handler data structure.
		struct sigaction handler;
		handler.sa_handler = reroot::signal_handler;
		handler.sa_flags = SA_RESTART;

		// Ensure USR1 & USR2 are mutually blocking.
		sigemptyset (&handler.sa_mask);
		sigaddset (&handler.sa_mask, SIGUSR1);
		sigaddset (&handler.sa_mask, SIGUSR2);

		// Register handlers.
		sigaction (SIGUSR1, &handler, 0);
		sigaction (SIGUSR2, &handler, 0);
	}

	// Register termination signal handlers to ensure the System V IPC
	// message queue is properly deallocated on exit.
	{
		// Initialize signal handler data structure.
		struct sigaction handler;
		handler.sa_handler = reroot::signal_exit;
		handler.sa_flags = 0;
		sigemptyset (&handler.sa_mask);

		// Register handlers for HUP, INT & TERM.  Other termination
		// signals should leave the queue allocated for debugging.
		sigaction (SIGHUP, &handler, 0);
		sigaction (SIGINT, &handler, 0);
		sigaction (SIGTERM, &handler, 0);
	}

	// Start message loop.
	reroot::message_loop ();

	return 0;
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
