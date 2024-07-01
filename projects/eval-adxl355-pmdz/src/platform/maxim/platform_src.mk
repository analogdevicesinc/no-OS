INCS += $(PLATFORM_DRIVERS)/maxim_gpio.h      \
        $(PLATFORM_DRIVERS)/maxim_spi.h       \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.h  \
        $(PLATFORM_DRIVERS)/maxim_irq.h       \
        $(PLATFORM_DRIVERS)/maxim_uart.h      \
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
        $(PLATFORM_DRIVERS)/maxim_gpio.c      \
        $(PLATFORM_DRIVERS)/maxim_spi.c       \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.c  \
        $(PLATFORM_DRIVERS)/maxim_irq.c       \
        $(PLATFORM_DRIVERS)/maxim_uart.c      \
        $(PLATFORM_DRIVERS)/maxim_init.c      \
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.c
