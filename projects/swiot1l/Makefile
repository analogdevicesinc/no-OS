PLATFORM = maxim
TARGET = max32650

NO_OS_IP=192.168.100.200
NO_OS_NETMASK=255.255.255.0
NO_OS_GATEWAY=0.0.0.0

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

CFLAGS += -DNO_OS_LWIP_NETWORKING
CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR
CFLAGS += -DNO_OS_LWIP_INIT_ONETIME=1
