// File metadata
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

#ifndef FILE_H
# define FILE_H

# include <map>
# include <string>

namespace reroot
{
	class file;
	typedef std::map <std::string, file> file_db;
}

// Stores file metadata & status information.
class reroot::file
{
	public:
		// For storing file status.
		enum file_status
		{
			unmodified,
			modified,
			created,
			removed
		};

		// C'tor.
		file (file_status const s = unmodified);

		// For accessing file status.
		file_status get_status () const;
		void create ();
		void modify ();
		void remove ();

		// For accessing file metadata status.
		bool metadata_is_modified () const;

	private:
		file_status status;
		bool metadata_modified;
};

// Initialize file status.
inline
reroot::file::file (file_status const s):
	status (s),
	metadata_modified (false)
{}

// Return file status.
inline reroot::file::file_status
reroot::file::get_status () const
{
	return status;
}

// Set status to reflect that the file has been created.
inline void
reroot::file::create ()
{
	// If file was previously removed, mark as modified (we have effectively
	// unremoved it & modified it).  Otherwise simply flag as created.
	status = (status == removed? modified : created);
}

// Set status to reflect that the file has been modified.
inline void
reroot::file::modify ()
{
	// Created implies modified.
	if (status != created)
		status = modified;
}

// Set status to reflect that the file has been removed.
inline void
reroot::file::remove ()
{
	// If file was originally created here, mark it as unmodified as it
	// didn't exist in the first place.  Otherwise flag as removed.
	status = (status == created? unmodified : removed);

	// Reset metadata in case the file is recreated.  FIXME: Reset metadata.
	metadata_modified = false;
}

// Return metadata status.
inline bool
reroot::file::metadata_is_modified () const
{
	return metadata_modified;
}

#endif
