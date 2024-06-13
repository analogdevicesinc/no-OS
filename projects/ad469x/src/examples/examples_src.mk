ifeq (y, $(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y, $(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic_example/basic_example.c
INCS += $(PROJECT)/src/examples/basic_example/basic_example.h
endif


ifeq (y,$(strip $(IIO_TRIGGER_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_TRIGGER_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_trigger_example/iio_trigger_example.h
endif

ifeq (y, $(strip $(IIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/iio/iio_app/iio_app.c	\
	$(DRIVERS)/adc/ad4692/iio_ad4692.c	\
	$(NO-OS)/iio/iio.c	\
	$(NO-OS)/iio/iiod.c	\
	$(NO-OS)/util/no_os_fifo.c

SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h

INCS += $(NO-OS)/iio/iio_app/iio_app.h	\
	$(DRIVERS)/adc/ad4692/iio_ad4692.h	\
	$(NO-OS)/iio/iio.h	\
	$(NO-OS)/iio/iiod.h	\
	$(NO-OS)/iio/iio_types.h	\
	$(NO-OS)/include/no_os_fifo.h
endif
