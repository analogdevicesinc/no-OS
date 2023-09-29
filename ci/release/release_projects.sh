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

# The No-OS Release procedure consists of 4 tasks in the Releases section of Azure Pipelines:
# 1. Delete GitHub Release - deletes the old release in order to create a new one with same release
#                            tag.
# 2. Prepare Release - the release_projects.sh script is passed as input file from the no-OS
#                      sources affilated to the Release.
# 3. Create GitHub Release - generate new release using the same release tag and new sources and
# 4. Upload Artifactory - the upload_to_artifactory.sh script is passed as input file from the no-OS
#                         sources affilated to the Release. Uploads artifacts to Artifactory in
#                         conjunction with upload_to_artifactory.py python script.

# This is a script for the intermediate stage in the remote release pipeline following the new CI
# infrastructure for building the projects. The script gets all platform specific binaries and merge
# them into project specific archives that are subsequent published as github releases.

# Release Environment Variables
RELEASE_TAG="last_commit"
BINARY_ALIAS="noos_projects_binaries"
WORKING_DIRECTORY="${1:-$(System.DefaultWorkingDirectory)}"
BINARY_PATH=$WORKING_DIRECTORY/$BINARY_ALIAS
ARTIFACT="${2:-$(ARTIFACT_NAME)}" #currently set in Azure Pipelines Variables Group

# List of source directories
SOURCE_DIRECTORIES=(
	"$BINARY_PATH/${ARTIFACT}_STM32/"
	"$BINARY_PATH/${ARTIFACT}_Maxim/"
	"$BINARY_PATH/${ARTIFACT}_Mbed/"
	"$BINARY_PATH/${ARTIFACT}_Pico/"
	"$BINARY_PATH/${ARTIFACT}_ADuCM3029/"
	"$BINARY_PATH/${ARTIFACT}_Xilinx/"
)

# Destination directory
DESTINATION_DIRECTORY="$BINARY_PATH/${ARTIFACT}"

#Update Release Tag in preparation for new release
update_release_tag() {
	if [[ $(git show-ref --tags | grep "$RELEASE_TAG") ]]; then
		# delete last_commit tag locally
		git tag -d $RELEASE_TAG
		# delete last_commit tag remotely
		git push origin :refs/tags/$RELEASE_TAG
	fi
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
