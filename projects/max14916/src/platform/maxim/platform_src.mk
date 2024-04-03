INCS += $(PLATFORM_DRIVERS)/maxim_spi.h		\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h	\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h		\
	$(PLATFORM_DRIVERS)/maxim_irq.h


SRCS += $(PLATFORM_DRIVERS)/maxim_spi.c		\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c	\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c		\
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_delay.c	
