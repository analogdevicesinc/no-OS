TINYIIOD = y
QUAD_MXFE = y

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

ifeq (y,$(strip $(QUAD_MXFE)))
	CFLAGS += -DQUAD_MXFE -DNO_OS_LOG_LEVEL=NO_OS_LOG_DEBUG
endif
