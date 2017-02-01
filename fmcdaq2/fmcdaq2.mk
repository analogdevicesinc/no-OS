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

# PLATFORM ?= undef 
# 
# ifeq "$(PLATFORM)" "xilinx"
# else ifeq "$(PLATFORM)" "altera"
# else
# $(error "Platform is undefined!")
# endif

M_INC_DIRS := $(NOOS-DIR)/common_drivers/platform_drivers
M_INC_DIRS += $(NOOS-DIR)/common_drivers/adc_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/dac_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/dmac_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/xcvr_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/jesd_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/dac_buffer
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9144
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9523
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9680

M_HDR_FILES :=

M_SRC_FILES := $(NOOS-DIR)/ad-fmcdaq2-ebz/ad_fmcdaq2_ebz.c

