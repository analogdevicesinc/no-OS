CAPI_STM32_DRIVERS := $(NO-OS)/capi/platform/stm32

NO_OS_INC_DIRS += \
	$(CAPI_STM32_DRIVERS) \
	$(NO-OS)/drivers/platform/stm32 \
	$(NO-OS)/include

SRCS += $(CAPI_STM32_DRIVERS)/stm32_capi_uart.c \
	$(CAPI_STM32_DRIVERS)/stm32_capi_gpio.c \
	$(CAPI_STM32_DRIVERS)/stm32_capi_alloc.c \
	$(CAPI_STM32_DRIVERS)/stm32_capi_time.c
