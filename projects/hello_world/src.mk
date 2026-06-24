
SRCS += $(DRIVERS)/api/no_os_uart.c	\
	$(DRIVERS)/api/no_os_spi.c	\
	$(DRIVERS)/api/no_os_gpio.c	\
	$(NO-OS)/util/no_os_fifo.c	\
	$(NO-OS)/util/no_os_list.c	\
	$(NO-OS)/util/no_os_util.c	\
	$(NO-OS)/util/no_os_alloc.c	\
	$(NO-OS)/util/no_os_mutex.c

SRCS += $(PLATFORM_DRIVERS)/xilinx_spi.c	\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c

INCS += $(INCLUDE)/no_os_delay.h	\
	$(INCLUDE)/no_os_error.h	\
	$(INCLUDE)/no_os_fifo.h		\
	$(INCLUDE)/no_os_irq.h		\
	$(INCLUDE)/no_os_lf256fifo.h	\
	$(INCLUDE)/no_os_list.h		\
	$(INCLUDE)/no_os_uart.h		\
	$(INCLUDE)/no_os_spi.h		\
	$(INCLUDE)/no_os_gpio.h		\
	$(INCLUDE)/no_os_util.h		\
	$(INCLUDE)/no_os_alloc.h	\
	$(INCLUDE)/no_os_mutex.h	\
	$(INCLUDE)/no_os_print_log.h

INCS += $(PLATFORM_DRIVERS)/xilinx_spi.h	\
	$(PLATFORM_DRIVERS)/xilinx_gpio.h
