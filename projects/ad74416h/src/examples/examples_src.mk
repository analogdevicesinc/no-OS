ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(TEST_EXAMPLE)))
CFLAGS += -DTEST_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/test/test_example.c
INCS += $(PROJECT)/src/examples/test/test_example.h
endif

ifeq (y,$(strip $(VOLTAGE_OUTPUT_EXAMPLE)))
CFLAGS += -DVOLTAGE_OUTPUT_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/voltage_output/voltage_output.c
INCS += $(PROJECT)/src/examples/voltage_output/voltage_output.h
endif

ifeq (y,$(strip $(CURRENT_OUTPUT_EXAMPLE)))
CFLAGS += -DCURRENT_OUTPUT_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/current_output/current_output.c
INCS += $(PROJECT)/src/examples/current_output/current_output.h
endif

ifeq (y,$(strip $(VOLTAGE_INPUT_EXAMPLE)))
CFLAGS += -DVOLTAGE_INPUT_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/voltage_input/voltage_input.c
INCS += $(PROJECT)/src/examples/voltage_input/voltage_input.h
endif

ifeq (y,$(strip $(CURRENT_INPUT_EXT_EXAMPLE)))
CFLAGS += -DCURRENT_INPUT_EXT_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/current_input_ext/current_input_ext.c
INCS += $(PROJECT)/src/examples/current_input_ext/current_input_ext.h
endif

ifeq (y,$(strip $(MULTIPLE_DEVICES_EXAMPLE)))
CFLAGS += -DMULTIPLE_DEVICES_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/multiple_devices/multiple_devices.c
INCS += $(PROJECT)/src/examples/multiple_devices/multiple_devices.h
endif

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
