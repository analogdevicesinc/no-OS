EXAMPLE ?= mqtt_baremetal
BOARD_REV ?= HP_NON_IS
HP_STATIC_IP ?= y
CONNECTION_TYPE ?= PLAIN_CONNECTION #If running an MQTT example, must select a connection type i.e. PLAIN_CONNECTION or SECURE_CONNECTION

CFLAGS += -D$(BOARD_REV)=1
PLATFORM=maxim
TARGET=max32690

LIB_FLAGS += -lm
LDFLAGS += -u_printf_float

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
