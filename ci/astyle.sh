#!/bin/bash
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
