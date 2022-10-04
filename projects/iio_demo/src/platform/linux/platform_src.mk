CFLAGS += -DNO_OS_NETWORKING \
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

SRCS += $(DRIVERS)/platform/linux/linux_uart.c \
	$(DRIVERS)/platform/linux/linux_delay.c


INCS += $(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_trng.h		
