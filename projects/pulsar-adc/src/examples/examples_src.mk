
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

INCS += $(DRIVERS)/adc/pulsar_adc/iio_pulsar_adc.h
SRCS += $(DRIVERS)/adc/pulsar_adc/iio_pulsar_adc.c

IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app

endif

INCS += $(DRIVERS)/adc/pulsar_adc/pulsar_adc.h
SRCS += $(DRIVERS)/adc/pulsar_adc/pulsar_adc.c

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
