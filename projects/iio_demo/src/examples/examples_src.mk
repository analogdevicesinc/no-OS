ifeq (y,$(strip $(IIO_EXAMPLE)))
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIO_TIMER_TRIGGER_EXAMPLE)))
CFLAGS += -DIIO_TIMER_TRIGGER_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_timer_trigger_example/iio_timer_trigger_example.h

SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h

SRCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo_trig.c \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo_trig.c
endif

ifeq (y,$(strip $(IIO_SW_TRIGGER_EXAMPLE)))
CFLAGS += -DIIO_SW_TRIGGER_EXAMPLE

SRCS += $(PROJECT)/src/examples/iio_sw_trigger_example/iio_sw_trigger_example.c
INCS += $(PROJECT)/src/examples/iio_sw_trigger_example/iio_sw_trigger_example.h

SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h

SRCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo_trig.c
SRCS += $(DRIVERS)/dac/dac_demo/iio_dac_demo_trig.c
endif

IIOD=y

SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.h \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.h

SRCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.c \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.c

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
