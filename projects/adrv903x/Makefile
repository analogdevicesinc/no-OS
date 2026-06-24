EXAMPLE ?= iio_example

# Uncomment to use the desired platform
PLATFORM = xilinx

include ../../tools/scripts/generic_variables.mk

include ../../tools/scripts/examples.mk

include src.mk

include ../../tools/scripts/generic.mk

# Platform-specific clock and transceiver sources.
# ARCH is set by generic.mk (via xilinx.mk) from the XSA file.
# On a clean build the first make invocation has ARCH empty (arch.txt
# not yet generated); the recursive make that actually compiles will
# have it set.  The ifneq below is therefore evaluated correctly in
# the recursive invocation that matters (update / build).
ifneq (,$(findstring cortexa72,$(strip $(ARCH))))
# Versal Tetra: HMC7044 clock, no ADXCVR or CLKGEN IPs
CFLAGS += -DPLATFORM_VERSAL
endif

LIB_FLAGS += -lm
EXTRA_LIBS_NAMES += m
LDFLAGS += -Xlinker --defsym=_HEAP_SIZE=0x800000
LDFLAGS += -Wl,--wrap=fseek,--wrap=fclose
CFLAGS += -DADI_ADRV903X_API_VERSION_RANGE_CHECK=0
