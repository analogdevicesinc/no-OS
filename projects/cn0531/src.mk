#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio/iio_app

# Add to SRCS source files to be build in the project
SRC_DIRS += $(NO-OS)/drivers/dac/ad5791

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)
SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_timer.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.c
INCS += $(INCLUDE)/no_os_spi.h \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.h

IIOD=y

