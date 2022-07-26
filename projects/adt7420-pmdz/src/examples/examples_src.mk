ifeq (y,$(strip $(IIO_EXAMPLE)))
TINYIIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
endif

ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app
INCS += $(DRIVERS)/temperature/adt7420/iio_adt7420.h

SRCS += $(DRIVERS)/temperature/adt7420/iio_adt7420.c 

INCS += $(INCLUDE)/no_os_list.h \
		$(PLATFORM_DRIVERS)/uart_extra.h
endif
