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

SRCS += $(PROJECT)/src/app/ad9656_fmc.c \
	$(PROJECT)/src/app/ad9508.c \
	$(PROJECT)/src/app/ad9553.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app
endif
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c \
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c \
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
        $(DRIVERS)/adc/ad9656/ad9656.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(NO-OS)/util/no_os_util.c \
	$(NO-OS)/jesd204/jesd204-core.c \
	$(NO-OS)/jesd204/jesd204-fsm.c
ifeq (y,$(strip $(TINYIIOD)))
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
        $(PROJECT)/src/devices/adi_hal/parameters.h \
        $(PROJECT)/src/app/ad9508.h \
        $(PROJECT)/src/app/ad9553.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h \
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h \
        $(DRIVERS)/adc/ad9656/ad9656.h
INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h
INCS +=	$(INCLUDE)/no_os_axi_io.h \
        $(INCLUDE)/no_os_spi.h \
        $(INCLUDE)/no_os_error.h \
        $(INCLUDE)/no_os_delay.h \
        $(INCLUDE)/no_os_util.h \
        $(INCLUDE)/no_os_units.h \
        $(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/jesd204.h \
	$(NO-OS)/jesd204/jesd204-priv.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h
endif
