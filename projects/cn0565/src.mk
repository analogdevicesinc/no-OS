SRC_DIRS += $(PROJECT)/src/app
SRC_DIRS += $(PROJECT)/src/mux_board

INCS +=	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_timer.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_rtc.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_dma.h \

SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_timer.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(DRIVERS)/afe/ad5940/bia_measurement.c \
	$(DRIVERS)/afe/ad5940/ad5940.c

INCS += $(DRIVERS)/afe/ad5940/bia_measurement.h \
	$(DRIVERS)/afe/ad5940/ad5940.h

ifeq (stm32,$(strip $(PLATFORM)))
SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c \
	$(PLATFORM_DRIVERS)/stm32_gpio.c \
	$(PLATFORM_DRIVERS)/stm32_spi.c \
	$(PLATFORM_DRIVERS)/stm32_dma.c  \
	$(PLATFORM_DRIVERS)/stm32_i2c.c \
	$(PLATFORM_DRIVERS)/stm32_irq.c \
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.c \
	$(PLATFORM_DRIVERS)/stm32_uart.c \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

INCS +=	$(INCLUDE)/no_os_delay.h \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h \
	$(PLATFORM_DRIVERS)/stm32_uart.h \
	$(PLATFORM_DRIVERS)/stm32_irq.h \
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.h \
	$(PLATFORM_DRIVERS)/stm32_spi.h \
	$(PLATFORM_DRIVERS)/stm32_dma.h  \
	$(PLATFORM_DRIVERS)/stm32_i2c.h \
	$(PLATFORM_DRIVERS)/stm32_gpio.h \
	$(PLATFORM_DRIVERS)/stm32_hal.h
endif

ifeq (aducm3029,$(strip $(PLATFORM)))
SRCS += $(PLATFORM_DRIVERS)/aducm3029_delay.c \
	$(PLATFORM_DRIVERS)/aducm3029_gpio.c \
	$(PLATFORM_DRIVERS)/aducm3029_spi.c \
	$(PLATFORM_DRIVERS)/aducm3029_timer.c \
	$(PLATFORM_DRIVERS)/aducm3029_i2c.c \
	$(PLATFORM_DRIVERS)/aducm3029_irq.c \
	$(PLATFORM_DRIVERS)/aducm3029_gpio_irq.c \
	$(PLATFORM_DRIVERS)/aducm3029_rtc.c \
	$(PLATFORM_DRIVERS)/aducm3029_uart.c \
	$(PLATFORM_DRIVERS)/aducm3029_uart_stdio.c \
	$(PLATFORM_DRIVERS)/platform_init.c

INCS +=	$(INCLUDE)/no_os_delay.h \
	$(PLATFORM_DRIVERS)/aducm3029_uart_stdio.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(PLATFORM_DRIVERS)/aducm3029_irq.h \
	$(PLATFORM_DRIVERS)/aducm3029_gpio_irq.h \
	$(PLATFORM_DRIVERS)/aducm3029_spi.h \
	$(PLATFORM_DRIVERS)/aducm3029_timer.h \
	$(PLATFORM_DRIVERS)/aducm3029_i2c.h \
	$(PLATFORM_DRIVERS)/aducm3029_rtc.h \
	$(PLATFORM_DRIVERS)/aducm3029_spi.h \
	$(PLATFORM_DRIVERS)/aducm3029_uart.h \
	$(PLATFORM_DRIVERS)/aducm3029_gpio.h \
	$(PLATFORM_DRIVERS)/platform_init.h
endif

ifeq (y,$(strip $(IIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app
SRCS += $(DRIVERS)/afe/ad5940/iio_ad5940.c \
	$(DRIVERS)/switch/adg2128/iio_adg2128.c
INCS += $(DRIVERS)/afe/ad5940/iio_ad5940.h \
	$(DRIVERS)/switch/adg2128/iio_adg2128.h
endif
