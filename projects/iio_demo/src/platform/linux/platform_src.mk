CFLAGS += -DENABLE_IIO_NETWORK \
	-DDISABLE_SECURE_SOCKET

LIBRARIES += iio

SRCS += $(NO-OS)/network/linux_socket/linux_socket.c \
	$(NO-OS)/network/tcp_socket.c
	
INCS += $(NO-OS)/network/linux_socket/linux_socket.h \
	$(NO-OS)/network/tcp_socket.h                \
	$(NO-OS)/network/network_interface.h         \
	$(NO-OS)/network/noos_mbedtls_config.h
	

SRCS += $(NO-OS)/util/no_os_circular_buffer.c
INCS += $(INCLUDE)/no_os_circular_buffer.h

SRCS += $(DRIVERS)/platform/generic/no_os_uart.c \
	$(DRIVERS)/platform/generic/delay.c


INCS += $(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_trng.h		
