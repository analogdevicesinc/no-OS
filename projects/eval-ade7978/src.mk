
NO_OS_INC_DIRS += \
	$(INCLUDE) \
    $(PLATFORM_DRIVERS) \
    $(PLATFORM_DRIVERS)/../common/ \
    $(PROJECT)/src/ \
    $(DRIVERS)/meter/ade7978

SRCS += $(PROJECT)/src/main.c \

SRCS +=	$(NO-OS)/util/no_os_alloc.c			\
	$(DRIVERS)/api/no_os_gpio.c 			\
	$(NO-OS)/util/no_os_mutex.c			\
	$(NO-OS)/util/no_os_lf256fifo.c			\
	$(NO-OS)/util/no_os_list.c			\
	$(NO-OS)/util/no_os_util.c			\
	$(NO-OS)/util/no_os_crc8.c 			\
	$(NO-OS)/util/no_os_crc16.c 			\
	$(DRIVERS)/api/no_os_irq.c			\
	$(DRIVERS)/api/no_os_uart.c			\
	$(DRIVERS)/api/no_os_pwm.c			\
	$(DRIVERS)/api/no_os_spi.c  			\
	$(DRIVERS)/api/no_os_timer.c  			\
	$(DRIVERS)/api/no_os_dma.c 			

SRCS +=	$(PLATFORM_DRIVERS)/maxim_irq.c			\
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.c  		\
	$(PLATFORM_DRIVERS)/maxim_delay.c     		\
	$(PLATFORM_DRIVERS)/maxim_init.c      		\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c		\
	$(PLATFORM_DRIVERS)/maxim_gpio.c      		\
	$(PLATFORM_DRIVERS)/maxim_spi.c       		\
	$(PLATFORM_DRIVERS)/maxim_uart.c		\
	$(PLATFORM_DRIVERS)/maxim_pwm.c

# Application entry point
SRCS += $(PROJECT)/src/main.c \
    $(PROJECT)/src/platform/platform.c \
    $(PROJECT)/src/common/common_data.c \
    $(PROJECT)/src/interrupt/interrupt.c \

# ADE7978 driver files
SRCS += $(DRIVERS)/meter/ade7978/ade7978.c
