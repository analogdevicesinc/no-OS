INCS += $(PLATFORM_DRIVERS)/pico_gpio.h     \
        $(PLATFORM_DRIVERS)/pico_gpio_irq.h \
        $(PLATFORM_DRIVERS)/pico_irq.h      \
        $(PLATFORM_DRIVERS)/pico_spi.h      \
        $(PLATFORM_DRIVERS)/pico_timer.h    \
        $(PLATFORM_DRIVERS)/pico_uart.h

SRCS += $(PLATFORM_DRIVERS)/pico_delay.c    \
        $(PLATFORM_DRIVERS)/pico_gpio.c     \
        $(PLATFORM_DRIVERS)/pico_gpio_irq.c \
        $(PLATFORM_DRIVERS)/pico_irq.c      \
        $(PLATFORM_DRIVERS)/pico_spi.c      \
        $(PLATFORM_DRIVERS)/pico_timer.c    \
        $(PLATFORM_DRIVERS)/pico_uart.c
