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
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(NO-OS)/util/no_os_clk.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/jesd204/jesd204-core.c \
	$(NO-OS)/jesd204/jesd204-fsm.c
ifeq (y,$(strip $(QUAD_MXFE)))
SRCS += $(DRIVERS)/frequency/adf4371/adf4371.c
endif
ifeq (y,$(strip $(IIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/iio/iio_app/iio_app.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(PLATFORM_DRIVERS)/xilinx_irq.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_fifo.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c \
	$(DRIVERS)/api/no_os_irq.c
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
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_clk.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_units.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/jesd204.h \
	$(NO-OS)/jesd204/jesd204-priv.h
ifeq (y,$(strip $(QUAD_MXFE)))
INCS += $(DRIVERS)/frequency/adf4371/adf4371.h
endif
ifeq (y,$(strip $(IIOD)))
INCS += $(NO-OS)/iio/iio_app/iio_app.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif
