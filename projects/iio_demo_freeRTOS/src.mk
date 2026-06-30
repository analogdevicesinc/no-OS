SRCS += $(DRIVERS)/api/no_os_uart.c     \
    $(NO-OS)/util/no_os_fifo.c      \
    $(NO-OS)/util/no_os_list.c      \
    $(NO-OS)/util/no_os_util.c      \
    $(NO-OS)/util/no_os_alloc.c

INCS += $(INCLUDE)/no_os_delay.h     \
    $(INCLUDE)/no_os_error.h     \
    $(INCLUDE)/no_os_fifo.h      \
    $(INCLUDE)/no_os_irq.h       \
    $(INCLUDE)/no_os_lf256fifo.h \
    $(INCLUDE)/no_os_list.h      \
    $(INCLUDE)/no_os_timer.h     \
    $(INCLUDE)/no_os_uart.h      \
    $(INCLUDE)/no_os_gpio.h      \
    $(INCLUDE)/no_os_util.h      \
    $(INCLUDE)/no_os_alloc.h     \
    $(INCLUDE)/no_os_mutex.h     \
    $(INCLUDE)/no_os_semaphore.h

INCS += $(DRIVERS)/adc/adc_demo/adc_demo.h \
    $(DRIVERS)/dac/dac_demo/dac_demo.h

SRCS += $(DRIVERS)/adc/adc_demo/adc_demo.c \
    $(DRIVERS)/dac/dac_demo/dac_demo.c

# Platform specific, free-rtos config file
FREERTOS_CONFIG_PATH := $(PROJECT)/src/platform/$(PLATFORM)/FreeRTOSConfig.h
