SRC_DIRS += $(PROJECT)/src

SRCS +=	$(NO-OS)/util/util.c

INCS +=	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/i2c.h \
	$(INCLUDE)/no-os/util.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/timer.h \
	$(INCLUDE)/no-os/irq.h \
	$(INCLUDE)/no-os/rtc.h \
    $(INCLUDE)/no-os/util.h \
    $(INCLUDE)/no-os/print_log.h \
	$(INCLUDE)/no-os/gpio.h

SRCS += $(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/i2c.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/accel/adxl355/adxl355.c

INCS += $(DRIVERS)/accel/adxl355/adxl355.h

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_gpio.c \
	$(PLATFORM_DRIVERS)/stm32_spi.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

INCS +=	$(INCLUDE)/no-os/delay.h \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h \
	$(PLATFORM_DRIVERS)/stm32_spi.h \
	$(PLATFORM_DRIVERS)/stm32_gpio.h \
	$(PLATFORM_DRIVERS)/stm32_hal.h
