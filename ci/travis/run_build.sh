#!/bin/bash

set -e

. ./ci/travis/lib.sh

build_default() {
    . ./ci/travis/build_projects.sh
}

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

build_${BUILD_TYPE:-default}
