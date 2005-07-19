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

#include "file.h"
#include "message.h"
#include "messagequeue.h"
#include "rerootd.h"

// Message loop - wait for a message, deal with it (maybe reply), loop.
void __attribute__ ((noreturn))
reroot::message_loop (message_queue const &queue)
{
	file_db db;
	message msg;

	// The message loop.
	while (true)
	{
		// Get message.
		queue >> msg;

		// Choose appropriate handler.
		switch (msg.get_type ())
		{
		case def:
			;
		}
	}
}
