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

TINYIIOD=y

SRCS += $(PROJECT)/src/main.c
SRCS += $(DRIVERS)/rf-transceiver/ad9361/ad9361_api.c			\
	$(DRIVERS)/rf-transceiver/ad9361/ad9361.c			\
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_conv.c			\
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_util.c
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(DRIVERS)/spi/spi.c						\
	$(DRIVERS)/gpio/gpio.c						\
	$(NO-OS)/util/util.c
SRCS +=	$(PLATFORM_DRIVERS)/axi_io.c
SRCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c				\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c
ifeq (linux,$(strip $(PLATFORM)))
SRCS +=	$(PLATFORM_DRIVERS)/linux_delay.c
else
SRCS +=	$(PLATFORM_DRIVERS)/delay.c
endif
ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c					\
	$(NO-OS)/util/xml.c						\
	$(NO-OS)/util/fifo.c						\
	$(NO-OS)/util/list.c						\
	$(NO-OS)/iio/iio_ad9361/iio_ad9361.c				\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.c				\
	$(NO-OS)/iio/iio_axi_dac/iio_axi_dac.c
endif
INCS += $(DRIVERS)/rf-transceiver/ad9361/common.h			\
	$(PROJECT)/src/app_config.h
INCS += $(DRIVERS)/rf-transceiver/ad9361/ad9361.h			\
	$(PROJECT)/src/parameters.h					\
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_util.h			\
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_api.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h
ifeq (linux,$(strip $(PLATFORM)))
INCS +=	$(PLATFORM_DRIVERS)/linux_spi.h					\
	$(PLATFORM_DRIVERS)/linux_gpio.h
else
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h
endif
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(INCLUDE)/xml.h						\
	$(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h				\
	$(NO-OS)/iio/iio_ad9361/iio_ad9361.h				\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h				\
	$(NO-OS)/iio/iio_axi_dac/iio_axi_dac.h
endif
