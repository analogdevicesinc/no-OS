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

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h \
	$(PROJECT)/src/common/app_config.h

SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

SRCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c \
	$(DRIVERS)/axi_core/tx_generator/tx_generator.c \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.c \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.c \
	$(DRIVERS)/frequency/si5391/si5391.c \
	$(PROJECT)/src/devices/ad9081k/ad9081k.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_clk.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_list.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(NO-OS)/jesd204/jesd204-core.c \
	$(NO-OS)/jesd204/jesd204-fsm.c
SRCS += $(DRIVERS)/axi_core/jesd204/axi_adxcvr.c

INCS += $(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h \
	$(DRIVERS)/axi_core/tx_generator/tx_generator.h \
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_rx.h \
	$(DRIVERS)/axi_core/jesd204/axi_jesd204_tx.h \
	$(DRIVERS)/frequency/si5391/si5391.h \
	$(PROJECT)/src/devices/ad9081k/ad9081k.h
INCS += $(DRIVERS)/axi_core/jesd204/axi_adxcvr.h
LIBRARIES += lwip
ALL_IGNORED_FILES += $(NO-OS)/libraries/lwip/lwip/src/netif/slipif.c

SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/xemacps/lwip_xemacps.c \
	$(NO-OS)/network/lwip_raw_socket/lwip_socket.c \
	$(DRIVERS)/net/xemacps/no_os_xemacps.c
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/xemacps/lwip_xemacps.h \
	$(NO-OS)/network/lwip_raw_socket/lwip_socket.h \
	$(DRIVERS)/net/xemacps/no_os_xemacps.h

SRCS += $(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_fifo.c

LIBRARIES += iio
SRCS += $(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c \
	$(PROJECT)/src/devices/ad9081k/iio_ad9081k.c \
	$(NO-OS)/iio/iio_app/iio_app.c
INCS += $(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h \
	$(PROJECT)/src/devices/ad9081k/iio_ad9081k.h \
	$(NO-OS)/iio/iio_app/iio_app.h

# K IIOD control + data plane (k_data engine + 6 IIO devices)
SRCS += $(PROJECT)/src/k_iio/k_data.c \
	$(PROJECT)/src/k_iio/iio_k_tx.c \
	$(PROJECT)/src/k_iio/iio_k_rx.c \
	$(PROJECT)/src/k_iio/iio_k_regmap.c \
	$(PROJECT)/src/k_iio/iio_k_hdlregs.c \
	$(PROJECT)/src/k_iio/iio_k_gpio.c \
	$(PROJECT)/src/k_iio/iio_k_time.c
INCS += $(PROJECT)/src/k_iio/k_data.h \
	$(PROJECT)/src/k_iio/iio_k_tx.h \
	$(PROJECT)/src/k_iio/iio_k_rx.h \
	$(PROJECT)/src/k_iio/iio_k_regmap.h \
	$(PROJECT)/src/k_iio/iio_k_hdlregs.h \
	$(PROJECT)/src/k_iio/iio_k_gpio.h \
	$(PROJECT)/src/k_iio/iio_k_time.h

INCS += $(INCLUDE)/no_os_axi_io.h \
	$(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_clk.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/jesd204.h \
	$(NO-OS)/jesd204/jesd204-priv.h
