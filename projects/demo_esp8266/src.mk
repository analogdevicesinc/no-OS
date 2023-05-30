SRCS += $(PROJECT)/src/demo_esp8266.c	\
	$(PROJECT)/src/parameters.c

INCS += $(PROJECT)/src/parameters.h

INCS += $(INCLUDE)/no_os_delay.h		\
	$(INCLUDE)/no_os_error.h		\
	$(INCLUDE)/no_os_gpio.h			\
	$(INCLUDE)/no_os_print_log.h		\
	$(INCLUDE)/no_os_rtc.h			\
	$(INCLUDE)/no_os_spi.h			\
	$(INCLUDE)/no_os_timer.h		\
	$(INCLUDE)/no_os_irq.h			\
	$(INCLUDE)/no_os_uart.h			\
	$(INCLUDE)/no_os_lf256fifo.h		\
	$(INCLUDE)/no_os_util.h			\
	$(INCLUDE)/no_os_list.h			\
	$(INCLUDE)/no_os_circular_buffer.h	\
	$(INCLUDE)/no_os_alloc.h

SRCS += $(NO-OS)/util/no_os_lf256fifo.c		\
	$(DRIVERS)/api/no_os_gpio.c		\
	$(DRIVERS)/api/no_os_irq.c		\
	$(DRIVERS)/api/no_os_spi.c		\
	$(DRIVERS)/api/no_os_timer.c		\
	$(DRIVERS)/api/no_os_uart.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_circular_buffer.c	\
	$(NO-OS)/util/no_os_util.c		\
	$(NO-OS)/util/no_os_alloc.c

INCS += $(PLATFORM_DRIVERS)/platform_init.h   		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_delay.h		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_rtc.h		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.h		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio_irq.h	\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h

SRCS += $(PLATFORM_DRIVERS)/platform_init.c		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_rtc.c		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c		\
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.c		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio_irq.c 	\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c		\
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c

SRC_DIRS += $(NO-OS)/network

DISABLE_SECURE_SOCKET=y

LIBRARIES += mqtt
