#!/bin/bash

# Usage:
#
# pkginfo {[-?|-h|--help] [-v|--verbose] [-f|--config <config file>]}
# pkginfo {[-a|--query-all] | [-i|--query-installed]}
# pkginfo {[[-w|--which] <file>] | [-l|--list] <package>] | package}
# Display information on installed and available software packages.

source "%LIBPKG%" || exit 1

# Usage: usage
# Print help screen
function usage {
	cat <<-EOF
		Usage:
		$(basename $0) [<options> ...] [<package>]

		Display information on installed and available software packages.

		Options:
		-?|-h|--help			Show this help screen
		-v|--verbose			Verbose output
		-f|--config <config file>	Specify alternative configuration file

		-a|--query-all			List all packages
		-i|--query-installed		List all installed packages
		-d|--dependencies <package>	List dependencies of <package>
		-p|--patches <package>		List patches to be applied to <package>
		-l|--list <package>		List all files installed by <package>
		-w|--which <file>		List the package <file> belongs to

	EOF
}


# Usage: query_package <package>
# Gather and display information about <package>
function query_package {

	(( $# != 1 )) && usage_err "query_package" "<package>" "$*"

	local fullname log_date homepage log_version pkg_version
	local size_real size_occupied

	fullname="$1"

	[[ ! -f "$logdir/$1.log" ]] && log_date="not installed"
	if [[ -r "$logdir/$1.log" ]]; then
		log_date="$(date -r "$logdir/$1.log" "+%Y-%m-%d %H:%M (%a)")"
		if (( $verbose == 1 )); then
			local i
			size_real=0
			size_occupied=0

			for i in $(sed -e 's/# Old # //' -e '/# Del #.*/d' \
						"$logdir/$1.log" 2> /dev/null)
			do
				[[ -e "$i" ]] &&
				size_real=$(( $size_real + \
				$(ls -l "$i" 2> /dev/null | \
				  awk -F\  '{print $5}') ))

				[[ -e "$i" ]] &&
				size_occupied=$(( $size_occupied + \
				$(du -b "$i" 2> /dev/null | \
				  awk -F\  '{print $1}') ))
			done
			size_real=$(( $size_real / 1024 ))
			size_occupied=$(( $size_occupied / 1024 ))
		fi
	fi
	

	[[ -f "$logdir/.vtab" ]] &&
	if [[ "$(grep "^$1	" "$logdir/.vtab" | sed -e 's/	//')"x != x ]]
	then
		[[ ! -f "$logdir/$1.log" ]] && log_date="orphaned entry"
		log_version="$(sed -n "s/^$1	//p" "$logdir/.vtab")"
	fi

	if [[ -r "$profiles/$1.pkg" ]]; then
		fullname="$(get_conf_var name "$profiles/$1.pkg")"
		pkg_version="$(get_conf_var version "$profiles/$1.pkg")"
		homepage="$(get_conf_var homepage "$profiles/$1.pkg")"
	fi

	if (( $verbose == 0 )); then
		printf "%-32.32s " "$1"

		if [[ -n "$log_version" ]]; then
			printf "%-24.24s" "$log_version"
		else
			if [[ -f "$logdir/$1.log" ]]; then
				printf "%-24.24s" "n/a${pkg_version:+ ($pkg_version)}"
			else
				printf "%-24.24s" "$pkg_version"
			fi
		fi
		printf " %-22.22s\n" "$log_date"

	else
		echo
		echo "Package : $fullname"
		echo "Version : ${log_version:-n/a} / ${pkg_version:-n/a}"
		[[ -n "$homepage" ]] && echo "Homepage: $homepage"
		echo "Last installed: $log_date"
		if [[ -n "$size_real" ]]; then
			echo "Installed size: $size_real kb / $size_occupied kb"
		else
			echo "Installed size: n/a"
		fi
		echo
	fi

	return 0
}


# Usage: query_installed
# Run query_package on every installed package
function query_installed {

	# All installed packages should have a corresponding log file in
	# $logdir, so we check those first.

	local pkg
	
	ls -rt "$logdir"/*.log 2> /dev/null | sed -e 's/.*\///' -e 's/\.log//' | \
	while read pkg; do
		query_package "$pkg"
	done

	pkg=

	# It is theoretically possible that a package has an entry in .vtab,
	# but no corresponding log file.  This part finds those packages.
	sed -e 's/	.*//' "$logdir/.vtab" 2> /dev/null | \
	while read pkg; do
		[[ ! -f "$logdir/$pkg.log" ]] &&
		query_package "$pkg"
	done
}


# Usage: query_all
# Run query_package on all packages that have `log_install=yes'
# in their .pkg profile
function query_all {

	# All packages that have a the log_install variable set in their
	# .pkg file can be installed and actually occupy space on the
	# filesystem.  List only those.

	grep -L "^[	 ]*log_install=no" "$profiles"/*.pkg 2> /dev/null | \
	sed -e '/REFERENCE/d' -e 's/.*\///' -e 's/\.pkg//' | \
	while read pkg; do
		query_package "$pkg"
	done
}


# Usage: print_deps <package>
# List dependencies of a package.
function print_deps {
	[[ -n "$1" ]] ||
		usage_error "print_deps" "<package>" "$*"

	local dep

	for dep in $(get_dependencies "$1")
	do
		(( $verbose == 1 )) &&
			echo "$(get_longname "$dep" 0)" ||
			echo "$dep"
	done
}


# Usage: print_patches <package>
# List patches in a package's profile. (Not for stage1/2)
function print_patches {
	[[ -n "$1" ]] ||
		usage_error "print_patches" "<package>" "$*"

	(
		source_profile "$1"

		for (( i=0; i < ${#patches[@]}; i++))
		do
			echo "$(basename $(remove_compression "${patches[$i]}"))"

			if (( "$verbose" == 1 )); then
				local patch
				patch=$(location_of "${patches[$i]}") &&
		                [[ -n "$patch" ]] &&
        		        cat_file "$patch" | \
					sed '/^+++/,$d; /^---/,$d; /^diff/,$d'
				#echo
			fi
		done
	)
}


# Usage: print_files <package>
# List all files installed by <package>.
function print_files {
	[[ -n "$1" ]] ||
		usage_error "print_files" "<package>" "$*"

	[[ -f "$logdir/$1.log" ]] ||
		error "$1 is not installed."

	sed -e 's/# Old # //' -e '/# Del #.*/d' "$logdir/$1.log"
}


# Usage: find_owner <file>
# Find the package that <file> belongs to.
function find_owner {
	[[ -n "$1" ]] ||
		usage_error "find_owner" "<file>" "$*"
	local f="$1"
	
	if [[ "$(echo "$f" | cut -c 1)" != "/" ]]; then
		# $f is not an absolute pathname. Prepend PWD and
		# strip out any bla/../'s and ./'s
		f="$(echo $(pwd)/$f | sed -e 's,[^/]*/\.\./,,g' -e 's,\./,,g')"
	fi

	[[ -e "$f" ]] ||
		error "\"$f\" does not exist."

	egrep -l "(^# Old # |^)$f$" "$logdir"/*.log 2> /dev/null | \
	sed -e 's@.*/\(.*\)\.log$@\1@' | while read f
	do
		(( $verbose == 1 )) &&
			echo "$(get_longname "$f" 1)" ||
			echo "$f"
	done
}


(( $# < 1 )) && script_usage_err "Too few arguments."

verbose=0

logdir=$(get_logdir) ||
error "$(basename $0): cannot read install-log configuration file."

source_config

while [[ "$1" = -* ]]
do
	case "$1" in
		(-\?|-h|--help)		usage; exit 0 ;;

		(-v | --verbose)
				[[ $verbose == 0 ]] && verbose=1 || verbose=0 ;;

		(-a | --query-all)
				query_all
				exit ;;

		(-i | --query-installed)
				query_installed
				exit ;;

		(-d | --dependencies)
				[[ -n "$2" ]] ||
				script_usage_error "No package specified."
				print_deps "$2"
				exit ;;

		(-p | --patches)
				[[ -n "$2" ]] ||
				script_usage_error "No package specified."
				print_patches "$2"
				exit ;;

		(-l | --list)
				[[ -n "$2" ]] ||
				script_usage_err "No package specified."
				print_files "$2"
				exit ;;

		(-w | --which)
				[[ -n "$2" ]] ||
				script_usage_err "Missing argument."
				find_owner "$2"
				exit ;;

		(-f | --config)
				[[ "x$2" != x && -f "$2" ]] ||
				script_usage_err "No config file specified."
				pkgconfig="$2"
				source_config
				shift ;;

		(*)		script_usage_err "Unrecognized option \"$1\"." ;;
	esac
	shift
done

# The remaining arguments should all be packages.

while (( $# != 0 )); do
	query_package "$1"
	shift
done

