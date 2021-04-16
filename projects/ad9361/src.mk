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

ifeq (y, $(strip $(TINYIIOD)))
SRCS += $(NO-OS)/network/linux_socket/linux_socket.c	\
		$(NO-OS)/network/tcp_socket.c	\
	$(PLATFORM_DRIVERS)/linux_uart.c
endif
else
SRCS +=	$(PLATFORM_DRIVERS)/delay.c		\
	$(PLATFORM_DRIVERS)/irq.c	
endif

ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRCS += $(NO-OS)/iio/iio_app/iio_app.c \
	$(NO-OS)/util/circular_buffer.c
SRCS += $(NO-OS)/util/fifo.c						\
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
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h				\
	$(INCLUDE)/irq.h						\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h
ifeq (linux,$(strip $(PLATFORM)))
INCS +=	$(PLATFORM_DRIVERS)/linux_spi.h					\
	$(PLATFORM_DRIVERS)/linux_gpio.h	\
	$(PLATFORM_DRIVERS)/linux_uart.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(NO-OS)/network/tcp_socket.h	\
	$(NO-OS)/network/linux_socket/linux_socket.h	\
	$(NO-OS)/network/network_interface.h	\
	$(NO-OS)/network/noos_mbedtls_config.h	
endif
else
INCS +=	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h	\
	$(PLATFORM_DRIVERS)/irq_extra.h			
endif
INCS +=	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/spi.h						\
	$(INCLUDE)/gpio.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/delay.h						\
	$(INCLUDE)/util.h
ifeq (y,$(strip $(TINYIIOD)))
INCS += $(NO-OS)/iio/iio_app/iio_app.h 

INCS += $(INCLUDE)/fifo.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(NO-OS)/iio/iio_ad9361/iio_ad9361.h				\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h				\
	$(NO-OS)/iio/iio_axi_dac/iio_axi_dac.h			\
	$(INCLUDE)/circular_buffer.h
endif
ifeq (xilinx,$(strip $(PLATFORM)))
SRCS +=	$(PLATFORM_DRIVERS)/irq.c	\
		$(PLATFORM_DRIVERS)/uart.c
INCS += $(PLATFORM_DRIVERS)/irq_extra.h				\
	$(PLATFORM_DRIVERS)/spi_extra.h					\
	$(PLATFORM_DRIVERS)/gpio_extra.h	\
	$(PLATFORM_DRIVERS)/uart_extra.h	
endif
