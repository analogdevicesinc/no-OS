ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
endif

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/uart_extra.h
