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
M_SRC_DIRS += $(NOOS-DIR)/common_drivers/dac_core
M_SRC_DIRS += $(NOOS-DIR)/common_drivers/dmac_core
M_SRC_DIRS += $(NOOS-DIR)/drivers/ad9739a
M_SRC_DIRS += $(NOOS-DIR)/drivers/adf4350

M_SRC_FILES := $(NOOS-DIR)/ad9739a-fmc-ebz/ad9739a_fmc_ebz.c

