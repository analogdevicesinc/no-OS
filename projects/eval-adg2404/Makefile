###############################################################################
#
# ADG2404 Project for MAX32655FTHR
#
###############################################################################

# Target microcontroller and board
TARGET := MAX32655
TARGET_UC := MAX32655
TARGET_LC := max32655

# Board: FTHR_Apps_P1 for MAX32655FTHR
BOARD := FTHR_Apps_P1

# Project name
PROJECT := adg2404

# MaximSDK path
MAXIM_PATH ?= $(HOME)/MaximSDK

# no-OS path
NO_OS_PATH ?= $(HOME)/no-OS

# Sanitize paths
MAXIM_PATH := $(subst \,/,$(MAXIM_PATH))
NO_OS_PATH := $(subst \,/,$(NO_OS_PATH))

# Locate SDK paths
LIBS_DIR := $(abspath $(MAXIM_PATH)/Libraries)
CMSIS_ROOT := $(LIBS_DIR)/CMSIS

# Source files
SRCS := src/examples/basic/basic_example.c
SRCS += src/common/common_data.c

# Add no-OS driver source files
SRCS += $(NO_OS_PATH)/drivers/mux/adg2404/adg2404.c
SRCS += $(NO_OS_PATH)/drivers/platform/maxim/max32655/maxim_gpio.c
SRCS += $(NO_OS_PATH)/drivers/platform/maxim/max32655/maxim_delay.c

# no-OS API sources
SRCS += $(NO_OS_PATH)/drivers/api/no_os_gpio.c

# no-OS utility sources (for memory allocation)
SRCS += $(NO_OS_PATH)/util/no_os_alloc.c
SRCS += $(NO_OS_PATH)/util/no_os_mutex.c

# Include paths
IPATH := .
IPATH += src/common
IPATH += $(NO_OS_PATH)/include
IPATH += $(NO_OS_PATH)/util
IPATH += $(NO_OS_PATH)/drivers/api
IPATH += $(NO_OS_PATH)/drivers/mux/adg2404
IPATH += $(NO_OS_PATH)/drivers/platform/maxim/max32655
IPATH += $(NO_OS_PATH)/drivers/platform/maxim/common

# Add CMSIS and peripheral includes
IPATH += $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Include
IPATH += $(CMSIS_ROOT)/Core/Include
IPATH += $(LIBS_DIR)/PeriphDrivers/Include/$(TARGET_UC)
IPATH += $(LIBS_DIR)/Boards/$(TARGET_UC)/$(BOARD)/Include

# Compiler settings
COMPILER := GCC
DEBUG := 1
MXC_OPTIMIZE_CFLAGS := -Og

# Project compiler flags
PROJ_CFLAGS := -Wall -DMXC_ASSERT_ENABLE

# Floating point ABI
MFLOAT_ABI := softfp
export MFLOAT_ABI

# Disable auto-search (we specify sources explicitly)
AUTOSEARCH := 0

# Export required variables
export TARGET
export TARGET_UC
export TARGET_LC
export CMSIS_ROOT

# Include project.mk for additional customization
PROJECTMK := $(abspath ./project.mk)
-include $(PROJECTMK)

# Add system-level initialization files
VPATH += $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source

# Include the libraries
include $(LIBS_DIR)/libs.mk

# Include the rules for building for this target
include $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Source/$(COMPILER)/$(TARGET_LC).mk

# Default target
.DEFAULT_GOAL := all

all:
	$(PREFIX)-size --format=berkeley $(BUILD_DIR)/$(PROJECT).elf

# Library clean
libclean:
	$(MAKE) -f ${PERIPH_DRIVER_DIR}/periphdriver.mk clean.periph

# Full clean
distclean: clean libclean

# Reset - full project reset (like no-OS reset)
# Cleans build dir, peripheral libraries, and VS Code config
reset: clean libclean
	@echo -  RMDIR .vscode
	@rm -rf .vscode

# Flash target using OpenOCD
flash: all
	$(MAXIM_PATH)/Tools/OpenOCD/openocd -s $(MAXIM_PATH)/Tools/OpenOCD/scripts \
		-f interface/cmsis-dap.cfg \
		-f target/max32655.cfg \
		-c "adapter speed 1000; init; reset halt; program $(BUILD_DIR)/$(PROJECT).elf verify reset exit"

.PHONY: all clean libclean distclean reset flash
