HEAP_SIZE=0x80000
STACK_SIZE=0x40000

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
CFLAGS += -DNO_OS_LWIP_NETWORKING

ifndef NO_OS_STATIC_IP
NO_OS_STATIC_IP = n
endif

ifeq (y,$(strip $(NO_OS_STATIC_IP)))
CFLAGS += -DNO_OS_STATIC_IP
endif
