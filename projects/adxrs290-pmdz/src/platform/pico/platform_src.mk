INCS += $(PLATFORM_DRIVERS)/pico_uart.h  \
        $(PLATFORM_DRIVERS)/pico_gpio.h  \
        $(PLATFORM_DRIVERS)/pico_spi.h   \
        $(PLATFORM_DRIVERS)/pico_irq.h

SRCS += $(PLATFORM_DRIVERS)/pico_uart.c  \
        $(PLATFORM_DRIVERS)/pico_spi.c   \
        $(PLATFORM_DRIVERS)/pico_gpio.c  \
        $(PLATFORM_DRIVERS)/pico_delay.c \
        $(PLATFORM_DRIVERS)/pico_irq.c

SRCS += $(NO-OS)/util/no_os_lf256fifo.c \
        $(DRIVERS)/api/no_os_irq.c
