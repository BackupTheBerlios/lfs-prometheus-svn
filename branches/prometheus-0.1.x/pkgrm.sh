#!/bin/bash

# Copyright (C) 2003-2004 Oliver Brakmann <obrakmann@gmx.net> &
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
# pkgrm {[-?|-h|--help] | [{-f|--config} <config file>]
#	[-b|--backup [<backup dir>]] <package> [ <package>  ...]}
# Remove <package>

source "%LIBPKG%" || exit 1

# Usage: usage
# Print help screen
function usage {
	cat <<-EOF
		Usage:
		$(basename $0) [<options>] <package> [<package> ...]

		Remove an installed package from the system.

		Options:
		-?|-h|--help			Show this help screen

		-b|--backup [<backup dir>]	Backup package in <backup dir> before removal
		-f|--config <config file>	Specify alternative configuration file

	EOF
}

# Usage: remove_logs <package>
# Remove install-log log file and .vtab entry for <package>
function remove_logs {

	(( $# != 1 )) && usage_error "remove_logs" "<package>" "$*"

	local tmpfile=/tmp/.vtab.$$

	# remove entry for <package> from .vtab
	[[ -r "$logdir/.vtab" ]] &&
	grep -v "^$1	" "$logdir/.vtab" > $tmpfile &&
	mv $tmpfile "$logdir/.vtab" ||
	warning "remove_logs: could not update vtab (is it empty?)."

	# remove install-log log file for <package>
	[[ -f "$logfile" ]] && rm -f "$logfile" ||
	error "remove_logs: could not remove $1 installation log file."

	return 0
}	

# Usage: rm_empty_tree <directory> [directory ...]
# Recursively remove a directory tree. Stop at the first non-empty directory.
function rm_empty_tree {

	while [[ -n "$1" ]]; do

		if rmdir "$1" 2> /dev/null; then
			rm_empty_tree "${1%/*}" 
		fi
		shift
	done

	return 0
}

# Usage collect_dirs <logfile>
# Build a list of those directories that the current pkg stored files in.
function collect_dirs {

	[[ -z "$1" ]] && return 1

	# Do not remove '# Del #'eleted items from the list,
	# the directories might still exist
	sed -e 's/# Old # //' -e 's/# Del # //' "$1" | \
	while read dir; do
		echo "${dir%/*}"
	done | sort | uniq
}

# Usage: remove_files <package>
# Remove all files belonging to <package> that were logged during installation
function remove_files {

	(( $# != 1 )) && usage_error "remove_files" "<package>" "$*"

	[[ -r "$logfile" ]] ||
	error "remove_files: cannot read $1 installation log file."

	# Weed out `# Old #' markings and remove lines marked `# Del #'
	sed -e 's/# Old # //' -e '/# Del #.*/d' "$logfile" | xargs -r rm -f ||
	error "remove_files: could not remove package $1."

	rm_empty_tree $(collect_dirs "$logfile")

	return 0
}

# Usage: backup_package <package>
# Create a backup of a working installation of <package> in $builddir
function backup_package {

	(( $# != 1 )) && usage_error "backup_package" "<package>" "$*"

	[[ -r "$logfile" ]] ||
	error "backup_package: cannot read $1 installation log file."

	[[ -d "${backupdir:=$builddir}" ]] || 
	error "backup_package: $backupdir does not exist."

	# xargs -r aborts when stdin is empty.
	# tar -P does not strip leading slashes in filenames.
	# ls is used to filter out deleted files that tar may choke on.
	sed -e 's/# Old # //' -e '/# Del #.*/d'	"$logfile" |
	xargs -r ls 2> /dev/null | xargs -r tar -Pcjf "$backupdir/$1.bak.tar.bz2" &&
	echo "Backed up \"$longname\" in $backupdir/$1.bak.tar.bz2" ||
	error "backup_package: could not create backup."

	return 0
}

# Usage: remove_package <package>
# Remove <package> from system
function remove_package {

	(( $# != 1 )) && usage_error "remove_package" "<package>" "$*"

	(( $(id -u) == 0 )) || error "remove_package: only root can do this."

	logdir=$(get_logdir) ||
	error "remove_package: cannot read install-log configuration file."

	longname=$(get_longname "$1" 1)

	logfile="$logdir/$1.log"
	[[ -r "$logfile" ]] ||
	error "remove_package: \"longname\" not installed."

	local reply

	echo
	echo "Do you really want to remove \"$longname\"? [y/n]"
	echo -n "(installed on $(date -r "$logfile" "+%Y-%m-%d %H:%M (%a)")) "
	until [[ "$reply" == y ]]
	do
		read reply
		[[ "$reply" == n ]] && return 1;
	done

	if (( $backup == 1 )); then
		reply=
		echo
		echo "Do you want to create a backup of the current"
		echo -n "\"$longname\" installation? [y/n] "
		until [[ "$reply" == y || "$reply" == n ]]
		do
			read reply
		done
		if [[ "$reply" == y ]]; then
			echo "Creating backup..."
			backup_package "$1"
		fi
	fi

	echo "Removing \"$longname\"..."
	remove_files "$1" &&
	remove_logs "$1" ||
	error "remove_package: could not remove package \"$longname\"."

	return 0
}



(( $# < 1 )) && script_usage_err "Too few arguments."

backup=0
backupdir=

while (( $# > 0 ))
do
	case "$1" in
		("" | \
		-\? | \
		-h  | \
		--help)		usage; exit 0 ;;
		(-b | --backup)	(( $backup == 0 )) && backup=1 || backup=0
				[[ "x$2" != x && -d "$2" ]] &&
				backupdir="$2" &&
				shift ;;
		(-f | --config)	[[ "x$2" != x && -f "$2" ]] ||
				script_usage_err "No config file specified."
				pkgconfig="$2"
				shift ;;
		(*)		source_config
				remove_package "$1" ;;
	esac
	shift
done
