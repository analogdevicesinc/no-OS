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

ifeq "$(EMBEDDED_GCOV_DIR)" ""
$(error EMBEDDED_GCOV_DIR must be specified")
endif

# This is the name of the build output file
PROJECT_NAME=libembedded_gcov

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
PROJECT=$(PROJECT_NAME)
else
PROJECT=$(PROJECT_NAME)_$(PROJECT_VARIANT)
endif

ifeq "$(TARGET)" ""
TARGET=MAX32690
$(info TARGET should be set on upper layer Makefile, set target to MAX32520 if not set)
endif

TARGET_UC:=$(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)

ifeq "$(COMPILER)" ""
COMPILER=GCC
$(info COMPILER should be set on upper layer Makefile, set compiler to GCC if not set)
endif

ifeq "$(BUILD_DIR)" ""
BUILD_DIR=./Build
endif

include $(EMBEDDED_GCOV_MK_DIR)/embedded_gcov_files.mk

# # Where to find header files for this project
IPATH += $(EMBEDDED_GCOV_INCLUDE_DIR)
SRCS  += $(EMBEDDED_GCOV_C_FILES)
VPATH += $(dir $(SRCS))

# Open compile guard flag
#PROJ_CFLAGS += -DSPI_OA_EN
# Use absolute paths if building within eclipse environment.
ifeq "$(ECLIPSE)" "1"
SRCS := $(abspath $(SRCS))
endif

# Only building libraries.
MAKECMDGOALS=lib

# Include the rules for building for this target
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk
