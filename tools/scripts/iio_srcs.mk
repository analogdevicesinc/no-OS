SRCS += $(NO-OS)/iio/iio.c
SRCS += $(NO-OS)/iio/iiod.c
SRCS += $(NO-OS)/util/no_os_circular_buffer.c

INCS += $(NO-OS)/iio/iio.h
INCS += $(NO-OS)/iio/iio_types.h
INCS += $(NO-OS)/iio/iiod.h
INCS += $(NO-OS)/iio/iiod_private.h
INCS += $(INCLUDE)/no_os_circular_buffer.h

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
ifeq (aducm3029,$(strip $(PLATFORM)))
SRCS	 += $(PLATFORM_DRIVERS)/no_os_timer.c
endif
endif
