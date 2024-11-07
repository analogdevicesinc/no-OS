#!/bin/bash
# SPDX-License-Identifier: GPL-2.0+
#
# Copyright (C) 2024 Analog Devices, Inc.
# Author: Darius Berghe darius.berghe@analog.com
# 
# Use this script to convert Windows paths to Unix (Git Bash)
# paths from scripts that need to be sourced to set up the
# environment.
# 
# Besides the path conversion it also filters out paths that
# contain "gnuwin" keyword, because these typically contain
# tools that overlap with Git Bash tools but may not work well.
#
# Usage:
#   source git-bash-paths.sh path/to/yourscript.sh
#
script_to_source=$1

# Capture the environment in the main shell
original_env=$(/usr/bin/env)

# Run the script in a subprocess, capturing the modified environment
modified_env=$(bash -c "source $script_to_source; /usr/bin/env")

# Convert and populate environments into associative arrays for easy comparison
declare -A original_env_map modified_env_map

while IFS='=' read -r key value; do
    original_env_map["$key"]="$value"
done <<< "$original_env"

while IFS='=' read -r key value; do
    modified_env_map["$key"]="$value"
done <<< "$modified_env"

# Find the differences
for key in "${!modified_env_map[@]}"; do
    if [[ "${original_env_map[$key]}" != "${modified_env_map[$key]}" ]]; then
        if [[ -z "${original_env_map[$key]}" ]]; then
            echo -n "Added: "
        else
            echo -n "Modified: "
        fi
        filteredpaths="${modified_env_map[$key]}"
        if [[ "$key" == "PATH" ]]; then
                unixpaths=$(echo ${modified_env_map[$key]} | sed -E 's|([A-Z]):\\|/\L\1/|g; s|\\|/|g')
                filteredpaths=$(echo "$unixpaths" | tr ':' '\n' | grep -v "gnuwin" | paste -sd ':' -)
        fi
        echo "$key=$filteredpaths"
        export "$key=$filteredpaths"
    fi
done

