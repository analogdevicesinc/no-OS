SRCS += $(NO-OS)/libraries/iio/iio.c
SRCS += $(NO-OS)/libraries/iio/libtinyiiod/parser.c
SRCS += $(NO-OS)/libraries/iio/libtinyiiod/tinyiiod.c
					
INCS += $(NO-OS)/libraries/iio/iio.h
INCS += $(NO-OS)/libraries/iio/iio_types.h
INCS += $(NO-OS)/libraries/iio/libtinyiiod/tinyiiod.h
INCS += $(NO-OS)/libraries/iio/libtinyiiod/tinyiiod-private.h
INCS += $(NO-OS)/libraries/iio/libtinyiiod/compat.h

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
SRCS	 += $(NO-OS)/util/circular_buffer.c
SRCS	 += $(PLATFORM_DRIVERS)/timer.c
endif
