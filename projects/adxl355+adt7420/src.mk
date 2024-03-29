
SRCS += $(PROJECT)/src/example.c

INCS += $(PROJECT)/src/parameters.h

INCS += $(INCLUDE)/no_os_alloc.h		\
	$(INCLUDE)/no_os_error.h		\
	$(INCLUDE)/no_os_mutex.h		\
	$(INCLUDE)/no_os_util.h			\
	$(INCLUDE)/no_os_lf256fifo.h		\
	$(INCLUDE)/no_os_list.h			\
	$(INCLUDE)/no_os_irq.h			\
	$(INCLUDE)/no_os_uart.h			\
	$(INCLUDE)/no_os_gpio.h			\
	$(INCLUDE)/no_os_delay.h		\
	$(INCLUDE)/no_os_print_log.h		\
	$(INCLUDE)/no_os_units.h		\
	$(INCLUDE)/no_os_dma.h			\
	$(INCLUDE)/no_os_i2c.h			\
	$(INCLUDE)/no_os_spi.h			\
	$(INCLUDE)/no_os_init.h			\
	$(INCLUDE)/no_os_timer.h		\
	$(PLATFORM_DRIVERS)/maxim_dma.h		\
	$(PLATFORM_DRIVERS)/maxim_irq.h		\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h	\
	$(PLATFORM_DRIVERS)/maxim_gpio.h	\
	$(PLATFORM_DRIVERS)/maxim_i2c.h		\
	$(PLATFORM_DRIVERS)/maxim_spi.h		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.h

SRCS +=	$(NO-OS)/util/no_os_alloc.c		\
	$(NO-OS)/util/no_os_mutex.c		\
	$(NO-OS)/util/no_os_lf256fifo.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_util.c		\
	$(DRIVERS)/api/no_os_irq.c		\
	$(DRIVERS)/api/no_os_dma.c		\
	$(DRIVERS)/api/no_os_uart.c		\
	$(DRIVERS)/api/no_os_gpio.c		\
	$(DRIVERS)/api/no_os_i2c.c		\
	$(DRIVERS)/api/no_os_spi.c		\
	$(DRIVERS)/api/no_os_timer.c		\
	$(PLATFORM_DRIVERS)/maxim_dma.c		\
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c	\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/maxim_gpio.c	\
        $(PLATFORM_DRIVERS)/maxim_init.c	\
	$(PLATFORM_DRIVERS)/maxim_delay.c	\
	$(PLATFORM_DRIVERS)/maxim_i2c.c		\
	$(PLATFORM_DRIVERS)/maxim_spi.c		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.c

IIOD = y
CFLAGS += -DIIO_LWIP_EXAMPLE=1
CFLAGS += -DNO_OS_LWIP_NETWORKING
LIBRARIES += lwip

CFLAGS += -DNO_OS_STATIC_IP=y

INCS += $(DRIVERS)/accel/adxl355/adxl355.h
SRCS += $(DRIVERS)/accel/adxl355/adxl355.c

INCS += $(DRIVERS)/accel/adxl355/iio_adxl355.h
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355.c

INCS += $(DRIVERS)/temperature/adt7420/adt7420.h
SRCS += $(DRIVERS)/temperature/adt7420/adt7420.c

INCS += $(DRIVERS)/temperature/adt7420/iio_adt7420.h
SRCS += $(DRIVERS)/temperature/adt7420/iio_adt7420.c

INCS += $(NO-OS)/iio/iio_app/iio_app.h
SRCS += $(NO-OS)/iio/iio_app/iio_app.c

INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c

SRC_DIRS += $(NO-OS)/network

DISABLE_SECURE_SOCKET=y

LIBRARIES += mqtt
