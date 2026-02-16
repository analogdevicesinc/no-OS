NO_OS_INC_DIRS += \
	$(INCLUDE) \
	$(DRIVERS)/mux/adg2404

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_dma.c  \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_list.c  \
	$(NO-OS)/util/no_os_lf256fifo.c

SRCS += $(DRIVERS)/mux/adg2404/adg2404.c
