include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk
include $(PROJECT)/src/common/common_src.mk

SRCS += $(DRIVERS)/api/no_os_uart.c     			\
	$(DRIVERS)/api/no_os_spi.c     				\
	$(DRIVERS)/api/no_os_timer.c    			\
	$(DRIVERS)/api/no_os_i2c.c     				\
	$(DRIVERS)/api/no_os_irq.c     				\
	$(DRIVERS)/api/no_os_dma.c     				\
	$(DRIVERS)/api/no_os_gpio.c     			\
        $(NO-OS)/util/no_os_fifo.c      			\
        $(NO-OS)/util/no_os_list.c      			\
        $(NO-OS)/util/no_os_lf256fifo.c 			\
        $(NO-OS)/util/no_os_util.c      			\
        $(NO-OS)/util/no_os_alloc.c     			\
        $(NO-OS)/util/no_os_mutex.c

INCS += $(INCLUDE)/no_os_delay.h     				\
        $(INCLUDE)/no_os_error.h     				\
        $(INCLUDE)/no_os_fifo.h      				\
        $(INCLUDE)/no_os_gpio.h      				\
        $(INCLUDE)/no_os_irq.h       				\
        $(INCLUDE)/no_os_lf256fifo.h 				\
        $(INCLUDE)/no_os_list.h      				\
        $(INCLUDE)/no_os_uart.h      				\
        $(INCLUDE)/no_os_spi.h      				\
        $(INCLUDE)/no_os_i2c.h      				\
        $(INCLUDE)/no_os_dma.h      				\
        $(INCLUDE)/no_os_timer.h      				\
        $(INCLUDE)/no_os_util.h      				\
        $(INCLUDE)/no_os_units.h      				\
        $(INCLUDE)/no_os_init.h      				\
        $(INCLUDE)/no_os_print_log.h    			\
        $(INCLUDE)/no_os_alloc.h     				\
        $(INCLUDE)/no_os_mutex.h

# LCD driver
SRCS +=	$(NO-OS)/drivers/display/nhd_c12832a1z/nhd_c12832a1z.c
INCS +=	$(NO-OS)/drivers/display/nhd_c12832a1z/nhd_c12832a1z.h


# Platforms
INCS += $(PROJECT)/src/platform/platform_includes.h
INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c 

# PQM library
ifdef PQLIB_PATH
INCS += ${PQLIB_PATH}/include/config/adi_pqlib_cfg.h   		\
	${PQLIB_PATH}/include/ade9430.h       			\
	${PQLIB_PATH}/include/adi_pqlib_debug.h       		\
	${PQLIB_PATH}/include/adi_pqlib_error.h       		\
	${PQLIB_PATH}/include/adi_pqlib_memory.h      		\
	${PQLIB_PATH}/include/adi_pqlib_profile.h     		\
	${PQLIB_PATH}/include/adi_pqlib_version.h     		\
	${PQLIB_PATH}/include/adi_pqlib.h     

EXTRA_MATH_PQM = ${PQLIB_PATH}/libadi_pqlib_cm4_gcc.a
endif

ifeq ($(INTERFACE), usb)
endif

ifeq ($(INTERFACE), serial)
endif

ifeq ($(INTERFACE), ethernet)
	INCS += $(DRIVERS)/net/w5500/w5500.h
	INCS += $(NO-OS)/network/w5500_network/w5500_network.h
	INCS += $(NO-OS)/network/network_interface.h
	INCS += $(NO-OS)/network/tcp_socket.h

	SRCS += $(DRIVERS)/net/w5500/w5500.c
	SRCS += $(NO-OS)/network/w5500_network/w5500_network.c
	SRCS += $(NO-OS)/network/tcp_socket.c
endif

ifeq ($(INTERFACE), ethernet_t1l)
	LIBRARIES += lwip
	INCS += $(NO-OS)/network/tcp_socket.h			\
		$(NO-OS)/network/noos_mbedtls_config.h		\
		$(NO-OS)/network/network_interface.h		\
		$(INCLUDE)/no_os_crc8.h				\
		$(DRIVERS)/net/adin1110/adin1110.h		\
		$(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.h

	SRCS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110/lwip_adin1110.c	\
		$(DRIVERS)/net/adin1110/adin1110.c					\
		$(NO-OS)/util/no_os_crc8.c						\
		$(NO-OS)/network/tcp_socket.c
endif

INCS += $(INCLUDE)/no_os_crc8.h
SRCS += $(NO-OS)/util/no_os_crc8.c

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h

EXTRA_FILES +=	$(EXTRA_MATH_PQM) ${EXTRA_MATH_LIB}


