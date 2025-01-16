################################################################################
# Copyright (C) 2017-2018 Maxim Integrated Products, Inc., All rights Reserved.
# 
# This software is protected by copyright laws of the United States and
# of foreign countries. This material may also be protected by patent laws
# and technology transfer regulations of the United States and of foreign
# countries. This software is furnished under a license agreement and/or a
# nondisclosure agreement and may only be used or reproduced in accordance
# with the terms of those agreements. Dissemination of this information to
# any party or parties not specified in the license agreement and/or
# nondisclosure agreement is expressly prohibited.
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
# OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the name of Maxim Integrated
# Products, Inc. shall not be used except as stated in the Maxim Integrated
# Products, Inc. Branding Policy.
#
# The mere transfer of this software does not imply any licenses
# of trade secrets, proprietary technology, copyrights, patents,
# trademarks, maskwork rights, or any other form of intellectual
# property whatsoever. Maxim Integrated Products, Inc. retains all
# ownership rights.
################################################################################
#
# Authors : Benjamin VINOT <benjamin.vinot@maximintegrated.com>
#           Stephane DI VITO <stephane.divito@maximintegrated.com>

$(info [INFO] In ./maxq10xx_api, target $(MAKECMDGOALS))

#ifeq ($(MASTERMAKEFILE),)
#$(error You must not call this makefile directly. Please use the top folder's Makefile.)
#endif

ifeq (,$(findstring $(MAKECMDGOALS), clean))

ifeq (,$(findstring $(TARGET_SMNGR), maxq106x maxq1065 maxq108x))
    $(error Target Security Manager is not defined correctly!)
endif

ifndef TARGET
TARGET := pc
endif

#DEBUG passed from caller

LIBRARY:= $(TARGET_SMNGR)_api
CFLAGS+= -DAARDVARK=345678 -DCHEETAH=876543 -DFTDI4222=476198876
ARFLAGS= rcs
CFLAGS+= -Wall -Wextra -Wshadow -Wstrict-prototypes 
#-std=gnu99 -pedantic -Werror
CFLAGS+= -fPIC

ifeq ($(TARGET),raspberry)
CFLAGS+= -std=gnu99
endif


#### SPI ####
ifeq ($(SPI),YES)

CFLAGS+= -DUSE_PROTOCOL_SPI

