INCS += $(PLATFORM_DRIVERS)/stm32_gpio.h      \
        $(PLATFORM_DRIVERS)/stm32_dma.h       \
	$(PLATFORM_DRIVERS)/stm32_hal.h       \
	$(PLATFORM_DRIVERS)/stm32_irq.h       \
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.h  \
	$(PLATFORM_DRIVERS)/stm32_uart.h      \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c     \
        $(PLATFORM_DRIVERS)/stm32_gpdma.c     \
	$(PLATFORM_DRIVERS)/stm32_gpio.c      \
	$(PLATFORM_DRIVERS)/stm32_irq.c       \
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.c  \
	$(PLATFORM_DRIVERS)/stm32_uart.c      \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

ifdef AD405X_SPI
INCS += $(PLATFORM_DRIVERS)/stm32_spi.h
SRCS += $(PLATFORM_DRIVERS)/stm32_spi.c
endif
ifdef AD405X_I3C
INCS += $(PLATFORM_DRIVERS)/stm32_i3c.h
SRCS += $(PLATFORM_DRIVERS)/stm32_i3c.c
endif
