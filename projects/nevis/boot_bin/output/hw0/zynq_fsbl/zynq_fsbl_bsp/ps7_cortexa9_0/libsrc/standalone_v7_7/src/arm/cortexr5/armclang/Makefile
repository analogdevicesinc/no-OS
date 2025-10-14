##############################################################################
# Copyright (c) 2020 Xilinx, Inc.  All rights reserved.
#
# SPDX-License-Identifier: MIT
#
##############################################################################
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

RELEASEDIR=../../../lib/
INCLUDEDIR=../../../include/

INCLUDES=-I./. -I${INCLUDEDIR}

INCLUDEFILES=*.h

SRCFILES:=$(wildcard *.c)

ASSEMBLY_SRCFILES:=$(wildcard *.S)

COBJS	=	$(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(wildcard *.c))))
AOBJS1	=	$(RELEASEDIR)/asm_vectors.o
AOBJS2	=	$(RELEASEDIR)/boot.o
AOBJS3	=	$(RELEASEDIR)/xil-crt0.o
AOBJS4	=	$(RELEASEDIR)/cpu_init.o
OBJS    =	$(COBJS) $(AOBJS1) $(AOBJS2) $(AOBJS3) $(AOBJS4)

ASOURCES1 = asm_vectors.S
ASOURCES2 = boot.S
ASOURCES3 = xil-crt0.S
ASOURCES4 = cpu_init.S

OBJECTS = $(OBJS)

libs: standalone_libs

standalone_libs: ${OBJECTS}

DEPFILES := $(SRCFILES:%.c=$(RELEASEDIR)%.d)

include $(wildcard $(DEPFILES))

ASSEMBLY_DEPFILES := $(ASSEMBLY_SRCFILES:%.S=$(RELEASEDIR)%.d)

include $(wildcard $(ASSEMBLY_DEPFILES))

include $(wildcard ../../../../dep.mk)

$(AOBJS1): $(ASOURCES1)
ifeq ($(findstring asm_vectors.S,$(wildcard *.S)),asm_vectors.S)
	${COMPILER} $(INCLUDES) --target=arm-arm-none-eabi -E -o asm_vectors_post.s asm_vectors.S
endif
	${AS} --cpu=Cortex-R5 -o $*.o asm_vectors_post.s --depend=$*.d

$(AOBJS2): $(ASOURCES2)
ifeq ($(findstring boot.S,$(wildcard *.S)),boot.S)
	${COMPILER} $(INCLUDES) --target=arm-arm-none-eabi -E -o boot_post.s boot.S
endif
	${AS} --cpu=Cortex-R5 -o $*.o boot_post.s --depend=$*.d

$(AOBJS3): $(ASOURCES3)
ifeq ($(findstring xil-crt0.S,$(wildcard *.S)),xil-crt0.S)
	${COMPILER} $(INCLUDES) --target=arm-arm-none-eabi -E -o xil-crt0_post.s xil-crt0.S
endif
	${AS} --cpu=Cortex-R5 -o $*.o xil-crt0_post.s --depend=$*.d

$(AOBJS4): $(ASOURCES4)
ifeq ($(findstring cpu_init.S,$(wildcard *.S)),cpu_init.S)
	${COMPILER} $(INCLUDES) --target=arm-arm-none-eabi -E -o cpu_init_post.s cpu_init.S
endif
	${AS} --cpu=Cortex-R5 -o $*.o cpu_init_post.s --depend=$*.d

$(RELEASEDIR)%.o: %.c
	${COMPILER} $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES)  $(DEPENDENCY_FLAGS) -o $@ $<

.PHONY: include
include: $(addprefix $(INCLUDEDIR),$(wildcard *.h)) $(addprefix $(INCLUDEDIR), $(subst includes_ps/,,$(wildcard includes_ps/*.h)))



$(INCLUDEDIR)%.h: includes_ps/%.h
	$(CP) $< $@


$(INCLUDEDIR)%.h: %.h
	$(CP) $< $@

clean:
	rm -rf ${OBJECTS}
	rm -rf $(DEPFILES)
	rm -rf $(ASSEMBLY_DEPFILES)
