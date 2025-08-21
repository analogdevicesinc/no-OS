IIOD ?= n
MB ?= n
ZU ?= n
include ../../tools/scripts/generic_variables.mk

# Uncomment to select the profile:
PROFILE = tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88
#PROFILE = tx_bw200_ir245p76_rx_bw200_or245p76_orx_bw200_or245p76_dc245p76
#PROFILE = tx_bw400_ir491p52_rx_bw200_or245p76_orx_bw400_or491p52_dc245p76

ifeq (n,$(strip $(MB)))
ifeq (n,$(strip $(ZU)))
include arm_src.mk
else
include zu_src.mk
endif
else
include mb_src.mk
endif

include ../../tools/scripts/generic.mk