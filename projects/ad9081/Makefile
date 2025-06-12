IIOD ?= n
QUAD_MXFE = n

# Uncomment to select the profile:
#PROFILE = vcu118_quad_ad9081_204c_txmode_11_rxmode_4_revc
#PROFILE = vcu118_quad_ad9081_204b_txmode_9_rxmode_10_revc
PROFILE = vcu118_ad9081_m8_l4

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

ifeq (y,$(strip $(QUAD_MXFE)))
	CFLAGS += -DQUAD_MXFE
endif
