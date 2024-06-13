ifeq (y,$(strip $(ADXL380)))
CFLAGS += -DADXL380_DEV=1
else ifeq (y,$(strip $(ADXL382)))
CFLAGS += -DADXL382_DEV=1
endif

ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/example_main.c
INCS += $(PROJECT)/src/examples/basic/example_main.h
endif

ifeq (y,$(strip $(SLF_TST_EXAMPLE)))
CFLAGS += -DSLF_TST_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/selftest_example/st_example_main.c
INCS += $(PROJECT)/src/examples/selftest_example/st_example_main.h
endif