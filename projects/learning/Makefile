
PLATFORM = maxim
TARGET = max32655


include ../../tools/scripts/generic_variables.mk
include ../../tools/scripts/generic.mk


SRCS += $(PROJECT)/main.c

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c	\
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/maxim_gpio.c	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c

INCS += $(PLATFORM_DRIVERS)/maxim_irq.h		\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_gpio.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h	\
	$(DRIVERS)/platform/maxim/common/maxim_dma.h

SRCS += $(DRIVERS)/api/no_os_uart.c     \
        $(DRIVERS)/api/no_os_gpio.c     \
        $(DRIVERS)/api/no_os_irq.c      \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c     \
        $(NO-OS)/util/no_os_mutex.c     \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_lf256fifo.c

INCS += $(INCLUDE)/no_os_delay.h     \
        $(INCLUDE)/no_os_error.h     \
        $(INCLUDE)/no_os_irq.h       \
        $(INCLUDE)/no_os_uart.h      \
        $(INCLUDE)/no_os_gpio.h      \
        $(INCLUDE)/no_os_util.h      \
        $(INCLUDE)/no_os_alloc.h     \
        $(INCLUDE)/no_os_mutex.h     \
        $(INCLUDE)/no_os_list.h      \
        $(INCLUDE)/no_os_print_log.h

        