ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic_example/basic_example.c
INCS += $(PROJECT)/src/examples/basic_example/basic_example.h
endif

ifeq (xilinx,$(PLATFORM))
CFLAGS += -DSPI_ENGINE_OFFLOAD_EXAMPLE
endif
ifeq (y,$(strip $(IIO_EXAMPLE)))
CFLAGS += -DIIO_EXAMPLE -DIIO_SUPPORT

SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h

INCS +=	$(DRIVERS)/adc/ad738x/iio_ad738x.h
SRCS += $(DRIVERS)/adc/ad738x/iio_ad738x.c
IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app
endif

INCS += $(DRIVERS)/adc/ad738x/ad738x.h
SRCS += $(DRIVERS)/adc/ad738x/ad738x.c
