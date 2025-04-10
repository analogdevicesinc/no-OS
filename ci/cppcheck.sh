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

build_cppheck() {
	mkdir -p build
	pushd build

	# libtinyxml2-a and libpcre3 dependencis for cppcheck-1.90
	wget http://mirrors.kernel.org/ubuntu/pool/universe/t/tinyxml2/libtinyxml2-6a_7.0.0+dfsg-1build1_amd64.deb
	sudo dpkg -i ./libtinyxml2-6a_7.0.0+dfsg-1build1_amd64.deb

	wget https://launchpad.net/~ubuntu-security-proposed/+archive/ubuntu/ppa/+build/23754764/+files/libpcre3_8.39-9ubuntu0.1_amd64.deb
	sudo dpkg -i ./libpcre3_8.39-9ubuntu0.1_amd64.deb

	wget http://mirrors.kernel.org/ubuntu/pool/universe/c/cppcheck/cppcheck_1.90-4build1_amd64.deb
	sudo dpkg -i ./cppcheck_1.90-4build1_amd64.deb

	popd
}

parse_cppcheck_options() {
	[[ ! -f .cppcheckignore ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --suppressions-list=.cppcheckignore"
	[[ ! -f ./ci/config.cppcheck ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --library=./ci/config.cppcheck"
}

run_cppcheck() {
	cppcheck -j${NUM_JOBS} --quiet --force --error-exitcode=1 $CPPCHECK_OPTIONS .
}

build_cppheck

parse_cppcheck_options

run_cppcheck
