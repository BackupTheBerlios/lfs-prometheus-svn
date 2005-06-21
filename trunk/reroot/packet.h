// Reroot communication data structures
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

// Data structures used in messages.  These must be shared between the libreroot
// C sources & rerootd C++ sources to ensure they interpret messages sent by
// each other correctly.

#ifndef PACKET_H
# define PACKET_H

# ifdef __cplusplus
#  define BEGIN_DECLS extern "C" {
#  define END_DECLS }
#  define ENUM(X)
#  define STRUCT(X)
# else
#  define BEGIN_DECLS
#  define END_DECLS
#  define ENUM(X) typedef enum X X;
#  define STRUCT(X) typedef struct X X;
# endif

// Maximum packet size, minus the PID, in bytes.
# define packet_data_size (sizeof (packet) - sizeof (long))

// Maximum packet body size in bytes.  See below.
# define packet_body_size (512 - (sizeof (long) + 4 * sizeof (unsigned)))

BEGIN_DECLS

// Message types.
enum message_type
{
	def
};
ENUM (message_type)

// Messages are split into packets of known maximum size.  PID is of the sender
// or receiver, whichever *isn't* the daemon.  Type is unsigned rather than
// message_type as the C99 & C++98 standards have different requirements for the
// sizes of enumerated types (so far as I can tell), which could theoretically
// mess up alignment & cause the daemon & libreroot to interpret the structure
// differently.
struct packet
{
	long pid;
	unsigned type,			// Type of message represented in body.
	         body_size,		// Size of entire message body.
	         packets_left,		// Number of packets left in message.
	         packet_size;		// Size of packet, minus PID.
	char body [packet_body_size];	// Attached data.
};
STRUCT (packet)

END_DECLS

# undef BEGIN_DECLS
# undef END_DECLS
# undef ENUM
# undef STRUCT
#endif
