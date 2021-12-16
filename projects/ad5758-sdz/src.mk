# See No-OS/tool/scripts/src_model.mk for variable description
SRC_DIRS += $(PROJECT)/src
SRCS += $(DRIVERS)/api/gpio.c \
        $(DRIVERS)/api/spi.c \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c \
	$(PLATFORM_DRIVERS)/delay.c \
	$(DRIVERS)/dac/ad5758/ad5758.c

INCS += $(INCLUDE)/no-os/gpio.h \
	$(INCLUDE)/no-os/spi.h \
        $(INCLUDE)/no-os/error.h \
        $(INCLUDE)/no-os/delay.h \
        $(INCLUDE)/no-os/print_log.h \
        $(PLATFORM_DRIVERS)/gpio_extra.h \
	$(PLATFORM_DRIVERS)/spi_extra.h	\
	$(DRIVERS)/dac/ad5758/ad5758.h
