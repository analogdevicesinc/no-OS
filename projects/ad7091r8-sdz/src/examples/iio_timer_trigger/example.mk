IIOD = y
CFLAGS += -DIIO_TIMER_TRIGGER_EXAMPLE

INCS += $(DRIVERS)/adc/ad7091r8/iio_ad7091r8.h
SRCS += $(DRIVERS)/adc/ad7091r8/iio_ad7091r8.c

SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h

SRCS += $(DRIVERS)/adc/ad7091r8/iio_ad7091r8_trig.c
