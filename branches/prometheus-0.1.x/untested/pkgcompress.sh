#! /bin/bash

# FIXME: Option to convert ZIP archives to tarballs before compression stage.
# Otherwise exclude ZIP files from compression.  Other archive formats?

# Usage:
#
# pkgcompress {[-h|-?|--help] | [{-f|--config} <config file>] <package>
#								[<package> ...]}
# Recompress files in <package>.

# Global prometheus functions.
source "%LIBPKG%" || exit 1

# Usage: usage
# Print help text.
function usage {
	cat <<-EOF
		Usage:
		$0 [<options>] <package> [<package> ...]

		Recompress patches and archives needed to install <package>.
		This is useful to minimise discspace needed to archive source
		code.

		Options:
		{-h|-?|--help}			Show this help screen
		{-f|--config} <config file>	Specify alternative \
configuration file

	EOF
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

		(*)		script_usage_err "unrecognised option: $1"
	esac
	shift
done

# The remaining arguments should all be packages.
(( $# == 0 )) && script_usage_err "no packages to compress"

source_config

# If there is more than one package, compress each in a separate instance.
if (( $# > 1 )); then
	for package in "$@"; do
		"$0" --config "$pkgconfig" "$package" ||
		error "could not compress $package"
	done
	exit
fi

# Only one package.
source_profile "$1"

# (Re)compress every file in current package.
list_files | while read file; do

	# Check $file exists.  location_of will warn if it doesn't.
	file=$(location_of "$file")
	[[ -z "$file" ]] && continue

	# Identify current compression.
	case "$(get_compression "$file")" in
		(*bz*)	file_comp=bzip2 ;;
		(*z|*Z) file_comp=gzip ;;
		(*)	file_comp=none
	esac

	# Maybe we don't need to (re)compress?
	[[ "$compression" == "$file_comp" ]] && continue

	# Uncompress $file if necessary.
	if [[ "$file_comp" != none ]]; then
		echo "Uncompressing $file..."

		case "$file_comp" in
			(gzip)	gunzip "$file";;
			(bzip2)	bunzip2 "$file"
		esac &&

		# Get uncompressed filename, & check it exists.
		uncompfile=$(remove_compression "$file") &&
		uncompfile=$(location_of "$uncompfile") &&
		[[ -n "$uncompfile" ]] ||
		error "uncompression of $file failed"

		file="$uncompfile"
	fi

	# Compress $file if necessary.
	if [[ "$compression" != none ]]; then
		echo "Compressing $file..."
		compress "$file"
	fi
done
