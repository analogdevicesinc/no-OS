IIOD=y
INCS += $(DRIVERS)/position/admt4000/iio_admt4000.h
SRCS += $(DRIVERS)/position/admt4000/iio_admt4000.c \
	$(DRIVERS)/position/admt4000/iio_admt4000_trig.c

INCS += $(DRIVERS)/motor/tmc5240/iio_tmc5240.h
SRCS += $(DRIVERS)/motor/tmc5240/iio_tmc5240.c

CFLAGS += -DFIRMWARE_VERSION=\"$(FIRMWARE_VERSION)\"
