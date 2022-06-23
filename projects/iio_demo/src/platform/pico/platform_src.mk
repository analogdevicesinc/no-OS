INCS += $(PLATFORM_DRIVERS)/pico_uart.h  \
        $(PLATFORM_DRIVERS)/pico_irq.h

SRCS += $(PLATFORM_DRIVERS)/pico_uart.c  \
        $(PLATFORM_DRIVERS)/pico_delay.c \
        $(PLATFORM_DRIVERS)/pico_irq.c   \
        $(PLATFORM_DRIVERS)/pico_timer.c

SRCS += $(NO-OS)/util/no_os_lf256fifo.c \
        $(DRIVERS)/api/no_os_irq.c      \
        $(DRIVERS)/api/no_os_timer.c

