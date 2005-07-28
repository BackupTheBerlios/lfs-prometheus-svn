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

#include <fstream>
#include <stdexcept>
#include <string>

#include "file.h"
#include "rerootd.h"

using namespace std;

namespace
{
	// Write file metadata to file stream.
	inline ostream &
	operator << (ostream &os, reroot::file_meta const &meta)
	{
		return os << meta.mode << '\t' << meta.uid << '\t' << meta.gid;
	}

	// Write file database entry to file stream.
	ostream &
	operator << (ostream &os, reroot::file const &file)
	{
		// Create string describing file status.
		string status;
		switch (file.get_status ())
		{
		case reroot::file::unmodified:
			status = "unmodified";
			break;

		case reroot::file::modified:
			status = "modified";
			break;

		case reroot::file::created:
			status = "created";
			break;

		case reroot::file::removed:
			status = "removed";
		}

		// Write file information.
		os << status << '\t'
		   << file.metadata_is_modified () << '\t'
		   << file.get_metadata ();

		return os;
	}

	// Dump file database to file stream.
	void
	dump (ostream &os, reroot::file_db const &db)
	{
		typedef reroot::file_db::const_iterator iterator;

		// Write 1 record per line, metadata first, then filename.
		iterator const end = db.end ();
		for (iterator i = db.begin (); os && i != end; ++i)
		{
			if (i->second.is_stale ())
				os << "STALE:\t";
			os << i->second << '\t' << i->first << endl;
		}
	}

	// Write file database to file stream, cleaning database on the fly.
	void
	write_clean (ostream &/*os*/, reroot::file_db &/*db*/)
	{
		// FIXME.
	}
}

// Write file database to index file.
void
reroot::write_index (file_db &db, bool const dump_all)
{
	// Error message.
	static char const file_error [] = "Cannot open index file: ",
	                  write_error [] = "Error writing to index file: ";

	// Try to open & write file.
	ofstream os (index_file.c_str ());
	if (os)
	{
		// Is this a clean index or simple database dump?
		if (dump_all)
			dump (os, db);
		else
			write_clean (os, db);
	}
	else
		throw runtime_error (file_error + index_file);

	// Check write succeeded.
	if (!os)
		throw runtime_error (write_error + index_file);
}
