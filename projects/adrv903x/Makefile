EXAMPLE ?= basic_example

# Uncomment to use the desired platform
PLATFORM = xilinx

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk

LIB_FLAGS += -lm
LDFLAGS += -Xlinker --defsym=_HEAP_SIZE=0x800000
LDFLAGS += -Wl,--wrap=fseek,--wrap=fclose
CFLAGS += -DADI_ADRV903X_API_VERSION_RANGE_CHECK=0
