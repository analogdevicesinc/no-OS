CC = arm-none-eabi-gcc

SKIPDIR = -path ./platform -prune -o \
	-path ./axi_core -prune -o \
	-path ./rf-transceiver/navassa -prune -o \
	-path ./adc/ad9081 -prune -o \
	-path ./rf-transceiver/madura -prune -o

INCLUDES = -I../include/ \
	 -I../projects/drivers/util/ \
	 -I./dac/ad917x/ad917x_api/ \
	 -I../projects/ad9361/src/ \
	 -I./adc/ad9208/ad9208_api \
	 -I./adc/ad9083/ad9083_api \
	 -I../projects/ad9083/src/uc \
	 -I./axi_core/axi_adc_core \
	 -I./axi_core/axi_dac_core \
	 -I./axi_core/spi_engine \
	 -I./axi_core/axi_dmac \
	 -I./axi_core/clk_axi_clkgen \
	 -I./platform/xilinx \
	 -I../projects/adrv9009/src/devices/adi_hal \
	 -I../iio \
	 -I./display

IIOD_FLAGS += -DIIO_SUPPORT

CFLAGS = -c -Wall -Wformat=0 $(IIOD_FLAGS) $(INCLUDES)

SRCS = $(shell find . $(SKIPDIR) -name '*.c')
OBJS = $(SRCS:.c=.o)

%.o:%.c
	@echo CC $(notdir $<)
	@$(CC) -c $< $(CFLAGS)

all: $(OBJS)

clean:
	rm -f *.o
