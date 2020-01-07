#!/bin/bash

set -e

sudo apt-get update

. ./ci/travis/lib.sh

build_astyle() {
    . ./ci/travis/astyle.sh
}

build_cppcheck() {
    . ./ci/travis/cppcheck.sh
}

build_ad9361_generic() {
    sudo apt-get install libmatio-dev
    make -C ./ad9361/sw -f Makefile.generic
}

build_ad9361_linux() {
    sudo apt-get install libmatio-dev
    make -C ./ad9361/sw -f Makefile.linux
}

build_drivers() {
    sudo apt-get install gcc-arm-none-eabi libnewlib-arm-none-eabi
    make -C ./drivers -f Makefile
}

build_doxygen() {
    sudo apt-get install -y graphviz
    # Install a recent version of doxygen
	DOXYGEN_URL="https://sourceforge.net/projects/doxygen/files/rel-1.8.17/doxygen-1.8.17.src.tar.gz/"
	cd ${DEPS_DIR}
	[ -d "doxygen" ] || {
		mkdir doxygen && wget --quiet -O - ${DOXYGEN_URL} | tar --strip-components=1 -xz -C doxygen
	}

    # Install Doxygen
    cd doxygen
    patch ./src/docparser.cpp < ${TRAVIS_BUILD_DIR}/ci/travis/patch_doxy.patch
    mkdir -p build && cd build
    cmake ..
    make -j${NUM_JOBS}
    sudo make install
    cd ../..

    # Build Documentation
    cd ${TRAVIS_BUILD_DIR}/doc
    mkdir -p build && cd build
    cmake ..
    cd ../..

    ./ci/travis/doxygen.sh
}

build_${BUILD_TYPE}
