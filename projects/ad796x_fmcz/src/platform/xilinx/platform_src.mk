SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_timer.c
	
ifeq (y,$(strip $(NETWORKING)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
endif

SRCS += $(NO-OS)/util/no_os_circular_buffer.c \
	$(PLATFORM_DRIVERS)/xilinx_timer.c

INCS += $(INCLUDE)/no_os_circular_buffer.h \
	$(INCLUDE)/no_os_timer.h           \
	$(PLATFORM_DRIVERS)/xilinx_timer.h  \
	$(PLATFORM_DRIVERS)/rtc_extra.h

SRCS += $(NO-OS)/util/no_os_lf256fifo.c  \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.c

INCS += $(INCLUDE)/no_os_rtc.h          \
	$(INCLUDE)/no_os_gpio.h         \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
