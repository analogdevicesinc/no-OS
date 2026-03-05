SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c	\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c	\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c	\
	$(DRIVERS)/api/no_os_irq.c

INCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h	\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h

SRCS += $(NO-OS)/util/no_os_lf256fifo.c
