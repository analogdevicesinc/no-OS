#!/bin/bash

set -e

echo_red() { printf "\033[1;31m$*\033[m\n"; }

COMMIT_RANGE="$1"
if [ -z "$COMMIT_RANGE"]
then 
	COMMIT_RANGE=$TRAVIS_COMMIT_RANGE
	if [ -z "$TRAVIS_PULL_REQUEST_SHA" ]
	then
		echo "Using only latest commit, since there is no Pull Request"
		COMMIT_RANGE=HEAD~1
	fi
fi

FILES=$(git diff --name-only $COMMIT_RANGE)

is_source_file() {
	local file="$1"
	local ext_list=".c .h"

	for ext in $ext_list; do
		[[ "${file: -2}" == "$ext" ]] && return 0
	done;

	return 1
}

if [ ! -d build/astyle/build/gcc ]
then	
	mkdir -p build
	pushd build
	wget "https://sourceforge.net/projects/astyle/files/astyle/astyle 3.1/astyle_3.1_linux.tar.gz"
	tar -xzf astyle_3.1_linux.tar.gz
	pushd ./astyle/build/gcc
	make
	popd
	popd
fi

export PATH="build/astyle/build/gcc;$PATH"

for file in $FILES; do
	if is_source_file $file
	then 
		astyle --options=./ci/scripts/astyle_config $file
	fi
done;

git diff --exit-code || {
	echo_red "Code style issues found." 
	exit 1
}
