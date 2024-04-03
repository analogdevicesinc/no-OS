SRCS += $(PROJECT)/src/app/main.c

INCS += $(PROJECT)/src/app/parameters.h

INCS += $(INCLUDE)/no_os_delay.h     \
	$(INCLUDE)/no_os_error.h     \
	$(INCLUDE)/no_os_gpio.h      \
	$(INCLUDE)/no_os_i2c.h       \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_spi.h       \
	$(INCLUDE)/no_os_irq.h      \
	$(INCLUDE)/no_os_dma.h      \
	$(INCLUDE)/no_os_list.h      \
	$(INCLUDE)/no_os_timer.h      \
	$(INCLUDE)/no_os_uart.h      \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_units.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_circular_buffer.h \
	$(INCLUDE)/no_os_trng.h \
	$(INCLUDE)/no_os_rtc.h \
	$(DRIVERS)/rtc/pcf85263/pcf85263.h \
	$(DRIVERS)/meter/ade9430/ade9430.h \
	$(DRIVERS)/display/nhd_c12832a1z/nhd_c12832a1z.h \

INCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_delay.h     \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h      \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_i2c.h       \
        $(PLATFORM_DRIVERS)/../common/$(PLATFORM)_dma.h       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio_irq.h  \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_rtc.h       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h      \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_timer.h     \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_trng.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/meter/ade9430/ade9430.c \
	$(DRIVERS)/rtc/pcf85263/pcf85263.c \
	$(DRIVERS)/api/no_os_i2c.c  \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_gpio.c  \
	$(DRIVERS)/api/no_os_irq.c  \
	$(DRIVERS)/api/no_os_spi.c  \
	$(DRIVERS)/api/no_os_dma.c  \
	$(DRIVERS)/api/no_os_uart.c  \
	$(DRIVERS)/api/no_os_timer.c  \
	$(DRIVERS)/api/no_os_trng.c  \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_util.c	\
	$(NO-OS)/util/no_os_circular_buffer.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(DRIVERS)/display/nhd_c12832a1z/nhd_c12832a1z.c

SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c     \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c      \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c       \
        $(PLATFORM_DRIVERS)/../common/$(PLATFORM)_dma.c       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_i2c.c       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_rtc.c       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio_irq.c  \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c       \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c      \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_timer.c      \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_trng.c

SRC_DIRS += $(NO-OS)/network

LIBRARIES += mqtt

ifeq (y,$(strip $(AZURE_IOT_HUB)))
INCS += $(PROJECT)/src/app/iot_sample_common.h
SRCS += $(PROJECT)/src/app/iot_sample_common.c

MBED_TLS_CONFIG_FILE = $(PROJECT)/src/app/noos_mbedtls_config.h

LIBRARIES += mbedtls
LIBRARIES += azure-sdk-for-c
endif
