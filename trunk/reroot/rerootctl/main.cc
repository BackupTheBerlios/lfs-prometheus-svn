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
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>

#include "actions.h"
#include "arguments.h"
#include "messagequeue.h"

using namespace reroot;
using namespace std;

int main (int argc, char *argv [])
try
{
	// Parse commandline.
	Arguments const args (argc, argv);

	if (args.unlink_queue ())
		// Unlink message queue.
		MessageQueue::unlink_queue (args.message_queue (), true);
	else
		// Process actions.
		args.actions ().run (args.message_queue ());

	return EXIT_SUCCESS;
}
catch (exception const &x)
{
	// All standard & reroot exceptions.
	cerr << argv [0] << ": " << x.what () << endl;
	return EXIT_FAILURE;
}
catch (...)
{
	// We should never get here.  Maybe errno is set?  Even if it is, it
	// may not correspond to the exception, but report it anyway.
	cerr << argv [0] << ": Caught unrecognized exception";
	if (int const err = errno)
		cerr << ": C error number " << err << ": " << strerror (err);
	cerr << endl;
	return EXIT_FAILURE;
}
