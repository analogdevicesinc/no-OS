#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio/iio_app

# Add to SRCS source files to be build in the project
SRC_DIRS += $(NO-OS)/drivers/dac/ad5791

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)
SRC_DIRS += $(DRIVERS)/irq
SRCS += $(DRIVERS)/spi/spi.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.c
INCS += $(INCLUDE)/spi.h \
	$(DRIVERS)/platform/$(PLATFORM)/spi_extra.h
SRC_DIRS += $(DRIVERS)/gpio

TINYIIOD=y

