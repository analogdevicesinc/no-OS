ifneq ($(if $(findstring ftd2xx, $(LIBRARIES)), 1),)
INCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_i2c.h
SRCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_i2c.c	\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_delay.c
else
INCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_i2c.h		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h

SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_i2c.c		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c
endif
