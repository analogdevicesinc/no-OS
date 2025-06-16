SRCS += $(PROJECT)/main.c \
	$(PROJECT)/console.c \
	$(PROJECT)/uartLoader.c
INCS += $(PROJECT)/console.h \
	$(PROJECT)/../dual-loader/loader.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c \
	$(NO-OS)/util/no_os_alloc.c
INCS += $(INCLUDE)/no_os_gpio.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_irq.h \
	$(INCLUDE)/no_os_error.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_list.h \
	$(INCLUDE)/no_os_alloc.h \
	$(INCLUDE)/no_os_util.h
