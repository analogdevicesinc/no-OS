INCS += $(PLATFORM_DRIVERS)/maxim_gpio.h \
	$(PLATFORM_DRIVERS)/maxim_uart.h \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_gpio.c \
	$(PLATFORM_DRIVERS)/maxim_delay.c \
	$(PLATFORM_DRIVERS)/maxim_uart.c \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c
