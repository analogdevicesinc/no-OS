include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/examples/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(DRIVERS)/crypto/maxq1065/maxq1065.h
SRCS += $(DRIVERS)/crypto/maxq1065/maxq1065.c
SRC_DIRS += $(DRIVERS)/crypto/maxq1065/maxq1065_include

INCS += $(INCLUDE)/no_os_crc8.h
INCS += $(DRIVERS)/net/adin1110/adin1110.h
INCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h
SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
SRCS += $(NO-OS)/util/no_os_crc8.c

SRCS += $(DRIVERS)/api/no_os_uart.c     \
	$(DRIVERS)/api/no_os_irq.c     	\
	$(DRIVERS)/api/no_os_spi.c     	\
	$(DRIVERS)/api/no_os_gpio.c     	\
	$(DRIVERS)/api/no_os_trng.c     	\
	$(DRIVERS)/api/no_os_timer.c     	\
        $(NO-OS)/util/no_os_fifo.c      \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_lf256fifo.c \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c     \
        $(NO-OS)/util/no_os_mutex.c

INCS += $(INCLUDE)/no_os_delay.h     \
        $(INCLUDE)/no_os_error.h     \
        $(INCLUDE)/no_os_fifo.h      \
        $(INCLUDE)/no_os_spi.h      \
        $(INCLUDE)/no_os_init.h      \
        $(INCLUDE)/no_os_trng.h      \
        $(INCLUDE)/no_os_timer.h      \
        $(INCLUDE)/no_os_gpio.h      \
        $(INCLUDE)/no_os_print_log.h      \
        $(INCLUDE)/no_os_units.h      \
        $(INCLUDE)/no_os_irq.h       \
        $(INCLUDE)/no_os_gpio.h       \
        $(INCLUDE)/no_os_lf256fifo.h \
        $(INCLUDE)/no_os_list.h      \
        $(INCLUDE)/no_os_uart.h      \
        $(INCLUDE)/no_os_util.h      \
        $(INCLUDE)/no_os_alloc.h     \
        $(INCLUDE)/no_os_mutex.h
