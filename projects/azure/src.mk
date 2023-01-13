
LDFLAGS += -lrt

SRCS +=	$(PROJECT)/src/azure.c

INCS +=	$(INCLUDE)/no_os_error.h			\
	$(INCLUDE)/no_os_delay.h			\
	$(INCLUDE)/no_os_util.h				\
	$(INCLUDE)/no_os_timer.h			\
	$(PLATFORM_DRIVERS)/linux_timer.h

SRCS +=	$(DRIVERS)/api/no_os_timer.c			\
	$(PLATFORM_DRIVERS)/linux_delay.c		\
	$(PLATFORM_DRIVERS)/linux_timer.c

INCS += $(INCLUDE)/no_os_delay.h			\
	$(INCLUDE)/no_os_util.h				\
	$(INCLUDE)/no_os_trng.h

SRCS += $(NO-OS)/network/linux_socket/linux_socket.c	\
	$(NO-OS)/network/tcp_socket.c

INCS += $(NO-OS)/network/linux_socket/linux_socket.h	\
	$(NO-OS)/network/tcp_socket.h			\
	$(NO-OS)/network/network_interface.h		\
	$(NO-OS)/network/noos_mbedtls_config.h

LIBRARIES += mqtt
LIBRARIES += mbedtls

