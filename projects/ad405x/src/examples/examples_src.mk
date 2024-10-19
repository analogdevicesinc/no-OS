ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(BASIC_EXAMPLE_I3C)))
CFLAGS += -DBASIC_EXAMPLE_I3C=1
SRCS += $(PROJECT)/src/examples/basic_i3c/basic_example_i3c.c
INCS += $(PROJECT)/src/examples/basic_i3c/basic_example_i3c.h
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/adc/ad405x/iio_ad405x.h
SRCS += $(DRIVERS)/adc/ad405x/iio_ad405x.c

INCS += $(INCLUDE)/no_os_list.h
endif
