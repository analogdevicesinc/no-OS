################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

SRCS += $(DRIVERS)/api/no_os_uart.c     \
        $(DRIVERS)/api/no_os_gpio.c     \
	$(DRIVERS)/api/no_os_spi.c  	\
	$(DRIVERS)/api/no_os_irq.c 	\
	$(DRIVERS)/api/no_os_timer.c	\
        $(DRIVERS)/api/no_os_i2c.c	\
        $(DRIVERS)/api/no_os_eeprom.c   \
        $(NO-OS)/util/no_os_fifo.c      \
	$(NO-OS)/util/no_os_lf256fifo.c \
        $(NO-OS)/util/no_os_mutex.c     \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c

INCS += $(INCLUDE)/no_os_delay.h     \
        $(INCLUDE)/no_os_error.h     \
	$(INCLUDE)/no_os_gpio.h      \
	$(INCLUDE)/no_os_spi.h       \
        $(INCLUDE)/no_os_i2c.h       \
        $(INCLUDE)/no_os_eeprom.h    \
        $(INCLUDE)/no_os_mutex.h     \
        $(INCLUDE)/no_os_fifo.h      \
        $(INCLUDE)/no_os_irq.h       \
        $(INCLUDE)/no_os_lf256fifo.h \
        $(INCLUDE)/no_os_list.h      \
        $(INCLUDE)/no_os_print_log.h \
        $(INCLUDE)/no_os_timer.h     \
        $(INCLUDE)/no_os_uart.h      \
        $(INCLUDE)/no_os_util.h      \
        $(INCLUDE)/no_os_alloc.h

INCS += $(DRIVERS)/frequency/adf5611/adf5611.h
SRCS += $(DRIVERS)/frequency/adf5611/adf5611.c

INCS += $(DRIVERS)/eeprom/24xx32a/24xx32a.h
SRCS += $(DRIVERS)/eeprom/24xx32a/24xx32a.c
