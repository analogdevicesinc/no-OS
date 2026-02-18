NO_OS_INC_DIRS += \
		$(INCLUDE) \
		$(PROJECT)/src/ \
		$(DRIVERS)/api/

INCS += $(DRIVERS)/switch/adgs6414d/adgs6414d.h \
		$(INCLUDE)/no_os_delay.h

SRCS += $(DRIVERS)/api/no_os_spi.c \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c \
		$(DRIVERS)/api/no_os_dma.c \
		$(DRIVERS)/api/no_os_timer.c \
		$(NO-OS)/util/no_os_list.c \
		$(DRIVERS)/api/no_os_uart.c \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_alloc.c \
		$(NO-OS)/util/no_os_mutex.c \
		$(NO-OS)/util/no_os_crc8.c

SRCS += $(DRIVERS)/switch/adgs6414d/adgs6414d.c
