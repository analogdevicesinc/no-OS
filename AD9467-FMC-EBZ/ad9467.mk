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

M_INC_DIRS += $(NOOS-DIR)/drivers/AD9467
M_INC_DIRS += $(NOOS-DIR)/drivers/AD9517
M_INC_DIRS += $(NOOS-DIR)/AD9467-FMC-EBZ/

M_HDR_FILES :=

M_SRC_FILES := $(NOOS-DIR)/AD9467-FMC-EBZ/main.c

