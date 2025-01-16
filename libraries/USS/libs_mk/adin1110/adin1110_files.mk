################################################################################
 # Copyright (C) 2016 Maxim Integrated Products, Inc. (now owned by Analog
 # Devices, Inc.), 2024 Analog Devices, Inc., All Rights Reserved. This software
 # is proprietary and confidential to Analog Devices, Inc. and its licensors.
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
 # $Date: 2018-08-31 14:08:14 -0500 (Fri, 31 Aug 2018) $
 # $Revision: 37586 $
 #
 ###############################################################################

# This is the name of the build output file

ifeq "$(TARGET)" ""
$(error TARGET must be specified)
endif

TARGET_UC:=$(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)
ifeq "$(COMPILER)" ""
$(error COMPILER must be specified)
endif

# This is the path to the CMSIS root directory
ifeq "$(CMSIS_ROOT)" ""
CMSIS_ROOT=../CMSIS
endif
ifeq "$(LIBS_DIR)" ""
LIBS_DIR = $(CMSIS_ROOT)/..
endif

PERIPH_DRIVER := $(LIBS_DIR)/PeriphDrivers

ADIN1110_INCLUDE_DIR += $(ADIN1110_DIR)/inc
ADIN1110_INCLUDE_DIR += $(ADIN1110_DIR)/common
ADIN1110_INCLUDE_DIR += $(ADIN1110_DIR)/common/hal
ADIN1110_INCLUDE_DIR += $(ADIN1110_DIR)/common/hal/ports/maxim
ADIN1110_INCLUDE_DIR += $(PERIPH_DRIVER)/Include/$(TARGET_UC)

ADIN1110_C_FILES += $(ADIN1110_DIR)/src/adi_mac.c
ADIN1110_C_FILES += $(ADIN1110_DIR)/src/adi_phy.c
ifeq ($(SPI_OA_SUPPORT), 1)
ADIN1110_C_FILES += $(ADIN1110_DIR)/src/adi_spi_oa.c
else
ADIN1110_C_FILES += $(ADIN1110_DIR)/src/adi_spi_generic.c
endif
ADIN1110_C_FILES += $(ADIN1110_DIR)/src/adin1110.c

ADIN1110_C_FILES += $(ADIN1110_DIR)/common/hal/fcs/fcs.c

ifeq "$(TARGET_UC)" "MAX32690"
ADIN1110_C_FILES += $(ADIN1110_DIR)/common/hal/ports/maxim/hal.c
endif

ifeq "$(TARGET_UC)" "MAX32650"
ADIN1110_C_FILES += $(ADIN1110_DIR)/common/hal/ports/maxim/hal.c
endif

# Where to find header files for this project
ADIN1110_H_FILES +=  $(shell find $(ADIN1110_INCLUDE_DIR) -wholename '*.h')
