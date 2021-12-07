################################################################################
 # Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
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
 # $Date: 2018-08-09 18:45:02 -0500 (Thu, 09 Aug 2018) $ 
 # $Revision: 36818 $
 #
 ###############################################################################

ifndef MAXIM_LIBRARIES
$(error MAXIM_LIBRARIES not defined.$(ENDL))
endif

# This is the name of the build output file
ifeq "$(PROJECT)" ""
PROJECT=max32660
endif

# Specify the target processor
TARGET=MAX32660
TARGET_REV=0x4131

# Create Target name variables
TARGET_UC:=$(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)

PLATFORM_RELATIVE_PATH = $(patsubst $(MAXIM_LIBRARIES)%,Maxim%,$1)
PLATFORM_FULL_PATH = $(patsubst Maxim%,$(MAXIM_LIBRARIES)%,$1)
CREATED_DIRECTORIES += Maxim

ifneq "$(HEAP_SIZE)" ""
CFLAGS+=-D__HEAP_SIZE=$(HEAP_SIZE)
endif
ifneq "$(STACK_SIZE)" ""
CFLAGS+=-D_STACK_SIZE=$(STACK_SIZE)
endif

# Select 'GCC' or 'IAR' compiler
COMPILER=GCC

CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as

LDFLAGS = -mcpu=cortex-m4 	\
	-Wl,--gc-sections 	\
	--specs=nosys.specs	\
	-mfloat-abi=hard 	\
	-mfpu=fpv4-sp-d16 	\
	--entry=Reset_Handler		
	

CFLAGS=-mthumb                                                                 \
        -mcpu=cortex-m4                                                         \
        -mfloat-abi=hard                                                        \
        -mfpu=fpv4-sp-d16                                                       \
        -Wa,-mimplicit-it=thumb                                                 \
        -fsingle-precision-constant                                             \
        -ffunction-sections                                                     \
        -fdata-sections                                                         \
        -MD                                                                     \
        -Wall                                                                   \
        -Wdouble-promotion                                                      \
        -Wno-format                                                      \
	-g									\
	-c	

CFLAGS += -I$(MAXIM_LIBRARIES)/CMSIS/Include	\
	-I$(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Include	\
	-I$(MAXIM_LIBRARIES)/Boards/EvKit_V1/Include		

CFLAGS += -DTARGET_REV=$(TARGET_REV) \
	-DTARGET=$(TARGET)		\
	-DMAXIM_PLATFORM

$(info $(MAXIM_LIBRARIES))
$(info $(CFLAGS))

LSCRIPT = $(MAXIM_LIBRARIES)/CMSIS/Device/Maxim/MAX32660/Source/GCC/max32660.ld


#EXTRA_LIBS += 

# Specify the board used
ifeq "$(BOARD)" ""
BOARD=EvKit_V1
endif

# This is the path to the CMSIS root directory
ifeq "$(MAXIM_PATH)" ""
LIBS_DIR=/home/xvr/Documents/Maxim/sf_shared2/Firmware/MAX32660/Libraries
#LIBS_DIR=../../../Libraries
else
LIBS_DIR=/$(subst \,/,$(subst :,,$(MAXIM_PATH))/Firmware/$(TARGET_UC)/Libraries)
endif
CMSIS_ROOT=$(LIBS_DIR)/CMSIS

PROJ_CFLAGS+=-DMXC_ASSERT_ENABLE 

MXC_OPTIMIZE_CFLAGS=-O1


#.PHONY $(BINARY).gdb
#$(BINARY).gdb:
#	@echo target remote localhost:3333 > $(BINARY).gdb
#	@echo load $(BINARY) >> $(BINARY).gdb
#	@echo file $(BINARY) >> $(BINARY).gdb
#	@echo monitor reset halt >> $(BINARY).gdb
#	@echo tb main >> $(BINARY).gdb
#	@echo tb HardFault_Handler >> $(BINARY).gdb
#	@echo tui enable > $(BINARY).gdb
#	@echo c >> $(BINARY).gdb



