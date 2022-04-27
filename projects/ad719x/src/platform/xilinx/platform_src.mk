SRCS += $(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_irq.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio_irq.c \
	$(PLATFORM_DRIVERS)/delay.c

INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h \
	$(PLATFORM_DRIVERS)/gpio_irq_extra.h
