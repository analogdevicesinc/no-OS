ifeq (y,$(strip $(BASIC_EXAMPLE)))
CFLAGS += -DBASIC_EXAMPLE
SRCS += $(PROJECT)/src/examples/basic_example/basic_example.c
INCS += $(PROJECT)/src/examples/basic_example/basic_example.h
endif

ifeq (y,$(strip $(IIO_EXAMPLE)))
TINYIIOD=y
CFLAGS += -DIIO_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/iio_example/iio_example.c
INCS += $(PROJECT)/src/examples/iio_example/iio_example.h
endif

ifeq (y,$(strip $(DMA_EXAMPLE)))
CFLAGS += -DDMA_EXAMPLE=1
SRCS += $(PROJECT)/src/examples/dma_example/dma_example.c
INCS += $(PROJECT)/src/examples/dma_example/dma_example.h
endif

ifeq (y,$(strip $(TINYIIOD)))
SRCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.c
INCS += $(PLATFORM_DRIVERS)/$(PLATFORM)_uart.h
endif
