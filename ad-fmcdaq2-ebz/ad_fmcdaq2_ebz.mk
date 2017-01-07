## M_SDK_DIR:
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

M_INC_DIRS := $(M_SDK_DIR)/common_drivers/xilinx_platform_drivers
M_INC_DIRS := $(M_SDK_DIR)/common_drivers/adc_core
M_INC_DIRS := $(M_SDK_DIR)/common_drivers/dac_core
M_INC_DIRS := $(M_SDK_DIR)/common_drivers/adxcvr_core
M_INC_DIRS += $(M_SDK_DIR)/drivers/ad9144
M_INC_DIRS += $(M_SDK_DIR)/drivers/ad9523
M_INC_DIRS += $(M_SDK_DIR)/drivers/ad9680

M_SRC_FILES := $(M_SDK_DIR)/ad-fmcdaq2-ebz/ad_fmcdaq2_ebz.c


