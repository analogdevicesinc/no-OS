NO_OS_INC_DIRS += \
	$(NO-OS)/capi/inc

SRCS += $(NO-OS)/capi/src/capi_uart.c \
	$(NO-OS)/capi/src/capi_alloc.c \
	$(NO-OS)/capi/src/capi_time.c \
	$(NO-OS)/capi/src/capi_gpio.c \
	$(NO-OS)/capi/src/capi_irq.c \
	$(NO-OS)/capi/src/capi_spi.c \
	$(NO-OS)/capi/src/capi_timer.c \
	$(NO-OS)/capi/src/capi_i2c.c \
	$(NO-OS)/capi/src/capi_dma.c
