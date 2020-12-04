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

M_INC_DIRS := $(NOOS-DIR)/common_drivers/platform_drivers
M_INC_DIRS += $(NOOS-DIR)/common_drivers/adc_core
M_INC_DIRS += $(NOOS-DIR)/common_drivers/dmac_core
M_INC_DIRS += $(NOOS-DIR)/drivers/adc/ad9265

M_HDR_FILES := $(NOOS-DIR)/ad9265-fmc-125ebz/config.h

M_SRC_FILES := $(NOOS-DIR)/ad9265-fmc-125ebz/ad9265_fmc_125ebz.c

