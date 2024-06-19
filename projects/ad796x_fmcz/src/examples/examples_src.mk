ifeq (y,$(strip $(IIO_EXAMPLE)))
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h

SRCS += $(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c
INCS += $(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h

IIOD=y
SRC_DIRS += $(NO-OS)/iio/iio_app
endif

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic_example/basic_example.c
INCS += $(PROJECT)/src/examples/basic_example/basic_example.h
endif

INCS += $(DRIVERS)/adc/ad796x/ad796x.h

SRCS += $(DRIVERS)/adc/ad796x/ad796x.c

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
