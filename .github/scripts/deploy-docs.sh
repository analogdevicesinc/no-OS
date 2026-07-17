#!/bin/bash

DEPS_DIR=${TOP_DIR}/deps

echo "Building and deploying docs for no-os ..."

echo "Installing dependencies ..."

sudo apt-get install -y graphviz python3-pip
pip3 install pip --upgrade
pip3 install -r ${TOP_DIR}/doc/sphinx/source/requirements.txt

DOXYGEN_URL="https://sourceforge.net/projects/doxygen/files/rel-${VERSION}/doxygen-${VERSION}.src.tar.gz/"
DOXYGEN_THEME_URL="https://github.com/analogdevicesinc/doctools/releases/download/latest/adi-harmonic-doxygen-theme.tar.gz"

mkdir -p ${DEPS_DIR}/doxygen ${DEPS_DIR}/doxygen-theme
cd ${DEPS_DIR}

# Download doxygen tools
wget ${DOXYGEN_URL} >/dev/null
tar --strip-components=1 -xvf doxygen-${VERSION}.src.tar.gz -C doxygen

# Download doxygen theme
wget ${DOXYGEN_THEME_URL} >/dev/null
tar --strip-components=1 -xvf adi-harmonic-doxygen-theme.tar.gz -C doxygen

# Install doxygen
cd doxygen
patch ./src/docparser.cpp < ${TOP_DIR}/.github/config/patch_doxy.patch
mkdir -p build && cd build
cmake ..
make -j${NUM_JOBS}
sudo make install
cd ../..

# Generate *.dox files for drivers and projects
. ${TOP_DIR}/.github/scripts/gen_dox.sh

# Build the documentation
echo "Building documentation ..."
cd ${TOP_DIR}/doc/doxygen
mkdir -p build && cd build
cmake ..
cd ../../..

# Run build and upload documentation script
echo "Uploading the documentation to gh-pages ..."
. ${TOP_DIR}/.github/scripts/documentation.sh

echo "Finished!"
