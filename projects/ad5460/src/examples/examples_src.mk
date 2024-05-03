ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif
ifeq (y,$(strip $(VOLTAGE_OUTPUT)))
CFLAGS += -DVOLTAGE_OUTPUT=1
SRCS += $(PROJECT)/src/examples/voltage_output/voltage_output.c
INCS += $(PROJECT)/src/examples/voltage_output/voltage_output.h
endif

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
