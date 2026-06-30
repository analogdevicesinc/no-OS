IIOD=y

INCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.h \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.h

SRCS += $(DRIVERS)/adc/adc_demo/iio_adc_demo.c \
        $(DRIVERS)/dac/dac_demo/iio_dac_demo.c

INCS += $(INCLUDE)/no_os_list.h \
        $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
