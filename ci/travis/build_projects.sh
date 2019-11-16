#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "linux" ] 
then 
	echo "Building AD9361-Linux"
	make -C ./ad9361/sw -f Makefile.linux
	echo "Building Drivers"
	make -C ./drivers -f Makefile
fi

echo "Building AD9361-Generic"
make -C ./ad9361/sw -f Makefile.generic

