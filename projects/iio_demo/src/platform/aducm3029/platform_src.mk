SRCS += $(PLATFORM_DRIVERS)/delay.c \
	$(DRIVERS)/api/no_os_irq.c
	
ifeq '$(USE_TCP_SOCKET)' 'y'
CFLAGS += -DUSE_TCP_SOCKET
endif
ENABLE_IIO_NETWORK = y

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y

SRC_DIRS += $(NO-OS)/network

SRCS += $(NO-OS)/util/no_os_circular_buffer.c \
	$(DRIVERS)/api/no_os_timer.c
	
INCS += $(INCLUDE)/no_os_circular_buffer.h \
	$(INCLUDE)/no_os_timer.h           \
	$(PLATFORM_DRIVERS)/aducm3029_timer.h  \
	$(PLATFORM_DRIVERS)/rtc_extra.h
endif

SRCS += $(NO-OS)/util/no_os_lf256fifo.c  \
	$(PLATFORM_DRIVERS)/no_os_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.c

INCS += $(INCLUDE)/no_os_rtc.h          \
	$(INCLUDE)/no_os_gpio.h         \
	$(PLATFORM_DRIVERS)/irq_extra.h \
	$(PLATFORM_DRIVERS)/uart_extra.h
