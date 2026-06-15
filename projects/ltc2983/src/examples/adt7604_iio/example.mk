IIOD = y
CFLAGS += -DADT7604_IIO_EXAMPLE=1

INCS += $(DRIVERS)/temperature/ltc2983/iio_ltc2983.h
SRCS += $(DRIVERS)/temperature/ltc2983/iio_ltc2983.c
