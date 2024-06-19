ifeq (y,$(strip $(FRAME_RX_TX)))
CFLAGS += -DFRAME_RX_TX
SRCS += $(PROJECT)/src/examples/frame_rx_tx/frame_rx_tx_example.c
INCS += $(PROJECT)/src/examples/frame_rx_tx/frame_rx_tx_example.h
endif

INCS += $(DRIVERS)/net/adin1110/adin1110.h
SRCS += $(DRIVERS)/net/adin1110/adin1110.c
