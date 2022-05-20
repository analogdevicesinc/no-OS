INCS += $(PLATFORM_DRIVERS)/stm32_delay.h     \
        $(PLATFORM_DRIVERS)/stm32_hal.h       \
        $(PLATFORM_DRIVERS)/stm32_irq.h       \
        $(PLATFORM_DRIVERS)/stm32_uart.h      \
        $(PLATFORM_DRIVERS)/stm32_timer.h     \
        $(PLATFORM_DRIVERS)/stm32_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c     \
        $(PLATFORM_DRIVERS)/stm32_irq.c       \
        $(PLATFORM_DRIVERS)/stm32_uart.c      \
        $(PLATFORM_DRIVERS)/stm32_timer.c     \
        $(PLATFORM_DRIVERS)/stm32_uart_stdio.c

ICNS += $(INCLUDE)/no_os_irq.h

SRCS += $(NO-OS)/util/no_os_lf256fifo.c \
        $(DRIVERS)/api/no_os_timer.c    \
        $(DRIVERS)/api/no_os_irq.c
