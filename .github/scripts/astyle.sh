#!/bin/bash
# Copyright 2023(c) Analog Devices, Inc.
#
# SPDX-License-Identifier: BSD-3-Clause


set -e

ASTYLE_EXT_LIST="${ASTYLE_EXT_LIST} .c .h"
COMMIT_RANGE="$1"
# PR comment body. Left empty when the style check passes, so pr-comment
# deletes a stale comment.
ASTYLE_REPORT="${ASTYLE_REPORT:-${WORKSPACE}/astyle_report.md}"

get_script_path() {
	local script="$1"

	[ -n "$script" ] || return 1

	if [ -f "${WORKSPACE}/.github/scripts/$script" ] ; then
		echo ".github/scripts/$script"
	elif [ -f "${WORKSPACE}/build/$script" ] ; then
		echo "${WORKSPACE}/build/$script"
	else
		return 1
	fi
}

parse_commit_range() {
	local operation_name="${1:-check}"

	if [ -z "$COMMIT_RANGE" ]; then
		COMMIT_RANGE="${COMMIT_RANGE}"
	fi

	if [ -z "$COMMIT_RANGE" ]  && [ -n "$TARGET_BRANCH" ]; then
		git fetch --depth=50 origin $TARGET_BRANCH
		git branch $TARGET_BRANCH origin/$TARGET_BRANCH
		COMMIT_RANGE="${TARGET_BRANCH}.."
	fi

	if [ -z "$COMMIT_RANGE" ]; then
		echo "Using only latest commit, since there is no Pull Request"
		COMMIT_RANGE=HEAD~1
	fi

	echo "Running $operation_name on commit range '$COMMIT_RANGE'"
	echo "Commits should be:"
	if ! git rev-parse $COMMIT_RANGE ; then
		echo "Failed to parse commit range '$COMMIT_RANGE'"
		echo "Using only latest commit"
		COMMIT_RANGE=HEAD~1
	fi
}

is_valid_file() {
	[[ -f ".astyleignore" ]] &&
	while read -r fpath; do
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


build_astyle() {
	if [ ! -d build/astyle/build/gcc ]; then
		mkdir -p build
		pushd build
		wget --no-check-certificate "https://sourceforge.net/projects/astyle/files/astyle/astyle 3.1/astyle_3.1_linux.tar.gz"
		tar -xzf astyle_3.1_linux.tar.gz
		pushd ./astyle/build/gcc
		make -j${NUM_JOBS}
		popd
		popd
	fi
}

write_report() {
	local diff

	diff="$(git diff)"
	{
		echo "### :x: Astyle check failed"
		echo
		echo "These files are not formatted with \`.github/config/astyle_config\`:"
		echo
		git diff --name-only | sed 's/^/- `/; s/$/`/'
		echo
		echo "<details><summary>Suggested changes</summary>"
		echo
		echo '```diff'
		# Keep the comment under the GitHub comment size limit.
		echo "${diff:0:50000}"
		echo '```'
		echo "</details>"
		if [ -n "$COMMIT_SHA" ]; then
			local sha="\`${COMMIT_SHA:0:7}\`"

			[ -n "$COMMIT_URL" ] && sha="[$sha]($COMMIT_URL)"
			echo
			echo "_Posted automatically by the astyle check for commit $sha._"
		fi
	} > "$ASTYLE_REPORT"
}

run_astyle() {
	git diff --name-only --diff-filter=d $COMMIT_RANGE | while read -r file; do
		if is_source_file "$file" && is_valid_file "$file"
		then
			${WORKSPACE}/build/astyle/build/gcc/bin/astyle --options="${WORKSPACE}/.github/config/astyle_config" "$file"
		fi
	done;

	git diff --exit-code || {
		echo "Code style issues found."
		write_report
		exit 1
	}
}

parse_commit_range

build_astyle

: > "$ASTYLE_REPORT"
run_astyle
