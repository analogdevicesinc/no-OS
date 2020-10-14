# File where libraries are handled

#	IIO
ifneq ($(if $(findstring iio, $(LIBRARIES)), 1),)
# Generic part
IIO_DIR						= $(NO-OS)/libraries/iio
IIO_LIB						= $(IIO_DIR)/libiio.a
EXTRA_LIBS					+= $(IIO_LIB)
EXTRA_LIBS_PATHS			+= $(IIO_DIR)
EXTRA_LIBS_INCLUDE_PATHS	+= $(IIO_DIR)
ifeq ($(wildcard $(IIO_DIR)/libtinyiiod/.git),)
INIT_SUBMODULES				+= git submodule update --init --remote -- $(IIO_DIR)/libtinyiiod;
endif

# Rules
CLEAN_IIO	= $(MAKE) -C $(IIO_DIR) clean
$(IIO_LIB):
	$(MAKE) -C $(IIO_DIR)

# Custom settings
CFLAGS += -DIIO_SUPPORT
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
EXTRA_LIBS_INCLUDE_PATHS	+= $(MBEDTLS_DIR)/include
ifeq ($(wildcard $(MBEDTLS_DIR)/.git),)
INIT_SUBMODULES				+= git submodule update --init --remote -- $(MBEDTLS_DIR);
endif

#Rules
MBED_TLS_CONFIG_FILE = $(NO-OS)/network/noos_mbedtls_config.h
CLEAN_MBEDTLS	= $(call remove_fun,$(MBEDTLS_LIB_DIR)/*.o $(MBEDTLS_LIBS))
$(MBEDTLS_LIB_DIR)/libmbedcrypto.a: $(MBED_TLS_CONFIG_FILE)
	-$(CLEAN_MBEDTLS)
	$(MAKE) -C $(MBEDTLS_LIB_DIR)
$(MBEDTLS_LIB_DIR)/libmbedx509.a: $(MBEDTLS_LIB_DIR)/libmbedcrypto.a
$(MBEDTLS_LIB_DIR)/libmbedtls.a: $(MBEDTLS_LIB_DIR)/libmbedx509.a

# Custom settings
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
EXTRA_LIBS_INCLUDE_PATHS	+= $(FATFS_DIR)/source

# Rules
CLEAN_FATFS	= $(MAKE) -C $(NO-OS)/libraries/fatfs clean
$(FATFS_LIB):
	$(MAKE) -C $(FATFS_DIR)

# Custom settings
CFLAGS		+= -I$(DRIVERS)/sd-card -I$(INCLUDE)

endif

#	MQTT
ifneq ($(if $(findstring mqtt, $(LIBRARIES)), 1),)
# Generic part
MQTT_DIR					= $(NO-OS)/libraries/mqtt
MQTT_LIB					= $(MQTT_DIR)/libmqtt.a
EXTRA_LIBS					+= $(MQTT_LIB)
EXTRA_LIBS_PATHS			+= $(MQTT_DIR)
EXTRA_LIBS_INCLUDE_PATHS	+= $(MQTT_DIR)

CLEAN_MQTT	= $(MAKE) -C $(MQTT_DIR) clean
$(MQTT_LIB):
	$(MAKE) -C $(MQTT_DIR)

endif

LIB_TARGETS			+= $(IIO_LIB) $(MBEDTLS_TARGETS) $(FATFS_LIB) $(MQTT_LIB)
EXTRA_LIBS_NAMES	= $(subst lib,,$(basename $(notdir $(EXTRA_LIBS))))
LIB_FLAGS			= $(addprefix -l,$(EXTRA_LIBS_NAMES))
LIB_DIR_FLAGS		= $(addprefix -L,$(EXTRA_LIBS_PATHS))
INCLUDE_DIRS		+= $(EXTRA_LIBS_INCLUDE_PATHS)

#Convert variable to linux.mk naming
ifneq (aducm3029,$(strip $(PLATFORM)))
INC_PATHS += $(addprefix -I,$(INCLUDE_DIRS))
LIB_PATHS += $(LIB_DIR_FLAGS)
LIBS += $(LIB_FLAGS)
endif

# Build project Release Configuration
PHONY := libs
ifneq ($(INIT_SUBMODULES),)
libs:
	$(INIT_SUBMODULES)
	@$(MAKE) libs
else
libs: $(LIB_TARGETS)
	
endif

PHONY += clean_libs
clean_libs:
	-$(CLEAN_MBEDTLS)
	-$(CLEAN_FATFS)
	-$(CLEAN_MQTT)
	-$(CLEAN_IIO)
