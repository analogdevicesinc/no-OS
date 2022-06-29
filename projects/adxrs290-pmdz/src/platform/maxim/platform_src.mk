INCS += $(INCLUDE)/no_os_rtc.h

INCS += $(PLATFORM_DRIVERS)/maxim_delay.h     \
        $(PLATFORM_DRIVERS)/gpio_extra.h      \
        $(PLATFORM_DRIVERS)/spi_extra.h       \
        $(PLATFORM_DRIVERS)/gpio_irq_extra.h  \
        $(PLATFORM_DRIVERS)/irq_extra.h       \
        $(PLATFORM_DRIVERS)/rtc_extra.h       \
        $(PLATFORM_DRIVERS)/maxim_uart.h      \
        $(PLATFORM_DRIVERS)/maxim_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
        $(PLATFORM_DRIVERS)/maxim_gpio.c      \
        $(PLATFORM_DRIVERS)/maxim_spi.c       \
        $(PLATFORM_DRIVERS)/maxim_rtc.c       \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.c  \
        $(PLATFORM_DRIVERS)/maxim_irq.c       \
        $(PLATFORM_DRIVERS)/maxim_uart.c      \
        $(PLATFORM_DRIVERS)/maxim_stdio.c
