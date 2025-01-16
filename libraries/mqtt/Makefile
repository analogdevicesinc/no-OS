remove_fun = rm -rf $(1)
MXQ_FW_UPDATE_CAP:=1
export MXQ_FW_UPDATE_CAP

ifeq "$(TARGET)" ""
$(error TARGET must be specified)
endif

TARGET_UC:=$(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LC:=$(shell echo $(TARGET) | tr A-Z a-z)
ifeq "$(COMPILER)" ""
$(error COMPILER must be specified)
endif

ifeq "$(MAXQ10XX_API_LIBS_DIR)" ""
MAXQ10XX_API_LIBS_DIR = $(shell pwd)
endif

APISRCDIR		:= $(MAXQ10XX_API_DIR)/src
TARGETDIR	:= $(MAXQ10XX_API_DIR)/src/target/target_$(TARGET_CLIENT)


MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/include/internal

ifeq "$(USE_USS_PSA)" "YES"
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/src/api/internal
endif

MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/maxq1080_hal/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(CMSIS_ROOT)/../PeriphDrivers/Include/$(TARGET_UC)
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Include

MAXQ10XX_API_LIBS_C_FILES += $(TARGETDIR)/host_max32xxx.c
MAXQ10XX_API_LIBS_C_FILES += $(TARGETDIR)/host_max32xxx_i2c.c
MAXQ10XX_API_LIBS_C_FILES += $(TARGETDIR)/host_max32xxx_mailbox.c
MAXQ10XX_API_LIBS_C_FILES += $(TARGETDIR)/host_max32xxx_spi.c

MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/MXQ_API.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/MXQ_CMD.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/MXQ_GCU.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/MXQ_Print.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/MXQ_Transport.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/crc.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/aes/aes.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-gcm.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-internal.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-internal-dec.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-internal-enc.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/aes_gcm/wpa_debug.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/ecc/uECC.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/hmac/hkdf.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/hmac/hmac_sha2.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/api/internal/crypto/hmac/sha2.c

ifeq "$(USE_USS_PSA)" "YES"
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/crypto_common.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/crypto_unimplemented.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/crypto_hash.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/crypto_sign.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/crypto_aead.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/psa_key_management.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/crypto_mac.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/crypto_key_mgmt_utilities.c
MAXQ10XX_API_LIBS_C_FILES += $(APISRCDIR)/psa/psa_cipher.c
endif

# Where to find header files for this project
MAXQ10XX_API_LIBS_H_FILES +=  $(shell find $(MAXQ10XX_API_LIBS_INCLUDE_DIR) -wholename '*.h')

MQTT_DIR	= $(realpath .)
PAHO_DIR	= $(MQTT_DIR)/paho.mqtt.embedded-c
PAHO_PACKET_DIR = $(PAHO_DIR)/MQTTPacket/src
PAHO_CLIENT_DIR = $(PAHO_DIR)/MQTTClient-C/src

SRCS = mqtt_client.c mqtt_noos_support.c
SRCS += $(PAHO_PACKET_DIR)/MQTTConnectClient.c\
	$(PAHO_PACKET_DIR)/MQTTDeserializePublish.c\
	$(PAHO_PACKET_DIR)/MQTTFormat.c\
	$(PAHO_PACKET_DIR)/MQTTPacket.c\
	$(PAHO_PACKET_DIR)/MQTTSerializePublish.c\
	$(PAHO_PACKET_DIR)/MQTTSubscribeClient.c\
	$(PAHO_PACKET_DIR)/MQTTUnsubscribeClient.c

SRCS += $(PAHO_CLIENT_DIR)/MQTTClient.c

SRCS += $MAXQ10XX_API_LIBS_C_FILES

OBJS = $(SRCS:.c=.o)

CFLAGS += -DMQTTCLIENT_PLATFORM_HEADER="mqtt_noos_support.h" \
		-I"$(PAHO_CLIENT_DIR)" \
		-I"$(PAHO_PACKET_DIR)" \
		-I"$(MQTT_DIR)"

LIBRARY = libmqtt.a

.PHONEY = all clean

ifeq ($(wildcard $(PAHO_DIR)/README.md),)
all:
	git submodule update --init --remote -- $(PAHO_DIR)
	$(MAKE) $(LIBRARY)
else
all: $(LIBRARY)
endif

export BUILD_DIR=C:/Users/NBroderi/Documents/GitHub/nBroderi-no-OS/libraries/USS/libs_mk/maxq10xx_api/bin/softfp
export MAXQ10XX_API_MK_DIR=C:/Users/NBroderi/Documents/GitHub/nBroderi-no-OS/libraries/USS/libs_mk/maxq10xx_api
export MAXQ10XX_FW_MK_DIR=C:/Users/NBroderi/Documents/GitHub/nBroderi-no-OS/libraries/USS/libs_mk/maxq10xx_fw
include ${MAXQ10XX_API_MK_DIR}/maxq10xx_api_files.mk
include $(MAXQ10XX_API_MK_DIR)/maxq10xx_api.mk

$(LIBRARY): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	-$(call remove_fun,$(OBJS) $(LIBRARY))
