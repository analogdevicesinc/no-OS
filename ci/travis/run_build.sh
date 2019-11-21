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

build_${BUILD_TYPE:-default}
