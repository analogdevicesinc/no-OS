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

SRCS += $(PROJECT)/src/app/ad9656_fmc.c                                 \
	$(PROJECT)/src/app/ad9508.c   					\
	$(PROJECT)/src/app/ad9508.h					\
	$(PROJECT)/src/app/ad9553.c					\
	$(PROJECT)/src/app/ad9553.h
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c		\
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
        $(DRIVERS)/adc/ad9656/ad9656.c					\
        $(DRIVERS)/spi/spi.c						\
        $(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
        $(PLATFORM_DRIVERS)/xilinx_spi.c				\
        $(PLATFORM_DRIVERS)/delay.c
INCS +=	$(PROJECT)/src/app/app_config.h					\
        $(PROJECT)/src/devices/adi_hal/parameters.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h		\
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h		\
        $(DRIVERS)/adc/ad9656/ad9656.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
        $(INCLUDE)/spi.h						\
        $(INCLUDE)/error.h						\
        $(INCLUDE)/delay.h						\
        $(INCLUDE)/util.h
