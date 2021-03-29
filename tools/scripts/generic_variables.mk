# Create variables that will be used within src.mk

# This variables will be set to their default value if not set by the user
# It assumes that the makefile is invoked from No-OS/project/some_project

# For these platforms, the user may optionally pass the PLATFORM variable:
# PLATFORM = xilinx
# PLATFORM = altera
# PLATFORM = aducm3029

# For these platforms, the user must set the PLATFORM variable:
# PLATFORM = stm32

ifeq '$(LOCAL_BUILD)' 'y'

include ..\..\tools\scripts\local_variables.mk

endif

#Variables needed by makefile
NO-OS			?= $(realpath ../..)
PROJECT			?= $(realpath .)
BUILD_DIR		?= $(PROJECT)/build
WORKSPACE		?= $(BUILD_DIR)

#Useful variables for src.mk
INCLUDE			?= $(NO-OS)/include
DRIVERS 		?= $(NO-OS)/drivers
PLATFORM_DRIVERS	?= $(NO-OS)/drivers/platform/$(PLATFORM)

#------------------------------------------------------------------------------
#                          EVALUATE PLATFORM
#------------------------------------------------------------------------------

# By default link noos sources into the build directory of the project.
# This way all needed files will be in one place and they can be tracked on git
LINK_SRCS ?= y

HARDWARE ?= $(wildcard *.xsa) $(wildcard *.hdf) $(wildcard *.sopcinfo) $(wildcard pinmux_config.c)
#If platform not set get it from HARDWARE file
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
ifneq '' '$(findstring pinmux_config.c,$(HARDWARE))'
PLATFORM = aducm3029
else
$(error No HARDWARE found)
endif
endif
endif
endif
endif
