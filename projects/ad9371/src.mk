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

#PROFILE =  tx_bw100_orx_bw100_rx_bw100
#PROFILE = tx_bw100_orx_bw100_rx_bw40
PROFILE = tx_bw200_orx_bw200_rx_bw100
SRCS := $(PROJECT)/src/app/headless.c					\
	$(PROJECT)/profiles/$(PROFILE)/myk_ad9528init.c			\
	$(PROJECT)/profiles/$(PROFILE)/myk.c				\
	$(PROJECT)/profiles/$(PROFILE)/myk.h				\
	$(PROJECT)/src/devices/ad9528/ad9528.c				\
	$(PROJECT)/src/devices/adi_hal/common.c				\
	$(PROJECT)/src/devices/mykonos/mykonos.c			\
	$(PROJECT)/src/devices/mykonos/mykonos_debug/mykonos_dbgjesd.c	\
	$(PROJECT)/src/devices/mykonos/mykonos_gpio.c			\
	$(PROJECT)/src/devices/mykonos/mykonosMmap.c			\
	$(PROJECT)/src/devices/mykonos/mykonos_user.c
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
	$(PROJECT)/src/devices/adi_hal/common.h				\
	$(PROJECT)/src/devices/adi_hal/parameters.h			\
	$(PROJECT)/src/devices/mykonos/mykonos_debug/mykonos_dbgjesd.h	\
	$(PROJECT)/src/devices/mykonos/mykonos_debug/t_mykonos_dbgjesd.h\
	$(PROJECT)/src/devices/mykonos/mykonos_gpio.h			\
	$(PROJECT)/src/devices/mykonos/mykonos.h			\
	$(PROJECT)/src/devices/mykonos/mykonos_macros.h			\
	$(PROJECT)/src/devices/mykonos/mykonos_user.h			\
	$(PROJECT)/src/devices/mykonos/mykonos_version.h		\
	$(PROJECT)/src/devices/mykonos/t_mykonos_gpio.h			\
	$(PROJECT)/src/devices/mykonos/t_mykonos.h			\
	$(PROJECT)/src/firmware/Mykonos_M3.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h
INCS +=	$(PLATFORM_DRIVERS)/xilinx_platform_drivers.h			\
	$(PLATFORM_DRIVERS)/spi_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/util.h
