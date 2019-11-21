CC = arm-none-eabi-gcc
CFLAGS = -c -Wall -I../include/ \
	 -I../projects/drivers/util/ \
	 -I./dac/ad917x/ad917x_api/ \
	 -I../projects/ad9361/src/ \
	 -I./adc/ad9208/ad9208_api \
	 -I./axi_core/axi_adc_core

SUBDIRS = $(shell find . -path ./platform -prune -o -name '*.c')

all:
	$(CC) $(CFLAGS) $(SUBDIRS)

clean:
	rm *.o
