IIOD=y
INCS += $(DRIVERS)/position/admt4000/iio_admt4000.h
SRCS += $(DRIVERS)/position/admt4000/iio_admt4000.c \
	$(DRIVERS)/position/admt4000/iio_admt4000_trig.c

INCS += $(NO-OS)/libraries/precision-converters-library/common/common.h
INCS += $(NO-OS)/libraries/precision-converters-library/board_info/board_info.h
SRCS += $(NO-OS)/libraries/precision-converters-library/common/common.c
SRCS += $(NO-OS)/libraries/precision-converters-library/board_info/board_info.c

# Include IIO support for Trinamic device if TMC_DEVICE is defined
ifneq ($(strip $(TMC_DEVICE)),)
TMC_DEVICE_LOWER = $(shell echo $(TMC_DEVICE) | tr A-Z a-z)
INCS += $(DRIVERS)/motor/$(TMC_DEVICE_LOWER)/iio_$(TMC_DEVICE_LOWER).h
SRCS += $(DRIVERS)/motor/$(TMC_DEVICE_LOWER)/iio_$(TMC_DEVICE_LOWER).c
endif

CFLAGS += -DIIO_IGNORE_BUFF_OVERRUN_ERR

CFLAGS += -DFIRMWARE_VERSION=\"$(FIRMWARE_VERSION)\"
