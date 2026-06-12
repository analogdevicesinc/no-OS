NO_OS_INC_DIRS += \
	$(INCLUDE) \
	$(PROJECT)/src/ \
	$(DRIVERS)/api/

INCS += $(DRIVERS)/power/ltc3337/ltc3337.h

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c \
	$(PROJECT)/src/common/common_data.c \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_dma.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(DRIVERS)/power/ltc3337/ltc3337.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c
