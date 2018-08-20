## Makefile

include ../../scripts/noos.mk

M_HDF_FILE := $(HDL-DIR)/projects/adrv9364z7020/ccbob_cmos/adrv9364z7020_ccbob_cmos.sdk/system_top.hdf

M_COMPILER_DEFINES := CONFIG_H_
M_COMPILER_DEFINES += PICOZED_SDR_CMOS
M_COMPILER_DEFINES += PICOZED_SDR_CMOS_SWAP_PORTS

include $(NOOS-DIR)/adrv9364z7020/adrv9364z7020.mk
include $(NOOS-DIR)/scripts/zynq_ps7.mk

