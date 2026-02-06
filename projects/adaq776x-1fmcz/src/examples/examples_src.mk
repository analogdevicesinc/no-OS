ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic_example/basic_example.c
INCS += $(PROJECT)/src/examples/basic_example/basic_example.h
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
CFLAGS += -DIIO_EXAMPLE=1
CFLAGS += -DIIO_SUPPORT=1

SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h

INCS += $(DRIVERS)/adc/ad7768-1/iio_ad77681.h
SRCS += $(DRIVERS)/adc/ad7768-1/iio_ad77681.c

IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app

endif

ifeq (adaq7767-1,$(strip $(ADAQ776x_DEV)))
CFLAGS += -DADAQ77671_DEV
else ifeq (adaq7768-1,$(strip $(ADAQ776x_DEV)))
CFLAGS += -DADAQ77681_DEV
else ifeq (adaq7769-1,$(strip $(ADAQ776x_DEV)))
CFLAGS += -DADAQ77691_DEV
endif