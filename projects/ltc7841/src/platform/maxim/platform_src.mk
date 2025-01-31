NO_OS_INC_DIRS += \
        $(PLATFORM_DRIVERS) \
        $(PLATFORM_DRIVERS)/../common/

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
        $(PLATFORM_DRIVERS)/maxim_gpio.c      \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.c  \
        $(PLATFORM_DRIVERS)/maxim_irq.c       \
        $(PLATFORM_DRIVERS)/../common/maxim_dma.c       \
        $(PLATFORM_DRIVERS)/maxim_i2c.c       \
        $(PLATFORM_DRIVERS)/maxim_uart.c      \
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.c
