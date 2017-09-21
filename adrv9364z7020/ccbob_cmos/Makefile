## Makefile

PLATFORM := XILINX
MODE := 1R1T

include ../../scripts/noos.mk
include $(NOOS-DIR)/ad9361/sw/ad9361_noos.mk

M_HDF_FILE := $(HDL-DIR)/projects/adrv9364z7020/ccbob_cmos/adrv9364z7020_ccbob_cmos.sdk/system_top.hdf
M_DEFINES += PICOZED_SDR
M_DEFINES += PICOZED_SDR_CMOS
M_DEFINES += PICOZED_SDR_CMOS_SWAP_PORTS

include $(NOOS-DIR)/scripts/zynq.mk

