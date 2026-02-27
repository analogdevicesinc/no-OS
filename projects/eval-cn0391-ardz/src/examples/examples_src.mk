ifeq (basic_example, $(EXAMPLE))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (iio_example, $(EXAMPLE))
CFLAGS += -DIIO_EXAMPLE
CFLAGS += -DIIO_SUPPORT

SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h

SRCS += $(DRIVERS)/adc/ad7124/iio_ad7124.c
INCS += $(DRIVERS)/adc/ad7124/iio_ad7124.h

SRCS += $(PROJECT)/src/common/iio_cn0391.c
INCS += $(PROJECT)/src/common/iio_cn0391.h

IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app
endif

ifeq (iio_lwip_example, $(EXAMPLE))
CFLAGS += -DIIO_LWIP_EXAMPLE
CFLAGS += -DIIO_SUPPORT

SRC_DIRS += $(NO-OS)/iio/iio_app

include $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.mk
endif
