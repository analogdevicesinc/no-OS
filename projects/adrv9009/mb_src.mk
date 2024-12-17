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

SRCS += $(PROJECT)/src/app/headless1.c \
	$(DRIVERS)/frequency/ad9528/ad9528.c \
	$(PROJECT)/src/devices/adi_hal/no_os_hal.c \
	$(DRIVERS)/frequency/hmc7044/hmc7044.c \
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

# Talise API sources
SRC_DIRS += $(DRIVERS)/rf-transceiver/talise

NO_OS_INC_DIRS := \
	$(INCLUDE)/ \
	$(PROJECT)/src/app/ \
	$(PROJECT)/src/devices/adi_hal/ \
	$(PROJECT)/profiles/$(PROFILE)/ \
	$(PLATFORM_DRIVERS)/ \
	$(NO-OS)/jesd204/ \
	$(DRIVERS)/frequency/ad9528/ \
	$(DRIVERS)/frequency/hmc7044/ \
	$(DRIVERS)/axi_core/axi_adc_core/ \
	$(DRIVERS)/axi_core/axi_dac_core/ \
	$(DRIVERS)/axi_core/axi_dmac/ \
	$(DRIVERS)/axi_core/jesd204/

ifeq (xilinx,$(strip $(PLATFORM)))
NO_OS_INC_DIRS += $(DRIVERS)/axi_core/clk_axi_clkgen/
else
NO_OS_INC_DIRS += $(DRIVERS)/axi_core/clk_altera_a10_fpll/
endif
ifeq (y,$(strip $(IIOD)))
NO_OS_INC_DIRS += \
	$(DRIVERS)/axi_core/iio_axi_adc/ \
	$(DRIVERS)/axi_core/iio_axi_dac/
endif
