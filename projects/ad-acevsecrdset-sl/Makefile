include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

CFLAGS += -DNO_OS_LOG_LEVEL=NO_OS_LOG_DEBUG

ifdef REV_A
CFLAGS += -DREV_A=1
else
CFLAGS += -DREV_D=1
endif
