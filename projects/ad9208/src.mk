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

SRC_DIRS += $(PROJECT)/src \
		$(DRIVERS)/adc/ad9208 \
		$(DRIVERS)/axi_core/axi_adc_core \
		$(DRIVERS)/axi_core/axi_dmac \
		$(DRIVERS)/axi_core/clk_axi_clkgen

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(DRIVERS)/axi_core/iio_axi_adc \
	    $(DRIVERS)/api/no_os_irq.c \
		$(NO-OS)/iio/iio_app

SRCS	+= $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
		$(DRIVERS)/api/no_os_uart.c \
		$(NO-OS)/util/no_os_list.c 
INCS	+= $(INCLUDE)/no_os_uart.h \
		$(INCLUDE)/no_os_lf256fifo.h \
		$(INCLUDE)/no_os_list.h \
		$(INCLUDE)/no_os_irq.h \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
		
endif

SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/frequency/hmc7044/hmc7044.c
SRCS += $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c \
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.c \
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_clk.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/jesd204/jesd204-core.c \
	$(NO-OS)/jesd204/jesd204-fsm.c
SRCS +=	$(PLATFORM_DRIVERS)/xilinx_axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/xilinx_delay.c

INCS += $(DRIVERS)/frequency/hmc7044/hmc7044.h \
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h \
	$(DRIVERS)/axi_core/jesd204/jesd204_clk.h \
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h
INCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_clk.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/jesd204.h \
	$(NO-OS)/jesd204/jesd204-priv.h
