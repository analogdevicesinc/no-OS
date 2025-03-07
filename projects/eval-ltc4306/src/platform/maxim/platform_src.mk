INCS += $(INCLUDE)/no_os_rtc.h

INCS += $(PLATFORM_DRIVERS)/maxim_i2c.h       \
		$(PLATFORM_DRIVERS)/maxim_irq.h      \
		$(PLATFORM_DRIVERS)/maxim_rtc.h      \
		$(PLATFORM_DRIVERS)/../common/maxim_dma.h      \
		$(PLATFORM_DRIVERS)/maxim_uart.h      \
		$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
		$(PLATFORM_DRIVERS)/maxim_i2c.c       \
		$(PLATFORM_DRIVERS)/maxim_rtc.c       \
		$(PLATFORM_DRIVERS)/maxim_irq.c      \
		$(PLATFORM_DRIVERS)/../common/maxim_dma.c      \
		$(PLATFORM_DRIVERS)/maxim_init.c      \
		$(PLATFORM_DRIVERS)/maxim_uart.c      \
		$(PLATFORM_DRIVERS)/maxim_uart_stdio.c
