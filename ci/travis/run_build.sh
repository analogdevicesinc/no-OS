#!/bin/bash

set -e

. ./ci/travis/lib.sh

build_default() {
    . ./ci/travis/build_projects.sh
    . ./ci/travis/cppcheck.sh
}

build_astyle() {
    . ./ci/travis/astyle.sh
}

build_${BUILD_TYPE:-default}
