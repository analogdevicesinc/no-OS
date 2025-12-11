
SRCS += $(PROJECT)/src/example.c

INCS += $(INCLUDE)/no_os_error.h 	\
	$(INCLUDE)/no_os_uart.h 	\
	$(INCLUDE)/no_os_lf256fifo.h	\
	$(INCLUDE)/no_os_irq.h      	\
	$(INCLUDE)/no_os_alloc.h 	\
	$(PLATFORM_DRIVERS)/stm32_hal.h

INCS += $(PLATFORM_DRIVERS)/stm32_irq.h	\
	$(PLATFORM_DRIVERS)/stm32_uart.h	\
	$(PLATFORM_DRIVERS)/stm32_gpio.h	\
	$(INCLUDE)/no_os_list.h      \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_uart.c		\
	$(PLATFORM_DRIVERS)/stm32_uart.c	\
	$(NO-OS)/util/no_os_lf256fifo.c		\
	$(NO-OS)/util/no_os_alloc.c 		\
	$(DRIVERS)/api/no_os_irq.c  \
	$(PLATFORM_DRIVERS)/stm32_irq.c	\
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(INCLUDE)/capi_uart.h			\
	$(PLATFORM_DRIVERS)/stm32_capi_uart.h

SRCS += $(DRIVERS)/api/capi_uart.c		\
	$(PLATFORM_DRIVERS)/stm32_capi_uart.c
