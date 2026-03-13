include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c \
	$(PROJECT)/src/common/common_data.c \
	$(PROJECT)/src/common/cn0391.c \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.c

INCS += $(PROJECT)/src/common/common_data.h \
	$(PROJECT)/src/common/cn0391.h \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.h

SRCS += $(DRIVERS)/adc/ad7124/ad7124.c
INCS += $(DRIVERS)/adc/ad7124/ad7124.h

SRCS += $(DRIVERS)/temperature/thermocouples/no_os_thermocouple.c \
	$(DRIVERS)/temperature/rtd/no_os_rtd.c
INCS += $(DRIVERS)/temperature/thermocouples/no_os_thermocouple.h \
	$(DRIVERS)/temperature/rtd/no_os_rtd.h

SRCS += $(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(DRIVERS)/api/no_os_timer.c

SRCS += $(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_crc8.c

INCS += $(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_dma.h \
	$(INCLUDE)/no_os_timer.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_crc8.h \
	$(INCLUDE)/no_os_print_log.h
