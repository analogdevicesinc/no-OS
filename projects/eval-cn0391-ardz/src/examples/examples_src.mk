ifdef CN0391_FILTER_TYPE
CFLAGS += -DCN0391_FILTER_TYPE=$(CN0391_FILTER_TYPE)
endif

ifeq (basic_example, $(EXAMPLE))
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
endif


ifeq (iio_lwip_example, $(EXAMPLE))
CFLAGS += -DIIO_SUPPORT

SRC_DIRS += $(NO-OS)/iio/iio_app

include $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.mk
endif
