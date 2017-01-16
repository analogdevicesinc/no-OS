## Makefile

include ../../scripts/noos.mk

M_SOPCINFO_FILE := $(HDL-DIR)/projects/adrv9371x/a10gx/system_bd.sopcinfo
M_SOF_FILE := adrv9371x_a10gx.sof

include $(NOOS-DIR)/adrv9371x/adrv9371x.mk

## work around for altera nios2-bsp bug

M_HDR_FILES += bsp/linker_modified.h

.PHONY: default
default: all

bsp/linker_modified.h: bsp/system.h
	cp bsp/linker.x bsp/linker_original.x
	sed -i 's/SIZEOF (.rodata)+ SIZEOF (.rwdata)/SIZEOF (.rodata)/' bsp/linker.x
	touch bsp/linker_modified.h

include $(NOOS-DIR)/scripts/nios_ii.mk

