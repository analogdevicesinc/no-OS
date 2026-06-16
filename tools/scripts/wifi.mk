WIFI_DIR := $(NO-OS)/network/wifi

SRCS += $(WIFI_DIR)/wifi.c		\
	$(WIFI_DIR)/at_parser.c		\
	$(DRIVERS)/api/no_os_net.c	\
	$(NO-OS)/network/tcp_socket.c

INCS += $(WIFI_DIR)/wifi.h		\
	$(WIFI_DIR)/at_parser.h		\
	$(WIFI_DIR)/at_params.h		\
	$(INCLUDE)/no_os_net.h		\
	$(NO-OS)/network/tcp_socket.h	\
	$(NO-OS)/network/network_interface.h
