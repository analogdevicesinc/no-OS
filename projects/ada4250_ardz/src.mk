#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(DRIVERS)/amplifiers/ada4250
SRC_DIRS += $(NO-OS)/iio/iio_app

SRC_DIRS += $(INCLUDE)

SRCS += $(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_timer.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_irq.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_gpio.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_timer.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_rtc.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_delay.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_uart.c

INCS += $(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.h \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_irq.h \
	$(DRIVERS)/platform/$(PLATFORM)/aducm3029_gpio.h \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_uart.h \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_rtc.h \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_timer.h

IGNORED_FILES += $(PLATFORM_DRIVERS)/aducm3029_uart_stdio.c
IGNORED_FILES += $(PLATFORM_DRIVERS)/aducm3029_uart_stdio.h

IIOD=y
