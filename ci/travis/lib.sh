#!/bin/sh -xe

COMMON_SCRIPTS="astyle.sh astyle_config cppcheck.sh"

get_script_path() {
	local script="$1"

	[ -n "$script" ] || return 1

	if [ -f "ci/travis/$script" ] ; then
		echo "ci/travis/$script"
	elif [ -f "build/$script" ] ; then
		echo "build/$script"
	else
		return 1
	fi
}

command_exists() {
	local cmd=$1
	[ -n "$cmd" ] || return 1
	type "$cmd" >/dev/null 2>&1
}

ensure_command_exists() {
	local cmd="$1"
	local package="$2"
	[ -n "$cmd" ] || return 1
	[ -n "$package" ] || package="$cmd"
	! command_exists "$cmd" || return 0
	# go through known package managers
	for pacman in apt-get brew yum ; do
		command_exists $pacman || continue
		$pacman install -y $package || {
			# Try an update if install doesn't work the first time
			$pacman -y update && \
				$pacman install -y $package
		}
		return $?
	done
	return 1
}

ensure_command_exists sudo

echo_red() { printf "\033[1;31m$*\033[m\n"; }
echo_green() { printf "\033[1;32m$*\033[m\n"; }

# Other scripts will download lib.sh [this script] and lib.sh will
# in turn download the other scripts it needs.
# This gives way more flexibility when changing things, as they propagate
for script in $COMMON_SCRIPTS ; do
	[ ! -f "ci/travis/$script" ] || continue
	[ ! -f "build/$script" ] || continue
	mkdir -p build
	ensure_command_exists wget
	wget https://raw.githubusercontent.com/analogdevicesinc/no-OS/master/ci/travis/$script \
		-O build/$script
done
