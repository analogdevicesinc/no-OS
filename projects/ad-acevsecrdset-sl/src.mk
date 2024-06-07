# Platform drivers
INCS += $(PLATFORM_DRIVERS)/maxim_gpio.h      \
        $(PLATFORM_DRIVERS)/maxim_spi.h       \
        $(PLATFORM_DRIVERS)/../common/maxim_dma.h       \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.h  \
		$(PLATFORM_DRIVERS)/maxim_hal.h       \
        $(PLATFORM_DRIVERS)/maxim_irq.h       \
        $(PLATFORM_DRIVERS)/maxim_uart.h      \
		$(PLATFORM_DRIVERS)/maxim_i2c.h 	  \
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/maxim_delay.c     \
        $(PLATFORM_DRIVERS)/maxim_init.c      \
        $(PLATFORM_DRIVERS)/maxim_gpio.c      \
        $(PLATFORM_DRIVERS)/maxim_spi.c       \
        $(PLATFORM_DRIVERS)/../common/maxim_dma.c       \
        $(PLATFORM_DRIVERS)/maxim_gpio_irq.c  \
        $(PLATFORM_DRIVERS)/maxim_irq.c       \
        $(PLATFORM_DRIVERS)/maxim_uart.c      \
		$(PLATFORM_DRIVERS)/maxim_i2c.c       \
        $(PLATFORM_DRIVERS)/maxim_uart_stdio.c

# State machine files
SRCS += $(PROJECT)/src/state_machine/state_machine.c
INCS += $(PROJECT)/src/state_machine/state_machine.h

# Pilot phase files
SRCS += $(PROJECT)/src/pilot/pilot.c
INCS += $(PROJECT)/src/pilot/pilot.h

ifneq (y,$(strip $(REV_A)))
# Interrupt phase
SRCS += $(PROJECT)/src/inter/inter.c
INCS += $(PROJECT)/src/inter/inter.h
endif

# User interface files
SRCS += $(PROJECT)/src/interface/interface.c
INCS += $(PROJECT)/src/interface/interface.h

# Power supply files
SRCS += $(PROJECT)/src/supply/supply.c
INCS += $(PROJECT)/src/supply/supply.h

# RCD supply files
SRCS += $(PROJECT)/src/rcd/rcd.c
INCS += $(PROJECT)/src/rcd/rcd.h

# Relay supply files
SRCS += $(PROJECT)/src/relay/relay.c
INCS += $(PROJECT)/src/relay/relay.h

# Self-tests files
SRCS += $(PROJECT)/src/self_test/self_test.c
INCS += $(PROJECT)/src/self_test/self_test.h

# Application entry point
SRCS += $(PROJECT)/src/main.c

# Platform specific definitions
INCS += $(PROJECT)/src/platform/parameters.h
SRCS += $(PROJECT)/src/platform/parameters.c 

# Common data (will ultimately be moved to specific module files)
INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

# No-OS files 
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
		$(INCLUDE)/no_os_dma.h \
		$(INCLUDE)/no_os_units.h \
		$(INCLUDE)/no_os_init.h \
		$(INCLUDE)/no_os_alloc.h \
		$(INCLUDE)/no_os_crc8.h \
		$(INCLUDE)/no_os_crc16.h \
		$(INCLUDE)/no_os_init.h \
		$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(DRIVERS)/api/no_os_i2c.c  \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c  \
		$(DRIVERS)/api/no_os_spi.c  \
		$(DRIVERS)/api/no_os_timer.c  \
		$(DRIVERS)/api/no_os_uart.c \
		$(DRIVERS)/api/no_os_dma.c \
		$(NO-OS)/util/no_os_list.c \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_alloc.c \
		$(NO-OS)/util/no_os_crc8.c \
		$(NO-OS)/util/no_os_crc16.c \
		$(NO-OS)/util/no_os_mutex.c

# ADT75 driver files
INCS += $(DRIVERS)/temperature/adt75/adt75.h
SRCS += $(DRIVERS)/temperature/adt75/adt75.c

# ADE9113 driver files
INCS += $(DRIVERS)/meter/ade9113/ade9113.h
SRCS += $(DRIVERS)/meter/ade9113/ade9113.c

