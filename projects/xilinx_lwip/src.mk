LIBRARIES += lwip

# slipif.c requires sio_* functions which we don't implement.
ALL_IGNORED_FILES += $(NO-OS)/libraries/lwip/lwip/src/netif/slipif.c

CFLAGS += -DGEM_INSTANCE=1
CFLAGS += -DSSIZE_MAX=INT_MAX

SRCS += $(PROJECT)/src/platform/xilinx/main.c
SRCS += $(PROJECT)/src/platform/xilinx/parameters.c
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/common/app_config.h
INCS += $(PROJECT)/src/common/common_data.h
INCS += $(PROJECT)/src/platform/platform_includes.h
INCS += $(PROJECT)/src/platform/xilinx/parameters.h

ifeq ($(ECHO_EXAMPLE),y)
CFLAGS += -DECHO_EXAMPLE
SRCS += $(PROJECT)/src/examples/echo_lwip/echo_lwip_example.c
INCS += $(PROJECT)/src/examples/echo_lwip/echo_lwip_example.h
# lwIP TCP echo server
SRCS += $(NO-OS)/libraries/lwip/lwip/contrib/apps/tcpecho_raw/tcpecho_raw.c
INCS += $(NO-OS)/libraries/lwip/lwip/contrib/apps/tcpecho_raw/tcpecho_raw.h
else
IIOD = y
SRCS += $(PROJECT)/src/examples/iio_lwip/iio_lwip_example.c
INCS += $(PROJECT)/src/examples/iio_lwip/iio_lwip_example.h
# Virtual IIO demo devices
SRCS += $(DRIVERS)/adc/adc_demo/adc_demo.c \
        $(DRIVERS)/adc/adc_demo/iio_adc_demo.c \
        $(DRIVERS)/dac/dac_demo/dac_demo.c \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.c
INCS += $(DRIVERS)/adc/adc_demo/adc_demo.h \
        $(DRIVERS)/adc/adc_demo/iio_adc_demo.h \
        $(DRIVERS)/dac/dac_demo/dac_demo.h \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.h
endif

# CAPI Ethernet PHY + MAC common layer
INCS += $(NO-OS)/capi/include/capi_eth_phy.h
SRCS += $(NO-OS)/capi/src/capi_eth_phy.c
INCS += $(NO-OS)/capi/include/capi_eth_mac.h
SRCS += $(NO-OS)/capi/src/capi_eth_mac.c

# Marvell 88E1510 PHY driver (CAPI)
INCS += $(DRIVERS)/net/phy/mrvl_88e1510/capi_marvell_88e1510.h
SRCS += $(DRIVERS)/net/phy/mrvl_88e1510/capi_marvell_88e1510.c

# XEmacPs MAC driver (PS GEM, CAPI)
INCS += $(DRIVERS)/net/xemacps/capi_xemacps.h
SRCS += $(DRIVERS)/net/xemacps/capi_xemacps.c

# # XAxiEmac MAC driver (PL AXI Ethernet) - stubs
# INCS += $(DRIVERS)/net/xaxiemac/no_os_xaxiemac.h
# SRCS += $(DRIVERS)/net/xaxiemac/no_os_xaxiemac.c

# Generic CAPI lwIP glue (works with any capi_eth_mac + capi_eth_phy pair)
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/capi/lwip_capi.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/capi/lwip_capi.c

# HAL
SRCS += $(DRIVERS)/api/no_os_uart.c \
        $(DRIVERS)/api/no_os_irq.c \
        $(NO-OS)/util/no_os_lf256fifo.c \
        $(NO-OS)/util/no_os_list.c \
        $(NO-OS)/util/no_os_util.c \
        $(NO-OS)/util/no_os_alloc.c \
        $(NO-OS)/util/no_os_mutex.c \
        $(NO-OS)/util/no_os_fifo.c

INCS += $(INCLUDE)/no_os_delay.h \
        $(INCLUDE)/no_os_error.h \
        $(INCLUDE)/no_os_irq.h \
        $(INCLUDE)/no_os_uart.h \
        $(INCLUDE)/no_os_lf256fifo.h \
        $(INCLUDE)/no_os_list.h \
        $(INCLUDE)/no_os_util.h \
        $(INCLUDE)/no_os_alloc.h \
        $(INCLUDE)/no_os_mutex.h \
        $(INCLUDE)/no_os_fifo.h \
        $(INCLUDE)/no_os_gpio.h \
        $(INCLUDE)/no_os_print_log.h

# Xilinx platform drivers
SRCS += $(PLATFORM_DRIVERS)/xilinx_uart.c \
        $(PLATFORM_DRIVERS)/xilinx_gpio.c \
        $(PLATFORM_DRIVERS)/xilinx_irq.c \
        $(PLATFORM_DRIVERS)/xilinx_delay.c

INCS += $(PLATFORM_DRIVERS)/xilinx_uart.h \
        $(PLATFORM_DRIVERS)/xilinx_gpio.h \
        $(PLATFORM_DRIVERS)/xilinx_irq.h
