CFLAGS += -DNO_OS_LWIP_NETWORKING
LIBRARIES += lwip

NO_OS_IP=192.168.97.40
NO_OS_NETMASK=255.255.0.0
NO_OS_GATEWAY=192.168.97.1

# Override the shared lwipopts.h with a project specific one.
# This -I must appear before the auto-generated include paths (added later
# by generic.mk line 252), so it is placed directly in CFLAGS here.
CFLAGS += -I$(PROJECT)/src/examples/iperf_lwip_adin1110

INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(DRIVERS)/net/oa_tc6/oa_tc6.h
INCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
INCS += $(INCLUDE)/no_os_crc8.h

SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(DRIVERS)/net/oa_tc6/oa_tc6.c
SRCS += $(NO-OS)/network/lwip_raw_socket/lwip_socket.c
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c
SRCS += $(PROJECT)/src/examples/iperf_lwip_adin1110/iperf_lwip_adin1110.c

CFLAGS += -DCONFIG_OA_RX_FRAME_BUFF_NUM=10
