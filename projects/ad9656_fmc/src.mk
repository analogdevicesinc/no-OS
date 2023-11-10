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

SRCS += $(PROJECT)/src/app/ad9656_fmc.c
ifeq (y,$(strip $(IIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app
endif
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c \
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c \
        $(DRIVERS)/axi_core/jesd204/jesd204_clk.c \
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
        $(DRIVERS)/adc/ad9656/ad9656.c \
        $(DRIVERS)/frequency/ad9553/ad9553.c \
        $(DRIVERS)/frequency/ad9508/ad9508.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(NO-OS)/util/no_os_clk.c \
        $(NO-OS)/util/no_os_util.c \
        $(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/jesd204/jesd204-core.c \
	$(NO-OS)/jesd204/jesd204-fsm.c
ifeq (y,$(strip $(IIOD)))
SRCS += $(NO-OS)/util/no_os_fifo.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(NO-OS)/util/no_os_list.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
        $(DRIVERS)/api/no_os_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c
endif
SRCS +=	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
        $(PLATFORM_DRIVERS)/xilinx_spi.c \
        $(PLATFORM_DRIVERS)/xilinx_delay.c
INCS +=	$(PROJECT)/src/app/app_config.h \
        $(PROJECT)/src/devices/adi_hal/parameters.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h \
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h \
        $(DRIVERS)/axi_core/jesd204/jesd204_clk.h \
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h \
        $(DRIVERS)/frequency/ad9553/ad9553.h \
        $(DRIVERS)/frequency/ad9508/ad9508.h \
        $(DRIVERS)/adc/ad9656/ad9656.h
INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h
INCS +=	$(INCLUDE)/no_os_axi_io.h \
        $(INCLUDE)/no_os_spi.h \
        $(INCLUDE)/no_os_error.h \
        $(INCLUDE)/no_os_delay.h \
        $(INCLUDE)/no_os_clk.h \
        $(INCLUDE)/no_os_util.h \
        $(INCLUDE)/no_os_units.h \
        $(INCLUDE)/no_os_print_log.h \
        $(INCLUDE)/no_os_alloc.h \
        $(INCLUDE)/no_os_mutex.h \
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
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h
endif
