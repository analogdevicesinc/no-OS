INCS += $(PLATFORM_DRIVERS)/xilinx_gpio.h \
	$(PLATFORM_DRIVERS)/xilinx_spi.h \
	$(PLATFORM_DRIVERS)/xilinx_uart.h \
	$(PLATFORM_DRIVERS)/xilinx_irq.h

SRCS += $(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_uart.c \
        $(PLATFORM_DRIVERS)/xilinx_irq.c