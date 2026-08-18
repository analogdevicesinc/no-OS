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
endif
