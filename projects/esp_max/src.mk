include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c
	
INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h

INCS += $(INCLUDE)/no_os_delay.h     \
		$(INCLUDE)/no_os_error.h     \
		$(INCLUDE)/no_os_gpio.h      \
		$(INCLUDE)/no_os_i2c.h       \
		$(INCLUDE)/no_os_print_log.h \
		$(INCLUDE)/no_os_spi.h       \
		$(INCLUDE)/no_os_irq.h      \
		$(INCLUDE)/no_os_list.h      \
		$(INCLUDE)/no_os_timer.h      \
		$(INCLUDE)/no_os_uart.h      \
		$(INCLUDE)/no_os_lf256fifo.h \
		$(INCLUDE)/no_os_util.h \
		$(INCLUDE)/no_os_units.h \
		$(INCLUDE)/no_os_circular_buffer.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(DRIVERS)/api/no_os_i2c.c  \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c  \
		$(DRIVERS)/api/no_os_spi.c  \
		$(DRIVERS)/api/no_os_timer.c  \
		$(NO-OS)/util/no_os_list.c \
		$(NO-OS)/util/no_os_util.c	\
		$(NO-OS)/util/no_os_circular_buffer.c

LIBRARIES += mqtt
SRC_DIRS += $(NO-OS)/network

