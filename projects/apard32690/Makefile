APARD32690_BASIC_EXAMPLE = n
APARD32690_ADIN1110_STANDALONE_EXAMPLE = n
APARD32690_ECHO_SERVER_EXAMPLE = y
APARD32690_STATIC_IP = y

PLATFORM=maxim
TARGET=max32690

ifndef APARD32690_STATIC_IP
APARD32690_STATIC_IP = n
endif

ifeq (y,$(strip $(APARD32690_STATIC_IP)))
NO_OS_IP=192.168.97.40
NO_OS_NETMASK=255.255.0.0
NO_OS_GATEWAY=0.0.0.0
endif

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
