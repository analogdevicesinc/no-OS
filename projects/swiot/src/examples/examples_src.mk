TINYIIOD=y
SRCS += $(PROJECT)/src/examples/swiot_fw/swiot_fw.c
INCS += $(PROJECT)/src/examples/swiot_fw/swiot_fw.h

SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(DRIVERS)/adc-dac/ad74413r/iio_ad74413r.h
SRCS += $(DRIVERS)/adc-dac/ad74413r/iio_ad74413r.c

INCS += $(DRIVERS)/digital-io/max14906/iio_max14906.h
SRCS += $(DRIVERS)/digital-io/max14906/iio_max14906.c

INCS += $(DRIVERS)/temperature/adt75/iio_adt75.h
SRCS += $(DRIVERS)/temperature/adt75/iio_adt75.c

SRCS += $(NO-OS)/iio/iio_trigger.c
INCS += $(NO-OS)/iio/iio_trigger.h
SRCS += $(DRIVERS)/adc-dac/ad74413r/iio_ad74413r_trig.c

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
