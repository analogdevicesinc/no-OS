SRC_DIRS += $(PROJECT)/src/app
SRC_DIRS += $(PROJECT)/src/mux_board

SRCS +=	$(NO-OS)/util/util.c

INCS +=	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/i2c.h \
	$(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/util.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/timer.h \
	$(INCLUDE)/no-os/irq.h \
	$(INCLUDE)/no-os/rtc.h \
	$(INCLUDE)/no-os/gpio.h

SRCS += $(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/api/i2c.c \
	$(DRIVERS)/api/irq.c \
	$(DRIVERS)/afe/ad5940/ad5940.c

INCS += $(DRIVERS)/afe/ad5940/ad5940.h

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_gpio.c \
	$(PLATFORM_DRIVERS)/stm32_irq.c \
	$(PLATFORM_DRIVERS)/stm32_spi.c \
	$(PLATFORM_DRIVERS)/stm32_i2c.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

INCS +=	$(INCLUDE)/no-os/delay.h \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h \
	$(PLATFORM_DRIVERS)/stm32_spi.h \
	$(PLATFORM_DRIVERS)/stm32_i2c.h \
	$(PLATFORM_DRIVERS)/stm32_gpio.h \
	$(PLATFORM_DRIVERS)/stm32_irq.h \
	$(PLATFORM_DRIVERS)/stm32_hal.h

ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app
SRCS += $(DRIVERS)/afe/ad5940/iio_ad5940.c
INCS += $(DRIVERS)/afe/ad5940/iio_ad5940.h \
	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/irq.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no-os/list.h 
endif
