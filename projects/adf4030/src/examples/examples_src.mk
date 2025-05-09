EXAMPLE ?= basic

ifeq (basic,$(strip $(EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (iio_example,$(strip $(EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/frequency/adf4030/iio_adf4030.h
SRCS += $(DRIVERS)/frequency/adf4030/iio_adf4030.c

INCS += $(INCLUDE)/no_os_list.h \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
endif
