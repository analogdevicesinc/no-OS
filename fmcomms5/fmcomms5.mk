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
##		4 channels of I/Q data (I-Q separate files)
NR_OF_CHAN := 8

M_INC_DIRS := $(NOOS-DIR)/ad9361/sw/platform_xilinx
M_INC_DIRS += $(NOOS-DIR)/ad9361/sw

M_HDR_FILES :=

M_SRC_FILES :=

M_COMPILER_DEFINES := XILINX_PLATFORM
M_COMPILER_DEFINES += FMCOMMS5
