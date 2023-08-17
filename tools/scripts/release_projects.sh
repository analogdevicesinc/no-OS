#!/bin/bash

# Copyright 2023(c) Analog Devices, Inc.
# 
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#     - Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     - Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#     - Neither the name of Analog Devices, Inc. nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#     - The use of this software may or may not infringe the patent rights
#       of one or more patent holders.  This license does not release you
#       from the requirement that you obtain separate licenses from these
#       patent holders to use this software.
#     - Use of the software either in source or binary form, must be run
#       on or directly connected to an Analog Devices Inc. component.
#    
# THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED.
# 
# IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
# RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# The No-OS Release procedure consists of 3 tasks in the Releases section of Azure Pipelines:
# 1. Delete GitHub Release - deletes the old release in order to create a new one with same release
#                            tag
# 2. Bash Script - this script (release_projects.sh) is passed as input file from the no-OS sources
#                  affilated to the Release.
# 3. Create GitHub Release - generate new release using the same release tag and new sources and
#                            binaries.

# Release Environment Variables
RELEASE_TAG="last_commit"
SRC_ALIAS="noos_projects_binaries"
BINARY_PATH=$(System.DefaultWorkingDirectory)/$SRC_ALIAS
#ARTIFACT_NAME=noos_exports #currently set in Azure Pipelines Variables Group

# List of source directories
SOURCE_DIRECTORIES=(
	"$BINARY_PATH/$(ARTIFACT_NAME)_STM32/"
	"$BINARY_PATH/$(ARTIFACT_NAME)_Maxim/"
	"$BINARY_PATH/$(ARTIFACT_NAME)_Mbed/"
	"$BINARY_PATH/$(ARTIFACT_NAME)_Pico/"
	"$BINARY_PATH/$(ARTIFACT_NAME)_ADuCM3029/"
	"$BINARY_PATH/$(ARTIFACT_NAME)_Xilinx/"
)

# Destination directory
DESTINATION_DIRECTORY="$BINARY_PATH/$(ARTIFACT_NAME)"

#Update Release Tag in preparation for new release
update_release_tag() {
	# delete last_commit tag locally
	git tag -d $RELEASE_TAG
	# delete last_commit tag remotely
	git push origin :refs/tags/$RELEASE_TAG
	# add last_commit tag locally
	git tag -f $RELEASE_TAG HEAD
	# add last_commit tag remotely
	git push -f --tags
}

#Unarchive and merge platform specific binaries under the same path
unzip_d() {
	local zipfile="$1"
	local zipfile_basename=$(basename "$zipfile")
	local zipdir="$DESTINATION_DIRECTORY/${zipfile_basename%.zip}"
	unzip -d "$zipdir" -o "$zipfile"
}

generate_release_files() {
	#Create folder holding all release files
	mkdir -p $DESTINATION_DIRECTORY

	#Get the binary archives from all platform builds and unarchive under same project path
	for source_dir in "${SOURCE_DIRECTORIES[@]}"; do
		find "$source_dir" -type f -name "*.zip" -print0 | while IFS= read -r -d $'\0' zip_file; do
		unzip_d "$zip_file"
		done
	done

 	#Archive projects under same project path
	for prj_folder in "$DESTINATION_DIRECTORY"/*/; do
		prj_name=$(basename "$prj_folder")
		cd $prj_folder && zip -r "$DESTINATION_DIRECTORY/$prj_name.zip" *
	done
}

update_release_tag

generate_release_files
