// File metadata declarations
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

# include "messages.h"

namespace reroot
{
	// File metadata database.
	class file;
	typedef std::map <std::string, file> file_db;

	// For manipulating the file database.
	void write_index (file_db &db, bool const dump);
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

		// C'tors.
		file () {}
		explicit file (file_meta const &meta,
			file_status const s = unmodified);

		// For accessing file status.
		file_status get_status () const;
		void create ();
		void modify ();
		void remove ();

		// For accessing data status.
		bool is_stale () const;

		// For accessing file metadata status.
		bool metadata_is_modified () const;
		file_meta const &get_metadata () const;
		void set_metadata (file_meta const &meta);

	private:
		// Data.
		file_status status;
		bool stale;
		bool metadata_modified;
		file_meta metadata;
};

// Initialize file status & metadata.
inline
reroot::file::file (file_meta const &meta, file_status const s):
	status (s),
	stale (false),
	metadata_modified (false),
	metadata (meta)
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
	status = created;
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
	// If the file was first created here, we can forget about it now.
	if (status == created)
		stale = true;

	status = removed;
}

// Return true if this entry should be removed from the file database.
inline bool
reroot::file::is_stale () const
{
	return stale;
}

// Return metadata status.
inline bool
reroot::file::metadata_is_modified () const
{
	return metadata_modified;
}

// Return file metadata.
inline reroot::file_meta const &
reroot::file::get_metadata () const
{
	return metadata;
}

// Set file metadata.
inline void
reroot::file::set_metadata (file_meta const &meta)
{
	metadata = meta;
	metadata_modified = true;
}

#endif
