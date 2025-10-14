###############################################################################
# Copyright (c) 2014 - 2020 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
###############################################################################

COMPILER=
ARCHIVER=
CP=cp
COMPILER_FLAGS=
LIB=librsa.a

RELEASEDIR=../../../lib
INCLUDEDIR=../../../include
INCLUDES=-I./include/ -I${INCLUDEDIR} 
LIBRSA_DIR = .

EXPORT_INCLUDE_DIR = $(LIBRSA_DIR)/include
			
libs:
	cp $(LIBRSA_DIR)/librsa.a $(RELEASEDIR)
	
.PHONY: include
include: xilrsa_includes

xilrsa_includes: 
	cp -r ${EXPORT_INCLUDE_DIR}/xilrsa.h ${INCLUDEDIR}

clean:
	rm -rf $(RELEASEDIR)/$(LIB)
