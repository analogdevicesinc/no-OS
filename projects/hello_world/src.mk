include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c
SRCS += $(PROJECT)/src/app/hello_world.c

INCS += $(PROJECT)/src/platform/platform_includes.h
INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
INCS += $(PROJECT)/src/app/hello_world.h

SRCS += $(DRIVERS)/api/no_os_uart.c	\
	$(NO-OS)/util/no_os_fifo.c	\
	$(NO-OS)/util/no_os_list.c	\
	$(NO-OS)/util/no_os_util.c	\
	$(NO-OS)/util/no_os_alloc.c	\
	$(NO-OS)/util/no_os_mutex.c

INCS += $(INCLUDE)/no_os_delay.h	\
	$(INCLUDE)/no_os_error.h	\
	$(INCLUDE)/no_os_fifo.h		\
	$(INCLUDE)/no_os_irq.h		\
	$(INCLUDE)/no_os_lf256fifo.h	\
	$(INCLUDE)/no_os_list.h		\
	$(INCLUDE)/no_os_uart.h		\
	$(INCLUDE)/no_os_util.h		\
	$(INCLUDE)/no_os_alloc.h	\
	$(INCLUDE)/no_os_mutex.h	\
	$(INCLUDE)/no_os_print_log.h
