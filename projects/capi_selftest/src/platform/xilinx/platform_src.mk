CAPI_XILINX_DRIVERS := $(NO-OS)/capi/platform/xilinx

NO_OS_INC_DIRS += \
	$(CAPI_XILINX_DRIVERS)

SRCS += $(CAPI_XILINX_DRIVERS)/xilinx_capi_uart_ps.c \
	$(CAPI_XILINX_DRIVERS)/xilinx_capi_irq_gic.c \
	$(CAPI_XILINX_DRIVERS)/xilinx_capi_gpio_ps.c
