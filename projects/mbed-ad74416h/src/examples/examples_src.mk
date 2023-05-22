ifeq (y,$(strip $(TEST_EXAMPLE)))
CFLAGS += -DTEST_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/test/test_example.c
INCS += $(PROJECT)/src/examples/test/test_example.h
endif
ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
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
