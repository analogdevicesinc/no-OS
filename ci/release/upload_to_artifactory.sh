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
#                         sources affilated to the Release.

# Upload artifacts to Artifactory in conjunction with upload_to_artifactory.py python script.

# Release Environment Variables
WORKING_DIRECTORY="${1:-$(System.DefaultWorkingDirectory)}"
ART_PATH="${2:-$(ARTIFACTORY_PATH)}"
ART_TOKEN="${3:-$(ARTIFACTORY_TOKEN)}"
# Get default working branch
BRANCH="${4:-$(Build.SourceBranchName)}"
BINARY_ALIAS="noos_projects_binaries"
SRC_ALIAS="_no-os_sources"
PYSCRIPT_PATH="ci/release/upload_to_artifactory.py"

# Timestamp Variable
timestamp=$(date +%Y_%m_%d-%H_%M)

# no-OS supported plafroms list
platform_list=("Xilinx" "STM32" "Pico" "Mbed" "Maxim" "ADuCM3029")

cd ${WORKING_DIRECTORY}/${SRC_ALIAS}
# Get latest commit SHA displaying first 7 characters
short_hash=`git rev-parse --short=7 HEAD`
cd ${WORKING_DIRECTORY}/${BINARY_ALIAS}

# Upload artifacts to Artifactory for each platform
for platform in "${platform_list[@]}"; do
	python3 ${WORKING_DIRECTORY}/${SRC_ALIAS}/${PYSCRIPT_PATH} \
	--base_path ${ART_PATH} --server_path no-OS/${BRANCH}/${timestamp}/${platform} \
	--local_path noos_exports_${platform} --token ${ART_TOKEN} --props_level 1 \
	--properties "git_sha=${short_hash}" --log_file upload_to_artifactory_log.txt --no_rel_path
done
