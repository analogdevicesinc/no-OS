ifeq (y,$(strip $(ADXL380)))
CFLAGS += -DADXL380_DEV=1
else ifeq (y,$(strip $(ADXL382)))
CFLAGS += -DADXL382_DEV=1
endif

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example_main.c
INCS += $(PROJECT)/src/examples/basic/basic_example_main.h
endif

ifeq (y,$(strip $(SELFTEST_EXAMPLE)))
CFLAGS += -DSELFTEST_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/selftest_example/selftest_example_main.c
INCS += $(PROJECT)/src/examples/selftest_example/selftest_example_main.h
endif

ifeq (y,$(strip $(FIFO_EXAMPLE)))
CFLAGS += -DFIFO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/fifo_example/fifo_example_main.c
INCS += $(PROJECT)/src/examples/fifo_example/fifo_example_main.h
endif

