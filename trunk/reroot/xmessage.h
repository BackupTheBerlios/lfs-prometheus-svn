// Shared reroot comunication exception declarations
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

#ifndef XMESSAGE_H
# define XMESSAGE_H

# include <stdexcept>
# include <string>

namespace reroot
{
	class xmessage;
}

// Exception thrown by message handling code.
class reroot::xmessage:
	public std::runtime_error
{
	public:
		xmessage (std::string const &action, int const errnum);

		int get_error_number () const;

	private:
		// The C error number if any.
		int const error_number;
};

// Return C error number.
inline int
reroot::xmessage::get_error_number () const
{
	return error_number;
}

#endif
