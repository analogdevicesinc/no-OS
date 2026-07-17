#!/bin/bash

echo "Installing tools for ${PLATFORM} platform..."

if [[ "$PLATFORM" == "pico" ]]; then
    echo "/home/runner/py-env/bin" >> $GITHUB_PATH
    echo "VIRTUAL_ENV=/home/runner/py-env" >> $GITHUB_PATH
    echo "PICO_SDK_PATH=${WORKSPACE}/libraries/pico-sdk" >> $GITHUB_ENV
elif [[ "$PLATFORM" == "maxim" ]]; then
    echo "/home/runner/py-env/bin" >> $GITHUB_PATH
    echo "VIRTUAL_ENV=/home/runner/py-env" >> $GITHUB_PATH
    echo "CMAKE_GENERATOR=Ninja" >> $GITHUB_ENV
elif [[ "$PLATFORM" == "xilinx" ]]; then
    cloudsmith download ${TOOLS_REPO} hardware.tar.gz --tag latest --outfile ${BUILDS_DIR}_${HDL_BRANCH}/hardware.tar.gz
    tar -xzvf ${BUILDS_DIR}_${HDL_BRANCH}/hardware.tar.gz -C ${BUILDS_DIR}_${HDL_BRANCH} --strip-components 2
fi
