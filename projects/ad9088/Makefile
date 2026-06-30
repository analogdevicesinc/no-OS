# Uncomment to use the desired platform
PLATFORM = xilinx

# Select the example you want to enable by chossing y for enabling and n for disabling
BASIC_EXAMPLE = y
#IIO_EXAMPLE = n

include ../../tools/scripts/generic_variables.mk

FIRMWARE ?= $(NO-OS)/drivers/rf-transceiver/apollo/firmware/204C_M4_L8_NP16_20p0_4x4.bin
NEW_FIRMWARE = $(dir $(FIRMWARE))usecase.bin
$(shell cp $(FIRMWARE) $(NEW_FIRMWARE))

include src.mk

include ../../tools/scripts/generic.mk
