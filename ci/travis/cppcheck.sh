#!/bin/bash

set -e

[[ ! -f .cppcheckignore ]] || CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} --suppressions-list=.cppcheckignore"
	
cppcheck --quiet --force --error-exitcode=1 $CPPCHECK_OPTIONS .
