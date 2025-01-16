CFLAGS += -DMQTTCLIENT_PLATFORM_HEADER="mqtt_noos_support.h"
export USS_DIR := $(NO-OS)/Libraries/USS
LIBS_MK_DIR := $(USS_DIR)/libs_mk
export MAXQ10XX_FW_LIBS_DIR := $(USS_DIR)/maxq10xx_fw
export MAXQ10XX_SDK_LIBS_DIR := $(USS_DIR)/maxq10xx-sdk
export MAXQ10XX_API_DIR := $(MAXQ10XX_SDK_LIBS_DIR)/maxq10xx-api
PREBUILT_FW_LIB_DIR := $(MAXQ10XX_FW_LIBS_DIR)/lib
TARGET := MAX32650
TARGET_UC := MAX32650
TARGET_LC := max32650
export TARGET
export TARGET_UC
export TARGET_LC
export MFLOAT_ABI := softfp
TARGET_SMNGR=maxq1065
export TARGET_SMNGR
DEBUG_MAXQ10XX_API=YES
export DEBUG_MAXQ10XX_API
SOCKETS_DEBUG=YES
export SOCKETS_DEBUG 
CRYPTO=MBEDTLS
export CRYPTO
TEST_TLS_VERSION=TLS-v1.3
export TEST_TLS_VERSION
TEST_CRYPTO_ALGORITHM=ECC
export TEST_CRYPTO_ALGORITHM

