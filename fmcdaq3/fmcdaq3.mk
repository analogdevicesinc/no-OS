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

M_INC_DIRS := $(NOOS-DIR)/common_drivers/fpga_platform_drivers
M_INC_DIRS += $(NOOS-DIR)/common_drivers/adc_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/dac_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/xcvr_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/jesd_core
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9152
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9528
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9680

M_HDR_FILES :=

M_SRC_FILES := $(NOOS-DIR)/fmcdaq3/fmcdaq3.c

