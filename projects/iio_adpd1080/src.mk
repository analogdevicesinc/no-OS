#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio/iio_app
SRC_DIRS += $(NO-OS)/drivers/photo-electronic/adpd188

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)

SRCS += $(DRIVERS)/api/no_os_gpio.c
SRCS += $(DRIVERS)/api/no_os_irq.c
SRCS +=	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_irq.c
SRCS += $(DRIVERS)/api/no_os_uart.c
INCS += $(INCLUDE)/no_os_irq.h
INCS += $(DRIVERS)/platform/$(PLATFORM)/aducm3029_irq.h

SRCS += $(DRIVERS)/api/no_os_i2c.c \
		$(DRIVERS)/api/no_os_timer.c
SRCS +=	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_i2c.c
INCS += $(INCLUDE)/no_os_i2c.h
INCS += $(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_i2c.h

SRCS += $(DRIVERS)/api/no_os_spi.c

IIOD=y

