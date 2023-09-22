PLATFORM = maxim
TARGET = max32650

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

CFLAGS += -DNO_OS_LWIP_NETWORKING
CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR
CFLAGS += -DNO_OS_LWIP_INIT_ONETIME=1

ifndef NO_OS_STATIC_IP
NO_OS_STATIC_IP = n
endif

ifeq (y,$(strip $(NO_OS_STATIC_IP)))
CFLAGS += -DNO_OS_STATIC_IP
endif
