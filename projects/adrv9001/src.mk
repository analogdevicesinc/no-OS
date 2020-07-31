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
# app
SRCS += $(DRIVERS)/rf-transceiver/navassa/adrv9002_init_data.c \
	$(DRIVERS)/rf-transceiver/navassa/adrv9002_conv.c \
	$(DRIVERS)/rf-transceiver/navassa/adrv9002.c \
	$(PROJECT)/src/app/headless.c
INCS += $(DRIVERS)/rf-transceiver/navassa/adrv9002.h \
	$(PROJECT)/src/app/ORxGainTable.h \
	$(PROJECT)/src/app/RxGainTable.h \
	$(PROJECT)/src/app/TxAttenTable.h
# hal
SRCS += $(PROJECT)/src/hal/no_os_platform.c
INCS += $(PROJECT)/src/hal/parameters.h \
	$(PROJECT)/src/hal/adi_platform.h \
	$(PROJECT)/src/hal/adi_platform_types.h \
	$(PROJECT)/src/firmware/Navassa_EvaluationFw.h
# no-OS drivers
SRCS += $(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(NO-OS)/drivers/gpio/gpio.c \
	$(DRIVERS)/spi/spi.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/delay.c \
	$(NO-OS)/util/util.c \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(PLATFORM_DRIVERS)/axi_io.c
INCS +=	$(INCLUDE)/spi.h \
	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(INCLUDE)/gpio.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h \
	$(INCLUDE)/error.h \
	$(INCLUDE)/delay.h \
	$(INCLUDE)/util.h \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(INCLUDE)/axi_io.h
# third_party
SRCS += $(DRIVERS)/rf-transceiver/navassa/third_party/jsmn/jsmn.c
INCS += $(DRIVERS)/rf-transceiver/navassa/third_party/adi_pmag_macros/adi_pmag_macros.h \
	$(DRIVERS)/rf-transceiver/navassa/third_party/jsmn/jsmn.h
# common
SRCS += $(DRIVERS)/rf-transceiver/navassa/common/adi_logging/adi_common_log.c \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_error/adi_common_error.c \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_common.c \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_hal/adi_common_hal.c
INCS += $(DRIVERS)/rf-transceiver/navassa/common/adi_error/adi_common_error_types.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_error/adi_common_error.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_hal/adi_common_hal_types.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_hal/adi_common_hal.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_libc/adi_common_libc_types.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_libc/adi_common_libc.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_logging/adi_common_log_types.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_logging/adi_common_log.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_common_types.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_common_macros.h \
	$(DRIVERS)/rf-transceiver/navassa/common/adi_common.h
# devices/private
SRCS += $(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_validators.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_rx.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_crc32.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_utilities.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_arm.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_bf_hal.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_init.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_gpio.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/src/adrv9001_powermanagement.c
INCS += $(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_arm_error_mapping.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_arm.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_arm_macros.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_crc32.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_gpio.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_init.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_init_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_powermanagement.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_reg_addr_macros.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_rx.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_utilities.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/adrv9001_validators.h
INCS += $(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_adc1_mem_map.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_adc2_mem_map.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_analog_rxb_mem_map.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_analog_rx_mem_map.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_analog_tx_mem_map.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_bf.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_bf_hal.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_pll_mem_map.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_core_1.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_core_2.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_core_3.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_core.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_rxb.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_rx.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_txb.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_nvs_regmap_tx.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_txdac_mem_map.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/private/include/bitfields/adrv9001_vco_adc_mem_map.h
# devices/public
SRCS += $(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_tx.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_arm.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_bbdc.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_gpio.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_auxdac.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_utilities.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_cals.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_auxadc.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_radio.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_mcs.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_ssi.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_rx_gaincontrol.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_stream.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_spi.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_rx.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_orx.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_dpd.c \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/src/adi_adrv9001_fh.c
INCS += $(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_arm.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_arm_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_auxadc.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_auxadc_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_auxdac.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_auxdac_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_bbdc.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_bbdc_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_cals.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_cals_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_clockSettings_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_defines.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_deviceSysConfig_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_dpd.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_dpd_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_error.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_gpio.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_gpio_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_mcs.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_mcs_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_orx.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_orx_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_pfirBuffer_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_powermanagement_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_profile_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_radio.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_radio_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_rx_gaincontrol.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_rx_gaincontrol_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_rx.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_rxSettings_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_rx_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_spi.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_spi_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_ssi.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_ssi_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_stream.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_stream_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_timing_docs.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_tx.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_txSettings_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_tx_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_user.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_utilities.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_utilities_types.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_version.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_Init_t_parser.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adrv9001_Init_t_parser.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_fh.h \
	$(DRIVERS)/rf-transceiver/navassa/devices/adrv9001/public/include/adi_adrv9001_fh_types.h
# IIO
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PROJECT)/src/app/app_iio.c \
	$(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/irq.c \
	$(NO-OS)/util/fifo.c \
	$(NO-OS)/util/xml.c \
	$(NO-OS)/iio/iio.c \
	$(NO-OS)/iio/iio_app/iio_app.c \
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.c \
	$(NO-OS)/iio/iio_axi_dac/iio_axi_dac.c
INCS += $(PROJECT)/src/app/app_iio.h \
	$(INCLUDE)/uart.h \
	$(INCLUDE)/irq.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(INCLUDE)/fifo.h \
	$(INCLUDE)/xml.h \
	$(NO-OS)/iio/iio.h \
	$(NO-OS)/iio/iio_types.h \
	$(NO-OS)/iio/iio_app/iio_app.h \
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h \
	$(NO-OS)/iio/iio_axi_dac/iio_axi_dac.h \
	$(NO-OS)/libraries/libtinyiiod/compat.h \
	$(NO-OS)/libraries/libtinyiiod/tinyiiod.h
endif