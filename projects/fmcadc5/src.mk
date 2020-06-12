################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

# Uncomment to select the profile

SRCS := $(PROJECT)/src/app/fmcadc5.c					\
	$(PROJECT)/src/devices/i5g/i5g.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PROJECT)/src/app/app_iio.c
endif
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
	$(DRIVERS)/adc/ad9625/ad9625.c					\
	$(DRIVERS)/spi/spi.c						\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/gpio.c					\
	$(PLATFORM_DRIVERS)/delay.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(NO-OS)/util/xml.c						\
	$(NO-OS)/util/fifo.c						\
	$(NO-OS)/iio/iio.c						\
	$(NO-OS)/iio/iio_app/iio_app.c					\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.c				\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c
endif
INCS :=	$(PROJECT)/src/app/app_config.h					\
	$(PROJECT)/src/devices/adi_hal/parameters.h			\
	$(PROJECT)/src/devices/i5g/i5g.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(PROJECT)/src/app/app_iio.h
endif
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h		\
	$(DRIVERS)/adc/ad9625/ad9625.h					
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/xml.h						\
	$(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h                                \
	$(NO-OS)/iio/iio.h						\
	$(NO-OS)/iio/iio_types.h					\
	$(NO-OS)/iio/iio_app/iio_app.h					\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h				\
	$(NO-OS)/libraries/libtinyiiod/tinyiiod.h			\
	$(NO-OS)/libraries/libtinyiiod/compat.h
endif
