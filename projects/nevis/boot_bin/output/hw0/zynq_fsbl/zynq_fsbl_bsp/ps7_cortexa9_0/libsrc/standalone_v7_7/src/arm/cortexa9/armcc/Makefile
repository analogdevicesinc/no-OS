###############################################################################
# Copyright (c) 2011 - 2020 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
###############################################################################
DRIVER_LIB_VERSION = 1.0

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

ECC_FLAGS	+= --cpu=Cortex-A9 --fpu=VFPv3_FP16 --vectorize

RELEASEDIR=../../../lib/
INCLUDEDIR=../../../include/

INCLUDES=-I./. -I${INCLUDEDIR}

SRCFILES:=$(wildcard *.c)

ASSEMBLY_SRCFILES:=$(wildcard *.S)
INCLUDEFILES=*.h

COBJS	=	$(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(wildcard *.c))))
AOBJS1	=	$(RELEASEDIR)asm_vectors.o
AOBJS2	=	$(RELEASEDIR)translation_table.o
OBJS    = $(COBJS) $(AOBJS1) $(AOBJS2) $(RELEASEDIR)boot.o

ASOURCES1 = asm_vectors.s
ASOURCES2 = translation_table.s

# Replace boot.o with boot_post.o
OBJECTS = $(OBJS)

libs: $(LIBS)
standalone_libs: $(OBJS)

DEPFILES := $(SRCFILES:%.c=$(RELEASEDIR)%.d)

include $(wildcard $(DEPFILES))

ASSEMBLY_DEPFILES := $(ASSEMBLY_SRCFILES:%.S=$(RELEASEDIR)%.d)

include $(wildcard $(ASSEMBLY_DEPFILES))

include $(wildcard ../../../../dep.mk)

$(RELEASEDIR)boot.o: boot.S
ifeq ($(findstring boot.S,$(wildcard *.S)),boot.S)
	${COMPILER} $(INCLUDES) -E -o boot_post.s boot.S
endif

	${AS} --cpu=Cortex-A9 --fpu=VFPv3 -o $*.o  boot_post.s --depend=$*.d

$(AOBJS1): $(ASOURCES1)

	${AS} --cpu=Cortex-A9 --fpu=VFPv3 --depend=$*.d $< -o $@

$(AOBJS2): $(ASOURCES2)

	${AS} --cpu=Cortex-A9 --fpu=VFPv3 --depend=$*.d $< -o $@

$(RELEASEDIR)%.o: %.c

	${COMPILER} -c $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) $(DEPENDENCY_FLAGS) $< -o $@


.PHONY: include
include: $(addprefix $(INCLUDEDIR),$(wildcard *.h))

$(INCLUDEDIR)%.h: %.h
	$(CP) $< $@

clean:
	rm -rf ${OBJECTS}
	rm -rf $(DEPFILES)
	rm -rf $(ASSEMBLY_DEPFILES)
