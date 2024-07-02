
ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic_example/basic_example.c
INCS += $(PROJECT)/src/examples/basic_example/basic_example.h
endif

ifeq (xilinx,$(PLATFORM))
CFLAGS += -DSPI_ENGINE_OFFLOAD_ENABLE
endif
ifeq (stm32,$(PLATFORM))
CFLAGS += -DUSE_STANDARD_SPI
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
CFLAGS += -DIIO_EXAMPLE

SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h

INCS += $(DRIVERS)/adc/ad400x/iio_ad400x.h
SRCS += $(DRIVERS)/adc/ad400x/iio_ad400x.c

IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app

endif

INCS += $(DRIVERS)/adc/ad400x/ad400x.h
SRCS += $(DRIVERS)/adc/ad400x/ad400x.c

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
