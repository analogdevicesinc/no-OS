NO_OS_INC_DIRS += \
	$(PLATFORM_DRIVERS)

SRCS += $(PLATFORM_DRIVERS)/xilinx_capi_uart_ps.c \
	$(PLATFORM_DRIVERS)/xilinx_capi_irq_gic.c \
	$(PLATFORM_DRIVERS)/xilinx_capi_gpio_ps.c
