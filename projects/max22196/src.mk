NO_OS_INC_DIRS += \
	$(INCLUDE) \
	$(PROJECT)/src/ \
	$(DRIVERS)/api/

INCS += $(DRIVERS)/digital-io/max22196/max22196.h

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c \
	$(PROJECT)/src/common/common_data.c \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/digital-io/max22196/max22196.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c
