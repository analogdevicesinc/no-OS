LIB_FLAGS += -lm
LIBRARIES += lwip
SRC_DIRS += $(NO-OS)/network
SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/iio
SRC_DIRS += $(NO-OS)/iio/iio_app
SRC_DIRS += $(NO-OS)/drivers/photo-electronic/adpd188
SRC_DIRS += $(NO-OS)/drivers/adc/ad7799
SRC_DIRS += $(NO-OS)/drivers/adc/ad7793
SRC_DIRS += $(DRIVERS)/net/adin1110
SRC_DIRS += $(NO-OS)/network/lwip_raw_socket/netdevs/adin1110
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE) \
		$(DRIVERS)/api \
		$(PLATFORM_DRIVERS) \
		$(PLATFORM_DRIVERS)/../common
SRC_DIRS += $(NO-OS)/src/Fonts
SRC_DIRS += $(NO-OS)/src/LCD
SRC_DIRS += $(NO-OS)/src/Lib

TINYIIOD=y

