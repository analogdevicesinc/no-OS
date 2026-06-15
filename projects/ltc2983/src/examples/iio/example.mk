IIOD = y
CFLAGS += -DIIO_EXAMPLE=1

INCS += $(DRIVERS)/temperature/ltc2983/iio_ltc2983.h
SRCS += $(DRIVERS)/temperature/ltc2983/iio_ltc2983.c
