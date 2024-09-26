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
#List of excluded driver folders for the documentation generation.

set -e

sudo apt-get update

TOP_DIR="$(pwd)"
DEPS_DIR="${TOP_DIR}/deps"

. ./ci/lib.sh

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
ensure_command_exists wget

build_astyle() {
    . ./ci/astyle.sh
}

build_cppcheck() {
    . ./ci/cppcheck.sh
}

build_drivers() {
    sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
    make -j${NUM_JOBS} -C ./drivers -f Makefile
}

build_documentation() {
    sudo apt-get install -y graphviz python3-pip
    # Install a recent version of pip and the requirements
    pip3 install pip --upgrade
    pip3 install -r ${TOP_DIR}/doc/sphinx/source/requirements.txt
    # Install a recent version of doxygen
	DOXYGEN_URL="https://sourceforge.net/projects/doxygen/files/rel-1.8.17/doxygen-1.8.17.src.tar.gz/"
	mkdir -p "${DEPS_DIR}"
	cd ${DEPS_DIR}
	[ -d "doxygen" ] || {
		mkdir doxygen && wget --quiet -O - ${DOXYGEN_URL} | tar --strip-components=1 -xz -C doxygen
	}

    # Install Doxygen
    cd doxygen
    patch ./src/docparser.cpp < ${TOP_DIR}/ci/patch_doxy.patch
    mkdir -p build && cd build
    cmake ..
    make -j${NUM_JOBS}
    sudo make install
    cd ../..

    #Generate *.dox files for drivers and projects
    . ${TOP_DIR}/ci/gen_dox.sh

    # Build Documentation
    cd ${TOP_DIR}/doc/doxygen
    mkdir -p build && cd build
    cmake ..
    cd ../../..

    # Run build and upload documentation script
    . ./ci/documentation.sh
}

build_${BUILD_TYPE}
