include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h

INCS += $(INCLUDE)/no_os_delay.h      \
	$(INCLUDE)/no_os_error.h      \
	$(INCLUDE)/no_os_gpio.h       \
	$(INCLUDE)/no_os_irq.h        \
	$(INCLUDE)/no_os_dma.h        \
	$(INCLUDE)/no_os_list.h       \
	$(INCLUDE)/no_os_lf256fifo.h  \
	$(INCLUDE)/no_os_print_log.h  \
	$(INCLUDE)/no_os_alloc.h      \
	$(INCLUDE)/no_os_mutex.h      \
	$(INCLUDE)/no_os_uart.h       \
	$(INCLUDE)/no_os_util.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_lf256fifo.c

INCS += $(DRIVERS)/mux/adg2404/adg2404.h
SRCS += $(DRIVERS)/mux/adg2404/adg2404.c
