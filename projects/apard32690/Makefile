PLATFORM=maxim
TARGET=max32690

LIBRARIES += maxq1065_lib
LIBRARIES += lwip

include ../../tools/scripts/generic_variables.mk

CFLAGS += -DMXQ_ON_NO_OS
CFLAGS += -DNO_OS_STATIC_IP
CFLAGS += -DNO_OS_LWIP_NETWORKING

include src.mk

include ../../tools/scripts/generic.mk
