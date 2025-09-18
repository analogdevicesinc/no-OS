################################################################################
#
# src.mk for MAX20362 project.
#
################################################################################

include $(PROJECT)/src/platform/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

# no-OS API includes
INCS += $(INCLUDE)/no_os_i2c.h     							\
        $(INCLUDE)/no_os_gpio.h     						\
        $(INCLUDE)/no_os_alloc.h      						\
        $(INCLUDE)/no_os_error.h       						\
        $(INCLUDE)/no_os_util.h 							\
        $(INCLUDE)/no_os_delay.h							\
        $(INCLUDE)/no_os_mutex.h        					\
        $(INCLUDE)/no_os_uart.h         					\
        $(INCLUDE)/no_os_lf256fifo.h    					\
        $(INCLUDE)/no_os_fifo.h         					\
        $(INCLUDE)/no_os_list.h         					\
        $(INCLUDE)/no_os_timer.h        					\
		$(INCLUDE)/no_os_irq.h								\
		$(INCLUDE)/no_os_dma.h								\
		$(INCLUDE)/no_os_print_log.h

# no-OS API sources
SRCS += $(DRIVERS)/api/no_os_uart.c     					\
		$(DRIVERS)/api/no_os_i2c.c							\
		$(DRIVERS)/api/no_os_gpio.c							\
        $(DRIVERS)/api/no_os_timer.c						\
		$(DRIVERS)/api/no_os_irq.c							\
		$(DRIVERS)/api/no_os_dma.c

# no-OS utilities
SRCS += $(NO-OS)/util/no_os_fifo.c      					\
        $(NO-OS)/util/no_os_list.c      					\
        $(NO-OS)/util/no_os_util.c      					\
        $(NO-OS)/util/no_os_alloc.c     					\
        $(NO-OS)/util/no_os_lf256fifo.c 					\
        $(NO-OS)/util/no_os_mutex.c

# MAX20362 driver
INCS += $(DRIVERS)/power/max20362/max20362.h
SRCS += $(DRIVERS)/power/max20362/max20362.c
