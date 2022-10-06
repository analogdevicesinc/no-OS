SRCS += $(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_i2c.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c

INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(PLATFORM_DRIVERS)/xilinx_i2c.h

ifeq (y,$(strip $(IIO_EXAMPLE)))

INCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(PLATFORM_DRIVERS)/irq_extra.h

SRCS += $(PLATFORM_DRIVERS)/xilinx_irq.c
endif
