SRC_DIRS += $(PROJECT)/src

SRCS +=	$(NO-OS)/util/no_os_util.c

INCS +=	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_timer.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_rtc.h \
    $(INCLUDE)/no_os_util.h \
    $(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_gpio.h

SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/accel/adxl355/adxl355.c

INCS += $(DRIVERS)/accel/adxl355/adxl355.h

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_gpio.c \
	$(PLATFORM_DRIVERS)/stm32_spi.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

INCS +=	$(INCLUDE)/no_os_delay.h \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h \
	$(PLATFORM_DRIVERS)/stm32_spi.h \
	$(PLATFORM_DRIVERS)/stm32_gpio.h \
	$(PLATFORM_DRIVERS)/stm32_hal.h
