#!/bin/bash

set -e

echo "Setup cloudsmith helper ..."
wget https://raw.githubusercontent.com/analogdevicesinc/wiki-scripts/refs/heads/main/utils/cloudsmith_utils/cloudsmith_helper.py \
    -O ${WORKSPACE}/tools/scripts/cloudsmith_temp.py
cat ${WORKSPACE}/tools/scripts/cloudsmith_temp.py | sed 's#/usr/bin/python3#/usr/bin/env python3#' > ${WORKSPACE}/tools/scripts/cloudsmith_helper.py
rm ${WORKSPACE}/tools/scripts/cloudsmith_temp.py
          
echo "Running project build ..."
    ${WORKSPACE}/tools/scripts/build_projects.py \
    ${WORKSPACE} \
    -export_dir ${RELEASES_DIR} \
    -log_dir ${LOGS_DIR} \
    -builds_dir ${BUILDS_DIR} \
    -platform ${PLATFORM} \
    -hdl_branch ${HDL_BRANCH} \
    -projects ${PROJECTS}
