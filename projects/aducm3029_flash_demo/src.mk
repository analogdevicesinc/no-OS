#See No-OS/tool/scripts/src_model.mk for variable description

INCS += $(INCLUDE)/no_os_flash.h \
	$(INCLUDE)/no_os_irq.h   \
	$(INCLUDE)/no_os_uart.h

SRCS += $(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_uart.c

SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)
