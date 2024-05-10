SRCS += $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_irq.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c
	
ifeq (y,$(strip $(IIOD)))
SRCS += $(PLATFORM_DRIVERS)/xilinx_uart.c
endif
	
INCS += $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h \
	$(PLATFORM_DRIVERS)/xilinx_spi.h \
	$(PLATFORM_DRIVERS)/xilinx_irq.h \
	$(PLATFORM_DRIVERS)/xilinx_gpio.h
	
ifeq (y,$(strip $(IIOD)))
INCS += $(PLATFORM_DRIVERS)/xilinx_uart.h
endif	
	
	
	
	
