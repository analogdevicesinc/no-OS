ifeq (y,$(strip $(ADMT4000_IIO)))
IIOD=y
CFLAGS += -DADMT4000_IIO=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app
INCS += $(DRIVERS)/magnetometer/admt4000/iio_admt4000.h
SRCS += $(DRIVERS)/magnetometer/admt4000/iio_admt4000.c 

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h

endif

ifeq (y,$(strip $(ADMT4000_TEST)))
CFLAGS += -DADMT4000_TEST=1
SRCS += $(ADMT_EXAMPLES)/test/admt4000_function_test.c		
INCS += $(ADMT_EXAMPLES)/test/admt4000_function_test.h	
endif

ifeq (y,$(strip $(ADMT4000_BASIC)))
CFLAGS += -DADMT4000_BASIC=1
SRCS += $(ADMT_EXAMPLES)/basic_example/admt4000_basic.c		
INCS += $(ADMT_EXAMPLES)/basic_example/admt4000_basic.h	
endif

ifeq (y,$(strip $(ADMT4000_DIAG)))
CFLAGS += -DADMT4000_DIAG=1
SRCS += $(ADMT_EXAMPLES)/diagnostic_example/admt4000_diag_example.c		
INCS += $(ADMT_EXAMPLES)/diagnostic_example/admt4000_diag_example.h	
endif

ifeq (y,$(strip $(ADMT4000_ECC)))
CFLAGS += -DADMT4000_ECC=1
SRCS += $(ADMT_EXAMPLES)/ecc_example/admt4000_ecc.c		
INCS += $(ADMT_EXAMPLES)/ecc_example/admt4000_ecc.h	
endif
