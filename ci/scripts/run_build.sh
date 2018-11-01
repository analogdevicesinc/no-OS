#!/bin/bash

set -e

build_default() {
    . ./ci/scripts/build_projects.sh
    . ./ci/scripts/cppcheck.sh
}

build_astyle() {
    . ./ci/scripts/astyle.sh
}

build_${BUILD_TYPE:-default}
