SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_i2c.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_delay.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_crc8.c

INCS += $(INCLUDE)/no_os_delay.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_i2c.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_util.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_mutex.h \
	$(INCLUDE)/no_os_crc8.h

SRCS += $(DRIVERS)/power/ltm3360b/ltm3360b.c
INCS += $(DRIVERS)/power/ltm3360b/ltm3360b.h

SRCS += $(PROJECT)/src/common/common_data.c \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.c \
	$(PROJECT)/src/platform/$(PLATFORM)/main.c \
	$(PROJECT)/src/examples/basic/basic_example.c

INCS += $(PROJECT)/src/common/common_data.h \
	$(PROJECT)/src/platform/$(PLATFORM)/parameters.h