ifeq '$(USE_TCP_SOCKET)' 'y'
ENABLE_IIO_NETWORK ?= y
CFLAGS += -DUSE_TCP_SOCKET
else
ENABLE_IIO_NETWORK ?= n
endif

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network

SRCS	 += $(NO-OS)/util/no_os_circular_buffer.c
INCS	 += $(INCLUDE)/no_os_circular_buffer.h
endif
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(INCLUDE)

SRCS += $(DRIVERS)/api/no_os_timer.c
