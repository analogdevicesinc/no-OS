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

SRCS += $(PROJECT)/src/main.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PROJECT)/src/app_iio.c
LIBRARIES += iio
endif
SRCS += $(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/gpio/gpio.c						\
	$(DRIVERS)/frequency/hmc7044/hmc7044.c				\
	$(DRIVERS)/adc/ad9208/ad9208.c 					\
	$(DRIVERS)/adc/ad9208/ad9208_api/ad9208_adc_api.c 		\
	$(DRIVERS)/adc/ad9208/ad9208_api/ad9208_api.c 			\
	$(DRIVERS)/adc/ad9208/ad9208_api/ad9208_jesd_api.c 		\
	$(DRIVERS)/adc/ad9208/ad9208_api/ad9208_reg.c 			\
	$(DRIVERS)/adc/ad9208/ad9208_api/ad9208_signal_monitor_api.c 
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c		\
	$(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c			\
	$(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/xilinx_spi.c				\
	$(PLATFORM_DRIVERS)/xilinx_gpio.c				\
	$(PLATFORM_DRIVERS)/delay.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(NO-OS)/util/fifo.c						\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.c				\
	$(NO-OS)/util/list.c						\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c
endif
INCS += $(PROJECT)/src/parameters.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(PROJECT)/src/app_iio.h
endif
INCS += $(DRIVERS)/frequency/hmc7044/hmc7044.h				\
	$(DRIVERS)/adc/ad9208/ad9208.h					\
	$(DRIVERS)/adc/ad9208/ad9208_api/ad9208_api.h			\
	$(DRIVERS)/adc/ad9208/ad9208_api/ad9208_reg.h			\
	$(DRIVERS)/adc/ad9208/ad9208_api/api_def.h			\
	$(DRIVERS)/adc/ad9208/ad9208_api/api_errors.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
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
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h                                \
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h
endif
