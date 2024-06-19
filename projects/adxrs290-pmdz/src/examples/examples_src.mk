ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_TRIGGER_EXAMPLE
CFLAGS +=-DIIO_IGNORE_BUFF_OVERRUN_ERR
SRCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.h
endif

ifeq (y,$(strip $(IIO_TIMER_TRIGGER_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_TIMER_TRIGGER_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app
SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h
INCS += $(DRIVERS)/gyro/adxrs290/iio_adxrs290.h

SRCS += $(DRIVERS)/gyro/adxrs290/iio_adxrs290.c \
	$(DRIVERS)/gyro/adxrs290/iio_adxrs290_trig.c

INCS += $(INCLUDE)/no_os_list.h
endif
