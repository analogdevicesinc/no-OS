SRCS += $(DRIVERS)/dac/ad3552r/ad3552r.c
INCS += $(DRIVERS)/dac/ad3552r/ad3552r.h

SRCS += $(NO-OS)/util/no_os_util.c \
        $(NO-OS)/util/no_os_alloc.c \
        $(NO-OS)/util/no_os_mutex.c \
        $(NO-OS)/util/no_os_sin_lut.c \
        $(NO-OS)/util/no_os_crc8.c \
	$(DRIVERS)/api/no_os_spi.c \
        $(DRIVERS)/api/no_os_gpio.c \
        $(NO-OS)/util/no_os_util.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_fifo.c

INCS += $(INCLUDE)/no_os_timer.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_crc8.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_fifo.h

SRCS += $(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c

INCS += $(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h

ifeq (y,$(strip $(IIOD)))
SRCS += $(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c
INCS += $(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
SRCS += $(DRIVERS)/dac/ad3552r/iio_ad3552r.c
INCS += $(DRIVERS)/dac/ad3552r/iio_ad3552r.h
endif