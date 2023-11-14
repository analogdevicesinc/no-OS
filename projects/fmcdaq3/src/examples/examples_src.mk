ifeq (y,$(strip $(FMCDAQ3_EXAMPLE)))
CFLAGS += -DFMCDAQ3_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/fmcdaq3/fmcdaq3_example.c
INCS += $(PROJECT)/src/examples/fmcdaq3/fmcdaq3_example.h
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
TINYIIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(TINYIIOD)))
LIBRARIES += iio
SRC_DIRS += $(NO-OS)/iio/iio_app
SRCS += $(NO-OS)/util/no_os_fifo.c 							\
		$(NO-OS)/util/no_os_list.c 							\
		$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.c 		\
		$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.c 		\
		$(DRIVERS)/adc/ad9680/iio_ad9680.c					\
		$(DRIVERS)/dac/ad9152/iio_ad9152.c 					\
		$(DRIVERS)/api/no_os_irq.c 							\
		$(DRIVERS)/api/no_os_uart.c 						\
		$(NO-OS)/util/no_os_lf256fifo.c 					
endif

ifeq (y,$(strip $(TINYIIOD)))
INCS +=	$(DRIVERS)/adc/ad9680/iio_ad9680.h 					\
		$(DRIVERS)/dac/ad9152/iio_ad9152.h					\
		$(INCLUDE)/no_os_fifo.h 							\
		$(INCLUDE)/no_os_irq.h 								\
		$(INCLUDE)/no_os_uart.h 							\
		$(INCLUDE)/no_os_lf256fifo.h 						\
		$(INCLUDE)/no_os_list.h 							\
		$(DRIVERS)/axi_core/iio_axi_adc/iio_axi_adc.h 		\
		$(DRIVERS)/axi_core/iio_axi_dac/iio_axi_dac.h
endif
