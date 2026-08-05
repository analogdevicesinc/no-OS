#!/bin/bash
# Copyright 2023(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause

set -e

build_cppheck() {
	mkdir -p build
	pushd build

	# libtinyxml2-a and libpcre3 dependencies for cppcheck-1.90
	wget http://archive.ubuntu.com/ubuntu/pool/universe/t/tinyxml2/libtinyxml2-6a_7.0.0+dfsg-1build1_amd64.deb
	sudo dpkg -i ./libtinyxml2-6a_7.0.0+dfsg-1build1_amd64.deb

	wget http://archive.ubuntu.com/ubuntu/pool/main/p/pcre3/libpcre3_8.39-9ubuntu0.1_amd64.deb
	sudo dpkg -i ./libpcre3_8.39-9ubuntu0.1_amd64.deb

	wget http://archive.ubuntu.com/ubuntu/pool/universe/c/cppcheck/cppcheck_1.90-4build1_amd64.deb
	sudo dpkg -i ./cppcheck_1.90-4build1_amd64.deb

	popd
}

parse_cppcheck_options() {
	[[ ! -f .cppcheckignore ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --suppressions-list=.cppcheckignore"
	[[ ! -f ./.github/config/config.cppcheck ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --library=./.github/config/config.cppcheck"
}

run_cppcheck() {
	cppcheck -j${NUM_JOBS} --quiet --force --error-exitcode=1 $CPPCHECK_OPTIONS .
}

build_cppheck

parse_cppcheck_options

run_cppcheck
