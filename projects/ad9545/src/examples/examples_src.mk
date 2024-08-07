ifeq (y,$(strip $(AD7091R2)))
CFLAGS += -DAD7091R2_DEV=1
else ifeq (y,$(strip $(AD7091R4)))
CFLAGS += -DAD7091R4_DEV=1
else
CFLAGS += -DAD7091R8_DEV=1
endif

INCS += $(INCLUDE)/no_os_list.h \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIO_TIMER_TRIGGER_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_TIMER_TRIGGER_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.h

SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h

SRCS += $(DRIVERS)/adc/ad7091r8/iio_ad7091r8_trig.c
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/adc/ad7091r8/iio_ad7091r8.h
SRCS += $(DRIVERS)/adc/ad7091r8/iio_ad7091r8.c

INCS += $(INCLUDE)/no_os_list.h \
		$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
endif
