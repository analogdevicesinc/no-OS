################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

SRCS := $(PROJECT)/src/ad7091r5_sdz.c
SRCS += $(DRIVERS)/adc/ad7091r5/ad7091r5.c				\
	$(DRIVERS)/i2c/i2c.c						\
	$(NO-OS)/util/list.c						\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_i2c.c				\
	$(PLATFORM_DRIVERS)/gpio.c					\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/delay.c
INCS +=	$(PLATFORM_DRIVERS)/i2c_extra.h					\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h				\
	$(PLATFORM_DRIVERS)/gpio_extra.h				\
	$(DRIVERS)/adc/ad7091r5/ad7091r5.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/i2c.h						\
	$(INCLUDE)/list.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/util.h
