INCS += $(DRIVERS)/rtc/pcf85263/pcf85263.h \
	$(DRIVERS)/display/nhd_c12832a1z/nhd_c12832a1z.h

SRCS += $(DRIVERS)/rtc/pcf85263/pcf85263.c \
	$(DRIVERS)/display/nhd_c12832a1z/nhd_c12832a1z.c

SRC_DIRS += $(NO-OS)/network

LIBRARIES += mqtt

ifeq (y,$(strip $(AZURE_IOT_HUB)))
INCS += $(PROJECT)/src/examples/iot_example/iot_sample_common.h
SRCS += $(PROJECT)/src/examples/iot_example/iot_sample_common.c

MBED_TLS_CONFIG_FILE = $(PROJECT)/src/examples/iot_example/noos_mbedtls_config.h

LIBRARIES += mbedtls
LIBRARIES += azure-sdk-for-c
endif
