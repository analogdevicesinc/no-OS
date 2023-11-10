# File where libraries are handled

# Used by the build utils
export EXTRA_LIBS_NAMES
export EXTRA_LIBS_PATHS

#	IIO
ifeq (y,$(strip $(IIOD)))

include $(NO-OS)/tools/scripts/iio_srcs.mk

CFLAGS += -DIIO_SUPPORT
endif

# FreeRTOS
ifeq '$(FREERTOS)' 'y'
CFLAGS += -DFREERTOS
include $(NO-OS)/tools/scripts/freertos.mk
endif

#	MBEDTLS
ifneq ($(if $(findstring mbedtls, $(LIBRARIES)), 1),)
# Generic part
MBEDTLS_DIR					= $(NO-OS)/libraries/mbedtls
MBEDTLS_LIB_DIR				= $(MBEDTLS_DIR)/library
MBEDTLS_LIB_NAMES			= libmbedtls.a libmbedx509.a libmbedcrypto.a
MBEDTLS_LIBS				= $(addprefix $(MBEDTLS_LIB_DIR)/,$(MBEDTLS_LIB_NAMES))
EXTRA_LIBS					+= $(MBEDTLS_LIBS)
EXTRA_LIBS_PATHS			+= $(MBEDTLS_LIB_DIR)
EXTRA_INC_PATHS		+= $(MBEDTLS_DIR)/include

#Rules
#If no application specific mbedtls config file is defined, use the standard one.
ifeq ($(MBED_TLS_CONFIG_FILE),)
MBED_TLS_CONFIG_FILE = $(NO-OS)/network/noos_mbedtls_config.h
endif
CLEAN_MBEDTLS	= $(call remove_file,$(MBEDTLS_LIB_DIR)/*.o $(MBEDTLS_LIB_DIR)/*.d $(MBEDTLS_LIBS))
$(MBEDTLS_LIB_DIR)/libmbedcrypto.a: $(MBED_TLS_CONFIG_FILE)
	-$(CLEAN_MBEDTLS)
	$(MAKE) -C $(MBEDTLS_LIB_DIR)
$(MBEDTLS_LIB_DIR)/libmbedx509.a: $(MBEDTLS_LIB_DIR)/libmbedcrypto.a
$(MBEDTLS_LIB_DIR)/libmbedtls.a: $(MBEDTLS_LIB_DIR)/libmbedx509.a

# Custom settings
CFLAGS 		+= -I$(MBEDTLS_DIR)/include
CFLAGS 		+= -I$(dir $(MBED_TLS_CONFIG_FILE)) \
			-DMBEDTLS_CONFIG_FILE=\"$(notdir $(MBED_TLS_CONFIG_FILE))\"
else
DISABLE_SECURE_SOCKET ?= y
endif

#	FATFS
ifneq ($(if $(findstring fatfs, $(LIBRARIES)), 1),)
# Generic part
FATFS_DIR					= $(NO-OS)/libraries/fatfs
FATFS_LIB					= $(FATFS_DIR)/libfatfs.a
EXTRA_LIBS					+= $(FATFS_LIB)
EXTRA_LIBS_PATHS			+= $(FATFS_DIR)
EXTRA_INC_PATHS	+= $(FATFS_DIR)/source

# Rules
CLEAN_FATFS	= $(MAKE) -C $(NO-OS)/libraries/fatfs clean
$(FATFS_LIB):
	$(MAKE) -C $(FATFS_DIR)

# Custom settings
CFLAGS		+= -I$(NO-OS)/drivers/sd-card -I$(NO-OS)/include

endif

#	MQTT
ifneq ($(if $(findstring mqtt, $(LIBRARIES)), 1),)

include $(NO-OS)/tools/scripts/mqtt_srcs.mk

endif

#	AZURE_SDK_FOR_C
ifneq ($(if $(findstring azure-sdk-for-c, $(LIBRARIES)), 1),)

include $(NO-OS)/tools/scripts/azure_sdk_for_c.mk

AZURE_LIBS			= $(AZURE_DIR_BUILD_LIBS)/iot/libaz_iot_hub.a
AZURE_LIBS			+= $(AZURE_DIR_BUILD_LIBS)/iot/libaz_iot_provisioning.a
AZURE_LIBS			+= $(AZURE_DIR_BUILD_LIBS)/platform/libaz_noplatform.a
AZURE_LIBS			+= $(AZURE_DIR_BUILD_LIBS)/platform/libaz_nohttp.a
AZURE_LIBS			+= $(AZURE_DIR_BUILD_LIBS)/iot/libaz_iot_adu.a
AZURE_LIBS			+= $(AZURE_DIR_BUILD_LIBS)/iot/libaz_iot_common.a
AZURE_LIBS			+= $(AZURE_DIR_BUILD_LIBS)/core/libaz_core.a

EXTRA_LIBS			+= $(AZURE_LIBS)
EXTRA_LIBS_PATHS		+= $(AZURE_DIR_BUILD_LIBS)/iot
EXTRA_LIBS_PATHS		+= $(AZURE_DIR_BUILD_LIBS)/platform
EXTRA_LIBS_PATHS		+= $(AZURE_DIR_BUILD_LIBS)/core

AZURE_BUILD_CMD = $(shell mkdir -p $(AZURE_DIR_BUILD) && \
		    cd $(AZURE_DIR_BUILD) && \
		    cmake -DCMAKE_TOOLCHAIN_FILE=../../noos-azure-toolchain.cmake .. >/dev/null && \
		    cmake --build . >/dev/null)

CLEAN_AZURE = $(shell rm -rf $(AZURE_DIR_BUILD))

$(AZURE_LIBS):
	$(AZURE_BUILD_CMD)

# Custom settings
CFLAGS += -I$(AZURE_DIR)/sdk/inc

endif

ifneq ($(if $(findstring lwip, $(LIBRARIES)), 1),)
include $(NO-OS)/tools/scripts/lwip.mk
INCS += $(NO-OS)/libraries/lwip/arch
INCS += $(NO-OS)/libraries/lwip/configs/lwipcfg.h
INCS += $(NO-OS)/libraries/lwip/configs/lwipopts.h
endif

LIB_TARGETS			+= $(IIO_LIB) $(MBEDTLS_LIBS) $(FATFS_LIB) $(MQTT_LIB) $(AZURE_LIBS)
EXTRA_LIBS_NAMES	= $(subst lib,,$(basename $(notdir $(EXTRA_LIBS))))
LIB_FLAGS			+= $(addprefix -l,$(EXTRA_LIBS_NAMES))
LIB_PATHS			+= $(addprefix -L,$(EXTRA_LIBS_PATHS))


#TODO remove afeter changes are done
#Convert variable to linux.mk naming
ifneq (aducm3029,$(strip $(PLATFORM)))
INC_PATHS += $(EXTRA_INC_PATHS)
LIBS += $(LIB_FLAGS)
endif

ifeq (mbed,$(strip $(PLATFORM)))
CLEAN_MBED_OS = $(call remove_dir_action,$(MBED_OS_BUILD_DIRECTORY) $(MBED_APP_JSON_DIRECTORY))
endif

# Build project Release Configuration
PHONY := libs
libs: $(LIB_TARGETS)

PHONY += clean_libs
clean_libs:
	-$(CLEAN_MBEDTLS)
	-$(CLEAN_FATFS)
	-$(CLEAN_MQTT)
	-$(CLEAN_IIO)
	-$(CLEAN_AZURE)
	-$(CLEAN_MBED_OS)
