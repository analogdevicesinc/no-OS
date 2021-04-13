#!/bin/bash

set -e

. ./ci/lib.sh

ASTYLE_EXT_LIST="${ASTYLE_EXT_LIST} .c .h"

COMMIT_RANGE="$1"

if [ -z "$COMMIT_RANGE" ]
then
	COMMIT_RANGE="${COMMIT_RANGE}"
fi

if [ -z "$COMMIT_RANGE" ]  && [ -n "$TARGET_BRANCH" ]
then
	COMMIT_RANGE="${TARGET_BRANCH}.."
fi

if [ -z "$COMMIT_RANGE" ]
then
	echo_green "Using only latest commit, since there is no Pull Request"
	COMMIT_RANGE=HEAD~1
fi

echo_green "Running astyle on commit range '$COMMIT_RANGE'"
echo_green "Commits should be:"
if ! git rev-parse $COMMIT_RANGE ; then
	echo_red "Failed to parse commit range '$COMMIT_RANGE'"
	echo_green "Using only latest commit"
	COMMIT_RANGE=HEAD~1
fi

is_valid_file(){
	[[ -f ".astyleignore" ]] &&
	while read -r fpath
	do
		[[ -z "$fpath" ]] && continue
		[[ "$file" == *"$fpath"* ]] && return 1
	done < ".astyleignore"
	return 0
}

is_source_file() {
	local file="$1"

	for ext in $ASTYLE_EXT_LIST; do
		[[ "${file: -2}" == "$ext" || "${file: -3}" == "$ext" ]] && return 0
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
	make -j3
	popd
	popd
fi

git diff --name-only --diff-filter=d $COMMIT_RANGE | while read -r file; do
	if is_source_file "$file" && is_valid_file "$file"
	then 
		./build/astyle/build/gcc/bin/astyle --options="$(get_script_path astyle_config)" "$file"
	fi
done;

git diff --exit-code || {
	echo_red "Code style issues found." 
	exit 1
}
