###############################################################################
# Copyright (c) 2011 - 2020 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
###############################################################################
DRIVER_LIB_VERSION = v1.0

include config.make

AS=$(ASSEMBLER)
COMPILER=$(COMPILER)
ARCHIVER=$(ARCHIVER)
CP=cp
COMPILER_FLAGS=
EXTRA_COMPILER_FLAGS=

LIB=libxil.a

CC_FLAGS = $(COMPILER_FLAGS)
ECC_FLAGS = $(EXTRA_COMPILER_FLAGS)

ECC_FLAGS	+= --fpu=VFPv3

RELEASEDIR=../../../lib/
INCLUDEDIR=../../../include/

INCLUDES=-I./. -I${INCLUDEDIR}
INCLUDEFILES=*.h

SRCFILES:=$(wildcard *.c)

ASSEMBLY_SRCFILES:=$(wildcard *.S)

OBJECTS =	$(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(wildcard *.c))))
OBJECTS +=	$(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(wildcard *.s))))

libs: $(LIBS)
standalone_libs: ${OBJECTS}

DEPFILES := $(SRCFILES:%.c=$(RELEASEDIR)%.d)

include $(wildcard $(DEPFILES))

include $(wildcard ../../../../dep.mk)

$(RELEASEDIR)%.o: %.s
	${AS} --cpu Cortex-A9 --fpu VFPv3 $(INCLUDES) -o $@ $<

$(RELEASEDIR)%.o: %.c
	${COMPILER} $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) $(DEPENDENCY_FLAGS) -o $@ $<


.PHONY: include
include:  $(addprefix $(INCLUDEDIR),$(wildcard *.h))
$(INCLUDEDIR)%.h: %.h
	$(CP) $< $@

clean:
	rm -rf ${OBJECTS}
	rm -rf $(DEPFILES)
