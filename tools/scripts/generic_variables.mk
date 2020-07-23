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

INCLUDE			?= $(NO-OS)/include
DRIVERS 		?= $(NO-OS)/drivers
PLATFORM_DRIVERS	?= $(NO-OS)/drivers/platform/$(PLATFORM)

ifeq (aducm3029,$(strip $(PLATFORM)))
WORKSPACE		?= $(NO-OS)/projects
else
WORKSPACE		?= $(PROJECT)/build
endif

