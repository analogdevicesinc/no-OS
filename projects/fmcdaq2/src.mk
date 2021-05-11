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

SRCS += $(PROJECT)/src/app/fmcdaq2.c					
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PROJECT)/src/app/app_iio.c
endif
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
	$(DRIVERS)/frequency/ad9523/ad9523.c				\
	$(DRIVERS)/adc/ad9680/ad9680.c					\
	$(DRIVERS)/dac/ad9144/ad9144.c					\
	$(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/gpio/gpio.c						\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/delay.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/util/fifo.c						\
	$(NO-OS)/util/list.c						\
	$(DRIVERS)/adc/ad9680/iio_ad9680.c				\
	$(DRIVERS)/dac/ad9144/iio_ad9144.c				\
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c				\
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c				\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c
endif
INCS +=	$(PROJECT)/src/app/app_config.h					\
	$(PROJECT)/src/devices/adi_hal/parameters.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(PROJECT)/src/app/app_iio.h					\
	$(DRIVERS)/adc/ad9680/iio_ad9680.h				\
	$(DRIVERS)/dac/ad9144/iio_ad9144.h
endif
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h		\
	$(DRIVERS)/frequency/ad9523/ad9523.h				\
	$(DRIVERS)/adc/ad9680/ad9680.h					\
	$(DRIVERS)/dac/ad9144/ad9144.h					
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h                                \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h				\
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif
