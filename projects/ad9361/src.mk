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
SRCS += $(DRIVERS)/rf-transceiver/ad9361/ad9361_api.c \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361.c \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_conv.c \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_util.c
SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/axi_sysid/axi_sysid.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c
SRCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_axi_io.c
SRCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c
ifeq (linux,$(strip $(PLATFORM)))
SRCS +=	$(PLATFORM_DRIVERS)/linux_delay.c
else
SRCS +=	$(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c
endif
ifeq (y,$(strip $(IIOD)))
LIBRARIES += iio

ifeq (linux,$(strip $(PLATFORM)))
CFLAGS += -DNO_OS_NETWORKING \
		-DDISABLE_SECURE_SOCKET
SRCS += $(NO-OS)/network/linux_socket/linux_socket.c \
		$(NO-OS)/network/tcp_socket.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(PLATFORM_DRIVERS)/linux_uart.c
else
SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(NO-OS)/util/no_os_lf256fifo.c
endif

SRCS += $(NO-OS)/util/no_os_fifo.c \
	$(NO-OS)/util/no_os_list.c \
	$(DRIVERS)/rf-transceiver/ad9361/iio_ad9361.c \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c \
	$(NO-OS)/iio/iio_app/iio_app.c \
	$(NO-OS)/util/no_os_circular_buffer.c
endif
INCS += $(DRIVERS)/rf-transceiver/ad9361/common.h \
	$(PROJECT)/src/app_config.h
INCS += $(DRIVERS)/rf-transceiver/ad9361/ad9361.h \
	$(PROJECT)/src/parameters.h \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_util.h \
	$(DRIVERS)/rf-transceiver/ad9361/ad9361_api.h
INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/axi_dac_core/axi_dac_core.h \
	$(INCLUDE)/no_os_irq.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/axi_sysid/axi_sysid.h
ifeq (linux,$(strip $(PLATFORM)))
CFLAGS += -DPLATFORM_MB
INCS +=	$(PLATFORM_DRIVERS)/linux_spi.h \
	$(PLATFORM_DRIVERS)/linux_gpio.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(PLATFORM_DRIVERS)/linux_uart.h
endif
INCS +=	$(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_print_log.h
ifeq (y,$(strip $(IIOD)))

ifeq (linux,$(strip $(PLATFORM)))
INCS += $(NO-OS)/network/tcp_socket.h \
	$(NO-OS)/network/linux_socket/linux_socket.h \
	$(NO-OS)/network/network_interface.h \
	$(NO-OS)/network/noos_mbedtls_config.h
endif

INCS += $(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(DRIVERS)/rf-transceiver/ad9361/iio_ad9361.h \
	$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h \
	$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h \
	$(NO-OS)/iio/iio_app/iio_app.h \
	$(INCLUDE)/no_os_circular_buffer.h

SRCS += $(DRIVERS)/api/no_os_uart.c
endif
ifeq (xilinx,$(strip $(PLATFORM)))
SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
	$(DRIVERS)/api/no_os_irq.c
	
INCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h	
endif
