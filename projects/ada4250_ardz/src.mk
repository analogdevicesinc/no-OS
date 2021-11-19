#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(DRIVERS)/amplifiers/ada4250
SRC_DIRS += $(NO-OS)/iio/iio_app
SRC_DIRS += $(NO-OS)/drivers/irq
SRC_DIRS += $(NO-OS)/drivers/gpio

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(INCLUDE)

SRCS += $(NO-OS)/util/util.c \
	$(NO-OS)/util/list.c \
	$(DRIVERS)/spi/spi.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.c \

INCS += $(INCLUDE)/spi.h \
	$(DRIVERS)/platform/$(PLATFORM)/spi_extra.h

IGNORED_FILES += $(PLATFORM_DRIVERS)/uart_stdio.c
IGNORED_FILES += $(PLATFORM_DRIVERS)/uart_stdio.h

TINYIIOD=y
