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

# zc706 + adrv9009-w/pcbz compatible profiles:
#	tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88
#
# adrv2crr-fmc + adrv9009-zu11eg compatible profiles:
# zcu102 + fmcomms8 compatible profiles:
#	tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88
#	tx_bw200_ir245p76_rx_bw200_or245p76_orx_bw200_or245p76_dc245p76
#	tx_bw400_ir491p52_rx_bw200_or245p76_orx_bw400_or491p52_dc245p76

# Uncomment to select the profile:
PROFILE = tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88
#PROFILE = tx_bw200_ir245p76_rx_bw200_or245p76_orx_bw200_or245p76_dc245p76
#PROFILE = tx_bw400_ir491p52_rx_bw200_or245p76_orx_bw400_or491p52_dc245p76
SRCS += $(PROJECT)/src/app/headless.c \
	$(PROJECT)/src/app/app_clocking.c \
	$(PROJECT)/src/app/app_jesd.c \
	$(PROJECT)/src/app/app_transceiver.c \
	$(PROJECT)/src/app/app_talise.c \
	$(DRIVERS)/frequency/ad9528/ad9528.c \
	$(PROJECT)/src/devices/adi_hal/no_os_hal.c \
	$(DRIVERS)/frequency/hmc7044/hmc7044.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_agc.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_arm.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_cals.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_error.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_gpio.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_hal.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_jesd204.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_radioctrl.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_rx.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_tx.c \
	$(DRIVERS)/rf-transceiver/talise/api/talise_user.c \
	$(PROJECT)/profiles/$(PROFILE)/talise_config.c
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c \
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/jesd204/jesd204-core.c \
	$(NO-OS)/jesd204/jesd204-fsm.c
ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app
LIBRARIES += iio
SRCS += $(NO-OS)/util/no_os_fifo.c \
	$(NO-OS)/util/no_os_list.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif
SRCS +=	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_clk.c
ifeq (xilinx,$(strip $(PLATFORM)))
SRCS += $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c
else
SRCS += $(DRIVERS)/axi_core/clk_altera_a10_fpll/clk_altera_a10_fpll.c \
	$(DRIVERS)/axi_core/jesd204/altera_a10_atx_pll.c \
	$(DRIVERS)/axi_core/jesd204/altera_a10_cdr_pll.c \
	$(DRIVERS)/axi_core/jesd204/altera_adxcvr.c \
	$(PLATFORM_DRIVERS)/altera_axi_io.c \
	$(PLATFORM_DRIVERS)/altera_spi.c \
	$(PLATFORM_DRIVERS)/altera_gpio.c \
	$(PLATFORM_DRIVERS)/altera_delay.c
endif
INCS +=	$(PROJECT)/src/app/app_config.h \
	$(PROJECT)/src/app/app_clocking.h \
	$(PROJECT)/src/app/app_jesd.h \
	$(PROJECT)/src/app/app_transceiver.h \
	$(PROJECT)/src/app/app_talise.h \
	$(DRIVERS)/frequency/ad9528/ad9528.h \
	$(PROJECT)/src/devices/adi_hal/adi_hal.h \
	$(PROJECT)/src/devices/adi_hal/common.h \
	$(PROJECT)/src/devices/adi_hal/parameters.h \
	$(DRIVERS)/frequency/hmc7044/hmc7044.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_agc.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_agc_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_arm.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_arm_macros.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_arm_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_cals.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_cals_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_error.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_error_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_gpio.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_gpio_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_hal.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_jesd204.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_jesd204_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_radioctrl.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_radioctrl_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_reg_addr_macros.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_rx.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_rx_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_tx.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_tx_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_types.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_user.h \
	$(DRIVERS)/rf-transceiver/talise/api/talise_version.h \
	$(DRIVERS)/rf-transceiver/talise/firmware/talise_arm_binary.h \
	$(DRIVERS)/rf-transceiver/talise/firmware/talise_stream_binary.h \
	$(PROJECT)/profiles/$(PROFILE)/talise_config.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h \
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.h
ifeq (xilinx,$(strip $(PLATFORM)))
INCS += $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h \
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h
else
INCS += $(DRIVERS)/axi_core/clk_altera_a10_fpll/clk_altera_a10_fpll.h \
	$(DRIVERS)/axi_core/jesd204/altera_a10_atx_pll.h \
	$(DRIVERS)/axi_core/jesd204/altera_a10_cdr_pll.h \
	$(DRIVERS)/axi_core/jesd204/altera_adxcvr.h
endif
INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_units.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_clk.h \
	$(INCLUDE)/jesd204.h \
	$(NO-OS)/jesd204/jesd204-priv.h
ifeq (y,$(strip $(IIOD)))
INCS +=	$(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif
