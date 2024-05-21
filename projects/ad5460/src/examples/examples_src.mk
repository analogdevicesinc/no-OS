ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(CHANNEL_OUTPUT_EXAMPLE)))
CFLAGS += -DCHANNEL_OUTPUT_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/channel_output_example/channel_output_example.c
INCS += $(PROJECT)/src/examples/channel_output_example/channel_output_example.h
endif

ifeq (y,$(strip $(VOLTAGE_OUTPUT_EXAMPLE)))
CFLAGS += -DVOLTAGE_OUTPUT_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/voltage_output_example/voltage_output_example.c
INCS += $(PROJECT)/src/examples/voltage_output_example/voltage_output_example.h
endif

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
