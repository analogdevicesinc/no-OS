## Makefile

PLATFORM := XILINX
MODE := 2R2T

include ../../scripts/noos.mk
include $(NOOS-DIR)/ad9361/sw/ad9361_noos.mk

M_HDF_FILE := $(HDL-DIR)/projects/adrv9361z7035/ccfmc_lvds/adrv9361z7035_ccfmc_lvds.sdk/system_top.hdf
M_DEFINES += PICOZED_SDR

include $(NOOS-DIR)/scripts/zynq.mk

