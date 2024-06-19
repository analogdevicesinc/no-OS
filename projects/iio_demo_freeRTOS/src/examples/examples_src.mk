ifeq (y,$(strip $(IIO_EXAMPLE)))
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.h \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.h

SRCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.c \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.c

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
