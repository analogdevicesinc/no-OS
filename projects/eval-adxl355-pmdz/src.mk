SRC_DIRS += $(PROJECT)/src

ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app
endif

INCS +=	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_timer.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_rtc.h \
    $(INCLUDE)/no_os_util.h \
    $(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_gpio.h

INCS += $(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h \
	$(PLATFORM_DRIVERS)/stm32_delay.h \
	$(PLATFORM_DRIVERS)/stm32_spi.h \
	$(PLATFORM_DRIVERS)/stm32_gpio.h \
	$(PLATFORM_DRIVERS)/stm32_hal.h

INCS += $(DRIVERS)/accel/adxl355/adxl355.h

ifeq (y,$(strip $(TINYIIOD)))
INCS += $(DRIVERS)/accel/adxl355/iio_adxl355.h

INCS += $(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/uart_extra.h

INCS += $(PROJECT)/src/app_config.h \
	$(PROJECT)/src/parameters.h
endif

SRCS += $(NO-OS)/util/no_os_util.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_gpio.c

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_gpio.c \
	$(PLATFORM_DRIVERS)/stm32_spi.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

SRCS += $(DRIVERS)/accel/adxl355/adxl355.c

ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355.c

endif
