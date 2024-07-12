ifeq (y,$(strip $(APARD32690_ADIN1110_STANDALONE_EXAMPLE)))
CFLAGS += -DAPARD32690_ADIN1110_STANDALONE_EXAMPLE
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c
INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(DRIVERS)/net/adin1110/adin1110.h

SRC_DIRS += $(PROJECT)/src/examples/adin1110_standalone_example
endif

ifeq (y,$(strip $(APARD32690_ECHO_SERVER_EXAMPLE)))
CFLAGS += -DAPARD32690_ECHO_SERVER_EXAMPLE
LIBRARIES += lwip
CFLAGS += -DNO_OS_STATIC_IP
CFLAGS += -DNO_OS_LWIP_NETWORKING
INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c

SRC_DIRS += $(PROJECT)/src/examples/tcp_echo_server_example
endif

ifeq (y,$(strip $(APARD32690_BASIC_EXAMPLE)))
CFLAGS += -DAPARD32690_BASIC_EXAMPLE
SRC_DIRS += $(PROJECT)/src/examples/basic_example
endif

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c	\
	$(PLATFORM_DRIVERS)/maxim_irq.c		\
	$(PLATFORM_DRIVERS)/maxim_gpio.c	\
	$(PLATFORM_DRIVERS)/maxim_spi.c		\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.c		\
	$(PLATFORM_DRIVERS)/maxim_timer.c	\
	$(PLATFORM_DRIVERS)/maxim_init.c	\
	$(PLATFORM_DRIVERS)/maxim_uart.c	\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c
	
INCS += $(PLATFORM_DRIVERS)/maxim_irq.h		\
	$(PLATFORM_DRIVERS)/maxim_uart.h	\
	$(PLATFORM_DRIVERS)/maxim_timer.h	\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h	\
	$(PLATFORM_DRIVERS)/maxim_gpio.h	\
	$(PLATFORM_DRIVERS)/maxim_spi.h		\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h
