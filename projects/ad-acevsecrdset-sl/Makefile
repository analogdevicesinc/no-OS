REV_A = n
REV_D = y
ifeq (y,$(strip $(REV_A)))
CFLAGS += -DREV_A
else
CFLAGS += -DREV_D
endif

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

CFLAGS += -DNO_OS_LOG_LEVEL=NO_OS_LOG_DEBUG
