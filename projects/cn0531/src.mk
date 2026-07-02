#See No-OS/tool/scripts/src_model.mk for variable description

SRCS += $(DRIVERS)/api/no_os_spi.c   \
	$(DRIVERS)/api/no_os_gpio.c  \
	$(DRIVERS)/api/no_os_irq.c   \
	$(DRIVERS)/api/no_os_timer.c \
	$(DRIVERS)/api/no_os_uart.c

INCS += $(INCLUDE)/no_os_spi.h  \
	$(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_irq.h

SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)
