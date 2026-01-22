################################################################################
 # Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
 #
 # Permission is hereby granted, free of charge, to any person obtaining a
 # copy of this software and associated documentation files (the "Software"),
 # to deal in the Software without restriction, including without limitation
 # the rights to use, copy, modify, merge, publish, distribute, sublicense,
 # and/or sell copies of the Software, and to permit persons to whom the
 # Software is furnished to do so, subject to the following conditions:
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
 #
 ###############################################################################

$(info [INFO] In ./maxq10xx_api, target $(MAKECMDGOALS))

ifeq ($(MASTERMAKEFILE),)
$(error You must not call this makefile directly. Please use the top folder's Makefile.)
endif

# Specify the project variant.
ifeq "$(MFLOAT_ABI)" "hardfp"
PROJECT_VARIANT=hardfp
else
ifeq "$(MFLOAT_ABI)" "hard"
PROJECT_VARIANT=hardfp
else
PROJECT_VARIANT=softfp
endif
endif

# Use these to specify the project.
ifeq "$(PROJECT_VARIANT)" ""
override PROJECT:=$(LIB_API_NAME)
else
override PROJECT:=$(LIB_API_NAME)_$(PROJECT_VARIANT)
endif

ifeq "$(TARGET)" ""
TARGET=MAX32520
$(info TARGET should be set on upper layer Makefile, set target to MAX32520 if not set)
endif

TARGET_UC:=$(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)
$(info $(TARGET_UC))

ifeq "$(COMPILER)" ""
COMPILER=GCC
$(info COMPILER should be set on upper layer Makefile, set compiler to GCC if not set)
endif

ifeq "$(BUILD_DIR)" ""
BUILD_DIR=./Build
endif

$(info )
$(info =======================================)
$(info MAXQ10XX API Configuration...)
$(info    TARGET SECURITY MANAGER   : $(TARGET_SMNGR))
$(info    TARGET                    : $(TARGET_CLIENT))
$(info    DEBUG                     : $(DEBUGTYPE))
$(info    PROTOCOL                  : $(PROTOCOL))
$(info    Compiler                  : $(CROSS_COMPIL)gcc)
$(info    DUMP_COMMM                : $(DUMP_COMM))
$(info =======================================)
$(info )

include ${MAXQ10XX_API_MK_DIR}/maxq10xx_api_files.mk

# # Where to find header files for this project
IPATH += $(MAXQ10XX_API_LIBS_INCLUDE_DIR)
SRCS  += $(MAXQ10XX_API_LIBS_C_FILES)
VPATH += $(dir $(SRCS))

ifeq ($(NO_SECURE_CHANNEL), YES)
PROJ_CFLAGS += -DNO_SECURE_CHANNEL
endif

ifeq ($(USS), USS_ST) # Single threaded or multi-threaded USS
PROJ_CFLAGS+= -DUSS_ST
else ifeq ($(USS), USS_MT)
PROJ_CFLAGS+= -DUSS_MT
endif

ifeq ($(TARGET_SMNGR), maxq108x)
PROJ_CFLAGS+= -DMXQ_FW_UPDATE_CAP -DMAXQ108x -DENABLE_RSA
else ifeq ($(TARGET_SMNGR), maxq1065)
PROJ_CFLAGS+= -DMXQ_FW_UPDATE_CAP -DMAXQ1065
endif

ifeq ($(TARGET_LC), max32657)
PROJ_CFLAGS +=-mthumb -mcpu=cortex-m33 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ggdb
else
PROJ_CFLAGS +=-mthumb -mcpu=cortex-m4 -mfloat-abi=softfp -mfpu=fpv4-sp-d16 -ggdb
endif

PROJ_CFLAGS+= -DAARDVARK=345678 -DCHEETAH=876543 -DFTDI4222=476198876
ARFLAGS= rcs
PROJ_CFLAGS+= -Wall -Wextra -Wshadow
#-std=gnu99 -pedantic -Werror
PROJ_CFLAGS+= -fPIC

ifeq ($(DEBUG_MAXQ10XX_API), YES)
PROJ_CFLAGS+= -DMXQ_DUMP_DEBUG=1
PROJ_CFLAGS+= -DMXQ_DUMP_ERROR=1
PROJ_CFLAGS+= -DMXQ_DUMP_COMM=2
else
PROJ_CFLAGS+= -DMXQ_DUMP_COMM=-1
PROJ_CFLAGS+= -DMXQ_DUMP_DEBUG=-1
endif

# Use absolute paths if building within eclipse environment.
ifeq "$(ECLIPSE)" "1"
SRCS := $(abspath $(SRCS))
endif

# Only building libraries.
MAKECMDGOALS=lib

# Include the rules for building for this target
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk
