#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio/iio_app

# Add to SRCS source files to be build in the project
SRC_DIRS += $(NO-OS)/drivers/dac/ad5791

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)
SRCS += $(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/irq.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.c
INCS += $(INCLUDE)/no_os_spi.h \
	$(DRIVERS)/platform/$(PLATFORM)/spi_extra.h

TINYIIOD=y

