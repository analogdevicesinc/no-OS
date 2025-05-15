#############################################################################
#									      		 							#
#     Shared variables:							       						#
#	- DRIVERS							       								#
#	- INCLUDE							       								#
#	- PLATFORM_DRIVERS						       							#
#	- NO-OS								       								#
#									       									#
#############################################################################

include $(PROJECT)/src/app_src.mk

INCS += $(INCLUDE)/no_os_delay.h				\
		$(INCLUDE)/no_os_dma.h					\
		$(INCLUDE)/no_os_error.h				\
		$(INCLUDE)/no_os_gpio.h					\
		$(INCLUDE)/no_os_print_log.h			\
		$(INCLUDE)/no_os_spi.h					\
		$(INCLUDE)/no_os_irq.h					\
		$(INCLUDE)/no_os_list.h					\
		$(INCLUDE)/no_os_uart.h					\
		$(INCLUDE)/no_os_lf256fifo.h			\
		$(INCLUDE)/no_os_util.h 				\
		$(INCLUDE)/no_os_units.h				\
		$(INCLUDE)/no_os_alloc.h				\
		$(INCLUDE)/no_os_mutex.h				\
		$(PROJECT)/src/platform/platform_includes.h

SRCS += $(DRIVERS)/api/no_os_dma.c		\
		$(DRIVERS)/api/no_os_gpio.c		\
		$(NO-OS)/util/no_os_lf256fifo.c	\
		$(DRIVERS)/api/no_os_irq.c		\
		$(DRIVERS)/api/no_os_spi.c		\
		$(DRIVERS)/api/no_os_uart.c		\
		$(NO-OS)/util/no_os_list.c		\
		$(NO-OS)/util/no_os_util.c		\
		$(NO-OS)/util/no_os_alloc.c		\
		$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/magnetometer/admt4000/admt4000.h
SRCS += $(DRIVERS)/magnetometer/admt4000/admt4000.c
