INCS += $(PLATFORM_DRIVERS)/linux_gpio.h     	\
        $(PLATFORM_DRIVERS)/linux_socket.h     	\
        $(PLATFORM_DRIVERS)/linux_spi.h 	\
        $(INCLUDE)/no_os_lf256fifo.h 		\
	$(PLATFORM_DRIVERS)/linux_uart.h

INCS += $(NO-OS)/network/tcp_socket.h 			\
	$(NO-OS)/network/linux_socket/linux_socket.h 	\
	$(NO-OS)/network/network_interface.h 		\
	$(NO-OS)/network/noos_mbedtls_config.h

SRCS += $(PLATFORM_DRIVERS)/linux_delay.c    	\
        $(PLATFORM_DRIVERS)/linux_gpio.c     	\
        $(PLATFORM_DRIVERS)/linux_spi.c

CFLAGS += -DNO_OS_NETWORKING 			\
		-DDISABLE_SECURE_SOCKET
SRCS += $(NO-OS)/network/linux_socket/linux_socket.c 	\
		$(NO-OS)/network/tcp_socket.c 		\
	$(NO-OS)/util/no_os_lf256fifo.c 		\
	$(PLATFORM_DRIVERS)/linux_uart.c