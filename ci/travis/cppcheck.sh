#!/bin/bash

set -e

cppcheck --quiet --force --error-exitcode=1 $CPPCHECK_OPTIONS .
