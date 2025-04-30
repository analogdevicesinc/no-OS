ifneq ($(if $(findstring ftd2xx, $(LIBRARIES)), 1),)
INCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_spi.h
SRCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_spi.c	\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_delay.c
else
INCS += $(PLATFORM_DRIVERS)/linux_spi.h		\
	$(PLATFORM_DRIVERS)/linux_uart.h	\

SRCS += $(PLATFORM_DRIVERS)/linux_delay.c	\
        $(PLATFORM_DRIVERS)/linux_spi.c		\
	$(PLATFORM_DRIVERS)/linux_uart.c
endif
