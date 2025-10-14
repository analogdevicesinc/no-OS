###############################################################################
# Copyright (c) 2002 - 2020 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
###############################################################################
#
# Makefile for profiler
#
#######################################################################

# PROFILE_ARCH_OBJS - Processor Architecture Dependent files defined here
include ../config.make

AS=mb-as
COMPILER = mb-gcc
ARCHIVER = mb-ar
CP = cp
COMPILER_FLAGS=-O2
EXTRA_COMPILER_FLAGS=
LIB = libxil.a
DUMMYLIB = libxilprofile.a

CC_FLAGS = $(subst -pg, , $(COMPILER_FLAGS))
ECC_FLAGS = $(subst -pg, , $(EXTRA_COMPILER_FLAGS))

RELEASEDIR = ../../../../lib
INCLUDEDIR = ../../../../include
INCLUDES = -I./. -I${INCLUDEDIR}

OBJS = _profile_init.o _profile_clean.o _profile_timer_hw.o profile_hist.o profile_cg.o
DUMMYOBJ = dummy.o
INCLUDEFILES = profile.h mblaze_nt_types.h _profile_timer_hw.h

libs : reallibs dummylibs

reallibs : $(OBJS) $(PROFILE_ARCH_OBJS)
	$(ARCHIVER) -r $(RELEASEDIR)/$(LIB) $(OBJS) $(PROFILE_ARCH_OBJS)

dummylibs : $(DUMMYOBJ)
	$(ARCHIVER) -r $(RELEASEDIR)/$(DUMMYLIB) $(DUMMYOBJ)

%.o:%.c
	$(COMPILER) $(CC_FLAGS) $(ECC_FLAGS) -c $< -o $@ $(INCLUDES)

%.o:%.S
	$(COMPILER) $(CC_FLAGS) $(ECC_FLAGS) -c $< -o $@ $(INCLUDES)

include:
	$(CP) -rf $(INCLUDEFILES) $(INCLUDEDIR)

clean:
	rm -f $(OBJS) $(PROFILE_ARCH_OBJS) $(LIB)