# Where to find header files for this project
IPATH += .
IPATH += $(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/include
IPATH += $(MAXQ10XX_API_DIR)/include
IPATH += $(MAXQ10XX_API_DIR)/include/internal
IPATH += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/ecc
IPATH += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/aes
IPATH += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/aes_gcm
IPATH += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/hmac
IPATH += $(MAXQ10XX_SDK_LIBS_DIR)



LDFLAGS += -L$(USS_DIR)/maxq10xx_fw/MAXQ10xxFW/maxq1080_hal/lib
LDFLAGS += -L$(USS_DIR)/maxq10xx_fw/lib

#EXTRA_LIBS_PATHS +=  $(NO-OS)/libraries/USS/maxq10xx-sdk/maxq10xx-api/include
#EXTRA_LIBS_PATHS += $(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/include
#EXTRA_LIBS_PATHS += $(MAXQ10XX_API_DIR)/include
#EXTRA_LIBS_PATHS += $(MAXQ10XX_API_DIR)/include/internal
#EXTRA_LIBS_PATHS += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/ecc
#EXTRA_LIBS_PATHS += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/aes
#EXTRA_LIBS_PATHS += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/aes_gcm
#EXTRA_LIBS_PATHS += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/hmac

#CFLAGS += -I$(EXTRA_LIBS_PATHS)
CFLAGS += -DUSE_MAXQ1065
CFLAGS += -DMAXQ1065
CFLAGS += -DUSE_PROTOCOL_SPI

CFLAGS += -DMXQ_NO_PRINTF

LDFLAGS += -L$(NO-OS)/libraries/USS/maxq10xx_fw/MAXQ10xxFW/maxq1080_hal/lib/
LDFLAGS += -L$(NO-OS)/libraries/USS/maxq10xx_fw/lib/
LDFLAGS	+= -L$(NO-OS)/libraries/USS/libs_mk/maxq10xx_api/bin/softfp/
LDFLAGS	+= -L$(NO-OS)/libraries/mbedtls/library


MQTT_DIR	= $(NO-OS)/libraries/mqtt
PAHO_DIR	= $(MQTT_DIR)/paho.mqtt.embedded-c

SRC_DIRS += $(PAHO_DIR)/MQTTPacket/src

SRCS += $(MQTT_DIR)/mqtt_client.c \
	$(MQTT_DIR)/mqtt_noos_support.c \
	$(PAHO_DIR)/MQTTClient-C/src/MQTTClient.c \
	$(MAXQ10XX_API_DIR)/src/api/MXQ_API.c 

INCS += $(MQTT_DIR)/mqtt_client.h \
	$(MQTT_DIR)/MXQ_API.h \
	$(MQTT_DIR)/MXQ_Config.h \
	$(MQTT_DIR)/MXQ_Error.h \
	$(MQTT_DIR)/MXQ_Host.h \
	$(MQTT_DIR)/MXQ_Types.h \
	$(MQTT_DIR)/sdk_version.h \
	$(MQTT_DIR)/mqtt_noos_support.h \
	$(PAHO_DIR)/MQTTClient-C/src/MQTTClient.h

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

# This is the path to the CMSIS root directory

CMSIS_ROOT=C:/analog/cfs/1.0.1/SDK/MAX/Libraries/CMSIS

LIBS_DIR=C:/analog/cfs/1.0.1/SDK/MAX/Libraries


PERIPH_DRIVER := $(LIBS_DIR)/PeriphDrivers

APISRCDIR		:= $(MAXQ10XX_API_DIR)/src
TARGET_CLIENT=max32xxx
TARGETDIR	:= $(MAXQ10XX_API_DIR)/src/target/target_$(TARGET_CLIENT)


MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/include/internal


MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/maxq1080_hal/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(CMSIS_ROOT)/../PeriphDrivers/Include/$(TARGET_UC)
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(CMSIS_ROOT)/Device/Maxim/$(TARGET_UC)/Include
MAXQ10XX_API_LIBS_INCLUDE_DIR +=  $(NO-OS)/libraries/USS/maxq10xx-sdk/maxq10xx-api/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/include
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/include/internal
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/ecc
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/aes
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/aes_gcm
MAXQ10XX_API_LIBS_INCLUDE_DIR += $(MAXQ10XX_API_DIR)/src/api/internal/crypto/hmac

SRCS += $(TARGETDIR)/host_max32xxx.c
SRCS += $(TARGETDIR)/host_max32xxx_i2c.c
SRCS += $(TARGETDIR)/host_max32xxx_mailbox.c
SRCS += $(TARGETDIR)/host_max32xxx_spi.c

SRCS += $(APISRCDIR)/api/MXQ_API.c
SRCS += $(APISRCDIR)/api/internal/MXQ_CMD.c
SRCS += $(APISRCDIR)/api/internal/MXQ_GCU.c
SRCS += $(APISRCDIR)/api/internal/MXQ_Print.c
SRCS += $(APISRCDIR)/api/internal/MXQ_Transport.c
SRCS += $(APISRCDIR)/api/internal/crypto/crc.c
SRCS += $(APISRCDIR)/api/internal/crypto/aes/aes.c
SRCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-gcm.c
SRCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-internal.c
SRCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-internal-dec.c
SRCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes-internal-enc.c
SRCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/wpa_debug.c
SRCS += $(APISRCDIR)/api/internal/crypto/ecc/uECC.c
SRCS += $(APISRCDIR)/api/internal/crypto/hmac/hkdf.c
SRCS += $(APISRCDIR)/api/internal/crypto/hmac/hmac_sha2.c
SRCS += $(APISRCDIR)/api/internal/crypto/hmac/sha2.c


# Where to find header files for this project
INCS +=  $(shell find $(MAXQ10XX_API_LIBS_INCLUDE_DIR) -wholename '*.h')
INCS += $(APISRCDIR)/api/internal/crypto/ecc/asm_arm.inc
INCS += $(APISRCDIR)/api/internal/crypto/ecc/curve-specific.inc
INCS += $(APISRCDIR)/api/internal/crypto/ecc/platform-specific.inc
INCS += $(APISRCDIR)/api/internal/crypto/ecc/types.h
INCS += $(APISRCDIR)/api/internal/crypto/ecc/uECC.h
INCS += $(APISRCDIR)/api/internal/crypto/ecc/uECC_vli.h
INCS += $(APISRCDIR)/api/internal/crypto/hmac/hkdf.h
INCS += $(APISRCDIR)/api/internal/crypto/hmac/hmac_sha2.h
INCS += $(APISRCDIR)/api/internal/crypto/hmac/sha2.h
INCS += $(APISRCDIR)/api/internal/crypto/aes/aes.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes_i.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/aes_wrap.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/build_config.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/common.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/crypto.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/includes.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/os.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/wpa_debug.h
INCS += $(APISRCDIR)/api/internal/crypto/aes_gcm/wpabuf.h

INCS += $(MAXQ10XX_API_DIR)/include/MXQ_API.h
INCS += $(MAXQ10XX_API_DIR)/include/MXQ_Config.h
INCS += $(MAXQ10XX_API_DIR)/include/MXQ_Error.h
INCS += $(MAXQ10XX_API_DIR)/include/MXQ_Host.h
INCS += $(MAXQ10XX_API_DIR)/include/MXQ_Types.h
INCS += $(MAXQ10XX_API_DIR)/include/sdk_version.h
INCS += $(MAXQ10XX_API_DIR)/include/USS_API.h
INCS += $(MAXQ10XX_API_DIR)/include/MXQ_CMD.h
INCS += $(MAXQ10XX_API_DIR)/include/internal/MXQ_Doc.h
INCS += $(MAXQ10XX_API_DIR)/include/internal/MXQ_GCU.h
INCS += $(MAXQ10XX_API_DIR)/include/internal/MXQ_Internal.h
INCS += $(MAXQ10XX_API_DIR)/include/internal/MXQ_Print.h
INCS += $(MAXQ10XX_API_DIR)/include/internal/MXQ_Transport.h
