
SRCS += $(PROJECT)/src/main.c
INCS += $(PROJECT)/src/app_config.h

SRCS += $(DRIVERS)/rf-transceiver/ad9361/ad9361_api.c \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361.c \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_conv.c \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_util.c

INCS += $(DRIVERS)/rf-transceiver/ad9361/ad9361_api.h \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361.h \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_util.h \
	$(DRIVERS)/rf-transceiver/ad9361/common.h

INCS +=	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_spi.h	\
	$(INCLUDE)/no_os_uart.h	\
	$(INCLUDE)/no_os_irq.h	\
	$(INCLUDE)/no_os_list.h	\
	$(INCLUDE)/no_os_dma.h	\
	$(INCLUDE)/no_os_mutex.h	\
	$(INCLUDE)/no_os_print_log.h	\
	$(INCLUDE)/no_os_units.h \
	$(INCLUDE)/no_os_lf256fifo.h

SRCS += $(NO-OS)/util/no_os_alloc.c	\
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_gpio.c	\
	$(NO-OS)/util/no_os_util.c	\
	$(NO-OS)/util/no_os_mutex.c	\
	$(DRIVERS)/api/no_os_dma.c	\
	$(DRIVERS)/api/no_os_irq.c	\
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_lf256fifo.c
	
SRCS += $(PLATFORM_DRIVERS)/maxim_gpio.c	\
	$(PLATFORM_DRIVERS)/maxim_spi.c	\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c \
	$(PLATFORM_DRIVERS)/maxim_delay.c \
	$(PLATFORM_DRIVERS)/maxim_irq.c \

INCS += $(PLATFORM_DRIVERS)/maxim_gpio.h	\
	$(PLATFORM_DRIVERS)/maxim_irq.h	\
	$(PLATFORM_DRIVERS)/maxim_spi.h	\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_irq.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h	\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h