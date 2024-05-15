PLATFORM = maxim
TARGET = max32650

ifndef SWIOT1L_STATIC_IP
SWIOT1L_STATIC_IP = n
endif

NO_OS_DOMAIN_NAME=swiot1l

ifeq (y,$(strip $(SWIOT1L_STATIC_IP)))
NO_OS_IP=192.168.97.40
NO_OS_NETMASK=255.255.0.0
NO_OS_GATEWAY=0.0.0.0
endif

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

CFLAGS += -DNO_OS_LWIP_NETWORKING
CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR
CFLAGS += -DNO_OS_LWIP_INIT_ONETIME=1
