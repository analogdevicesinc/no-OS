#!/bin/bash

set -e

. ./ci/travis/lib.sh

apt_update_install() {
	sudo -s <<-EOF
		apt-get -qq update
		apt-get -y install $@
	EOF
}

build_astyle() {
    . ./ci/travis/astyle.sh
}

build_cppcheck() {
    apt_update_install cppcheck
    [[ ! -f .cppcheckignore ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --suppressions-list=.cppcheckignore"
    cppcheck --quiet --force --error-exitcode=1 $CPPCHECK_OPTIONS .
}

build_ad9361_generic() {
    apt_update_install libmatio-dev
    make -C ./ad9361/sw -f Makefile.generic
}

build_ad9361_linux () {
    apt_update_install libmatio-dev
    make -C ./ad9361/sw -f Makefile.linux
}

build_drivers() {
    apt_update_install gcc-arm-none-eabi libnewlib-arm-none-eabi
    make -C ./drivers -f Makefile
}

build_${BUILD_TYPE}
