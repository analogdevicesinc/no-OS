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

INCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.h
SRCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.c

INCS += $(INCLUDE)/no_os_net.h
INCS += $(INCLUDE)/no_os_socket.h
SRCS += $(DRIVERS)/api/no_os_net.c
SRCS += $(DRIVERS)/api/no_os_socket.c

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
