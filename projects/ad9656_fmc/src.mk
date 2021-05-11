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
	$(PROJECT)/src/app/ad9553.c
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(PROJECT)/src/app/app_iio.c
endif
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c		\
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c			\
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c			\
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.c		\
        $(DRIVERS)/adc/ad9656/ad9656.c					\
        $(DRIVERS)/spi/spi.c						\
        $(NO-OS)/util/util.c
ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(NO-OS)/util/fifo.c					\
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c		\
	$(NO-OS)/util/list.c						\
	$(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c
endif
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c					\
        $(PLATFORM_DRIVERS)/xilinx_spi.c				\
        $(PLATFORM_DRIVERS)/delay.c
INCS +=	$(PROJECT)/src/app/app_config.h					\
        $(PROJECT)/src/devices/adi_hal/parameters.h                     \
        $(PROJECT)/src/app/ad9508.h					\
        $(PROJECT)/src/app/ad9553.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
        $(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
        $(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h		\
        $(DRIVERS)/axi_core/jesd204/axi_adxcvr.h			\
        $(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h			\
        $(DRIVERS)/axi_core/jesd204/xilinx_transceiver.h		\
        $(DRIVERS)/adc/ad9656/ad9656.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(PROJECT)/src/app/app_iio.h
endif
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h
INCS +=	$(INCLUDE)/axi_io.h						\
        $(INCLUDE)/spi.h						\
        $(INCLUDE)/error.h						\
        $(INCLUDE)/delay.h						\
        $(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(INCLUDE)/fifo.h					\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h                                \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h
endif