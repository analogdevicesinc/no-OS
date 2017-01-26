## Makefile

include ../../scripts/noos.mk

M_SOPCINFO_FILE := $(HDL-DIR)/projects/daq3/a10gx/system_bd.sopcinfo
M_SOF_FILE := daq3_a10gx.sof

include $(NOOS-DIR)/ad-fmcdaq3-ebz/ad_fmcdaq3_ebz.mk

## work around for altera nios2-bsp bug

M_HDR_FILES += bsp/linker_modified.h

.PHONY: default
default: all

bsp/linker_modified.h: bsp/system.h
	cp bsp/linker.x bsp/linker_original.x
	sed -i 's/SIZEOF (.rodata)+ SIZEOF (.rwdata)/SIZEOF (.rodata)/' bsp/linker.x
	touch bsp/linker_modified.h

include $(NOOS-DIR)/scripts/nios_ii.mk

