ifneq ($(if $(findstring ftd2xx, $(LIBRARIES)), 1),)
INCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_i2c.h		\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_platform.h	\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_uart.h
SRCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_i2c.c		\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_platform.c	\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_uart.c		\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_delay.c
else
$(error MAX31827-EVKIT project can only be run on MAC platform if FTD2XX library is used)
endif
