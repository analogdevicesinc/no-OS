ifeq (y,$(strip $(ADXL355)))
CFLAGS += -DADXL355_DEV=1
else ifeq (y,$(strip $(ADXL357)))
CFLAGS += -DADXL357_DEV=1
else
CFLAGS += -DADXL359_DEV=1
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
TINYIIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
TINYIIOD=y
CFLAGS += -DIIO_TRIGGER_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.h
endif

ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
endif

ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/accel/adxl355/iio_adxl355.h
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355.c

ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355_trig.c
endif

INCS += $(INCLUDE)/no_os_list.h \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
endif
