include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c
SRCS += $(PROJECT)/src/common/common_data.c
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

NO_OS_INC_DIRS += \
	$(INCLUDE) \
	$(PROJECT)/src/ \
	$(DRIVERS)/power/ltc4162l \

SRCS += $(DRIVERS)/api/no_os_gpio.c \
		$(NO-OS)/util/no_os_lf256fifo.c \
		$(DRIVERS)/api/no_os_irq.c  \
		$(DRIVERS)/api/no_os_i2c.c  \
		$(DRIVERS)/api/no_os_uart.c \
		$(DRIVERS)/api/no_os_dma.c \
		$(NO-OS)/util/no_os_list.c \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_alloc.c \
		$(NO-OS)/util/no_os_mutex.c

SRCS += $(DRIVERS)/power/ltc4162l/ltc4162l.c
