INCS += $(INCLUDE)/no_os_rtc.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c	\
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_rtc.c		\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/maxim_stdio.c
	
INCS += $(PLATFORM_DRIVERS)/irq_extra.h		\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/rtc_extra.h		\
	$(PLATFORM_DRIVERS)/maxim_stdio.h
	
SRCS += $(DRIVERS)/api/no_os_irq.c

SRCS += $(NO-OS)/util/no_os_lf256fifo.c
