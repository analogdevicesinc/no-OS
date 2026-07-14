LIBRARIES += iio

CFLAGS += -DNO_OS_NET \
	-DDISABLE_SECURE_SOCKET

SRCS += $(NO-OS)/network/linux/linux_net.c \
	$(NO-OS)/drivers/api/no_os_net.c     \
	$(NO-OS)/drivers/api/no_os_socket.c

INCS += $(NO-OS)/network/linux/linux_net.h \
	$(INCLUDE)/no_os_net.h               \
	$(INCLUDE)/no_os_socket.h


SRCS += $(NO-OS)/util/no_os_circular_buffer.c
INCS += $(INCLUDE)/no_os_circular_buffer.h

SRCS += $(DRIVERS)/platform/linux/linux_uart.c \
	$(DRIVERS)/platform/linux/linux_delay.c


INCS += $(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_trng.h
