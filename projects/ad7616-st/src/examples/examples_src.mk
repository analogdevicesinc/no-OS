ifeq (y,$(strip $(IIO_EXAMPLE)))
CFLAGS += -DIIO_EXAMPLE -DIIO_SUPPORT
LIBRARIES += iio

SRCS += $(PROJECT)/src/examples/iio/iio_example.c
INCS += $(PROJECT)/src/examples/iio/iio_example.h

INCS +=	$(DRIVERS)/adc/ad7616/iio_ad7616.h \
		$(NO-OS)/iio/iio.h \
		$(NO-OS)/iio/iiod.h \
		$(NO-OS)/iio/iiod_private.h \
		$(NO-OS)/iio/iio_types.h \
		$(NO-OS)/iio/iio_app/iio_app.h

SRCS += $(DRIVERS)/adc/ad7616/iio_ad7616.c \
		$(NO-OS)/iio/iio.c \
		$(NO-OS)/iio/iiod.c \
		$(NO-OS)/iio/iio_app/iio_app.c
endif

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
