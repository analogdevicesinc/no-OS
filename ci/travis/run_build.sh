#!/bin/bash

set -e

build_default() {
    . ./ci/travis/build_projects.sh
    . ./ci/travis/cppcheck.sh
}

build_astyle() {
    . ./ci/travis/astyle.sh
}

build_${BUILD_TYPE:-default}
