SRCS += $(NO-OS)/iio/iio.c
SRCS += $(NO-OS)/iio/iiod.c
SRCS += $(NO-OS)/iio/iio_trigger.c
SRCS += $(NO-OS)/iio/iio_app/iio_app.c

NO_OS_INC_DIRS += $(NO-OS)/iio \
		  $(NO-OS)/iio/iio_app \
		  $(INCLUDE)
SRC_DIRS += $(NO-OS)/util

ifeq (y,$(strip $(NETWORKING)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
endif
