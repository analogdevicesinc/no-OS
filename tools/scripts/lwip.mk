LWIP_DIR := $(NO-OS)/libraries/lwip/lwip

CFLAGS += -DLWIP_PROVIDE_ERRNO

INCS += $(LWIP_DIR)/src/include/lwip
INCS += $(LWIP_DIR)/src/include/compat
INCS += $(LWIP_DIR)/src/include/netif

SRC_DIRS += $(LWIP_DIR)/src/api
SRC_DIRS += $(LWIP_DIR)/src/core
SRC_DIRS += $(LWIP_DIR)/src/netif
SRC_DIRS += $(LWIP_DIR)/src/apps/mdns
SRC_DIRS += $(LWIP_DIR)/src/apps/lwiperf

ifeq (y,$(strip $(CONFIG_NO_OS_NET)))
INCS += $(NO-OS)/include/no_os_net.h
INCS += $(NO-OS)/include/no_os_socket.h
INCS += $(NO-OS)/network/lwip/lwip_net.h
SRCS += $(NO-OS)/network/lwip/lwip_net.c
else
INCS += $(NO-OS)/network/network_interface.h
INCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.h
INCS += $(NO-OS)/network/tcp_socket.h
SRCS += $(NO-OS)/network/tcp_socket.c
SRCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.c
endif

ifdef CONFIG_NO_OS_IP
CFLAGS += -DCONFIG_NO_OS_IP=\"$(CONFIG_NO_OS_IP)\"
endif

ifdef CONFIG_NO_OS_NETMASK
CFLAGS += -DCONFIG_NO_OS_NETMASK=\"$(CONFIG_NO_OS_NETMASK)\"
endif

ifdef CONFIG_NO_OS_GATEWAY
CFLAGS += -DCONFIG_NO_OS_GATEWAY=\"$(CONFIG_NO_OS_GATEWAY)\"
endif

ifdef CONFIG_NO_OS_DOMAIN_NAME
CFLAGS += -DCONFIG_NO_OS_DOMAIN_NAME=\"$(CONFIG_NO_OS_DOMAIN_NAME)\"
endif
