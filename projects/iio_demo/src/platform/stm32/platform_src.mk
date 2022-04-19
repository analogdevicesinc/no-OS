SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_irq.c   \
	$(PLATFORM_DRIVERS)/stm32_uart.c  \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c
	
INCS += $(PLATFORM_DRIVERS)/stm32_irq.h        \
	$(PLATFORM_DRIVERS)/stm32_hal.h        \
	$(PLATFORM_DRIVERS)/stm32_uart.h       \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h
	
SRCS += $(DRIVERS)/api/no_os_irq.c

SRCS += $(NO-OS)/util/no_os_lf256fifo.c
