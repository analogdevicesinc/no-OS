###############################################################################
# Copyright (c) 2013 - 2021 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
###############################################################################
DRIVER_LIB_VERSION = v1.0

COMPILER=
ARCHIVER=
CP=cp
COMPILER_FLAGS =
LIB=libxilffs.a

ifeq ($(notdir $(COMPILER)) , iccarm)
	EXTRA_ARCHIVE_FLAGS=--create
else
ifeq ($(notdir $(COMPILER)) , armcc)
	EXTRA_ARCHIVE_FLAGS=--create
else
ifeq ($(notdir $(COMPILER)) , armclang)
	EXTRA_ARCHIVE_FLAGS=-rc
else
	EXTRA_ARCHIVE_FLAGS=rc
endif
endif
endif

RELEASEDIR=../../../lib/
INCLUDEDIR=../../../include/
INCLUDES=-I./. -I${INCLUDEDIR}

FATFS_DIR = ./
FATFS_SRCS := $(wildcard *.c)
FATFS_OBJS = $(addprefix $(FATFS_DIR), $(FATFS_SRCS:%.c=%.o))
libs: libxilffs.a

libxilffs.a: $(FATFS_OBJS)
	$(ARCHIVER) $(EXTRA_ARCHIVE_FLAGS) ${RELEASEDIR}/${LIB} ${FATFS_OBJS}
DEPFILES := $(FATFS_SRCS:%.c=$(FATFS_DIR)%.d)

include $(wildcard $(DEPFILES))

include $(wildcard ../../../../dep.mk)

$(FATFS_DIR)%.o: $(FATFS_DIR)%.c
	$(COMPILER) $(COMPILER_FLAGS) $(EXTRA_COMPILER_FLAGS) $(INCLUDES) $(DEPENDENCY_FLAGS) -o $@ $<

.PHONY: include
include: libxilffs_includes

libxilffs_includes: $(addprefix $(INCLUDEDIR), $(subst include/,,$(wildcard include/*.h)))

$(INCLUDEDIR)%.h: include/%.h
	$(CP) $< $@

clean:
	rm -rf $(FATFS_OBJS)
	rm -rf ${RELEASEDIR}/${LIB}
	rm -rf ${DEPFILES}
