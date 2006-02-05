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

// The base class for a POSIX message queue handles its creation, opening,
// closing, and unlinking.

#ifndef MESSAGEQUEUE_H
# define MESSAGEQUEUE_H

# include <mqueue.h>
# include <string>

# include "libreroot.h"

namespace reroot
{
	class MessageQueue;
}

class shared reroot::MessageQueue
{
public:
	// Create a new queue or open an existing?
	enum OpenFlag
	{
		open = 0,
		create = O_CREAT | O_EXCL
	};

	// Open queue for reading or writing?
	enum AccessFlag
	{
		read = O_RDONLY,
		write = O_WRONLY
	};

	// When to unlink the queue.  Unlinking a queue means no further
	// processes will be able to open it, & it the memory used will be freed
	// once the last process closes it.
	enum UnlinkFlag
	{
		never,
		on_open,
		on_close
	};

	// For explicitly unlinking message queues.
	static void unlink_queue (std::string const &name, bool check = false);

	// C'tors & d'tor.
	MessageQueue (std::string const &prefix, std::string const &suffix,
		AccessFlag aflag, UnlinkFlag uflag = on_close);
	MessageQueue (std::string const &n, OpenFlag oflag, AccessFlag aflag,
		UnlinkFlag uflag = never);
	~MessageQueue ();

	// For accessing the queue name.
	std::string const &name () const;

protected:
	// Subclasses may need the queue descriptor.
	mqd_t queue () const;

private:
	// The name of the queue.
	std::string const queue_name;

	// The file descriptor corresponding to the queue.
	mqd_t const mqd;

	// Whether to unlink the queue on destruction.
	bool const unlink;

	// The compiler generated copy c'tor & assignment operator could cause
	// the message queue to be closed or unlinked multiple times.
	MessageQueue (MessageQueue const &);
	MessageQueue const &operator = (MessageQueue const &);
};

// Return the name of the message queue.
inline std::string const &reroot::MessageQueue::name () const
{
	return queue_name;
}

// Return the file descriptor corresponding to the POSIX message queue.
inline mqd_t reroot::MessageQueue::queue () const
{
	return mqd;
}

#endif
