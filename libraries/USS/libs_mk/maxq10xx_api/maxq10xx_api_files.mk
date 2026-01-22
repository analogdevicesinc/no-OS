################################################################################
 # Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 #
 # Permission is hereby granted, free of charge, to any person obtaining a
 # copy of this software and associated documentation files (the "Software"),
 # to deal in the Software without restriction, including without limitation
 # the rights to use, copy, modify, merge, publish, distribute, sublicense,
 # and/or sell copies of the Software, and to permit persons to whom the
 # Software is furnished to do so, subject to the following conditions:
 #
 # The above copyright notice and this permission notice shall be included
 # in all copies or substantial portions of the Software.
 #
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 # OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 # MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 # IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 # OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 # ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 # OTHER DEALINGS IN THE SOFTWARE.
 #
 # Except as contained in this notice, the name of Maxim Integrated
 # Products, Inc. shall not be used except as stated in the Maxim Integrated
 # Products, Inc. Branding Policy.
 #
 # The mere transfer of this software does not imply any licenses
 # of trade secrets, proprietary technology, copyrights, patents,
 # trademarks, maskwork rights, or any other form of intellectual
 # property whatsoever. Maxim Integrated Products, Inc. retains all
 # ownership rights.
 #
 # $Date: 2018-08-31 14:08:14 -0500 (Fri, 31 Aug 2018) $
 # $Revision: 37586 $
 #
 ###############################################################################

# This is the name of the build output file

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

MXQ_FW_UPDATE_CAP:=1
export MXQ_FW_UPDATE_CAP


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
