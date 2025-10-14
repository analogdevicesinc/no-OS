###############################################################################
# Copyright (c) 2012 - 2020 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
###############################################################################
DRIVER_LIB_VERSION = v1.0

# The following are defined in config.make
# LIBSOURCES - Based on if MicroBlaze support Exceptions
# LIBS - Do Build Profile Libraries
include config.make

AS=$(ASSEMBLER)
CC=$(COMPILER)
AR=$(ARCHIVER)
CP=cp
COMPILER_FLAGS=-O2 -c
EXTRA_COMPILER_FLAGS=
LIB=libxil.a

CC_FLAGS = $(subst -pg, , $(COMPILER_FLAGS))
ECC_FLAGS = $(subst -pg, , $(EXTRA_COMPILER_FLAGS))

RELEASEDIR=../../../lib/
INCLUDEDIR=../../../include/
INCLUDES=-I./. -I${INCLUDEDIR}

SRCFILES:=$(wildcard *.c)

ASSEMBLY_SRCFILES:=$(wildcard *.S)

OBJECTS =	$(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(filter-out $(wildcard microblaze_interrupt_handler.c), $(wildcard *.c)))))

ASSEMBLY_OBJECTS  = $(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(wildcard *.S))))

NON_LTO_OBJECTS = $(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(wildcard microblaze_interrupt_handler.c))))

NON_LTO_FILES = $(wildcard microblaze_interrupt_handler.c)
INCLUDEFILES=*.h

ECC_FLAGS_NO_FLTO1 = $(subst -flto,,$(ECC_FLAGS))
ECC_FLAGS_NO_FLTO = $(subst -ffat-lto-objects,,$(ECC_FLAGS_NO_FLTO1))

libs: $(LIBS)

standalone_libs: $(OBJECTS) $(ASSEMBLY_OBJECTS) $(NON_LTO_OBJECTS)
DEPFILES := $(SRCFILES:%.c=$(RELEASEDIR)%.d)

include $(wildcard $(DEPFILES))

ASSEMBLY_DEPFILES := $(ASSEMBLY_SRCFILES:%.S=$(RELEASEDIR)%.d)

include $(wildcard $(ASSEMBLY_DEPFILES))

include $(wildcard ../../../../dep.mk)

$(NON_LTO_OBJECTS): $(NON_LTO_FILES)
	echo "DEBUG NON_LTO_OBJECTS is $(NON_LTO_OBJECTS)"
	$(CC) $(CC_FLAGS) $(ECC_FLAGS_NO_FLTO) $(INCLUDES)  $(DEPENDENCY_FLAGS) -o $@ $<

$(RELEASEDIR)%.o: %.S
	$(CC) $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES)  $(DEPENDENCY_FLAGS) -o $@ $<

$(RELEASEDIR)%.o: %.c
	$(CC) $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) $(DEPENDENCY_FLAGS) -o $@ $<

profile_libs:
	$(MAKE) -C profile COMPILER_FLAGS="$(COMPILER_FLAGS)" EXTRA_COMPILER_FLAGS="$(EXTRA_COMPILER_FLAGS)" COMPILER="$(CC)" ARCHIVER="$(AR)" AS="$(AS)" libs

include: standalone_includes profile_includes

standalone_includes: $(addprefix $(INCLUDEDIR),$(wildcard *.h))

$(INCLUDEDIR)%.h: %.h
	$(CP) $< $@


profile_includes:
	$(MAKE) -C profile COMPILER_FLAGS="$(COMPILER_FLAGS)" EXTRA_COMPILER_FLAGS="$(EXTRA_COMPILER_FLAGS)" COMPILER="$(CC)" ARCHIVER="$(AR)" AS="$(AS)" include

clean:
	rm -rf ${OBJECTS}
	rm -rf ${ASSEMBLY_OBJECTS}
	rm -rf ${NON_LTO_OBJECTS}
	rm -rf $(DEPFILES)
	rm -rf $(ASSEMBLY_DEPFILES)
	$(MAKE) -C profile COMPILER_FLAGS="$(COMPILER_FLAGS)" EXTRA_COMPILER_FLAGS="$(EXTRA_COMPILER_FLAGS)" COMPILER="$(CC)" ARCHIVER="$(AR)" AS="$(AS)" clean
