#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(DRIVERS)/gyro/adxrs290 

# For the moment there is support only for aducm for iio with network backend
ifeq (aducm3029,$(strip $(PLATFORM)))
ifeq '$(USE_TCP_SOCKET)' 'y'
ENABLE_IIO_NETWORK ?= y
CFLAGS += -DUSE_TCP_SOCKET
else
ENABLE_IIO_NETWORK ?= n
endif
endif

ifeq (y,$(strip $(ENABLE_IIO_NETWORK)))
DISABLE_SECURE_SOCKET ?= y
SRC_DIRS += $(NO-OS)/network
SRCS	 += $(NO-OS)/util/no_os_circular_buffer.c
INCS	 += $(INCLUDE)/no_os_circular_buffer.h
endif

SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/iio/iio_app
SRC_DIRS += $(INCLUDE)

SRCS +=	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_gpio.c \
	$(NO-OS)/util/no_os_list.c \
	$(NO-OS)/util/no_os_fifo.c \
	$(NO-OS)/util/no_os_util.c \
	$(DRIVERS)/api/no_os_spi.c
