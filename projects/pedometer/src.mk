
SRCS += $(PROJECT)/src/main.c

INCS += $(INCLUDE)/no_os_alloc.h			\
	$(INCLUDE)/no_os_error.h			\
	$(INCLUDE)/no_os_mutex.h			\
	$(INCLUDE)/no_os_util.h				\
	$(INCLUDE)/no_os_lf256fifo.h			\
	$(INCLUDE)/no_os_list.h				\
	$(INCLUDE)/no_os_irq.h				\
	$(INCLUDE)/no_os_uart.h				\
	$(INCLUDE)/no_os_dma.h				\
	$(INCLUDE)/no_os_i2c.h				\
	$(INCLUDE)/no_os_gpio.h				\
	$(INCLUDE)/no_os_spi.h				\
	$(INCLUDE)/no_os_display.h			\
	$(INCLUDE)/no_os_delay.h			\
	$(PLATFORM_DRIVERS)/maxim_irq.h			\
	$(PLATFORM_DRIVERS)/maxim_uart.h		\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.h		\
	$(PLATFORM_DRIVERS)/maxim_i2c.h			\
	$(PLATFORM_DRIVERS)/maxim_gpio.h		\
	$(PLATFORM_DRIVERS)/../common/maxim_dma.h

SRCS +=	$(NO-OS)/util/no_os_alloc.c			\
	$(NO-OS)/util/no_os_mutex.c			\
	$(NO-OS)/util/no_os_lf256fifo.c			\
	$(NO-OS)/util/no_os_list.c			\
	$(NO-OS)/util/no_os_util.c			\
        $(NO-OS)/util/no_os_display.c			\
	$(NO-OS)/util/no_os_font_8x8.c			\
	$(DRIVERS)/api/no_os_irq.c			\
	$(DRIVERS)/api/no_os_uart.c			\
	$(DRIVERS)/api/no_os_i2c.c			\
	$(DRIVERS)/api/no_os_spi.c			\
	$(DRIVERS)/api/no_os_gpio.c			\
	$(PLATFORM_DRIVERS)/maxim_irq.c			\
	$(PLATFORM_DRIVERS)/maxim_uart_stdio.c		\
	$(PLATFORM_DRIVERS)/maxim_uart.c		\
	$(PLATFORM_DRIVERS)/maxim_i2c.c			\
	$(PLATFORM_DRIVERS)/maxim_gpio.c		\
	$(PLATFORM_DRIVERS)/maxim_delay.c

SRCS += $(NO-OS)/drivers/display/ssd_1306/ssd_1306.c 
SRCS += $(NO-OS)/drivers/display/display.c 
INCS += $(NO-OS)/drivers/display/ssd_1306/ssd_1306.h
INCS += $(NO-OS)/drivers/display/display.h
