IIOD = y
CFLAGS += -DNO_OS_LWIP_NETWORKING
LIBRARIES += lwip

ifdef ADIN1110_STATIC_IP
NO_OS_IP ?= 192.168.90.60
NO_OS_NETMASK ?= 255.255.0.0
NO_OS_GATEWAY ?= 0.0.0.0
endif


INCS += $(PROJECT)/src/common/iio_cn0391.h
SRCS += $(PROJECT)/src/common/iio_cn0391.c

ifdef CN0391_IIO_SUPPORT
CFLAGS += -DCN0391_IIO_SUPPORT
endif

INCS += $(INCLUDE)/no_os_crc8.h
SRCS += $(NO-OS)/util/no_os_crc8.c

INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(DRIVERS)/net/oa_tc6/oa_tc6.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(DRIVERS)/net/oa_tc6/oa_tc6.c

SRCS += $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.c