ifeq ($(TARGET_CLIENT),max32xxx)
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk
#CFLAGS+= -I$(abspath ../../maxq10xx_fw/MAXQ10xxFW/include/)
CFLAGS+= -I$(CMSIS_ROOT)/../PeriphDrivers/include/$(TARGET_UC)
CFLAGS+= -I$(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/include
CFLAGS+= -I$(CMSIS_ROOT)/Include
endif #($(TARGET_CLIENT)

ifeq ($(TARGET),pc)

ifndef PC_INTERFACE
$(error No PC_INTERFACE selected)
endif

CFLAGS+= -DPC_INTERFACE=$(PC_INTERFACE)

ifeq ($(PC_INTERFACE),CHEETAH)
$(warning Copy libcheetah.so/cheetah.dll shared library side to executables)

else ifeq ($(PC_INTERFACE),AARDVARK)
$(warning Copy libaardvark.so/aardvark.dll shared library side to executables)

else ifeq ($(PC_INTERFACE),FTDI4222)
#$(warning Copy XXXXX)

else
$(error Invalid PC interface "$(PC_INTERFACE)", use PC_INTERFACE=CHEETAH or AARDVARK or FTDI4222)

endif #$(PC_INTERFACE)

endif #($(TARGET)


#### I2C ####
else ifeq ($(I2C),YES)

CFLAGS+= -DUSE_PROTOCOL_I2C

ifeq ($(TARGET_CLIENT),max32xxx)
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk
#CFLAGS+= -I$(abspath ../../maxq10xx_fw/MAXQ10xxFW/include/)
CFLAGS+= -I$(CMSIS_ROOT)/../PeriphDrivers/include/$(TARGET_UC)
CFLAGS+= -I$(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/include
# CFLAGS+= -I$(CMSIS_ROOT)/Include
endif #($(TARGET_CLIENT)

ifeq ($(TARGET),pc)

ifndef PC_INTERFACE
$(error No PC_INTERFACE selected)
endif

CFLAGS+= -DPC_INTERFACE=$(PC_INTERFACE)

ifeq ($(PC_INTERFACE),AARDVARK)
$(info Using Totalphase Aardvark I2C interface)
$(warning Copy libaardvark.so/aardvark.dll shared library side to executables)

else ifeq ($(PC_INTERFACE),FTDI4222)
$(info Using FTDI4222 I2C interface)
#$(warning Copy XXXXX)

else
$(error Invalid PC interface "$(PC_INTERFACE)", use PC_INTERFACE=AARDVARK or FTDI4222)

endif #$(PC_INTERFACE)
endif #$(TARGET)

#### PIPE ####
else ifeq ($(PIPE),YES)

CFLAGS+= -DUSE_PROTOCOL_PIPE
PC_INTERFACE=PIPE

#### MAILBOX ####
else ifeq ($(MAILBOX), YES)
CFLAGS+= -DUSE_PROTOCOL_MAILBOX
ifeq ($(USS), USS_ST) # Single threaded or multi-threaded USS
CFLAGS+= -DUSS_ST
else ifeq ($(USS), USS_MT)
CFLAGS+= -DUSS_MT
endif
CFLAGS+= -I$(abspath ../../maxq10xx_fw/MAXQ10xxFW/maxq1080_hal/include/)
#CFLAGS+= -I$(abspath ../../maxq10xx_fw/MAXQ10xxFW/include/)

#### ERROR ####
else

$(error No valid protocol selected (SPI or I2C))

endif

#### SECURE CHANNEL SWITCH ####
ifeq ($(NO_SECURE_CHANNEL), YES)
CFLAGS += -DNO_SECURE_CHANNEL
endif

#### COMMUNICATION WITH MAXQ - DISPLAY OPTIONS ####
ifeq ($(DUMP_COMM),YES)

ifeq ($(COLOR),YES)
		CFLAGS+= -DMXQ_DUMP_COMM=2
else
		CFLAGS+= -DMXQ_DUMP_COMM=1
endif

else
	CFLAGS+= -DMXQ_DUMP_COMM=-1
endif


ifeq ($(DUMP_DEBUG),YES)
	CFLAGS+= -DMXQ_DUMP_DEBUG=1
else
	CFLAGS+= -DMXQ_DUMP_DEBUG=-1
endif

ifeq ($(DUMP_ERROR),YES)
	CFLAGS+= -DMXQ_DUMP_ERROR=1
else
	CFLAGS+= -DMXQ_DUMP_ERROR=-1
endif

ifeq ($(TARGET_SMNGR), maxq106x)
	CFLAGS+= -DMAXQ106x
endif

ifeq ($(TARGET_SMNGR), maxq108x)
	CFLAGS+= -DMXQ_FW_UPDATE_CAP -DMAXQ108x -DENABLE_RSA
else ifeq ($(TARGET_SMNGR), maxq1065)
	CFLAGS+= -DMXQ_FW_UPDATE_CAP -DMAXQ1065
endif

include src/target/target_$(TARGET)/target.makefile

ifdef DEBUG
CFLAGS		+=	-Og -g
else
CFLAGS		+=	-O2
endif

LIB_EXT		:= .a

endif #test clean target

# Generic benchmark
#CFLAGS += -DBENCHMARK -DBENCHMARK_CMD

ifdef MAXQ1065_BENCHMARK
CFLAGS += -DBENCHMARK -DMAXQ1065_BENCHMARK
endif

ifdef MAXQ108X_BENCHMARK
CFLAGS += -DBENCHMARK -DMAXQ108X_BENCHMARK
endif

DISTDIR     := bin/$(TARGET)
BUILDDIR	:= obj
LIBDIR      := lib/$(TARGET)
SRCDIR		:= src
TARGETDIR	:= src/target/target_$(TARGET)/
INCDIR      := include

SRCEXT		:= c
DEPEXT      := d
OBJEXT      := o

ifdef VERBOSE
Q :=
else
Q :=	@
endif

INC         := -I$(INCDIR) -I$(INCDIR)/internal
INCDEP      := -I$(INCDIR) -I$(INCDIR)/internal

#CC			:=	$(CROSS_COMPIL)gcc
#AR			:=	$(CROSS_COMPIL)ar
DOXYGEN		:=	doxygen

LIB		:=lib$(LIBRARY)$(LIB_EXT)

FIND_EXEC := /usr/bin/find

SOURCES         := $(shell $(FIND_EXEC) $(SRCDIR)/api -type f -wholename *.$(SRCEXT) )
TARGET_SOURCES  := $(shell $(FIND_EXEC) $(TARGETDIR) -type f -wholename *.$(SRCEXT))

SOURCES += $(TARGET_SOURCES)
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

ifndef DEBUG
DEBUGTYPE:=No
else
DEBUGTYPE:=Yes
endif

$(info )
$(info =======================================)
$(info MAXQ10XX API Configuration...)
$(info    TARGET SECURITY MANAGER   : $(TARGET_SMNGR))
$(info    TARGET                    : $(TARGET))
$(info    DEBUG                     : $(DEBUGTYPE))
$(info    PROTOCOL                  : $(PROTOCOL) over $(PC_INTERFACE))
$(info    Compiler                  : $(CROSS_COMPIL)gcc)
$(info    DUMP_COMMM                : $(DUMP_COMM))
$(info =======================================)
$(info )


#
# Rules
#

.PRECIOUS: $(BUILDDIR)/.%$(OBJEXT)
	
.PHONY: all clean distclean install resources doc
all:	directories $(DISTDIR)/$(LIB)

doc: # No need to rebuild it, it comes pre-generated
	markdown ./README.md > ./README.html
	@echo '[DOXYGEN] $@'
	$(Q)make -C doc all TARGET_SMNGR=$(TARGET_SMNGR)
	@echo "[DOCUMENTATION] SUCCESS"

clean:
	@echo '[RM] $(BUILDDIR)'
	$(Q)rm -rf -- $(BUILDDIR) || true
	@echo '[RM] bin'
	$(Q)rm -rf -- bin || true

distclean: clean

uninstall: clean

directories: 
	$(Q)mkdir -p $(DISTDIR)
	$(Q)mkdir -p $(BUILDDIR)

#
# Dependencies
# 
$(DISTDIR)/$(LIB): $(OBJECTS)
	@echo '[AR] $@'
	$(Q)$(AR) $(ARFLAGS) $(DISTDIR)/$(LIB) $^ 
	@echo '[BUILD] $@ successfully built in $(DISTDIR)'


$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT) ../buildconfig.*
	@echo '[CC] $<'
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	$(Q)$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	$(Q)cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	$(Q)sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	$(Q)sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	$(Q)rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

# End Of Makefile
