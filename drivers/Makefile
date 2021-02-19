CC = arm-none-eabi-gcc

SKIPDIR = -path ./platform -prune -o \
	-path ./axi_core -prune -o \
	-path ./rf-transceiver/navassa -prune -o

INCLUDES = -I../include/ \
	 -I../projects/drivers/util/ \
	 -I./dac/ad917x/ad917x_api/ \
	 -I../projects/ad9361/src/ \
	 -I./adc/ad9208/ad9208_api \
	 -I./adc/ad9083/ad9083_api \
	 -I./adc/ad9081/api \
	 -I./axi_core/axi_adc_core \
	 -I./axi_core/axi_dac_core \
	 -I./axi_core/spi_engine \
	 -I./axi_core/axi_dmac \
	 -I./axi_core/clk_axi_clkgen \
	 -I./platform/xilinx \
	 -I../projects/adrv9009/src/devices/adi_hal \
	 -I../libraries/iio

CFLAGS = -c -Wall -Wformat=0 $(INCLUDES)

SUBDIRS = $(shell find . $(SKIPDIR) -name '*.c')

all:
	$(CC) $(CFLAGS) $(SUBDIRS)

clean:
	rm *.o
