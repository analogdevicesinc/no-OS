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

INCS += $(NO-OS)/network/network_interface.h
INCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.h
INCS += $(NO-OS)/network/tcp_socket.h
SRCS += $(NO-OS)/network/tcp_socket.c
SRCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.c

ifdef NO_OS_IP
CFLAGS += -DNO_OS_IP=\"$(NO_OS_IP)\"
endif

ifdef NO_OS_NETMASK
CFLAGS += -DNO_OS_NETMASK=\"$(NO_OS_NETMASK)\"
endif

ifdef NO_OS_GATEWAY
CFLAGS += -DNO_OS_GATEWAY=\"$(NO_OS_GATEWAY)\"
endif

ifdef NO_OS_DOMAIN_NAME
CFLAGS += -DNO_OS_DOMAIN_NAME=\"$(NO_OS_DOMAIN_NAME)\"
endif
