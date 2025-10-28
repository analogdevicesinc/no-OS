CFLAGS += -DHP_AD7124_APP
SRC_DIRS += $(PROJECT)/src/examples/ad7124_app

SRCS += $(DRIVERS)/adc/ad7124/ad7124.c
INCS += $(DRIVERS)/adc/ad7124/ad7124.h
SRCS += $(DRIVERS)/adc/ad7124/ad7124_regs.c
INCS += $(DRIVERS)/adc/ad7124/ad7124_regs.h