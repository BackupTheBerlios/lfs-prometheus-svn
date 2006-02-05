#! /bin/bash

# Copyright (C) 2003-2004 Oliver Brakmann <oliverbrakmann@users.berlios.de> &
# Gareth Jones <gareth_jones@users.berlios.de>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA


# Usage:
#
# pkgget {[-h|-?|--help] | [{-f|--config} <config file>] [-l|--list] <package>
#								[<package> ...]}
# Check if files needed by <package>s are present on system.  Download any that
# are not.

# Global prometheus functions.
source "%LIBPKG%" || exit 1

# Usage: usage
# Print help text.
function usage {
	cat <<-EOF
		Usage:
		$(basename $0) [<options>] <package> [<package> ...]

		Check if any files required by <package>s are present on the
		system and download any that are not.  <package> can be a
		package name, (\`bash'), or a package profile, (\`bash.pkg').
		
		Options:
		-h|-?|--help			Show this help screen
		-f|--config <config file>	Specify alternative configuration file
		-l|--list			List download locations but do not download

	EOF
}

# Usage: remove_found
# Read list of files, output any that are not present in $srcdir or $builddir.
# Warn on any that are missing & cannot be downloaded.
function remove_found {
	local file location protocol
	while read file; do

		# Skip blank lines.
		[[ -z "$file" ]] && continue

		# Skip files that are present.  Suppress not-found warnings.
		location=$(location_of "$file" 2> /dev/null)
		[[ -n "$location" ]] && continue

		# $file not present.  Can we download it?
		protocol="${file%%://*}"
		[[ "$protocol" == "http" || "$protocol" == "ftp" ]] &&
		echo "$file" && continue

		# $file not present & cannot be downloaded.
		warning "$file not found and cannot be downloaded"
	done
}

# Usage: list_missing <package>
# Print missing files in <package>.
function list_missing {
	if (( $# != 1 )) || [[ -z "$1" ]]; then
		usage_error "list_missing" "<package>" "$*"
	fi

	# Run in a subshell to avoid contamination between packages.
	(
		source_profile "$1"
		list_files | remove_found
	)
}

# Usage: split_components
# Read URLs from stdin & output lines with three `:'-separated fields:
# <protocol>:<hostname>:<path>
# <hostname> will be byte-reversed for easy of sorting.
function split_components {
	local file protocol host path
	while read file; do

		# Get & remove $protocol from $file.
		protocol="${file%%://*}"
		file="${file#$protocol://}"

		# Get $host.  Byte-reverse it.
		host="${file%%/*}"
		host=$(echo "$host" | rev)

		# Get $path.
		path="${file#*/}"

		echo "$protocol:$host:$path"
	done
}

# Usage: merge_components
# Read lines from stdin containing three `:'-separated fields:
# <protocol>:<hostname>:<path>
# Reconstruct & output the original URLs, byte-reversing <hostname>.
function merge_components {
	local line protocol host path
	while read line; do

		# Get & remove $protocol from $line.
		protocol="${line%%:*}"
		line="${line#$protocol:}"

		# Get $host.  Byte-reverse it.
		host="${line%%:*}"
		host=$(echo "$host" | rev)

		# Get $path.
		path="${line#*:}"

		echo "$protocol://$host/$path"
	done
}

# Usage: sort_downloads
# Read a list of downloads & sort them by server.  This allows downloaders to
# use persistant connections.
function sort_downloads {
	split_components |
	sort -t : -k 1,1 -k 2,2 -k 3,3 |
	merge_components
}

# Usage: list_downloads <package> [<package> ...]
# Sort & print download locations of file missing for <package>s.
function list_downloads {
	(( $# == 0 )) &&
	usage_error "list_downloads" "<package> [<package> ...]"

	for package in "$@"; do
		list_missing "$package"
	done | sort_downloads
}

# --------------------------- START EXECUTION HERE --------------------------- #

# Check for options.
while [[ "$1" == -* ]]; do
	case "$1" in
		(-h|-\?|--help)	usage
				exit 0;;

		(-f|--config)	[[ -z "$2" ]] &&
				script_usage_err "$1: no config file specified"

				pkgconfig="$2"
				shift;;

		(-l|--list)	listonly=yes ;;

		(*)		script_usage_err "unrecognised option: $1"
	esac
	shift
done

# The remaining arguments should all be packages.
(( $# == 0 )) && script_usage_err "no packages to get"

source_config

if [[ "$listonly" == yes ]]; then
	list_downloads "$@"
else
	list_downloads "$@" | wget --input-file=- ||
	error "download failed"
fi
