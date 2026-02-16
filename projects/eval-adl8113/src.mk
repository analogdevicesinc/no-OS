include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk
include $(PROJECT)/src/examples/examples_src.mk

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c

INCS += $(PROJECT)/src/common/common_data.h
SRCS += $(PROJECT)/src/common/common_data.c

INCS += $(PROJECT)/src/platform/platform_includes.h

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h
SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c

INCS += $(INCLUDE)/no_os_delay.h     \
	$(INCLUDE)/no_os_error.h     \
	$(INCLUDE)/no_os_gpio.h      \
	$(INCLUDE)/no_os_print_log.h \
	$(INCLUDE)/no_os_alloc.h     \
	$(INCLUDE)/no_os_mutex.h     \
	$(INCLUDE)/no_os_util.h

SRCS += $(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/amplifiers/adl8113/adl8113.h
SRCS += $(DRIVERS)/amplifiers/adl8113/adl8113.c
