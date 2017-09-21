## Makefile

PLATFORM := XILINX
MODE := 1R1T

include ../../scripts/noos.mk
include $(NOOS-DIR)/ad9361/sw/ad9361_noos.mk

M_HDF_FILE := $(HDL-DIR)/projects/adrv9364z7020/ccbob_lvds/adrv9364z7020_ccbob_lvds.sdk/system_top.hdf
M_DEFINES += PICOZED_SDR

include $(NOOS-DIR)/scripts/zynq.mk

