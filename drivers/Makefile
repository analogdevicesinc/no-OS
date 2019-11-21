CC = arm-none-eabi-gcc

SKIPDIR = -path ./platform -prune -o \
	-path ./axi_core -prune -o

INCLUDES = -I../include/ \
	 -I../projects/drivers/util/ \
	 -I./dac/ad917x/ad917x_api/ \
	 -I../projects/ad9361/src/ \
	 -I./adc/ad9208/ad9208_api \
	 -I./axi_core/axi_adc_core

CFLAGS = -c -Wall -Wformat=0 $(INCLUDES)

SUBDIRS = $(shell find . $(SKIPDIR) -name '*.c')

all:
	$(CC) $(CFLAGS) $(SUBDIRS)

clean:
	rm *.o
