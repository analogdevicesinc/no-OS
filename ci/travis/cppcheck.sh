#!/bin/bash

set -e

cppcheck --quiet --force --error-exitcode=1 .
