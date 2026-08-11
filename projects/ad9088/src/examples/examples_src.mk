CFLAGS += -DB0 -I$(BUILD_DIR)/app/noos/drivers/rf-transceiver/apollo/private/inc/bitfields/b0/

ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h
endif

ifeq (y,$(strip $(DMA_EXAMPLE)))
CFLAGS += -DDMA_EXAMPLE
SRCS += $(PROJECT)/src/examples/dma/dma_example.c
INCS += $(PROJECT)/src/examples/dma/dma_example.h
SRCS += $(NO-OS)/util/no_os_tone.c
INCS += $(INCLUDE)/no_os_tone.h
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
IIOD=y
CFLAGS += -DIIO_EXAMPLE
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(IIOD)))
SRC_DIRS += $(NO-OS)/iio/iio_app

INCS += $(INCLUDE)/no_os_list.h \
	$(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
endif
