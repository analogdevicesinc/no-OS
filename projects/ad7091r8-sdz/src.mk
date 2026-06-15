# Device selection: AD7091R2 / AD7091R4 / AD7091R8 (default AD7091R8)
ifeq (y,$(strip $(AD7091R2)))
CFLAGS += -DAD7091R2_DEV
else ifeq (y,$(strip $(AD7091R4)))
CFLAGS += -DAD7091R4_DEV
else
CFLAGS += -DAD7091R8_DEV
endif

INCS += $(INCLUDE)/no_os_delay.h     \
		$(INCLUDE)/no_os_error.h     \
		$(INCLUDE)/no_os_gpio.h      \
		$(INCLUDE)/no_os_i2c.h       \
		$(INCLUDE)/no_os_dma.h       \
		$(INCLUDE)/no_os_print_log.h \
		$(INCLUDE)/no_os_spi.h       \
		$(INCLUDE)/no_os_irq.h      \
		$(INCLUDE)/no_os_list.h      \
		$(INCLUDE)/no_os_timer.h      \
		$(INCLUDE)/no_os_uart.h      \
		$(INCLUDE)/no_os_lf256fifo.h \
		$(INCLUDE)/no_os_util.h \
		$(INCLUDE)/no_os_units.h \
		$(INCLUDE)/no_os_init.h \
		$(INCLUDE)/no_os_alloc.h \
        	$(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(DRIVERS)/api/no_os_i2c.c  \
		$(DRIVERS)/api/no_os_dma.c  \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c  \
		$(DRIVERS)/api/no_os_spi.c  \
		$(DRIVERS)/api/no_os_timer.c  \
		$(DRIVERS)/api/no_os_uart.c \
		$(NO-OS)/util/no_os_list.c \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_alloc.c \
        	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/adc/ad7091r8/ad7091r8.h
SRCS += $(DRIVERS)/adc/ad7091r8/ad7091r8.c
