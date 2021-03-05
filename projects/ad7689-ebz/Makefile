ifeq (stm32,$(PLATFORM))
CFLAGS = -DSTM32F446xx
CHIPNAME = STM32F446RETx
endif

CFLAGS += -DLOG_LEVEL=7

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
