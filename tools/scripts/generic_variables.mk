# Create variables that will be used within src.mk

# This variables will be set to their default value if not set by the user
# It assumes that the makefile is invoked from No-OS/project/some_project

# User must set PLATFORM variable between:
# PLATFORM = xilinx
# PLATFORM = altera
# PLATFORM = aducm3029

PROJECT			?= $(realpath .)
TARGET 			?= $(notdir $(realpath .))
NO-OS			?= $(realpath ../..)
WORKSPACE		?= $(PROJECT)/build

INCLUDE			?= $(NO-OS)/include
DRIVERS 		?= $(NO-OS)/drivers
PLATFORM_DRIVERS	?= $(NO-OS)/drivers/platform/$(PLATFORM)

#------------------------------------------------------------------------------
#                          EVALUATE PLATFORM
#------------------------------------------------------------------------------

LEGACY_BUILD ?= n

ifneq '$(strip $(LEGACY_BUILD))' 'y'

HARDWARE ?= $(wildcard *.hdf) $(wildcard *.sopcinfo) $(wildcard pinmux_config.c)

#If platform not set get it from HARDWARE file
ifeq '' '$(PLATFORM)'
ifneq '' '$(findstring hdf,$(HARDWARE))'
PLATFORM = xilinx
else
ifneq '' '$(findstring sopcinfo,$(HARDWARE))'
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