NO_OS_INC_DIRS += \
	$(INCLUDE)/capi

SRCS += $(DRIVERS)/api/capi/capi_uart.c \
	$(DRIVERS)/api/capi/capi_gpio.c \
	$(DRIVERS)/api/capi/capi_alloc.c \
	$(DRIVERS)/api/capi/capi_time.c \
	$(DRIVERS)/api/capi/capi_spi.c
