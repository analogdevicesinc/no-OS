EXAMPLE ?= basic_example
IIOD=y

# Uncomment to use the desired platform
PLATFORM = xilinx

CFLAGS += -DGEM_INSTANCE=0 -DNO_OS_LWIP_NETWORKING -DSSIZE_MAX=INT_MAX

CFLAGS += -DONLY_IIOD

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk
