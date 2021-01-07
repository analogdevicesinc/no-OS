# Uncomment to use the desired platform
# PLATFORM = xilinx
# PLATFORM = altera
# PLATFORM = aducm3029
# PLATFORM = stm32

TINYIIOD=y

ifeq (stm32,$(PLATFORM))
CFLAGS = -DSTM32F446xx
CHIPNAME = STM32F446RETx
endif

include ../../tools/scripts/generic_variables.mk

include src.mk

include ../../tools/scripts/generic.mk
