#! /bin/bash
# FIXME: cross compile, build logs(?)

# Usage:
#
# pkgadd {[-h|-?|--help] | [{-f|--config} <config file>] [-b|--build-only]
#				[-l|--leave-files] <package> [<package> ...]}
# Build & install <package>s in commandline order.
#
# pkginstall {[-h|-?|--help] | [{-f|--config} <config file>] <package>
#								[<package> ...]}
# Install <package>.

# Global prometheus functions.
source "%LIBPKG%" || exit 1

# Usage: usage
# Print help text.
function usage {

	# Common usage.
	cat <<-EOF
		Usage:
		$(basename $0) [<options>] <package> [<package> ...]

	EOF

	# Description of pkgadd.
	[[ "$command" == "add" ]] &&
	cat <<-EOF
		Build and install <package>s in the order they are given on the
		commandline.  <package> can be a package name, (e.g. \`bash'),
		or a package profile, (e.g. \`../bash.pkg').

	EOF

	# Description of pkginstall.
	[[ "$command" == "install" ]] &&
	cat <<-EOF
		Install <package>s in the order they are given on the
		commandline.  <package> can be a package name, (e.g. \`bash'),
		or a package profile, (e.g. \`../bash.pkg').  Packages must have
		already been built, e.g. with \`pkgadd --build-only bash'.

		Normally, to install a package you should use pkgadd, as
		pkginstall is mainly for use by other prometheus commands.
		pkgadd will state when pkginstall should be called manually.

	EOF

	# Common options.
	cat <<-EOF
		Options:
		-h|-?|--help			Show this help screen
		-f|--config <config file>	Specify alternative configuration file
	EOF

	# pkgadd options.
	[[ "$command" == "add" ]] &&
	cat <<-EOF
		-b|--build-only			Do not install <package> after building it
		-l|--leave-files		Do not delete unneeded files after building

		Note that if --build-only is used, pkginstall will complete the
		installation.  --build-only implies --leave-files to facilitate
		this.
	EOF

	echo
}

# Usage: override_var <var1> <var2>
# Replace the value of <var1> with that of <var2> if it is set.
function override_var {
	if (( $# != 2 )) || [[ -z "$1" || -z "$2" ]]; then
		usage_error "override_var" "<var1> <var2>" "$*"
	fi

	is_set "$2" && eval "$1=${!2}"
}

# Usage: override_array <array1> <array2>
# Replace the value of each element of <array1> with an element of <array2> if
# it is set.
function override_array {
	if (( $# != 2 )) || [[ -z "$1" || -z "$2" ]]; then
		usage_error "override_array" "<array1> <array2>" "$*"
	fi

	local -i i size
	local elem1 elem2

	# Get size of <array2>.
	eval "size=\${#$2[@]}"

	is_set "$2" &&

	# Remove *all* elements of <array1>, otherwise some could be left
	# unchanged.
	if is_set "$1"; then
		unset "$1"
	fi &&

	# Ensure <array1> is created, even if it is empty.
	eval "$1=" &&

	# Copy each element of <array2> to <array1>.
	for (( i=0 ; i<size ; i++ )); do
		elem1="$1[$i]"
		elem2="$2[$i]"
		eval "$elem1=${!elem2}"
	done
}

# Usage: override_func <func1> <func2>
# Replace <func1> with <func2> if it is defined.
function override_func {
	if (( $# != 2 )) || [[ -z "$1" || -z "$2" ]]; then
		usage_error "override_func" "<func1> <func2>" "$*"
	fi

	is_func "$2" && eval "function $1 { $2; }"
}

# Usage: stage_overrides <stage>
# Override the functions & variables in a package profile with their
# counterparts from stage <stage>.
function stage_overrides {
	if (( $# != 1 )) || [[ -z "$1" ]]; then
		usage_error "stage_overrides" "<stage>" "$*"
	fi

	local i

	# Arrays.
	for i in archives patches archive_dirs patch_cmds; do
		override_array "$i" "stage$1_$i"
	done

	# Variables.
	for i in log_install install_as_root directory leavefiles \
		 optimize cc cflags cxxflags ldflags linguas lc_all; do
		override_var "$i" "stage$1_$i"
	done

	# Functions.
	for i in build_pkg install_pkg; do
		override_func "$i" "stage$1_$i"
	done
}

# Usage: pkgdir
# Work out package directory of current package.
function pkgdir {
	local archive="${archives[0]##*/}"

	# Only continue if necessary.
	[[ -z "$archive" ]] && return

	# Use value from profile if set.
	if [[ -z "$directory" ]]; then

		# Else try to work it out using first archive name.
		local suffix=$(get_compression "$archive")
		directory="${archive%$suffix}"
	fi

	# Make $directory absolute.  ${archive_dirs[0]} overrides $builddir if
	# set to a non-null value.
	directory="${archive_dirs[0]:-$builddir}/$directory"

	# Mark $directory for deletion when installation complete.
	temp_file "$directory"
}

# Usage: prepare_build_env
# Set & export the required build environment.
function prepare_build_env {

	# Only export CC if a value is given.
	unset CC
	[[ -n "$cc" ]] && export CC="$cc"

	# Optimization.  Only export specified values, even if they are null.
	unset CFLAGS CXXFLAGS LDFLAGS
	if [[ "$optimize" == yes ]]; then
		is_set cflags && export CFLAGS="$cflags"
		is_set cxxflags && export CXXFLAGS="$cxxflags"
		is_set ldflags && export LDFLAGS="$ldflags"
	fi

	# NLS.  Only export specified values, even if they are null.
	unset LINGUAS LC_ALL
	is_set linguas && export LINGUAS="$linguas"
	is_set lc_all && export LC_ALL="$lc_all"

	# LFS build directory may be different from the host's.
	is_set lfs_builddir && export builddir="$lfs_builddir"
}

# Usage: open_package {<package>|<profile>}
# Source the package profile, apply any stage overrides & complete any missing
# settings.  Then export required build flags to the environment.
function open_package {
	if (( $# != 1 )) || [[ -z "$1" ]]; then
		usage_error "open_package" "{<package>|<profile>}" "$*"
	fi

	source_profile "$1"

	unset log_target
	[[ x$log_install == xyes ]] &&
		case $log_build in
			(no)	 log_target="3>&1 > /dev/null 2>&1" ;;
			(file)	 log_target="3>&1 > $builddir/$package.log 2>&1" ;;
			(screen) log_target="3>&1" ;;
		esac

	# Override profile if necessary.
	[[ -n "$stage" ]] && stage_overrides "$stage"

	# Fill in any info not in profile.
	prepare_build_env
	pkgdir

	[[ -z "$name" ]] && name="$package"
}

# Usage: check_dependencies <package>
# Check for any missing dependencies and try to install the needed package(s).
function check_dependencies {
	[[ -n "$1" ]] || usage_error "check_dependencies" "<package>" "$@"

	# If the profile is not in $profiles, do not resolve deps and return.
	[[ -r "$profiles/$1.pkg" ]] || return

	local dependencies="$(get_dependencies "$1")"
	local dep
	local needed

	[[ -z "$dependencies" ]] && return

	for dep in $dependencies; do
		if pkginfo "$dep" | grep "not installed" > /dev/null; then
			needed="$needed $dep"
		fi
	done

	[[ -z "$needed" ]] && return

	echo -e "\nUnfulfilled dependencies for $longname:"

	for dep in $needed; do
		echo "  $dep" #$(get_longname "$dep" 0)"
	done

	local reply
	echo -en "\nInstall these packages now? [y/n] "
	until [[ "$reply" == y || "$reply" == n ]]; do
		read reply
	done
	[[ "$reply" == n ]] && error "Unfulfilled dependencies. Aborting."

	for dep in $needed; do
		pkgadd "$dep" || error "Could not install \
				$(get_longname $1 0) (needed by $longname)"
	done
}

# Usage: unpack_archives
# Unpack archives for $package in appropriate directories.
function unpack_archives {
	local archive dir
	local -i i
	for (( i=0 ; i<${#archives[@]} ; i++ )); do

		# Substitute $directory for `@d' & stop if $archive & those
		# following should not be unpacked.
		dir="${archive_dirs[$i]//@d/$directory}"
		[[ "$dir" == ":" ]] && break

		# Set default directory.
		[[ -z "$dir" ]] && dir="$builddir"

		# Locate & unpack $archive.
		archive=$(location_of "${archives[$i]}") &&
		[[ -n "$archive" ]] &&
		pushd "$dir" > /dev/null &&
		unpack "$archive" &&
		popd > /dev/null || error "cannot unpack ${archives[$i]} in $dir"
	done
}

# Usage: apply_patches
# Apply patches with appropriate commands.
function apply_patches {
	local patch cmd
	local -i i
	for (( i=0 ; i<${#patches[@]} ; i++ )); do

		# Substitute $directory for `@d' & stop if $patch & those
		# following should not be applied.
		cmd="${patch_cmds[$i]//@d/$directory}"
		[[ "$cmd" == ":" ]] && break

		# Set default command.
		[[ -z "$cmd" ]] && cmd="patch -Np1"

		# Locate & apply $patch.
		patch=$(location_of "${patches[$i]}") &&
		[[ -n "$patch" ]] &&
		pushd "$directory" > /dev/null &&
		cat_file "$patch" | eval "$cmd $log_target" &&
		popd > /dev/null || error "cannot apply ${patches[$i]}"
	done
}

# Usage: build_package
# Build $package.
function build_package {

	# Prepare for install-log.
	if [[ "$log_install" == yes ]]; then
		local logdir=$(get_logdir)
		[[ -f "$logdir"/.timestamp ]] ||
		touch "$logdir"/.timestamp
	fi

	# Call build_pkg if it is defined in $profile.
	if is_func build_pkg; then
		echo "Building \"$longname\"..."
		pushd "$directory" > /dev/null &&
		eval "build_pkg $log_target" &&
		popd > /dev/null ||
		error "building \"$longname\" failed"
	fi
}

# Usage: remove_installed
# Check if $package is already installed & allow the user to abort, or uninstall
# the existing package, which may be backed-up.
function remove_installed {

	# Is $package already installed?
	if pkginfo "$package" | grep -v "not installed" > /dev/null; then
		local reply

		# Give user a chance to abort.
		echo "\"$name\" is already installed."
		echo -n "Abort installation? [y/n] "
		until [[ "$reply" == y || "$reply" == n ]]; do
			read reply
		done
		echo

		[[ "$reply" == y ]] && return 1

		local long_name=$(get_longname "$package" 1)

		reply=

		# Shall we cleanly remove the previous installation?
		echo "Do you want to delete \"$long_name\""
		echo -n "prior to installation? [y/n] "
		until [[ "$reply" == y || "$reply" == n ]]; do
			read reply
		done
		echo

		if [[ "$reply" == y ]]; then
			reply=

			# Should we backup the previous installation?
			echo "Do you want to create a backup of the current"
			echo -n "$long_name installation? [y/n] "
			until [[ "$reply" == y || "$reply" == n ]]; do
				read reply
			done
			echo

			# Remove the package.
			if [[ "$reply" == y ]]; then
				yes | pkgrm --backup "$package"
			else
				yes | pkgrm "$package"
			fi
		fi
	fi
}

# Usage: install_package
# Install $package.
function install_package {

	# Call install_pkg if it is defined in $profile.
	if [[ "$buildonly" != yes ]] && is_func install_pkg; then

		# Do we need to become root?
		if [[ "$install_as_root" == yes ]] && (( $(id -u) != 0 )); then
			echo "You must be root to install \"$longname\"."
			su -c "pkginstall --config '$pkgconfig' '$profile'"

		else
			# Remove any previous installation (or allow the user
			# to abort).
			if [[ -z "$stage" ]]; then
				remove_installed || return
			fi

			echo "Installing \"$longname\"..."

			# Install <package>.
			pushd "$directory" > /dev/null &&
			eval "install_pkg $log_target" &&
			popd > /dev/null &&

			# Log install if necessary.
			if [[ "$log_install" == yes ]]; then
				install-log --force "$package.log" &&
				{
					local logdir="$(get_logdir)"
					[[ ! -f "$logdir"/.vtab ]] &&
						touch "$logdir"/.vtab

					grep -v "^$package	" \
						"$logdir"/.vtab > "$logdir"/.vtab.$$

					echo "$package	$version" >> \
							"$logdir"/.vtab.$$

					mv "$logdir"/.vtab{.$$,}
				}
			fi ||

			error "installation of \"$longname\" failed"

			# For resuming LFS builds.
			[[ -n "$stage" ]] &&
			touch "$builddir/.stage${stage}_$package"

			# Install $profile to $profiles if necessary.
			if [[ "$profiles/${profile##*/}" != "$profile" ]]; then
				cp "$profile" "$profiles" ||
				warning "could not install package profile $profile in $profiles"
			fi
		fi
	fi
}

# Usage: add_package
# Build & install $package.
function add_package {

	# If we're not building LFS, check for missing dependencies.
	[[ -z "$stage" ]] && check_dependencies "$package"

	echo 
	cat <<-EOF

		----- $longname -----

	EOF

	# For resuming an LFS build.
	if [[ -f "$builddir/.stage${stage}_$package" ]]; then
		cat <<-EOF
			"$longname" has already been built for stage $stage.
			Skipping build...

		EOF
		return
	fi

	unpack_archives &&
	apply_patches &&
	build_package &&
	install_package

	# Deal with unneeded files.
	local file
	if [[ "$leavefiles" == yes ]]; then

		# Warn about unneeded files & list them.
		warning "the following (probably unneeded) files/directories have been left:"

		for file in "${temp_files[@]}"; do
			echo "$file" >&2
		done
	else
		remove_temp_files
	fi

	# Advise to run pkginstall if necessary.
	if is_func install_pkg && [[ "$buildonly" == yes ]]; then
		cat <<-EOF
			You should run pkginstall to complete installation.

		EOF
	fi
}

# --------------------------- START EXECUTION HERE --------------------------- #

# Paranoia.
unset temp_files

umask 022
set +h

# Make sure we are either pkgadd or pkginstall.
command="${0##*pkg}"
[[ "$command" != "add" && "$command" != "install" ]] &&
script_usage_err "must be run as pkgadd or pkginstall"

# Check for options.
while [[ "$1" == -* ]]; do
	case "$1" in
		(-h|-\?|--help)	usage
				exit 0;;

		(-b|--build-only) [[ "$command" == "install" ]] &&
				script_usage_err "unrecognised option: $1"

				buildonly=yes
				leavefiles=yes ;;

		(-l|--leave-files) [[ "$command" == "install" ]] &&
				script_usage_err "unrecognised option: $1"

				leavefiles=yes ;;

		(-f|--config)	[[ -z "$2" ]] &&
				script_usage_err "$1: no config file specified"

				pkgconfig="$2"
				shift;;

		(*)		script_usage_err "unrecognised option: $1"
	esac
	shift
done

# The remaining arguments should all be packages.
(( $# == 0 )) && script_usage_err "no packages to install"

source_config

# If there is more than one package, install each in its own instance.
if (( $# > 1 )); then
	for package in "$@"; do
		buildonly="$buildonly" leavefiles="$leavefiles" \
		"$0" --config "$pkgconfig" "$package" ||
		error "could not install $package"
	done
	exit
fi

# There is only one package to install.  Do so in this instance.
open_package "$1"
longname=$(get_longname "$package" 0)

# What to do?
case "$command" in
	(add)		add_package;;
	(install)	install_package
esac
