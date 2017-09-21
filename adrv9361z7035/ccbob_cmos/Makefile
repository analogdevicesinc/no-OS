## Makefile

PLATFORM := XILINX
MODE := 2R2T

include ../../scripts/noos.mk
include $(NOOS-DIR)/ad9361/sw/ad9361_noos.mk

M_HDF_FILE := $(HDL-DIR)/projects/adrv9361z7035/ccbob_cmos/adrv9361z7035_ccbob_cmos.sdk/system_top.hdf
M_DEFINES += PICOZED_SDR
M_DEFINES += PICOZED_SDR_CMOS

include $(NOOS-DIR)/scripts/zynq.mk

