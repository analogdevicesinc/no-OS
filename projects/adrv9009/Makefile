IIOD ?= n
MB ?= n
include ../../tools/scripts/generic_variables.mk

ifeq (n,$(strip $(MB)))
include arm_src.mk
else
include mb_src.mk
endif

include ../../tools/scripts/generic.mk