CFLAGS += -DNO_OS_NETWORKING \
	-DDISABLE_SECURE_SOCKET

LIBRARIES += iio

SRCS += $(NO-OS)/network/linux_socket/linux_socket.c \
	$(DRIVERS)/api/no_os_net.c                   \
	$(DRIVERS)/api/no_os_socket.c

INCS += $(NO-OS)/network/linux_socket/linux_socket.h \
	$(INCLUDE)/no_os_net.h                       \
	$(INCLUDE)/no_os_socket.h                    \
	$(NO-OS)/network/noos_mbedtls_config.h
	

SRCS += $(NO-OS)/util/no_os_circular_buffer.c
INCS += $(INCLUDE)/no_os_circular_buffer.h

SRCS += $(DRIVERS)/platform/linux/linux_uart.c \
	$(DRIVERS)/platform/linux/linux_delay.c


INCS += $(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_trng.h		
