#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio/iio_demo

LIBRARIES += iio

SRCS += $(PLATFORM_DRIVERS)/uart.c					\
	$(PLATFORM_DRIVERS)/irq.c					\
	$(PLATFORM_DRIVERS)/axi_io.c					\
	$(PLATFORM_DRIVERS)/delay.c					\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.c			\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.c				\
	$(NO-OS)/util/xml.c						\
	$(NO-OS)/util/list.c						\
	$(NO-OS)/util/fifo.c						\
	$(NO-OS)/util/util.c						\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.c

INCS += $(INCLUDE)/xml.h						\
	$(INCLUDE)/fifo.h						\
	$(INCLUDE)/irq.h						\
	$(INCLUDE)/uart.h						\
	$(INCLUDE)/list.h						\
	$(INCLUDE)/util.h						\
	$(INCLUDE)/error.h						\
	$(INCLUDE)/axi_io.h						\
	$(INCLUDE)/delay.h						\
	$(DRIVERS)/axi_core/axi_dmac/axi_dmac.h				\
	$(DRIVERS)/axi_core/axi_adc_core/axi_adc_core.h			\
	$(PLATFORM_DRIVERS)/irq_extra.h					\
	$(PLATFORM_DRIVERS)/uart_extra.h				\
	$(NO-OS)/iio/iio_axi_adc/iio_axi_adc.h
