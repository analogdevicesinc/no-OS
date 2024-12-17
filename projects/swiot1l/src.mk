LIBRARIES += lwip

include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
INCS += $(NO-OS)/network/tcp_socket.h
INCS += $(NO-OS)/network/noos_mbedtls_config.h
INCS += $(NO-OS)/network/network_interface.h

SRCS += $(NO-OS)/network/tcp_socket.c

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c 


INCS += $(INCLUDE)/no_os_delay.h     \
		$(INCLUDE)/no_os_error.h     \
		$(INCLUDE)/no_os_gpio.h      \
		$(INCLUDE)/no_os_print_log.h \
		$(INCLUDE)/no_os_spi.h       \
		$(INCLUDE)/no_os_irq.h      \
		$(INCLUDE)/no_os_init.h      \
		$(INCLUDE)/no_os_list.h      \
		$(INCLUDE)/no_os_dma.h      \
		$(INCLUDE)/no_os_mutex.h      \
		$(INCLUDE)/no_os_crc8.h      \
		$(INCLUDE)/no_os_uart.h      \
		$(INCLUDE)/no_os_mutex.h      \
		$(INCLUDE)/no_os_i2c.h      \
		$(INCLUDE)/no_os_mdio.h      \
		$(INCLUDE)/no_os_timer.h      \
		$(INCLUDE)/no_os_lf256fifo.h \
		$(INCLUDE)/no_os_util.h \
		$(INCLUDE)/no_os_units.h \
		$(INCLUDE)/no_os_alloc.h \
		$(INCLUDE)/no_os_trng.h \
		$(NO-OS)/iio/

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c  \
		$(DRIVERS)/api/no_os_spi.c  \
		$(DRIVERS)/api/no_os_uart.c \
		$(DRIVERS)/api/no_os_i2c.c \
		$(DRIVERS)/api/no_os_timer.c \
		$(DRIVERS)/api/no_os_mdio.c \
		$(DRIVERS)/api/no_os_dma.c \
		$(NO-OS)/util/no_os_list.c \
		$(NO-OS)/util/no_os_crc8.c \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_mutex.c \
		$(NO-OS)/util/no_os_alloc.c \
		$(DRIVERS)/api/no_os_trng.c \

INCS += $(DRIVERS)/temperature/adt75/iio_adt75.h
INCS += $(DRIVERS)/adc-dac/ad74413r/ad74413r.h
SRCS += $(DRIVERS)/adc-dac/ad74413r/ad74413r.c

INCS += $(DRIVERS)/digital-io/max149x6/max149x6-base.h	\
	$(DRIVERS)/digital-io/max149x6/max14906.h
SRCS += $(DRIVERS)/digital-io/max149x6/max149x6-base.c	\
	$(DRIVERS)/digital-io/max149x6/max14906.c

INCS += $(DRIVERS)/temperature/adt75/adt75.h
SRCS += $(DRIVERS)/temperature/adt75/adt75.c

INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c

ifeq (y,$(strip $(SWIOT1L_DEFAULT_FW)))

IIOD=y
CFLAGS += -DSWIOT1L_DEFAULT_FW

SRCS += $(PROJECT)/src/swiot_fw.c
INCS += $(PROJECT)/src/swiot_fw.h
INCS += $(PROJECT)/src/common/swiot.h
SRCS += $(PROJECT)/src/common/swiot.c
SRC_DIRS += $(NO-OS)/iio/iio_app
SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h


INCS += $(DRIVERS)/adc-dac/ad74413r/iio_ad74413r.h
SRCS += $(DRIVERS)/adc-dac/ad74413r/iio_ad74413r.c
INCS += $(DRIVERS)/digital-io/max149x6/iio_max14906.h
SRCS += $(DRIVERS)/digital-io/max149x6/iio_max14906.c
INCS += $(DRIVERS)/temperature/adt75/iio_adt75.h
SRCS += $(DRIVERS)/temperature/adt75/iio_adt75.c

endif

ifeq (y,$(strip $(SWIOT1L_MQTT_EXAMPLE)))

ifndef SWIOT1L_MQTT_SERVER_IP
SWIOT1L_MQTT_SERVER_IP=10.32.51.2
endif

ifndef SWIOT1L_MQTT_SERVER_PORT
SWIOT1L_MQTT_SERVER_PORT=8883
endif


CFLAGS += -DSWIOT1L_MQTT_SERVER_IP=\"$(SWIOT1L_MQTT_SERVER_IP)\"
CFLAGS += -DSWIOT1L_MQTT_SERVER_PORT=$(SWIOT1L_MQTT_SERVER_PORT)

MBED_TLS_CONFIG_FILE = $(PROJECT)/src/app/noos_mbedtls_config.h

LIBRARIES += mbedtls
INCS += $(NO-OS)/libraries/mbedtls/include/mbedtls/ssl.h
INCS += $(NO-OS)/libraries/mbedtls/include/mbedtls/memory_buffer_alloc.h
SRC_DIRS += $(NO-OS)/libraries/mbedtls/library


CFLAGS += -DSWIOT1L_MQTT_EXAMPLE
CFLAGS += -DNO_OS_LWIP_NETWORKING

LIBRARIES += mqtt
SRCS += $(PROJECT)/src/examples/swiot1l-mqtt/swiot1l_mqtt.c
INCS += $(PROJECT)/src/examples/swiot1l-mqtt/swiot1l_mqtt.h
endif
