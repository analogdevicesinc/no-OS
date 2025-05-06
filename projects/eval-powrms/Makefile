LIBRARIES += lvgl

SRCS += $(PROJECT)/src/common/syscalls.c


include ../../tools/scripts/generic_variables.mk

HEAP_SIZE=0x80000
STACK_SIZE=0x20000

include src.mk

CFLAGS += -Os -ffunction-sections -fdata-sections -fno-strict-aliasing -flto
LDFLAGS += -Wl,--gc-sections -flto --specs=nosys.specs
# LDFLAGS += -Wl,--wrap=_getpid_r

include ../../tools/scripts/generic.mk
