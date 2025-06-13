ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(DUMMY_EXAMPLE)))
CFLAGS += -DDUMMY_EXAMPLE
SRCS += $(PROJECT)/src/examples/dummy/dummy_example.c
INCS += $(PROJECT)/src/examples/dummy/dummy_example.h
endif

ifeq (y,$(strip $(TRIPLE_TAP_EXAMPLE)))
ifneq 'maxim' '$(PLATFORM)'
$(error Triple Tap Example can only be run on Maxim platform at the moment!)
endif
CFLAGS += -DTRIPLE_TAP_EXAMPLE
SRCS += $(PROJECT)/src/examples/triple_tap/triple_tap_example.c
INCS += $(PROJECT)/src/examples/triple_tap/triple_tap_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

SRCS += $(DRIVERS)/accel/adxl367/iio_adxl367.c \
	$(DRIVERS)/api/no_os_irq.c \
	$(DRIVERS)/api/no_os_uart.c \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c \
	$(NO-OS)/util/no_os_fifo.c

INCS +=	$(DRIVERS)/accel/adxl367/iio_adxl367.h \
	$(INCLUDE)/no_os_uart.h \
	$(INCLUDE)/no_os_irq.h \
	$(INCLUDE)/no_os_fifo.h \
	$(INCLUDE)/no_os_lf256fifo.h
endif
