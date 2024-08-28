
SRCS += $(PROJECT)/src/main.c

INCS += $(INCLUDE)/no_os_alloc.h		\
	$(INCLUDE)/no_os_delay.h     \
	$(INCLUDE)/no_os_error.h		\
	$(INCLUDE)/no_os_gpio.h      \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_spi.h       \
	$(INCLUDE)/no_os_timer.h      \
	$(INCLUDE)/no_os_mutex.h		\
	$(INCLUDE)/no_os_util.h			\
	$(INCLUDE)/no_os_lf256fifo.h		\
	$(INCLUDE)/no_os_list.h			\
	$(INCLUDE)/no_os_irq.h			\
	$(INCLUDE)/no_os_units.h \
	$(INCLUDE)/no_os_init.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_crc8.h \
	$(INCLUDE)/no_os_crc16.h \
	$(INCLUDE)/no_os_uart.h			\
	$(INCLUDE)/no_os_pwm.h			\
	$(INCLUDE)/no_os_dma.h \
	$(PLATFORM_DRIVERS)/maxim_irq.h		\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h	\
	$(PLATFORM_DRIVERS)/maxim_gpio.h      \
    $(PLATFORM_DRIVERS)/maxim_spi.h       \
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.h  \
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h       \
	$(PLATFORM_DRIVERS)/maxim_pwm.h

SRCS +=	$(NO-OS)/util/no_os_alloc.c		\
	$(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_mutex.c		\
	$(NO-OS)/util/no_os_lf256fifo.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_util.c		\
	$(NO-OS)/util/no_os_crc8.c \
	$(NO-OS)/util/no_os_crc16.c \
	$(DRIVERS)/api/no_os_irq.c		\
	$(DRIVERS)/api/no_os_uart.c		\
	$(DRIVERS)/api/no_os_pwm.c		\
	$(DRIVERS)/api/no_os_spi.c  \
	$(DRIVERS)/api/no_os_timer.c  \
	$(DRIVERS)/api/no_os_dma.c \
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.c  \
	$(PLATFORM_DRIVERS)/maxim_delay.c     \
	$(PLATFORM_DRIVERS)/maxim_init.c      \
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c	\
	$(PLATFORM_DRIVERS)/maxim_gpio.c      \
	$(PLATFORM_DRIVERS)/maxim_spi.c       \
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c       \
	$(PLATFORM_DRIVERS)/maxim_pwm.c

# Application entry point
SRCS += $(PROJECT)/src/main.c
INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

# ADE9153A driver files
INCS += $(DRIVERS)/meter/ade9153a/ade9153a.h
SRCS += $(DRIVERS)/meter/ade9153a/ade9153a.c