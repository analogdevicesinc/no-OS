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

############################################################################
# Check if the documentation will be generated w/o warnings or errors
############################################################################
build_doc(){
        pushd ${TOP_DIR}/doc/doxygen
        (cd build && ! make doc 2>&1 | grep -E "warning:|error:") || {
                echo_red "Documentation incomplete or errors in the generation of it have occured!"
                exit 1
        }
        popd

        echo_green "Documentation was generated successfully!"
}

build_sphinx() {
        pushd ${TOP_DIR}/doc/sphinx

        sphinx-build ./source/ ./build

        popd
}

############################################################################
# If the current build is not a pull request and it is on master the 
# documentation will be pushed to the gh-pages branch
############################################################################
update_gh_pages() {
        REPO_SLUG="${REPO_SLUG:-analogdevicesinc/no-OS}"

        if [[ -z "${SYSTEM_PULLREQUEST_PULLREQUESTNUMBER}" && "${BUILD_SOURCEBRANCH}" == *"master"* ]] ; then
                UPDATE_GH_DOCS=1
        fi

        if [ "$UPDATE_GH_DOCS" = "1" ]
        then
                git config --global user.email "cse-ci-notifications@analog.com"
                git config --global user.name "CSE-CI"

                MASTER_COMMIT=$(git rev-parse --short HEAD)

                echo_green "Running Github docs update on commit '$MASTER_COMMIT'"

                git fetch --depth 1 origin +refs/heads/gh-pages:gh-pages

                rm -rf ${DEPS_DIR}

                git checkout gh-pages

                # Clear previous content in the root folder except the doc path which holds new builds
                find ${TOP_DIR} -mindepth 1 -maxdepth 1 ! \( -name "doc" -o -name ".git" \) -exec rm -r {} \;

                # Create doxygen folder holding new build content
                mkdir -p ${TOP_DIR}/doxygen
                cp -R ${TOP_DIR}/doc/doxygen/build/doxygen_doc/html/* ${TOP_DIR}/doxygen/

                # Add sphinx build content to root folder
                cp -R ${TOP_DIR}/doc/sphinx/build/* ${TOP_DIR}

                rm -rf ${TOP_DIR}/doc

                # Create .nojekyll file
                touch ${TOP_DIR}/.nojekyll

                CURRENT_COMMIT=$(git log -1 --pretty=%B)
                if [[ ${CURRENT_COMMIT:(-7)} != ${MASTER_COMMIT:0:7} ]]
                then
                        git add --all .
                        git commit --allow-empty --amend -m "Update documentation to ${MASTER_COMMIT:0:7}"
                        if [ -n "$GITHUB_DOC_TOKEN" ] ; then
                                git push https://${GITHUB_DOC_TOKEN}@github.com/${REPO_SLUG} gh-pages -f
                        else
                                git push origin gh-pages -f
                        fi

                        echo_green "Documetation updated!"
                else
                        echo_green "Documentation already up to date!"
                fi
        else
                echo_green "Documentation will be updated when this commit gets on master!"
        fi
}

build_doc

build_sphinx

update_gh_pages
