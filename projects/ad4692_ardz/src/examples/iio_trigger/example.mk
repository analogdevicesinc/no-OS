IIOD=y

CFLAGS +=-DIIO_IGNORE_BUFF_OVERRUN_ERR

INCS += $(DRIVERS)/adc/ad4692/iio_ad4692.h	\
	$(NO-OS)/iio/iio_trigger.h
SRCS += $(DRIVERS)/adc/ad4692/iio_ad4692.c	\
	$(NO-OS)/iio/iio_trigger.c
