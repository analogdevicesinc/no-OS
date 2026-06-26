EXAMPLE := apard_communication_example

APARD32690_STATIC_IP = y

PLATFORM=maxim
TARGET=max32690

ifndef APARD32690_STATIC_IP
APARD32690_STATIC_IP = n
endif

ifeq (y,$(strip $(APARD32690_STATIC_IP)))
CONFIG_NO_OS_IP=192.168.97.50
CONFIG_NO_OS_NETMASK=255.255.0.0
CONFIG_NO_OS_GATEWAY=0.0.0.0
endif

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
