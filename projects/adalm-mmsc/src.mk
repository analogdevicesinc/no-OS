include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(INCLUDE)/no_os_spi.h \
	$(DRIVERS)/adc/ad4080/ad4080.h \
	$(DRIVERS)/adc/ad4080/iio_ad4080.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_dma.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_timer.h

SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/adc/ad4080/ad4080.c \
	$(DRIVERS)/adc/ad4080/iio_ad4080.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(DRIVERS)/api/no_os_timer.c

CFLAGS+=-DFIFO_DEPTH=$(FIFO_DEPTH)
