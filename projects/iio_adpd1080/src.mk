#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio/iio_app
SRC_DIRS += $(NO-OS)/drivers/photo-electronic/adpd188

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)

SRCS += $(DRIVERS)/api/gpio.c
SRCS += $(DRIVERS)/api/irq.c
SRCS +=	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_irq.c
INCS += $(INCLUDE)/no_os_irq.h
INCS += $(DRIVERS)/platform/$(PLATFORM)/irq_extra.h

SRCS += $(DRIVERS)/api/i2c.c
SRCS +=	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_i2c.c
INCS += $(INCLUDE)/no_os_i2c.h
INCS += $(DRIVERS)/platform/$(PLATFORM)/i2c_extra.h

SRCS += $(DRIVERS)/api/spi.c

TINYIIOD=y

