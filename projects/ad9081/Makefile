IIOD ?= y
QUAD_MXFE = n

# Uncomment to select the profile:
#PROFILE = vcu118_quad_ad9081_204c_txmode_11_rxmode_4_revc
#PROFILE = vcu118_quad_ad9081_204b_txmode_9_rxmode_10_revc
#PROFILE = zcu102_ad9081_m8_l4
#PROFILE = zcu102_ad9081_txmode_0_rxmode_1
PROFILE = vcu118_ad9081_m8_l4
#PROFILE = vcu118_ad9081_jesd204c_tx_mode10_rx_mode11

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk

ifeq (y,$(strip $(QUAD_MXFE)))
	CFLAGS += -DQUAD_MXFE
endif
