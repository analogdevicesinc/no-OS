#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(DRIVERS)/amplifiers/ada4250
SRC_DIRS += $(NO-OS)/iio/iio_app

SRC_DIRS += $(INCLUDE)

SRCS += $(NO-OS)/util/util.c \
	$(NO-OS)/util/list.c \
	$(DRIVERS)/api/spi.c \
	$(DRIVERS)/api/irq.c \
	$(DRIVERS)/api/gpio.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_spi.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_irq.c \
	$(DRIVERS)/platform/$(PLATFORM)/$(PLATFORM)_gpio.c \
	$(DRIVERS)/platform/$(PLATFORM)/rtc.c \
	$(DRIVERS)/platform/$(PLATFORM)/delay.c \
	$(DRIVERS)/platform/$(PLATFORM)/timer.c \
	$(DRIVERS)/platform/$(PLATFORM)/uart.c

INCS += $(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_timer.h \
	$(DRIVERS)/platform/$(PLATFORM)/spi_extra.h \
	$(DRIVERS)/platform/$(PLATFORM)/irq_extra.h \
	$(DRIVERS)/platform/$(PLATFORM)/aducm3029_gpio.h \
	$(DRIVERS)/platform/$(PLATFORM)/uart_extra.h \
	$(DRIVERS)/platform/$(PLATFORM)/rtc_extra.h \
	$(DRIVERS)/platform/$(PLATFORM)/timer_extra.h

IGNORED_FILES += $(PLATFORM_DRIVERS)/uart_stdio.c
IGNORED_FILES += $(PLATFORM_DRIVERS)/uart_stdio.h

TINYIIOD=y
