SRCS += $(PROJECT)/src/main.c

INCS += $(PROJECT)/src/parameters.h
SRCS += $(PROJECT)/src/parameters.c

INCS += $(INCLUDE)/no_os_delay.h    \
	$(INCLUDE)/no_os_alloc.h 		\
	$(INCLUDE)/no_os_error.h		\
	$(INCLUDE)/no_os_gpio.h			\
	$(INCLUDE)/no_os_print_log.h  	\
	$(INCLUDE)/no_os_irq.h        	\
	$(INCLUDE)/no_os_rtc.h        	\
	$(INCLUDE)/no_os_list.h       	\
	$(INCLUDE)/no_os_timer.h      	\
	$(INCLUDE)/no_os_uart.h       	\
	$(INCLUDE)/no_os_lf256fifo.h  	\
	$(INCLUDE)/no_os_util.h       	\
	$(INCLUDE)/no_os_units.h	  	\
	$(INCLUDE)/no_os_mutex.h	  	\
		$(INCLUDE)/no_os_print_log.h	


SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_irq.c      \
	$(DRIVERS)/api/no_os_uart.c     \
	$(DRIVERS)/api/no_os_timer.c    \
	$(NO-OS)/util/no_os_list.c      \
	$(NO-OS)/util/no_os_util.c		\
	$(NO-OS)/util/no_os_alloc.c		\
		$(NO-OS)/util/no_os_mutex.c		


INCS += $(PLATFORM_DRIVERS)/stm32_gpio.h      \
	$(PLATFORM_DRIVERS)/stm32_hal.h       \
	$(PLATFORM_DRIVERS)/stm32_irq.h       \
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.h  \
	$(PLATFORM_DRIVERS)/stm32_uart.h      \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.h

SRCS += $(PLATFORM_DRIVERS)/stm32_delay.c     \
	$(PLATFORM_DRIVERS)/stm32_gpio.c      \
	$(PLATFORM_DRIVERS)/stm32_irq.c       \
	$(PLATFORM_DRIVERS)/stm32_gpio_irq.c  \
	$(PLATFORM_DRIVERS)/stm32_uart.c      \
	$(PLATFORM_DRIVERS)/stm32_uart_stdio.c

