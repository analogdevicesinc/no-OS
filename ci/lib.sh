#!/bin/sh -xe
# Copyright 2023(c) Analog Devices, Inc.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of Analog Devices, Inc. nor the names of its
#    contributors may be used to endorse or promote products derived from this
#    software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#List of excluded driver folders for the documentation generation.

COMMON_SCRIPTS="astyle.sh astyle_config cppcheck.sh"
NUM_JOBS=$(nproc)

get_script_path() {
	local script="$1"

	[ -n "$script" ] || return 1

	if [ -f "ci/$script" ] ; then
		echo "ci/$script"
	elif [ -f "build/$script" ] ; then
		echo "build/$script"
	else
		return 1
	fi
}

echo_red() { printf "\033[1;31m$*\033[m\n"; }
echo_green() { printf "\033[1;32m$*\033[m\n"; }

# Other scripts will download lib.sh [this script] and lib.sh will
# in turn download the other scripts it needs.
# This gives way more flexibility when changing things, as they propagate
download_common_scripts() {
	for script in $COMMON_SCRIPTS ; do
		[ ! -f "ci/$script" ] || continue
		[ ! -f "build/$script" ] || continue
		mkdir -p build
		wget https://raw.githubusercontent.com/analogdevicesinc/no-OS/main/ci/$script \
			-O build/$script
	done
}

#################################################################
# Parse commit range - shared implementation for CI scripts
#################################################################
parse_commit_range() {
	local operation_name="${1:-check}"

	if [ -z "$COMMIT_RANGE" ]
	then
		COMMIT_RANGE="${COMMIT_RANGE}"
	fi

	if [ -z "$COMMIT_RANGE" ]  && [ -n "$TARGET_BRANCH" ]
	then
		git fetch --depth=50 origin $TARGET_BRANCH
		git branch $TARGET_BRANCH origin/$TARGET_BRANCH
		COMMIT_RANGE="${TARGET_BRANCH}.."
	fi

	if [ -z "$COMMIT_RANGE" ]
	then
		echo_green "Using only latest commit, since there is no Pull Request"
		COMMIT_RANGE=HEAD~1
	fi

	echo_green "Running $operation_name on commit range '$COMMIT_RANGE'"
	echo_green "Commits should be:"
	if ! git rev-parse $COMMIT_RANGE ; then
		echo_red "Failed to parse commit range '$COMMIT_RANGE'"
		echo_green "Using only latest commit"
		COMMIT_RANGE=HEAD~1
	fi
}
