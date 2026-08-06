# Uncomment to use the desired platform
PLATFORM = xilinx

# Select the example you want to enable by chossing y for enabling and n for disabling
BASIC_EXAMPLE = n
DMA_EXAMPLE = y
#IIO_EXAMPLE = n

include ../../tools/scripts/generic_variables.mk

# CFLAGS += -DNO_OS_LOG_LEVEL=7
DEBUG=1

# Remote hw_server for `make run` (e.g. SSH tunnel on 127.0.0.1:3122).
# util.py's _connect_session() reads these from the environment; export so
# they reach the `vitis -s` subprocess. Override on the command line if needed.
export XSCT_REMOTE_HOST ?= 127.0.0.1
export XSCT_REMOTE_PORT ?= 3122

FIRMWARE ?= $(NO-OS)/drivers/rf-transceiver/apollo/firmware/204C_M4_L8_NP16_20p0_4x2.bin
NEW_FIRMWARE = $(dir $(FIRMWARE))usecase.bin
$(shell cp $(FIRMWARE) $(NEW_FIRMWARE))

include src.mk

include ../../tools/scripts/generic.mk
