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
##		2 channels of I/Q data (I-Q separate files)
NR_OF_CHAN := 4

M_INC_DIRS := $(NOOS-DIR)/ad9361/sw/platform_xilinx
M_INC_DIRS += $(NOOS-DIR)/ad9361/sw

M_HDR_FILES :=

M_SRC_FILES :=

M_COMPILER_DEFINES += XILINX_PLATFORM
M_COMPILER_DEFINES += PICOZED_SDR
M_COMPILER_DEFINES += AXI_ADC_NOT_LEGACY_DIGITAL_TUNE
M_COMPILER_DEFINES += HAVE_VERBOSE_MESSAGES
M_COMPILER_DEFINES += "HAVE_SPLIT_GAIN_TABLE 1"
M_COMPILER_DEFINES += "HAVE_TDD_SYNTH_TABLE 1"

# 2R2T mode
M_COMPILER_DEFINES += "AD9361_DEVICE 1"
M_COMPILER_DEFINES += "AD9364_DEVICE 0"

# 1R1T mode
# M_COMPILER_DEFINES += "AD9361_DEVICE 0"
# M_COMPILER_DEFINES += "AD9364_DEVICE 1"

M_COMPILER_DEFINES += "AD9363A_DEVICE 0"
