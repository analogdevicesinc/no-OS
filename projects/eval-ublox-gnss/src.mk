NO_OS_INC_DIRS += \
		$(INCLUDE) 			\
		$(PLATFORM_DRIVERS) 		\
		$(PLATFORM_DRIVERS)/../common/ 	\
		$(PROJECT)/src/ 		\
		$(DRIVERS)/platform/generic/ 		\
		$(DRIVERS)/gnss-gps/nmea_ubx

SRCS += $(PROJECT)/src/main.c

SRCS +=	$(NO-OS)/util/no_os_alloc.c		\
	$(DRIVERS)/api/no_os_gpio.c 		\
	$(NO-OS)/util/no_os_mutex.c		\
	$(NO-OS)/util/no_os_lf256fifo.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_util.c		\
	$(DRIVERS)/api/no_os_irq.c		\
	$(DRIVERS)/api/no_os_uart.c		\
	$(DRIVERS)/api/no_os_timer.c  		\
	$(DRIVERS)/api/no_os_dma.c		\
	$(DRIVERS)/api/no_os_gnss.c

SRCS +=	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_gpio_irq.c  	\
	$(PLATFORM_DRIVERS)/maxim_delay.c     	\
	$(PLATFORM_DRIVERS)/maxim_init.c      	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c	\
	$(PLATFORM_DRIVERS)/maxim_gpio.c      	\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c

# Application entry point
SRCS += $(PROJECT)/src/main.c 			\
	$(PROJECT)/src/interrupt/interrupt.c 	\
	$(PROJECT)/src/platform/platform.c

# u-blox GNSS driver files
SRCS += $(DRIVERS)/gnss-gps/nmea_ubx/nmea_ubx.c

# GNSS platform driver
SRCS += $(DRIVERS)/platform/generic/nmea_ubx_gnss.c