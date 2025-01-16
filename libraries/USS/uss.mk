# This Makefile is used to manage the inclusion of the various
# libraries that are available in the MaximSDK.  'include'-ing 
# libs.mk offers 'toggle switch' variables that can be used to
# manage the inclusion of the available libraries.

# Each library below may also have its own set of configuration
# variables that can be overridden.

# ***********************************
# Embedded GCOV (Disabled by default)
# ***********************************

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

LIB_EMBEDDED_GCOV ?= 0
ifeq ($(LIB_EMBEDDED_GCOV), 1)
ifeq "$(EMBEDDED_GCOV_MK_DIR)" ""
$(error EMBEDDED_GCOV_MK_DIR must be specified)
endif
include $(EMBEDDED_GCOV_MK_DIR)/embedded_gcov.mk
endif

# ***********************************
# maxq1080_fw (Disabled by default)
# ***********************************
LIB_MAXQ10XX_FW ?= 0
ifeq ($(LIB_MAXQ10XX_FW), 1)
ifeq "$(PROTOCOL)" "MAILBOX"
ifeq "$(MAXQ10XX_FW_MK_DIR)" ""
$(error MAXQ10XX_FW_MK_DIR must be specified)
endif
include $(MAXQ10XX_FW_MK_DIR)/maxq10xx_fw.mk
endif
endif

# ***********************************
# maxq10xx-api (Disabled by default)
# ***********************************
LIB_MAXQ10XX_API ?= 0
ifeq ($(LIB_MAXQ10XX_API), 1)
ifeq "$(MAXQ10XX_API_MK_DIR)" ""
$(error MAXQ10XX_API_MK_DIR must be specified)
endif
include $(MAXQ10XX_API_MK_DIR)/maxq10xx_api.mk
endif

# ***********************************
# adin1110 (Disabled by default)
# ***********************************
LIB_ADIN1110 ?= 0
ifeq ($(LIB_ADIN1110), 1)
ifeq "$(ADIN1110_MK_DIR)" ""
$(error ADIN1110_DIR must be specified)
endif
include $(ADIN1110_MK_DIR)/adin1110.mk
endif

# ************************
# wolfssl (Disabled by default)
# ************************
LIB_WOLFSSL ?= 0
ifeq ($(LIB_WOLFSSL), 1)
ifeq "$(WOLFSSL_MK_DIR)" ""
$(error WOLFSSL_MK_DIR must be specified)
endif
include $(WOLFSSL_MK_DIR)/wolfssl.mk
endif

# ***********************************
# mbedtls (Disabled by default)
# ***********************************
LIB_MBEDTLS ?= 0
ifeq ($(LIB_MBEDTLS), 1)
ifeq "$(MBEDTLS_MK_DIR)" ""
$(error MBEDTLS_MK_DIR must be specified)
endif
include $(MBEDTLS_MK_DIR)/mbedtls.mk
endif




$(info Overriding the default sla overrides from SBT-config.mk)
ifneq ($(TARGET), MAX32520)
ifeq ($(SBT),1)
LINKERFILE=$(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/maxq1080_hal/linker/$(TARGET_LC)_firmware_sla.ld
else
LINKERFILE=$(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/maxq1080_hal/linker/$(TARGET_LC)_firmware.ld
endif
else
LINKERFILE=$(MAXQ10XX_FW_LIBS_DIR)/MAXQ10xxFW/maxq1080_hal/linker/$(TARGET_LC)_firmware_sla.ld
endif