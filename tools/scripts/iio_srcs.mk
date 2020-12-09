SRCS += $(NO-OS)/libraries/iio/iio.c
SRCS += $(NO-OS)/libraries/iio/libtinyiiod/parser.c
SRCS += $(NO-OS)/libraries/iio/libtinyiiod/tinyiiod.c
					
INCS += $(NO-OS)/libraries/iio/iio.h
INCS += $(NO-OS)/libraries/iio/iio_types.h
INCS += $(NO-OS)/libraries/iio/libtinyiiod/tinyiiod.h
INCS += $(NO-OS)/libraries/iio/libtinyiiod/tinyiiod-private.h
INCS += $(NO-OS)/libraries/iio/libtinyiiod/compat.h

# Dependencies
SRCS += $(NO-OS)/util/util.c \
	$(NO-OS)/util/list.c \
	$(PLATFORM_DRIVERS)/delay.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/irq.c

INCS += $(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(INCLUDE)/util.h						\
	$(INCLUDE)/error.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
SRCS	 += $(NO-OS)/util/circular_buffer.c
SRCS	 += $(PLATFORM_DRIVERS)/timer.c
endif
