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

# Use these to specify the project.
ifeq "$(LIBRARY_VARIANT)" ""
override PROJECT=$(LIBRARY_NAME)
else
override PROJECT=$(LIBRARY_NAME)_$(LIBRARY_VARIANT)
endif

ifeq "$(COMPILER)" ""
COMPILER=GCC
$(info COMPILER should be set on upper layer Makefile, set compiler to GCC if not set)
endif

ifeq "$(BUILD_DIR)" ""
BUILD_DIR=./Build
endif

ifeq ($(LWIP_FREERTOS_SUPPORT), 1)
FREERTOS_DRIVER := $(LIBS_DIR)/FreeRTOS
endif

include $(MBEDTLS_MK_DIR)/mbedtls_files.mk

# # Where to find header files for this project
IPATH += $(MBEDTLS_LIB_DIR)/include
IPATH += $(MBEDTLS_LIB_DIR)/library
IPATH += $(MBEDTLS_LIB_DIR)/configs
IPATH += $(MBEDTLS_LIB_DIR)/tests/include
IPATH += $(MAXQ10XX_SDK_LIBS_DIR)
IPATH += $(PROJ_DIR)/include
IPATH += $(PROJ_DIR)/include/mbedtls

ifeq ($(LWIP_FREERTOS_SUPPORT), 1)
IPATH += $(FREERTOS_DRIVER)/Source/include
IPATH += $(FREERTOS_DRIVER)/Source/portable/GCC/ARM_CM4F
endif

#IPATH += ${MAXIM_PATH}/Libraries/lwIP/include
#IPATH += ${MAXIM_PATH}/Libraries/lwIP/include/Maxim
#IPATH += ${MAXIM_PATH}/Libraries/FreeRTOS/Source/include
#IPATH += ${MAXIM_PATH}/Libraries/FreeRTOS/Source/portable/GCC/ARM_CM4F
SRCS  += $(MBEDTLS_C_FILES)
VPATH += $(dir $(SRCS))

# Open compile guard flag
#PROJ_CFLAGS+=-D__MAXQ10XX__

# Use absolute paths if building within eclipse environment.
ifeq "$(ECLIPSE)" "1"
SRCS := $(abspath $(SRCS))
endif

# Only building libraries.
MAKECMDGOALS=lib

# Include the rules for building for this target
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk
