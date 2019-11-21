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

#PROFILE = tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88
#PROFILE = tx_bw200_ir245p76_rx_bw200_or245p76_orx_bw200_or245p76_dc245p76
PROFILE = tx_bw400_ir491p52_rx_bw200_or245p76_orx_bw400_or491p52_dc245p76
SRCS := $(PROJECT)/src/app/headless.c					\
	$(PROJECT)/src/devices/ad9528/ad9528.c				\
	$(PROJECT)/src/devices/adi_hal/no_os_hal.c			\
	$(PROJECT)/src/devices/talise/talise_agc.c			\
	$(PROJECT)/src/devices/talise/talise_arm.c			\
	$(PROJECT)/src/devices/talise/talise.c				\
	$(PROJECT)/src/devices/talise/talise_cals.c			\
	$(PROJECT)/src/devices/talise/talise_error.c			\
	$(PROJECT)/src/devices/talise/talise_gpio.c			\
	$(PROJECT)/src/devices/talise/talise_hal.c			\
	$(PROJECT)/src/devices/talise/talise_jesd204.c			\
	$(PROJECT)/src/devices/talise/talise_radioctrl.c		\
	$(PROJECT)/src/devices/talise/talise_rx.c			\
	$(PROJECT)/src/devices/talise/talise_tx.c			\
	$(PROJECT)/src/devices/talise/talise_user.c			\
	$(PROJECT)/profiles/$(PROFILE)/talise_config.c
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/spi.c					\
	$(PLATFORM_DRIVERS)/gpio.c					\
	$(PLATFORM_DRIVERS)/delay.c
INCS :=	$(PROJECT)/src/app/app_config.h					\
	$(PROJECT)/src/devices/ad9528/ad9528.h				\
	$(PROJECT)/src/devices/ad9528/t_ad9528.h			\
	$(PROJECT)/src/devices/adi_hal/adi_hal.h			\
	$(PROJECT)/src/devices/adi_hal/common.h				\
	$(PROJECT)/src/devices/adi_hal/parameters.h			\
	$(PROJECT)/src/devices/talise/talise_agc.h			\
	$(PROJECT)/src/devices/talise/talise_agc_types.h		\
	$(PROJECT)/src/devices/talise/talise_arm.h			\
	$(PROJECT)/src/devices/talise/talise_arm_macros.h		\
	$(PROJECT)/src/devices/talise/talise_arm_types.h		\
	$(PROJECT)/src/devices/talise/talise_cals.h			\
	$(PROJECT)/src/devices/talise/talise_cals_types.h		\
	$(PROJECT)/src/devices/talise/talise_error.h			\
	$(PROJECT)/src/devices/talise/talise_error_types.h		\
	$(PROJECT)/src/devices/talise/talise_gpio.h			\
	$(PROJECT)/src/devices/talise/talise_gpio_types.h		\
	$(PROJECT)/src/devices/talise/talise.h				\
	$(PROJECT)/src/devices/talise/talise_hal.h			\
	$(PROJECT)/src/devices/talise/talise_jesd204.h			\
	$(PROJECT)/src/devices/talise/talise_jesd204_types.h		\
	$(PROJECT)/src/devices/talise/talise_radioctrl.h		\
	$(PROJECT)/src/devices/talise/talise_radioctrl_types.h		\
	$(PROJECT)/src/devices/talise/talise_reg_addr_macros.h		\
	$(PROJECT)/src/devices/talise/talise_rx.h			\
	$(PROJECT)/src/devices/talise/talise_rx_types.h			\
	$(PROJECT)/src/devices/talise/talise_tx.h			\
	$(PROJECT)/src/devices/talise/talise_tx_types.h			\
	$(PROJECT)/src/devices/talise/talise_types.h			\
	$(PROJECT)/src/devices/talise/talise_user.h			\
	$(PROJECT)/src/devices/talise/talise_version.h			\
	$(PROJECT)/src/firmware/talise_arm_binary.h			\
	$(PROJECT)/src/firmware/talise_stream_binary.h			\
	$(PROJECT)/profiles/$(PROFILE)/talise_config_ad9528.h		\
	$(PROJECT)/profiles/$(PROFILE)/talise_config.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/util.h
