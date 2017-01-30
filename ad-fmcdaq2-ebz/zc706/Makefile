## Makefile

include ../../scripts/noos.mk

M_HDF_FILE := $(HDL-DIR)/projects/daq2/zc706/daq2_zc706.sdk/system_top.hdf

## To build, use make
## To run on hardware, use make run

include $(NOOS-DIR)/ad-fmcdaq2-ebz/ad_fmcdaq2_ebz.mk
include $(NOOS-DIR)/scripts/zynq.mk

.PHONY: clean
clean:
	rm -rf hw bsp sw .metadata .Xil SDK.log xsct.log sw.elf RemoteSystemsTempFiles webtalk xilsw

