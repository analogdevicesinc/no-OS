PLATFORM=maxim
# Default to MAX32672 if not specified
TARGET ?= max32672

include ../../tools/scripts/generic_variables.mk

# Add target-specific defines
ifeq ($(TARGET),max32672)
CFLAGS += -DTARGET_MAX32672
else ifeq ($(TARGET),max32690)
CFLAGS += -DTARGET_MAX32690
else
$(error Unsupported TARGET: $(TARGET). Use max32672 or max32690)
endif

include src.mk

include ../../tools/scripts/generic.mk
