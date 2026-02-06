IIOD = y
CFLAGS += -DHP_IIO_EXAMPLE
SRC_DIRS += $(PROJECT)/src/examples/iio_example

SRCS += $(DRIVERS)/adc/ad7124/ad7124.c
INCS += $(DRIVERS)/adc/ad7124/ad7124.h
SRCS += $(DRIVERS)/adc/ad7124/ad7124_regs.c
INCS += $(DRIVERS)/adc/ad7124/ad7124_regs.h
SRCS += $(DRIVERS)/adc/ad7124/iio_ad7124.c
INCS += $(DRIVERS)/adc/ad7124/iio_ad7124.h

SRCS += $(PROJECT)/src/drivers/max6613/max6613.c
INCS += $(PROJECT)/src/drivers/max6613/max6613.h

SRCS += $(PROJECT)/src/drivers/max42500/max42500.c
SRCS += $(PROJECT)/src/drivers/max42500/max42500_regs.c
INCS += $(PROJECT)/src/drivers/max42500/max42500.h
INCS += $(PROJECT)/src/drivers/max42500/max42500_regs.h

SRCS += $(PROJECT)/src/common/temperature_api.c
INCS += $(PROJECT)/src/common/temperature_api.h

INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c
INCS += $(DRIVERS)/net/oa_tc6/oa_tc6.h
SRCS += $(DRIVERS)/net/oa_tc6/oa_tc6.c

LIBRARIES += lwip
CFLAGS += -DNO_OS_STATIC_IP
CFLAGS += -DNO_OS_LWIP_NETWORKING
CFLAGS += -DCONFIG_OA_TC6_PROTECTION=1
