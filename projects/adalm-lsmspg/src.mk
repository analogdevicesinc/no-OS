INCS += $(PROJECT)/src/examples/example.h

INCS += $(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_dma.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_units.h \
	$(INCLUDE)/no_os_timer.h

SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(NO-OS)/util/no_os_list.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_timer.c

# AD559xR drivers
INCS += $(DRIVERS)/adc-dac/ad5592r/ad5592r-base.h \
	$(DRIVERS)/adc-dac/ad5592r/ad5592r.h \
	$(DRIVERS)/adc-dac/ad5592r/ad5593r.h

SRCS += $(DRIVERS)/adc-dac/ad5592r/ad5592r-base.c \
	$(DRIVERS)/adc-dac/ad5592r/ad5592r.c \
	$(DRIVERS)/adc-dac/ad5592r/ad5593r.c

# AD559xR IIO
INCS += $(DRIVERS)/adc-dac/ad5592r/iio_ad559xr.h

SRCS += $(DRIVERS)/adc-dac/ad5592r/iio_ad559xr.c

# LM75 drivers
INCS += $(DRIVERS)/temperature/lm75/lm75.h

SRCS += $(DRIVERS)/temperature/lm75/lm75.c

# LM75 IIO
INCS += $(DRIVERS)/temperature/lm75/iio_lm75.h

SRCS += $(DRIVERS)/temperature/lm75/iio_lm75.c
