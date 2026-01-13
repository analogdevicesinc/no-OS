INCS += $(PLATFORM_DRIVERS)/stm32_gpio.h      \
	$(PLATFORM_DRIVERS)/stm32_hal.h       \
	$(PLATFORM_DRIVERS)/stm32_uart.h      \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PLATFORM_DRIVERS)/stm32_i2c.h		  \
	$(PLATFORM_DRIVERS)/stm32_irq.h		  \

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c     \
	$(PLATFORM_DRIVERS)/stm32_gpio.c      \
	$(PLATFORM_DRIVERS)/stm32_uart.c      \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c \
	$(PLATFORM_DRIVERS)/stm32_i2c.c      \
	$(PLATFORM_DRIVERS)/stm32_irq.c      \
