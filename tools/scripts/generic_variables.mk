# Create variables that will be used within src.mk

# This variables will be set to their default value if not set by the user
# It assumes that the makefile is invoked from No-OS/project/some_project

# For these platforms, the user may optionally pass the PLATFORM variable:
# PLATFORM = xilinx
# PLATFORM = altera
# PLATFORM = aducm3029
# PLATFORM = stm32
# PLATFORM = maxim

ifeq '$(LOCAL_BUILD)' 'y'

include ..\..\tools\scripts\local_variables.mk

endif

#Variables needed by makefile
PROJECT			?= $(realpath .)
NO-OS			?= $(realpath $(PROJECT)/../../)
ROOT_DRIVE		?= $(firstword $(subst /, ,$(PROJECT)))/
BUILD_DIR		?= $(PROJECT)/build
WORKSPACE		?= $(BUILD_DIR)

#Useful variables for src.mk
INCLUDE			?= $(NO-OS)/include
DRIVERS 		?= $(NO-OS)/drivers
PLATFORM_DRIVERS	?= $(NO-OS)/drivers/platform/$(PLATFORM)

GIT_VERSION := $(shell git describe --all --long --dirty=-modified)
GIT_VERSION := $(subst heads/,,$(GIT_VERSION))
GIT_VERSION := $(subst -0-g,-,$(GIT_VERSION))
CFLAGS += -DNO_OS_VERSION=$(GIT_VERSION) \
		-DNO_OS_PROJECT=$(notdir $(PROJECT))

#Variables for compiling flags in generic.mk
CFLAGS_MFLOAT_TYPE 	?= hard

ifeq '$(NO_OS_USB_UART)' 'y'
CFLAGS += -DNO_OS_USB_UART
endif
#------------------------------------------------------------------------------
#                          EVALUATE PLATFORM
#------------------------------------------------------------------------------
# By default link noos sources into the build directory of the project.
# This way all needed files will be in one place and they can be tracked on git
LINK_SRCS ?= y

HARDWARE ?= $(wildcard *.xsa) $(wildcard *.hdf) $(wildcard *.sopcinfo) $(wildcard *.ioc) $(wildcard pinmux_config.c)
#If platform not set get it from HARDWARE file
ifneq '' '$(or $(findstring max,$(TARGET)), $(findstring MAX,$(TARGET)))'
PLATFORM = maxim
else
ifeq '' '$(PLATFORM)'
ifneq '' '$(findstring .xsa,$(HARDWARE))'
PLATFORM = xilinx
else
ifneq '' '$(findstring .hdf,$(HARDWARE))'
PLATFORM = xilinx
else
ifneq '' '$(findstring .sopcinfo,$(HARDWARE))'
PLATFORM = altera
else
ifneq '' '$(findstring .ioc,$(HARDWARE))'
PLATFORM = stm32
else
ifneq '' '$(findstring pinmux_config.c,$(HARDWARE))'
PLATFORM = aducm3029
else
$(error No HARDWARE or TARGET found. Please specify one of them.)
endif
endif
endif
endif
endif
endif
endif
