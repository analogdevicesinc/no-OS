INCS += $(PLATFORM_DRIVERS)/chibios_spi.h \
	$(PLATFORM_DRIVERS)/chibios_i2c.h \
	$(PLATFORM_DRIVERS)/chibios_gpio.h \
	$(PLATFORM_DRIVERS)/chibios_delay.h \
	$(PLATFORM_DRIVERS)/chibios_alloc.h \
	$(PLATFORM_DRIVERS)chibios_uart.h

SRCS += $(PLATFORM_DRIVERS)/chibios_spi.c \
	$(PLATFORM_DRIVERS)/chibios_i2c.c \
	$(PLATFORM_DRIVERS)/chibios_gpio.c \
	$(PLATFORM_DRIVERS)/chibios_delay.c \
	$(PLATFORM_DRIVERS)/chibios_alloc.c \
	$(PLATFORM_DRIVERS)/chibios_uart.c

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/chibios_alloc.c
