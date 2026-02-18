NO_OS_INC_DIRS += \
		$(PLATFORM_DRIVERS) \
		$(PLATFORM_DRIVERS)/../common/

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
		$(PLATFORM_DRIVERS)/maxim_irq.c       \
		$(PLATFORM_DRIVERS)/maxim_spi.c       \
		$(PLATFORM_DRIVERS)/maxim_uart.c      \
		$(PLATFORM_DRIVERS)/maxim_uart_stdio.c \
		$(PLATFORM_DRIVERS)/../common/maxim_dma.c
