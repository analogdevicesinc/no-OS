NO_OS_INC_DIRS += \
		$(INCLUDE) \
		$(DRIVERS)/switch/adg1712

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c \
		$(NO-OS)/util/no_os_list.c \
		$(DRIVERS)/api/no_os_uart.c \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_alloc.c \
		$(NO-OS)/util/no_os_mutex.c

SRCS += $(DRIVERS)/switch/adg1712/adg1712.c
