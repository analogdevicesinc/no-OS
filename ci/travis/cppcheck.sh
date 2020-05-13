#!/bin/bash

set -e

sudo apt-get install cppcheck 

[[ ! -f .cppcheckignore ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --suppressions-list=.cppcheckignore"
	
cppcheck -ilibraries/mbedtls --quiet --force --error-exitcode=1 $CPPCHECK_OPTIONS .
