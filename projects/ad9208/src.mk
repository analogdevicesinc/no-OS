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

ifeq (y,$(strip $(TINYIIOD)))
SRC_DIRS += $(DRIVERS)/axi_core/iio_axi_adc \
	    $(DRIVERS)/api/irq.c \
		$(NO-OS)/iio/iio_app

SRCS	+= $(PLATFORM_DRIVERS)/uart.c \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
		$(NO-OS)/util/list.c 
INCS	+= $(INCLUDE)/no-os/uart.h \
		$(INCLUDE)/no-os/list.h \
		$(INCLUDE)/no-os/irq.h \
		$(PLATFORM_DRIVERS)/irq_extra.h \
		$(PLATFORM_DRIVERS)/uart_extra.h
		
endif

SRCS += $(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/frequency/hmc7044/hmc7044.c
SRCS += $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c \
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c \
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(PLATFORM_DRIVERS)/delay.c

INCS += $(DRIVERS)/frequency/hmc7044/hmc7044.h \
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h \
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h
INCS +=	$(INCLUDE)/no-os/axi_io.h \
	$(INCLUDE)/no-os/spi.h \
	$(INCLUDE)/no-os/gpio.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/util.h
