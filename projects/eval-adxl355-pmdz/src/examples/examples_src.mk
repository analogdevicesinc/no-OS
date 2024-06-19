ifeq (y,$(strip $(ADXL355)))
CFLAGS += -DADXL355_DEV
else ifeq (y,$(strip $(ADXL357)))
CFLAGS += -DADXL357_DEV
else
CFLAGS += -DADXL359_DEV
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_TRIGGER_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.h
endif

ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
endif

ifeq (y,$(strip $(IIO_LWIP_EXAMPLE)))
IIOD = y
CFLAGS += -DIIO_LWIP_EXAMPLE
CFLAGS += -DNO_OS_LWIP_NETWORKING
LIBRARIES += lwip
SRCS += $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.c
INCS += $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.h
endif

ifeq (y,$(strip $(IIOD)))
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
