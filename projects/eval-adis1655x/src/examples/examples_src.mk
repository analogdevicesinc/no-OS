ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_TRIGGER_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/imu/iio_adis_internals.h \
	$(DRIVERS)/imu/iio_adis1655x.h
SRCS += $(DRIVERS)/imu/iio_adis.c \
	$(DRIVERS)/imu/iio_adis1655x.c
endif

ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h
SRCS += $(DRIVERS)/imu/iio_adis_trig.c
endif


INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h


