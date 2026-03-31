ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
endif

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(COMPREHENSIVE_EXAMPLE)))
CFLAGS += -DCOMPREHENSIVE_EXAMPLE
SRCS += $(PROJECT)/src/examples/comprehensive/comprehensive_example.c
INCS += $(PROJECT)/src/examples/comprehensive/comprehensive_example.h
endif
