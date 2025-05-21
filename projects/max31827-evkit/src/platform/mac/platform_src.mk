ifneq ($(if $(findstring ftd2xx, $(LIBRARIES)), 1),)
INCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_i2c.h
SRCS += $(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_i2c.c	\
	$(DRIVERS)/platform/ftd2xx/mpsse/ftd2xx_delay.c
endif
