// rerootd message declarations
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

#ifndef MESSAGE_H
# define MESSAGE_H

# include "packet.h"

// A message between the reroot daemon & a process using libreroot.
class message
{
	public:
		explicit message (long const p = 0, message_type const t = def);

		void set_pid (long const p);
		long get_pid () const;

		void set_type (message_type const t);
		message_type get_type () const;

	private:
		// PID of sender or receiver, whichever *isn't* the daemon.
		long pid;

		// Type of message represented.
		message_type type;
};

// Construct message.
inline
message::message (long const p, message_type const t):
	pid (p),
	type (t)
{}

// Set PID.
inline void
message::set_pid (long const p)
{
	pid = p;
}

// Return PID.
inline long
message::get_pid () const
{
	return pid;
}

// Set message type.
inline void
message::set_type (message_type const t)
{
	type = t;
}

// Return message type.
inline message_type
message::get_type () const
{
	return type;
}

#endif
