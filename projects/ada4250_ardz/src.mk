#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(DRIVERS)/amplifiers/ada4250

SRCS += $(NO-OS)/util/no_os_util.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c \
	$(NO-OS)/util/no_os_lf256fifo.c \
	$(DRIVERS)/api/no_os_spi.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(DRIVERS)/api/no_os_timer.c \
	$(DRIVERS)/api/no_os_uart.c

INCS += $(INCLUDE)/no_os_spi.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_lf256fifo.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_mutex.h
