SRCS += $(PLATFORM_DRIVERS)/aducm3029_delay.c \
	$(PLATFORM_DRIVERS)/aducm3029_gpio.c \
	$(PLATFORM_DRIVERS)/aducm3029_spi.c \
	$(PLATFORM_DRIVERS)/aducm3029_timer.c \
	$(PLATFORM_DRIVERS)/aducm3029_i2c.c \
	$(PLATFORM_DRIVERS)/aducm3029_irq.c \
	$(PLATFORM_DRIVERS)/aducm3029_gpio_irq.c \
	$(PLATFORM_DRIVERS)/aducm3029_rtc.c \
	$(PLATFORM_DRIVERS)/aducm3029_uart.c \
	$(PLATFORM_DRIVERS)/aducm3029_uart_stdio.c \
	$(PLATFORM_DRIVERS)/platform_init.c

INCS +=	$(INCLUDE)/no_os_delay.h \
	$(PLATFORM_DRIVERS)/aducm3029_uart_stdio.h \
	$(PLATFORM_DRIVERS)/aducm3029_uart.h \
	$(PLATFORM_DRIVERS)/aducm3029_irq.h \
	$(PLATFORM_DRIVERS)/aducm3029_gpio_irq.h \
	$(PLATFORM_DRIVERS)/aducm3029_spi.h \
	$(PLATFORM_DRIVERS)/aducm3029_timer.h \
	$(PLATFORM_DRIVERS)/aducm3029_i2c.h \
	$(PLATFORM_DRIVERS)/aducm3029_rtc.h \
	$(PLATFORM_DRIVERS)/aducm3029_gpio.h \
	$(PLATFORM_DRIVERS)/platform_init.h
