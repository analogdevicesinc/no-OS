
SRCS += $(PROJECT)/src/main.c
INCS += $(PROJECT)/src/parameters.h

INCS += $(PLATFORM_DRIVERS)/stm32_delay.h	\
	$(PLATFORM_DRIVERS)/stm32_gpio.h	\
	$(PLATFORM_DRIVERS)/stm32_hal.h		\
	$(PLATFORM_DRIVERS)/stm32_i2c.h		\
	$(PLATFORM_DRIVERS)/stm32_spi.h		\
        $(PLATFORM_DRIVERS)/stm32_pwm.h		\
	$(PLATFORM_DRIVERS)/stm32_timer.h	\
	$(PLATFORM_DRIVERS)/stm32_dma.h		\
	$(PLATFORM_DRIVERS)/stm32_irq.h		\
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.h	\
	$(PLATFORM_DRIVERS)/stm32_uart.h	\
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h  \
        $(PLATFORM_DRIVERS)/stm32_usb_uart.h

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c	\
	$(PLATFORM_DRIVERS)/stm32_gpio.c	\
	$(PLATFORM_DRIVERS)/stm32_i2c.c		\
	$(PLATFORM_DRIVERS)/stm32_spi.c		\
	$(PLATFORM_DRIVERS)/stm32_timer.c	\
	$(PLATFORM_DRIVERS)/stm32_dma.c		\
	$(PLATFORM_DRIVERS)/stm32_irq.c		\
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.c	\
	$(PLATFORM_DRIVERS)/stm32_uart.c	\
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

INCS += $(INCLUDE)/no_os_delay.h		\
	$(INCLUDE)/no_os_error.h		\
	$(INCLUDE)/no_os_gpio.h			\
	$(INCLUDE)/no_os_i2c.h			\
	$(INCLUDE)/no_os_print_log.h		\
	$(INCLUDE)/no_os_spi.h			\
        $(INCLUDE)/no_os_pwm.h			\
	$(INCLUDE)/no_os_irq.h			\
	$(INCLUDE)/no_os_list.h			\
	$(INCLUDE)/no_os_dma.h			\
	$(INCLUDE)/no_os_timer.h		\
	$(INCLUDE)/no_os_uart.h			\
	$(INCLUDE)/no_os_lf256fifo.h		\
	$(INCLUDE)/no_os_util.h			\
	$(INCLUDE)/no_os_units.h		\
	$(INCLUDE)/no_os_init.h			\
	$(INCLUDE)/no_os_alloc.h		\
        $(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c		\
	$(DRIVERS)/api/no_os_i2c.c		\
	$(NO-OS)/util/no_os_lf256fifo.c		\
	$(DRIVERS)/api/no_os_irq.c		\
	$(DRIVERS)/api/no_os_spi.c		\
	$(DRIVERS)/api/no_os_pwm.c		\
	$(DRIVERS)/api/no_os_timer.c		\
	$(DRIVERS)/api/no_os_uart.c		\
	$(DRIVERS)/api/no_os_dma.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_util.c		\
	$(NO-OS)/util/no_os_alloc.c		\
        $(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/accel/adxl38x/adxl38x.h
SRCS += $(DRIVERS)/accel/adxl38x/adxl38x.c

INCS += $(DRIVERS)/accel/adxl38x/iio_adxl38x.h
SRCS += $(DRIVERS)/accel/adxl38x/iio_adxl38x.c

IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app
