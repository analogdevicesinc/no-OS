include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

SRCS += $(DRIVERS)/api/no_os_uart.c     \
	$(DRIVERS)/api/no_os_irq.c     	\
	$(DRIVERS)/api/no_os_gpio.c     	\
	$(DRIVERS)/api/no_os_timer.c     	\
	$(DRIVERS)/api/no_os_dma.c     	\
	$(DRIVERS)/api/no_os_i2c.c  \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_lf256fifo.c \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c     \
        $(NO-OS)/util/no_os_mutex.c     \
        $(NO-OS)/util/no_os_crc8.c

INCS += $(INCLUDE)/no_os_delay.h     \
        $(INCLUDE)/no_os_error.h     \
        $(INCLUDE)/no_os_init.h      \
        $(INCLUDE)/no_os_timer.h      \
        $(INCLUDE)/no_os_gpio.h      \
        $(INCLUDE)/no_os_irq.h       \
        $(INCLUDE)/no_os_dma.h       \
        $(INCLUDE)/no_os_lf256fifo.h \
        $(INCLUDE)/no_os_list.h      \
        $(INCLUDE)/no_os_uart.h      \
        $(INCLUDE)/no_os_util.h      \
	$(INCLUDE)/no_os_i2c.h       \
        $(INCLUDE)/no_os_alloc.h     \
        $(INCLUDE)/no_os_mutex.h     \
        $(INCLUDE)/no_os_crc8.h

INCS += $(DRIVERS)/power/max42500/max42500.h
SRCS += $(DRIVERS)/power/max42500/max42500.c
