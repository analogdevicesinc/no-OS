## NOOS-DIR:
##		is the no-OS root variable (do not override)
##
## M_INC_DIRS:
##		list all your include folders (used only as compile directives)
##
## M_SRC_DIRS:
##		list all your source folders (all *.c files inside are compiled)
##
## M_SRC_FILES:
##		list all your selective source files

M_SRC_DIRS := $(NOOS-DIR)/common_drivers/platform_drivers
M_SRC_DIRS += $(NOOS-DIR)/common_drivers/adc_core
M_SRC_DIRS += $(NOOS-DIR)/common_drivers/dmac_core
M_SRC_DIRS += $(NOOS-DIR)/common_drivers/xcvr_core
M_SRC_DIRS += $(NOOS-DIR)/common_drivers/jesd_core
M_SRC_DIRS += $(NOOS-DIR)/ad-fmcjesdadc1-ebz/

M_SRC_FILES := $(NOOS-DIR)/ad-fmcjesdadc1-ebz/ad_fmcjesdadc1_ebz.c

