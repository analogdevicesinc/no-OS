SRCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.c
SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_delay.c

INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h \
	$(INCLUDE)/no_os_gpio.h
