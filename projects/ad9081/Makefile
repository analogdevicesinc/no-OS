TINYIIOD = y
QUAD_MXFE = n

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

ifeq (y,$(strip $(QUAD_MXFE)))
	CFLAGS += -D QUAD_MXFE
endif
