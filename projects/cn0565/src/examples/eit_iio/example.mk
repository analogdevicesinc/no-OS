IIOD=y

CFLAGS += -DENABLE_UART_STDIO

SRCS += $(DRIVERS)/afe/ad5940/iio_ad5940.c \
	$(DRIVERS)/switch/adg2128/iio_adg2128.c
INCS += $(DRIVERS)/afe/ad5940/iio_ad5940.h \
	$(DRIVERS)/switch/adg2128/iio_adg2128.h
