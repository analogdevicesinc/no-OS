#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/src
SRC_DIRS += $(NO-OS)/drivers/adc/ad7124

SRC_DIRS += $(NO-OS)/network
SRC_DIRS += $(PLATFORM_DRIVERS)
SRC_DIRS += $(NO-OS)/util
SRC_DIRS += $(INCLUDE)

LIBRARIES += iio

DISABLE_SECURE_SOCKET=y

