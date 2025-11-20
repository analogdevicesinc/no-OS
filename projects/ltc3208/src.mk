INCS += $(INCLUDE)/no_os_error.h	\
		$(INCLUDE)/no_os_gpio.h		\
		$(INCLUDE)/no_os_irq.h		\
		$(INCLUDE)/no_os_list.h		\
		$(INCLUDE)/no_os_uart.h		\
		$(INCLUDE)/no_os_lf256fifo.h\
		$(INCLUDE)/no_os_dma.h		\
		$(INCLUDE)/no_os_init.h		\
		$(INCLUDE)/no_os_util.h		\
		$(INCLUDE)/no_os_i2c.h		\
		$(INCLUDE)/no_os_mutex.h	\
		$(INCLUDE)/no_os_print_log.h \
		$(INCLUDE)/no_os_alloc.h	\
		$(INCLUDE)/no_os_delay.h

SRCS += $(DRIVERS)/api/no_os_gpio.c		\
		$(DRIVERS)/api/no_os_irq.c		\
		$(NO-OS)/util/no_os_list.c		\
		$(DRIVERS)/api/no_os_uart.c		\
		$(NO-OS)/util/no_os_lf256fifo.c	\
		$(DRIVERS)/api/no_os_dma.c		\
		$(DRIVERS)/api/no_os_i2c.c		\
		$(NO-OS)/util/no_os_util.c		\
		$(NO-OS)/util/no_os_mutex.c		\
		$(NO-OS)/util/no_os_alloc.c


INCS += $(DRIVERS)/led/ltc3208/ltc3208.h
SRCS += $(DRIVERS)/led/ltc3208/ltc3208.c
