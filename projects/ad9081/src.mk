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

SRCS := $(PROJECT)/src/app.c						\
	$(PROJECT)/src/app_clock.c					\
	$(PROJECT)/src/app_jesd.c					\
	$(DRIVERS)/adc/ad9081/ad9081.c					\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_adc.c			\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_dac.c			\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_device.c			\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_hal.c			\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_jesd.c			\
	$(DRIVERS)/frequency/hmc7044/hmc7044.c				\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c			\
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.c			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/delay.c					\
	$(PLATFORM_DRIVERS)/gpio.c					\
	$(PLATFORM_DRIVERS)/spi.c					\
	$(NO-OS)/util/clk.c						\
	$(NO-OS)/util/util.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PROJECT)/src/app_iio.c					\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(NO-OS)/util/xml.c						\
	$(NO-OS)/iio/iio.c						\
	$(NO-OS)/iio/iio_app/iio_app.c					\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.c				\
	$(NO-OS)/iio/iio_axi_dac/iio_axi_dac.c
endif
INCS :=	$(PROJECT)/src/app_clock.h					\
	$(PROJECT)/src/app_jesd.h					\
	$(PROJECT)/src/app_config.h					\
	$(PROJECT)/src/app_parameters.h					\
	$(DRIVERS)/adc/ad9081/ad9081.h					\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_ad9081.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_impala_tc.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_jrxa_des.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_jtx_dual_link.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_jtx_qbf_ad9081.h	\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_lcpll_28nm.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_main.h			\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_nb_coarse_nco.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_nb_ddc_dformat.h	\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_nb_fine_nco.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_rx_paging.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_ser_phy.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_bf_spi_only_up.h		\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_config.h			\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081.h				\
	$(DRIVERS)/adc/ad9081/api/adi_ad9081_hal.h			\
	$(DRIVERS)/adc/ad9081/api/adi_cms_api_common.h			\
	$(DRIVERS)/adc/ad9081/api/adi_cms_api_config.h			\
	$(DRIVERS)/frequency/hmc7044/hmc7044.h				\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h			\
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.h			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h		\
	$(PLATFORM_DRIVERS)/gpio_extra.h				\
	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/clk.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(PROJECT)/src/app_iio.h					\
	$(INCLUDE)/uart.h						\
	$(PLATFORM_DRIVERS)/uart_extra.h				\
	$(INCLUDE)/xml.h						\
	$(NO-OS)/iio/iio.h						\
	$(NO-OS)/iio/iio_types.h					\
	$(NO-OS)/iio/iio_app/iio_app.h					\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h				\
	$(NO-OS)/iio/iio_axi_dac/iio_axi_dac.h				\
	$(NO-OS)/libraries/libtinyiiod/compat.h				\
	$(NO-OS)/libraries/libtinyiiod/tinyiiod.h
endif
