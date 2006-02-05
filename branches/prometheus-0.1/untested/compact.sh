#! /bin/bash

# Usage:
#
# compact {[-h|-?|--help] | [-f|--force] [-k|--keep] <file>}
# Keep the smallest of <file>, <file>.gz & <file>.bz2.  Echo which file is kept.

# Global prometheus functions.
source "%LIBPKG%" || exit 1

# Usage: usage
# Print help text.
function usage {
	cat <<-EOF
		Usage:
		$0 [<options>] <file>

		Compress <file> with both gzip and bzip2.  Keep the smallest of
		the resulting files, including the original, and echo its name.
		This command is optimized for minimum disk usage.

		Options:
		{-h|-?|--help}		Show this help screen
		{-f|--force}		Force use of compression, even if \
original is smaller
		{-k|--keep}		Do not delete original <file>

	EOF
}

# Usage: largest_of <file> <file>
# Set the global variable $largest to the largest <file>.  If both <files>s are
# the same size, $largest is set to the second.
function largest_of {
	if (( $# != 2 )) || [[ -z "$1" || -z "$2" ]]; then
		usage_error "largest_of" "<file> <file>" "$*"
	fi

	if (( $(wc --bytes < "$1") > $(wc --bytes < "$2") )); then
		largest="$1"
	else
		largest="$2"
	fi
}

# --------------------------- START EXECUTION HERE --------------------------- #

# Check for options.
while [[ "$1" == -* ]]; do
	case "$1" in
		(-h|-\?|--help)	usage
				exit 0;;

		(-f|--force)	force_compression=yes ;;

		(-k|--keep)	keep_file=yes ;;

		(*)		script_usage_err "unrecognised option: $1"
	esac
	shift
done

# There should be one argument, the file to compact.
(( $# != 1 )) &&
if (( $# == 0 )); then
	script_usage_err "no files to compact"
else
	script_usage_err "too many arguments"
fi

file="$1"
file_bz2="$file.bz2"
file_gz="$file.gz"
remainder="$file_gz"

# Check $file is readable.
[[ -f "$file" && -r "$file" ]] || error "cannot compact file: $file"

# bzip2 compression - tell bzip2 to leave the original file.
bzip2 -9k "$file" || error "running bzip2 failed"

if [[ "$force_compression" != yes ]]; then
	largest_of "$file" "$file_bz2"

	# Remove $file_bz2 if it is larger than $file.
	if [[ "$largest" == "$file_bz2" ]]; then
		rm -f "$file_bz2"
		unset file_bz2
	fi
fi

# gzip compression - keep original file if necessary.
if [[ "$force_compression" == yes && "$keep_file" != yes ]]; then
	gzip -9 "$file" &&
	unset file
else
	gzip -9 - < "$file" > "$file_gz"
fi || error "running gzip failed"

# If we have both gzip & bzip2 files, remove the largest.
if [[ -n "$file_bz2" ]]; then
	largest_of "$file_gz" "$file_bz2"

	if [[ "$largest" == "$file_gz" ]]; then
		rm -f "$file_gz"
		remainder="$file_bz2"
	else
		rm -f "$file_bz2"
	fi
fi

# If we still have the original file, compare it with the remaining compressed
# file.
if [[ -n "$file" ]]; then
	largest_of "$file" "$remainder"

	# If compressed file is prefered, remove the original if allowed.
	if [[ "$force_compression" == yes || "$largest" == "$file" ]]; then
		[[ "$keep_file" != yes ]] && rm -f "$file"
	else
		rm -f "$remainder"
		remainder="$file"
	fi
fi

# Report the result.
echo "$remainder"
