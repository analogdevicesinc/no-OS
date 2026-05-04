IIOD = y
CFLAGS += -DNO_OS_LWIP_NETWORKING
LIBRARIES += lwip

NO_OS_IP=192.168.97.40
NO_OS_NETMASK=255.255.0.0
NO_OS_GATEWAY=192.168.97.1

# Override the shared lwipopts.h with a project specific one.
# This -I must appear before the auto-generated include paths (added later
# by generic.mk line 252), so it is placed directly in CFLAGS here.
CFLAGS += -I$(PROJECT)/src/examples/iio_lwip_example

INCS += $(DRIVERS)/temperature/adt7420/adt7420.h
INCS += $(DRIVERS)/temperature/adt7420/iio_adt7420.h
INCS += $(INCLUDE)/no_os_i2c.h
INCS += $(DRIVERS)/net/adin1140/adin1140.h
INCS += $(DRIVERS)/net/oa_tc6/oa_tc6.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1140/lwip_adin1140.h
INCS += $(INCLUDE)/no_os_crc8.h

SRCS += $(DRIVERS)/temperature/adt7420/adt7420.c
SRCS += $(DRIVERS)/temperature/adt7420/iio_adt7420.c
SRCS += $(DRIVERS)/api/no_os_i2c.c
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1140/lwip_adin1140.c
SRCS += $(NO-OS)/util/no_os_crc8.c
SRCS += $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.c

CFLAGS += -DCONFIG_OA_RX_FRAME_BUFF_NUM=10