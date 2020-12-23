#!/bin/bash

############################################################################
# Check if the documentation will be generated w/o warnings or errors
############################################################################
pushd ${TOP_DIR}/doc
(cd build && ! make doc 2>&1 | grep -E "warning:|error:") || {
        echo_red "Documentation incomplete or errors in the generation of it have occured!"
        exit 1
}
popd

echo_green "Documentation was generated successfully!"

############################################################################
# If the current build is not a pull request and it is on master the 
# documentation will be pushed to the gh-pages branch
############################################################################
REPO_SLUG="${REPO_SLUG:-analogdevicesinc/no-OS}"

if [[ "${TRAVIS_PULL_REQUEST}" == "false" && "${TRAVIS_BRANCH}" == "master" ]] ; then
        UPDATE_GH_DOCS=1
fi

if [ "$UPDATE_GH_DOCS" = "1" ]
then
        MASTER_COMMIT=$(git rev-parse --short HEAD)

	echo_green "Running Github docs update on commit '$MASTER_COMMIT'"

        pushd ${TOP_DIR}/doc
        git clone https://github.com/${REPO_SLUG} --depth 1 --branch=gh-pages doc/html &>/dev/null

        pushd doc/html
        rm -rf ./*
        popd

        cp -R build/doxygen_doc/html/* doc/html/

        pushd doc/html
        CURRENT_COMMIT=$(git log -1 --pretty=%B)
        if [[ ${CURRENT_COMMIT:(-7)} != ${MASTER_COMMIT:0:7} ]]
        then
                git add --all .
                git commit --allow-empty --amend -m "Update documentation to ${MASTER_COMMIT:0:7}"
                if [ -n "$GITHUB_DOC_TOKEN" ] ; then
                        git push https://${GITHUB_DOC_TOKEN}@github.com/${REPO_SLUG} gh-pages -f &>/dev/null
                else
                        git push origin gh-pages -f &>/dev/null
                fi

                echo_green "Documetation updated!"
        else
                echo_green "Documentation already up to date!"
        fi
        popd
else
        echo_green "Documentation will be updated when this commit gets on master!"
fi
