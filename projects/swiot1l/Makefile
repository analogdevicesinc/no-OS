SWIOT1L_DEFAULT_FW = y
SWIOT1L_MQTT_EXAMPLE = n

PLATFORM = maxim
TARGET = max32650

ifndef SWIOT1L_STATIC_IP
SWIOT1L_STATIC_IP = n
endif

NO_OS_DOMAIN_NAME=swiot1l

ifeq (y,$(strip $(SWIOT1L_STATIC_IP)))
NO_OS_IP=10.32.51.3
NO_OS_NETMASK=255.255.255.0
NO_OS_GATEWAY=10.32.51.1
endif

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
CFLAGS += -DNO_OS_LWIP_NETWORKING
CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR
CFLAGS += -DNO_OS_LWIP_INIT_ONETIME=1
CFLAGS += -g -O0
CFLAGS += -mfloat-abi=soft

LDFLAGS+=-L$(MBEDTLSDIR)/library -lmbedtls -lmbedcrypto -lmbedx509
