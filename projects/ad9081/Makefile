IIOD = y
QUAD_MXFE = n

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

ifeq (y,$(strip $(QUAD_MXFE)))
	CFLAGS += -DQUAD_MXFE
endif
