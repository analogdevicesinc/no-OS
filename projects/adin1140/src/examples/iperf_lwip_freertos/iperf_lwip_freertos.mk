CFLAGS += -DNO_OS_LWIP_NETWORKING
LIBRARIES += lwip
LIBRARIES += freertos

ALL_IGNORED_FILES += $(PLATFORM_DRIVERS)/maxim_delay.c

NO_OS_IP=192.168.97.40
NO_OS_NETMASK=255.255.0.0
NO_OS_GATEWAY=192.168.97.1

CFLAGS += -I$(PROJECT)/src/examples/iperf_lwip_freertos

INCS += $(DRIVERS)/net/adin1140/adin1140.h
INCS += $(DRIVERS)/net/oa_tc6/oa_tc6.h
INCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1140/lwip_adin1140.h
INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(INCLUDE)/no_os_semaphore.h

SRCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.c
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1140/lwip_adin1140.c
SRCS += $(NO-OS)/util/no_os_crc8.c
SRCS += $(PROJECT)/src/examples/iperf_lwip_freertos/iperf_lwip_freertos.c

FREERTOS_CONFIG_PATH = $(PROJECT)/src/FreeRTOSConfig.h

CFLAGS += -DCONFIG_OA_RX_FRAME_BUFF_NUM=10
CFLAGS += -DCONFIG_OA_THREAD_RX_LIMIT=10
CFLAGS += -DCONFIG_OA_SPI_BUFF_LEN=3264
