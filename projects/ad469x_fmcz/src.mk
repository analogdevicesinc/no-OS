include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/app_config.h
INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

SRCS += $(DRIVERS)/api/no_os_uart.c		\
	$(DRIVERS)/api/no_os_gpio.c		\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_util.c		\
	$(NO-OS)/util/no_os_alloc.c		\
	$(NO-OS)/util/no_os_mutex.c		\
	$(NO-OS)/util/no_os_circular_buffer.c	\
	$(NO-OS)/util/no_os_list.c		\
	$(NO-OS)/util/no_os_lf256fifo.c		\
	$(NO-OS)/util/no_os_fifo.c		\
	$(DRIVERS)/api/no_os_spi.c		\
	$(DRIVERS)/api/no_os_pwm.c

INCS += $(INCLUDE)/no_os_delay.h	\
	$(INCLUDE)/no_os_gpio.h		\
	$(INCLUDE)/no_os_error.h	\
	$(INCLUDE)/no_os_fifo.h		\
	$(INCLUDE)/no_os_irq.h		\
	$(INCLUDE)/no_os_lf256fifo.h	\
	$(INCLUDE)/no_os_list.h		\
	$(INCLUDE)/no_os_dma.h		\
	$(INCLUDE)/no_os_timer.h	\
	$(INCLUDE)/no_os_uart.h		\
	$(INCLUDE)/no_os_util.h		\
	$(INCLUDE)/no_os_alloc.h	\
	$(INCLUDE)/no_os_mutex.h	\
	$(INCLUDE)/no_os_mutex.h	\
	$(INCLUDE)/no_os_spi.h		\
	$(INCLUDE)/no_os_pwm.h		\
	$(INCLUDE)/no_os_print_log.h	\
	$(INCLUDE)/no_os_axi_io.h

INCS += $(DRIVERS)/adc/ad469x/ad469x.h
SRCS += $(DRIVERS)/adc/ad469x/ad469x.c

ifeq (ad4695,$(strip $(AD469X_DEV)))
CFLAGS += -DAD4695_DEV
else ifeq (ad4696,$(strip $(AD469X_DEV)))
CFLAGS += -DAD4696_DEV
else ifeq (ad4697,$(strip $(AD469X_DEV)))
CFLAGS += -DAD4697_DEV
else ifeq (ad4698,$(strip $(AD469X_DEV)))
CFLAGS += -DAD4698_DEV
endif

ifeq (y,$(strip $(AD469X_TEMP)))
CFLAGS += -DAD469X_TEMP_ENABLE
endif

ifeq (standard, $(strip $(AD469X_SEQ)))
CFLAGS += -DAD469X_SEQ_STANDARD
else ifeq (advanced,$(strip $(AD469X_SEQ)))
CFLAGS += -DAD469X_SEQ_ADVANCED
else ifeq (single,$(strip $(AD469X_SEQ)))
CFLAGS += -DAD469X_SEQ_SINGLE
endif

