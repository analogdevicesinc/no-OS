EXAMPLE ?= ad7124_app
BOARD_REV ?= HP_IS
HP_STATIC_IP ?= n

CFLAGS += -D$(BOARD_REV)=1
PLATFORM=maxim
TARGET=max32690

LIB_FLAGS += -lm

ifeq (y,$(strip $(HP_STATIC_IP)))
NO_OS_IP=192.168.97.60
NO_OS_NETMASK=255.255.255.0
NO_OS_GATEWAY=0.0.0.0
endif

HEAP_SIZE = 0x80000
STACK_SIZE = 0xFFFF

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
