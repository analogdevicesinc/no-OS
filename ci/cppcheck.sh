#!/bin/bash

set -e

sudo apt-get install cppcheck 

[[ ! -f .cppcheckignore ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --suppressions-list=.cppcheckignore"
[[ ! -f ./ci/config.cppcheck ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --library=./ci/config.cppcheck"
	
cppcheck --quiet --force --error-exitcode=1 $CPPCHECK_OPTIONS .
