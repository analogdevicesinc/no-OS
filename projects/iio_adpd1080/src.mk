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
INCS += $(INCLUDE)/no_os_irq.h
INCS += $(DRIVERS)/platform/$(PLATFORM)/irq_extra.h

SRCS += $(DRIVERS)/api/no_os_i2c.c
SRCS +=	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_i2c.c
INCS += $(INCLUDE)/no_os_i2c.h
INCS += $(DRIVERS)/platform/$(PLATFORM)/i2c_extra.h

SRCS += $(DRIVERS)/api/no_os_spi.c

ifeq '$(USE_TCP_SOCKET)' 'y'
CFLAGS += -DUSE_TCP_SOCKET
endif
ENABLE_IIO_NETWORK = y

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y

SRC_DIRS += $(NO-OS)/network

#SRCS += $(NO-OS)/util/no_os_circular_buffer.c \
#	$(PLATFORM_DRIVERS)/no_os_timer.c
#	
#INCS += $(INCLUDE)/no_os_circular_buffer.h \
#	$(INCLUDE)/no_os_timer.h           \
#	$(PLATFORM_DRIVERS)/timer_extra.h  \
#	$(PLATFORM_DRIVERS)/rtc_extra.h
endif

TINYIIOD=y

