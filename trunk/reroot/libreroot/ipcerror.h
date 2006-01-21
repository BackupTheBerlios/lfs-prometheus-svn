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

// Exception thrown if IPC fails.

#ifndef IPCERROR_H
# define IPCERROR_H

# include <sstream>
# include <stdexcept>
# include <string>

# include "libreroot.h"

namespace reroot
{
	class IPCError;
}

class shared reroot::IPCError:
	public std::runtime_error
{
public:
	// For throwing exceptions with detailed error messages.  Items must
	// have describe member functions.
	template <typename Item> __attribute__ ((noreturn))
		static void error (Item const &item, std::string const &what);
	template <typename Item1, typename Item2> __attribute__ ((noreturn))
		static void error (Item1 const &item1, Item2 const &item2,
		                   std::string const &what);

	// C'tor.  Requires a description of what failed.  If errnum is
	// non-zero, it is interpreted as a C error number & converted to a
	// string which is appended to the description.
	IPCError (std::string const &what, int errnum = 0);

	// So that the C error number can be queried.
	int get_error_number () const;

private:
	// The C error number.
	int const error_number;
};

// Throw an exception regarding item.
template <typename Item> __attribute__ ((noreturn))
void reroot::IPCError::error (Item const &item, std::string const &what)
{
	std::ostringstream os;
	os << what << std::endl;
	item.describe (os);
	throw IPCError (os.str ());
}

// Throw an exception regarding items.
template <typename Item1, typename Item2> __attribute__ ((noreturn))
void reroot::IPCError::error (Item1 const &item1,
                              Item2 const &item2,
                              std::string const &what)
{
	std::ostringstream os;
	os << what << std::endl;
	item1.describe (os);
	item2.describe (os);
	throw IPCError (os.str ());
}

// Return the C error number associated with the exception.
inline int reroot::IPCError::get_error_number () const
{
	return error_number;
}

#endif
