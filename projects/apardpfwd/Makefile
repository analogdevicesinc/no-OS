EXAMPLE := apard_communication_example

APARD32690_STATIC_IP = y

PLATFORM=maxim
TARGET=max32690

ifndef APARD32690_STATIC_IP
APARD32690_STATIC_IP = n
endif

ifeq (y,$(strip $(APARD32690_STATIC_IP)))
CFLAGS += -DNO_OS_STATIC_IP
endif

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
