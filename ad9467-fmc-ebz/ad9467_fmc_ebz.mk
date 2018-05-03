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
NR_OF_CHAN := 1

M_INC_DIRS := $(NOOS-DIR)/common_drivers/platform_drivers
M_INC_DIRS += $(NOOS-DIR)/common_drivers/adc_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/dmac_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/xcvr_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/xcvr_core/xcvr_modules
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9467
M_INC_DIRS += $(NOOS-DIR)/drivers/ad9517
M_INC_DIRS += $(NOOS-DIR)/ad9467-fmc-ebz

M_HDR_FILES :=

M_SRC_FILES :=

