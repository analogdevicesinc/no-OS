# See No-OS/tool/scripts/src_model.mk for variable description
SRC_DIRS += $(PROJECT)/src
SRCS += $(DRIVERS)/api/no_os_gpio.c \
        $(DRIVERS)/api/no_os_spi.c \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_spi.c \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c \
	$(DRIVERS)/dac/ad5758/ad5758.c \
	$(NO-OS)/util/no_os_util.c \
        $(NO-OS)/util/no_os_alloc.c \
	$(NO-OS)/util/no_os_mutex.c

INCS += $(INCLUDE)/no_os_gpio.h \
	$(INCLUDE)/no_os_spi.h \
        $(INCLUDE)/no_os_error.h \
        $(INCLUDE)/no_os_delay.h \
        $(INCLUDE)/no_os_print_log.h \
        $(INCLUDE)/no_os_util.h \
        $(INCLUDE)/no_os_alloc.h \
        $(INCLUDE)/no_os_mutex.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_gpio.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_spi.h	\
	$(DRIVERS)/dac/ad5758/ad5758.h
