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

SRCS := $(PROJECT)/src/main.c
SRCS += $(PROJECT)/src/ad9361_api.c					\
	$(PROJECT)/src/ad9361.c						\
	$(PROJECT)/src/ad9361_conv.c					\
	$(PROJECT)/src/ad9361_util.c
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
INCS := $(PROJECT)/src/common.h						\
	$(PROJECT)/src/config.h
INCS += $(PROJECT)/src/ad9361.h						\
	$(PROJECT)/src/ad9361_parameters.h				\
	$(PROJECT)/src/ad9361_util.h					\
	$(PROJECT)/src/ad9361_api.h
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
