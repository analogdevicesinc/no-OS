SRCS = $(PROJECT)/main.c \
       $(PROJECT)/baremetal.c

INCS = $(PROJECT)/baremetal.h

SRCS += $(PLATFORM_DRIVERS)/maxim_uart.c \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c \
	$(PLATFORM_DRIVERS)/maxim_irq.c \
	$(PLATFORM_DRIVERS)/maxim_delay.c
INCS += $(PLATFORM_DRIVERS)/maxim_uart.h \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h \
	$(PLATFORM_DRIVERS)/maxim_irq.h \
	$(PLATFORM_DRIVERS)/maxim_delay.h

SRCS += $(DRIVERS)/api/no_os_uart.c     \
	$(DRIVERS)/api/no_os_irq.c      \
        $(NO-OS)/util/no_os_fifo.c      \
	$(NO-OS)/util/no_os_lf256fifo.c \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c     \
        $(NO-OS)/util/no_os_mutex.c

INCS += $(INCLUDE)/no_os_delay.h     \
        $(INCLUDE)/no_os_error.h     \
        $(INCLUDE)/no_os_fifo.h      \
        $(INCLUDE)/no_os_irq.h       \
        $(INCLUDE)/no_os_lf256fifo.h \
        $(INCLUDE)/no_os_list.h      \
        $(INCLUDE)/no_os_timer.h     \
        $(INCLUDE)/no_os_uart.h      \
        $(INCLUDE)/no_os_util.h      \
        $(INCLUDE)/no_os_alloc.h     \
        $(INCLUDE)/no_os_mutex.h

INCS += /home/dari/Work/libiio/tinyiiod/tinyiiod.h \
	/home/dari/Work/libiio/include/iio/iio.h \
	/home/dari/Work/libiio/include/iio/iio-backend.h \
	/home/dari/Work/libiio/include/iio/iio-client.h \
	/home/dari/Work/libiio/include/iio/iio-lock.h \
	/home/dari/Work/libiio/include/iio/iio-debug.h



