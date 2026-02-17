NO_OS_INC_DIRS += $(INCLUDE) \
	$(DRIVERS)/switch/adg1736

SRCS += $(DRIVERS)/api/no_os_gpio.c  \
	$(DRIVERS)/api/no_os_irq.c   \
	$(DRIVERS)/api/no_os_uart.c  \
	$(DRIVERS)/api/no_os_dma.c   \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_util.c   \
	$(NO-OS)/util/no_os_alloc.c  \
	$(NO-OS)/util/no_os_mutex.c  \
	$(NO-OS)/util/no_os_list.c

SRCS += $(DRIVERS)/switch/adg1736/adg1736.c
