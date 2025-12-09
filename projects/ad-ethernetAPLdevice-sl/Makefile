EXAMPLE ?= ad7124_app
BOARD_REV ?= HP_IS
HP_STATIC_IP ?= n

CFLAGS += -D$(BOARD_REV)=1
PLATFORM=maxim
TARGET=max32690

LIB_FLAGS += -lm
LDFLAGS += -u_printf_float

ifeq (y,$(strip $(HP_STATIC_IP)))
NO_OS_IP=192.168.97.60
NO_OS_NETMASK=255.255.255.0
NO_OS_GATEWAY=192.168.97.1
endif

SBT=1

HEAP_SIZE = 0x80000
STACK_SIZE = 0xFFFF

ifeq ($(SBT), 1)
MAXIM_SBT_DIR ?= $(MAXIM_PATH)/Tools/SBT
MAXIM_SBT_DIR := $(subst \,/,$(MAXIM_SBT_DIR))
# ^ Must sanitize path for \ on Windows, since this may come from an environment
# variable.
 
export MAXIM_SBT_DIR # SBTs must have this environment variable defined to work
 
# SBT-config.mk and SBT-rules.mk are included further down this Makefile.
 
endif # SBT
<<<<<<< HEAD
<<<<<<< HEAD
 
=======

>>>>>>> 8ab825dbe (Fix for secure boot)
=======

>>>>>>> b7554c4f2 (Secure boot signing for a Hockey Puck No-OS build)
# *******************************************************************************
# Default goal selection.  This section allows you to override the default goal
# that will run if no targets are specified on the command-line.
# (ie. just running 'make' instead of 'make all')
 
# Configuration variables:
#   .DEFAULT_GOAL : Set the default goal if no targets were specified on the
#           command-line
#           ** "override" must be used with this variable. **
#           Ex: "override .DEFAULT_GOAL = mygoal"
 
ifeq "$(.DEFAULT_GOAL)" ""
ifeq ($(SBT),1)
override .DEFAULT_GOAL := sla
else
override .DEFAULT_GOAL := all
endif
endif
# Developer note:  'override' is used above for legacy Makefile compatibility.
# gcc.mk/gcc_riscv.mk need to hard-set 'all' internally, so this new system
# uses 'override' to come in over the top without breaking old projects.
 
# It's also necessary to explicitly set MAKECMDGOALS...
ifeq "$(MAKECMDGOALS)" ""
MAKECMDGOALS:=$(.DEFAULT_GOAL)
endif
<<<<<<< HEAD
=======

>>>>>>> b7554c4f2 (Secure boot signing for a Hockey Puck No-OS build)
# *******************************************************************************
# Include SBT config.  We need to do this here because it needs to know
# the current MAKECMDGOAL.
ifeq ($(SBT),1)
include ../../tools/scripts/SBT-config.mk
endif
<<<<<<< HEAD
=======

>>>>>>> b7554c4f2 (Secure boot signing for a Hockey Puck No-OS build)
include ../../tools/scripts/generic_variables.mk
 
include ../../tools/scripts/examples.mk
 
include src.mk
 
ifeq ($(SBT), 1)
include ../../tools/scripts/SBT-rules.mk
endif
 
# Get .DEFAULT_GOAL working.
ifeq "$(MAKECMDGOALS)" ""
MAKECMDGOALS:=$(.DEFAULT_GOAL)
endif

<<<<<<< HEAD
=======
include ../../tools/scripts/generic.mk

# Include the rules that integrate the SBTs.  SBTs are a special case that must be
# include after the core gcc rules to extend them.
>>>>>>> b7554c4f2 (Secure boot signing for a Hockey Puck No-OS build)
ifeq ($(SBT), 1)
include ../../tools/scripts/SBT-rules.mk
endif

<<<<<<< HEAD
=======

>>>>>>> b7554c4f2 (Secure boot signing for a Hockey Puck No-OS build)
# Get .DEFAULT_GOAL working.
ifeq "$(MAKECMDGOALS)" ""
MAKECMDGOALS:=$(.DEFAULT_GOAL)
endif
<<<<<<< HEAD

include ../../tools/scripts/generic.mk
=======
>>>>>>> b7554c4f2 (Secure boot signing for a Hockey Puck No-OS build)
