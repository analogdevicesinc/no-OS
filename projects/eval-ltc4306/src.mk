include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c
	
INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
ifeq ('maxim' , '$(PLATFORM)')
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c 
endif

INCS += $(INCLUDE)/no_os_delay.h        \
        $(INCLUDE)/no_os_error.h        \
        $(INCLUDE)/no_os_print_log.h    \
        $(INCLUDE)/no_os_i2c.h          \
        $(INCLUDE)/no_os_irq.h          \
        $(INCLUDE)/no_os_init.h          \
        $(INCLUDE)/no_os_list.h         \
        $(INCLUDE)/no_os_dma.h         \
        $(INCLUDE)/no_os_uart.h         \
        $(INCLUDE)/no_os_timer.h        \
        $(INCLUDE)/no_os_lf256fifo.h    \
        $(INCLUDE)/no_os_util.h         \
        $(INCLUDE)/no_os_units.h        \
        $(INCLUDE)/no_os_alloc.h        \
        $(INCLUDE)/no_os_mutex.h  

SRCS += $(NO-OS)/util/no_os_lf256fifo.c \
        $(DRIVERS)/api/no_os_irq.c      \
        $(DRIVERS)/api/no_os_dma.c      \
        $(DRIVERS)/api/no_os_timer.c    \
        $(DRIVERS)/api/no_os_i2c.c      \
        $(DRIVERS)/api/no_os_uart.c     \
        $(NO-OS)/util/no_os_list.c      \
        $(NO-OS)/util/no_os_util.c      \
        $(NO-OS)/util/no_os_alloc.c     \
        $(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/io-expander/ltc4306/ltc4306.h
SRCS += $(DRIVERS)/io-expander/ltc4306/ltc4306.c

INCS += $(DRIVERS)/dac/max538x/max538x.h
SRCS += $(DRIVERS)/dac/max538x/max538x.c
