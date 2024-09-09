SRCS += $(PROJECT)/src/main.c

SRCS += $(DRIVERS)/api/no_os_uart.c     \
        $(NO-OS)/util/no_os_fifo.c      \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c     \
        $(NO-OS)/util/no_os_mutex.c	\
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_irq.c  	\
	$(DRIVERS)/api/no_os_gpio.c

INCS += $(INCLUDE)/no_os_delay.h     \
        $(INCLUDE)/no_os_error.h     \
        $(INCLUDE)/no_os_fifo.h      \
        $(INCLUDE)/no_os_irq.h       \
        $(INCLUDE)/no_os_lf256fifo.h \
        $(INCLUDE)/no_os_list.h      \
        $(INCLUDE)/no_os_dma.h       \
        $(INCLUDE)/no_os_timer.h     \
        $(INCLUDE)/no_os_uart.h      \
        $(INCLUDE)/no_os_util.h      \
        $(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_gpio.h      \
        $(INCLUDE)/no_os_mutex.h

INCS += $(PLATFORM_DRIVERS)/maxim_gpio.h		\
	$(PLATFORM_DRIVERS)/maxim_irq.h 		\
	$(PLATFORM_DRIVERS)/maxim_uart.h 		\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c		\
	$(PLATFORM_DRIVERS)/maxim_gpio.c		\
	$(PLATFORM_DRIVERS)/maxim_irq.c			\
	$(PLATFORM_DRIVERS)/maxim_uart.c		\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c



