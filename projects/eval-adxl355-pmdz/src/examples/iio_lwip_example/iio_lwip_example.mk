IIOD = y
CFLAGS += -DNO_OS_LWIP_NETWORKING
LIBRARIES += lwip

INCS += $(DRIVERS)/accel/adxl355/iio_adxl355.h
SRCS += $(DRIVERS)/accel/adxl355/iio_adxl355.c
INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c
SRCS += $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.c
INCS += $(PROJECT)/src/examples/iio_lwip_example/iio_lwip_example.h