LIBRARIES += lvgl
IIOD=y

PLATFORM=maxim
TARGET=max32662

SRCS += $(PROJECT)/src/common/syscalls.c

include ../../tools/scripts/generic_variables.mk

HEAP_SIZE=0x90000
STACK_SIZE=0x10000

include src.mk

CFLAGS += -Os -ffunction-sections -fdata-sections -fno-strict-aliasing
LDFLAGS += -Wl,--gc-sections

include ../../tools/scripts/generic.mk
