#!/bin/bash
# Copyright 2023(c) Analog Devices, Inc.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#     - Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     - Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     - Neither the name of Analog Devices, Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#     - The use of this software may or may not infringe the patent rights
#       of one or more patent holders.  This license does not release you
#       from the requirement that you obtain separate licenses from these
#       patent holders to use this software.
#     - Use of the software either in source or binary form, must be run
#       on or directly connected to an Analog Devices Inc. component.
#
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED.
#
# IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
# RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set -e

. ./ci/lib.sh

ASTYLE_EXT_LIST="${ASTYLE_EXT_LIST} .c .h"

COMMIT_RANGE="$1"

is_valid_file() {
	[[ -f ".astyleignore" ]] &&
	while read -r fpath
	do
		[[ -z "$fpath" ]] && continue
		[[ "$file" == *"$fpath"* ]] && return 1
	done < ".astyleignore"
	return 0
}

is_source_file() {
	local file="$1"

	for ext in $ASTYLE_EXT_LIST; do
		[[ "${file: -2}" == "$ext" || "${file: -3}" == "$ext" ]] && return 0
	done;

	return 1
}

parse_commit_range() {
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

	echo_green "Running astyle on commit range '$COMMIT_RANGE'"
	echo_green "Commits should be:"
	if ! git rev-parse $COMMIT_RANGE ; then
		echo_red "Failed to parse commit range '$COMMIT_RANGE'"
		echo_green "Using only latest commit"
		COMMIT_RANGE=HEAD~1
	fi
}

build_astyle() {
	if [ ! -d build/astyle/build/gcc ]
	then
		mkdir -p build
		pushd build
		wget --no-check-certificate "https://sourceforge.net/projects/astyle/files/astyle/astyle 3.1/astyle_3.1_linux.tar.gz"
		tar -xzf astyle_3.1_linux.tar.gz
		pushd ./astyle/build/gcc
		make -j${NUM_JOBS}
		popd
		popd
	fi
}

run_astyle() {
	git diff --name-only --diff-filter=d $COMMIT_RANGE | while read -r file; do
		if is_source_file "$file" && is_valid_file "$file"
		then
			./build/astyle/build/gcc/bin/astyle --options="$(get_script_path astyle_config)" "$file"
		fi
	done;

	git diff --exit-code || {
		echo_red "Code style issues found."
		exit 1
	}
}

parse_commit_range

build_astyle

run_astyle
