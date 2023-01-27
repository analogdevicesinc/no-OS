ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
endif

# not available yet
ifeq (y,$(strip $(IIO_EXAMPLE)))
TINYIIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

# not available yet
ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
TINYIIOD=y
CFLAGS += -DIIO_TRIGGER_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.h
endif

ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/imu/iio_adis.h \
	$(DRIVERS)/imu/iio_adis16505.h
SRCS += $(DRIVERS)/imu/iio_adis.c \
	$(DRIVERS)/imu/iio_adis16505.c
endif

ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h
SRCS += $(DRIVERS)/imu/iio_adis_trig.c
endif


INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h


