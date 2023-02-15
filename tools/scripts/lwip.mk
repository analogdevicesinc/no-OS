LWIP_DIR = $(MAXIM_LIBRARIES)/lwIP

CFLAGS += -DLWIP_PROVIDE_ERRNO

CFLAGS += -I$(LWIP_DIR)/include
CFLAGS += -I$(LWIP_DIR)/include/compat
CFLAGS += -I$(LWIP_DIR)/include/compat/posix
CFLAGS += -I$(LWIP_DIR)/include/compat/posix/arpa
CFLAGS += -I$(LWIP_DIR)/include/compat/posix/net
CFLAGS += -I$(LWIP_DIR)/include/compat/posix/sys
CFLAGS += -I$(LWIP_DIR)/include/compat/stdc
CFLAGS += -I$(LWIP_DIR)/include/lwip
CFLAGS += -I$(LWIP_DIR)/include/lwip/apps
CFLAGS += -I$(LWIP_DIR)/include/lwip/priv
CFLAGS += -I$(LWIP_DIR)/include/lwip/prot
CFLAGS += -I$(LWIP_DIR)/include/Maxim
CFLAGS += -I$(LWIP_DIR)/include/Maxim/arch
CFLAGS += -I$(LWIP_DIR)/include/netif
CFLAGS += -I$(LWIP_DIR)/include/netif/ppp
CFLAGS += -I$(LWIP_DIR)/include/netif/ppp/polarssl

SRC_DIRS += $(LWIP_DIR)/api
SRC_DIRS += $(LWIP_DIR)/core
#SRC_DIRS += $(LWIP_DIR)/Maxim
SRC_DIRS += $(LWIP_DIR)/netif