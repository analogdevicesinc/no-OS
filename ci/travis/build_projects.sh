#!/bin/bash

set -e

if [ "$TRAVIS_OS_NAME" == "linux" ] 
then 
	make -C ./ad9361/sw -f Makefile.linux
fi

make -C ./ad9361/sw -f Makefile.generic
