SRCS += $(PLATFORM_DRIVERS)/aducm3029_delay.c \
	$(DRIVERS)/api/no_os_irq.c

ifeq (y,$(strip $(NO_OS_NETWORKING)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
endif

SRCS += $(NO-OS)/util/no_os_circular_buffer.c \
	$(DRIVERS)/api/no_os_timer.c
	
INCS += $(INCLUDE)/no_os_circular_buffer.h \
	$(INCLUDE)/no_os_timer.h           \
	$(PLATFORM_DRIVERS)/aducm3029_timer.h  \
	$(PLATFORM_DRIVERS)/aducm3029_rtc.h

SRCS += $(NO-OS)/util/no_os_lf256fifo.c  \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_timer.c

INCS += $(INCLUDE)/no_os_rtc.h          \
	$(INCLUDE)/no_os_gpio.h         \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
