SRCS += $(PLATFORM_DRIVERS)/delay.c \
	$(DRIVERS)/api/no_os_irq.c
	
ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y

SRC_DIRS += $(NO-OS)/network

SRCS += $(NO-OS)/util/no_os_circular_buffer.c \
	$(PLATFORM_DRIVERS)/no_os_timer.c

INCS += $(INCLUDE)/no_os_circular_buffer.h \
	$(INCLUDE)/no_os_timer.h           \
	$(PLATFORM_DRIVERS)/timer_extra.h  \
	$(PLATFORM_DRIVERS)/rtc_extra.h
endif

SRCS += $(NO-OS)/util/no_os_lf256fifo.c  \
	$(PLATFORM_DRIVERS)/no_os_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c

INCS += $(INCLUDE)/no_os_rtc.h          \
	$(INCLUDE)/no_os_gpio.h         \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h
