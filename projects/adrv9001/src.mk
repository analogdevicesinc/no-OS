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
# app
SRCS +=	$(DRIVERS)/rf-transceiver/navassa/adrv9002_conv.c \
	$(DRIVERS)/rf-transceiver/navassa/adrv9002.c \
	$(PROJECT)/src/app/headless.c
INCS += $(DRIVERS)/rf-transceiver/navassa/adrv9002.h \
	$(PROJECT)/src/app/ORxGainTable.h \
	$(PROJECT)/src/app/RxGainTable.h \
	$(PROJECT)/src/app/TxAttenTable.h \
	$(PROJECT)/src/app/Navassa_CMOS_profile.h \
	$(PROJECT)/src/app/Navassa_LVDS_profile.h
# hal
SRCS += $(PROJECT)/src/hal/no_os_platform.c
INCS += $(PROJECT)/src/hal/parameters.h \
	$(PROJECT)/src/hal/no_os_platform.h \
	$(PROJECT)/src/hal/adi_platform.h \
	$(PROJECT)/src/hal/adi_platform_types.h \
	$(PROJECT)/src/firmware/Navassa_EvaluationFw.h \
	$(PROJECT)/src/firmware/Navassa_Stream.h
# no-OS drivers
SRCS += $(PLATFORM_DRIVERS)/xilinx_gpio.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/api/spi.c \
	$(PLATFORM_DRIVERS)/xilinx_spi.c \
	$(PLATFORM_DRIVERS)/delay.c \
	$(NO-OS)/util/util.c \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(PLATFORM_DRIVERS)/axi_io.c
INCS +=	$(INCLUDE)/no-os/spi.h \
	$(PLATFORM_DRIVERS)/spi_extra.h \
	$(INCLUDE)/no-os/gpio.h \
	$(PLATFORM_DRIVERS)/gpio_extra.h \
	$(INCLUDE)/no-os/error.h \
	$(INCLUDE)/no-os/delay.h \
	$(INCLUDE)/no-os/util.h \
	$(INCLUDE)/no-os/print_log.h \
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(INCLUDE)/no-os/axi_io.h

# Navassa API sources
SRC_DIRS += $(DRIVERS)/rf-transceiver/navassa

# IIO
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app
SRCS += $(PLATFORM_DRIVERS)/uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
	$(NO-OS)/util/list.c \
	$(NO-OS)/util/fifo.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c \
	$(DRIVERS)/api/irq.c
INCS += $(INCLUDE)/no-os/uart.h \
	$(INCLUDE)/no-os/irq.h \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h \
	$(INCLUDE)/no-os/fifo.h \
	$(INCLUDE)/no-os/list.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif
