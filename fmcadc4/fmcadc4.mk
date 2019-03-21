## NOOS-DIR:
##		is the no-OS root variable (do not override)
##
## M_INC_DIRS:
##		list all your include folders (all *.h and *.c files from these folders are included
##		in compile and passed as directives)
##
## M_HDR_FILES:
##		if you want to hand pick files, use this variable to list header files.
##
## M_SRC_FILES:
##		if you want to hand pick files, use this variable to list source files.

## CAPTURE SCRIPT
NR_OF_CHAN := 4

M_INC_DIRS := $(NOOS-DIR)/common_drivers/platform_drivers
M_INC_DIRS += $(NOOS-DIR)/common_drivers/adc_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/xcvr_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/xcvr_core/xcvr_modules
M_INC_DIRS += $(NOOS-DIR)/common_drivers/jesd_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/dmac_core
M_INC_DIRS += $(NOOS-DIR)/drivers/frequency/ad9528
M_INC_DIRS += $(NOOS-DIR)/drivers/adc/ad9680

M_HDR_FILES := $(NOOS-DIR)/fmcadc4/config.h

M_SRC_FILES := $(NOOS-DIR)/fmcadc4/fmcadc4.c

