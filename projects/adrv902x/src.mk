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
include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h \
	$(PROJECT)/src/common/app_config.h \
	$(PROJECT)/src/common/ADRV9025_RxGainTable.h \
	$(PROJECT)/src/common/ADRV9025_TxAttenTable.h \
	$(PROJECT)/src/common/hal/no_os_platform.h \
	$(PROJECT)/src/common/firmware/ADRV9025_DPDCORE_FW.h \
	$(PROJECT)/src/common/firmware/ADRV9025_FW.h \
	$(PROJECT)/src/common/firmware/stream_image_6E3E00EFB74FE7D465FA88A171B81B8F.h \
	$(PROJECT)/src/common/firmware/ActiveUseCase_profile.h \
	$(PROJECT)/src/common/firmware/ActiveUtilInit_profile.h

SRCS += $(PROJECT)/src/common/common_data.c \
	$(PROJECT)/src/common/hal/no_os_platform.c

INCS += $(PROJECT)/src/platform/platform_includes.h \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c \
	$(DRIVERS)/frequency/ad9528/ad9528.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_clk.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c\
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/jesd204/jesd204-core.c \
	$(NO-OS)/jesd204/jesd204-fsm.c
SRCS += $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.c
ifeq (y,$(strip $(IIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_fifo.c \
	$(NO-OS)/util/no_os_list.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c \
	$(NO-OS)/iio/iio_app/iio_app.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c
endif

# Madura API sources
SRC_DIRS += $(DRIVERS)/rf-transceiver/madura

INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h \
	$(DRIVERS)/frequency/ad9528/ad9528.h
INCS += $(DRIVERS)/axi_core/jesd204/axi_adxcvr.h \
	$(DRIVERS)/axi_core/clk_axi_clkgen/clk_axi_clkgen.h
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_clk.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/jesd204.h \
	$(NO-OS)/jesd204/jesd204-priv.h
ifeq (y,$(strip $(IIOD)))
INCS += $(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h \
	$(NO-OS)/iio/iio_app/iio_app.h \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif

