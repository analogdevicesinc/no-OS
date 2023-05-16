ifeq (y,$(strip $(NETWORKING)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network

SRCS	 += $(NO-OS)/util/no_os_circular_buffer.c
INCS	 += $(INCLUDE)/no_os_circular_buffer.h
endif
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(INCLUDE)

SRCS += $(NO-OS)/util/no_os_alloc.c

CFLAGS += -DENABLE_UART_STDIO
