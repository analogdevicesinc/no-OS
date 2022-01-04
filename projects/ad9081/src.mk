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

SRCS += $(PROJECT)/src/app.c \
	$(PROJECT)/src/app_clock.c \
	$(PROJECT)/src/app_jesd.c \
	$(DRIVERS)/adc/ad9081/ad9081.c \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_adc.c \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_dac.c \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_device.c \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_hal.c \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_jesd.c \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_sync.c \
	$(DRIVERS)/frequency/hmc7044/hmc7044.c \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c \
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.c \
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
	$(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/gpio.c \
	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/delay.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(NO-OS)/util/clk.c \
	$(NO-OS)/util/util.c
ifeq (y,$(strip $(QUAD_MXFE)))
SRCS += $(DRIVERS)/frequency/adf4371/adf4371.c
endif
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/iio/iio_app/iio_app.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/xilinx_irq.c \
	$(NO-OS)/util/list.c \
	$(NO-OS)/util/fifo.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c \
	$(DRIVERS)/api/irq.c
endif
INCS +=	$(PROJECT)/src/app_clock.h \
	$(PROJECT)/src/app_jesd.h \
	$(PROJECT)/src/app_config.h \
	$(PROJECT)/src/parameters.h \
	$(DRIVERS)/adc/ad9081/ad9081.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_ad9081.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_impala_tc.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_jrxa_des.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_jtx_dual_link.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_jtx_qbf_ad9081.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_lcpll_28nm.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_main.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_nb_coarse_nco.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_nb_ddc_dformat.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_nb_fine_nco.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_rx_paging.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_ser_phy.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_spi_only_up.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_config.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081.h \
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_hal.h \
	$(DRIVERS)/adc/ad9081/api/adi_cms_api_common.h \
	$(DRIVERS)/adc/ad9081/api/adi_cms_api_config.h \
	$(DRIVERS)/frequency/hmc7044/hmc7044.h \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h \
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.h \
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h \
	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(INCLUDE)/no-os/axi_io.h \
	$(INCLUDE)/no-os/clk.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/gpio.h \
	$(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/util.h
ifeq (y,$(strip $(QUAD_MXFE)))
INCS += $(DRIVERS)/frequency/adf4371/adf4371.h
endif
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(NO-OS)/iio/iio_app/iio_app.h \
	$(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/irq.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no-os/list.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif
