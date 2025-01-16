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

MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/debug.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/mps_reader.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/mps_trace.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_ciphersuites.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_client.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_debug_helpers_generated.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_msg.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_ticket.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_tls.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_tls12_client.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_tls13_keys.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_tls13_client.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ssl_tls13_generic.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/x509.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/x509_crl.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/x509_crt.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/x509_csr.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/x509write_csr.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pkcs7.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/aes.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/asn1parse.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/asn1write.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/base64.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/bignum.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/bignum_core.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/bignum_mod.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/bignum_mod_raw.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ccm.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/cipher.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/cipher_wrap.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/cmac.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/constant_time.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ctr_drbg.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/des.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/dhm.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ecdh.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ecdsa.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ecp.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/ecp_curves.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/entropy.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/entropy_poll.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/error.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/gcm.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/hash_info.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/hkdf.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/hmac_drbg.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/lmots.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/lms.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/md.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/md5.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/memory_buffer_alloc.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/nist_kw.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/oid.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pem.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pk.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pk_wrap.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pkcs12.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pkcs5.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pkparse.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/pkwrite.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/platform.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/platform_util.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto_client.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto_hash.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto_ecp.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto_rsa.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto_mac.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_util.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto_slot_management.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/psa_crypto_driver_wrappers.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/rsa.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/rsa_alt_helpers.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/sha1.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/sha256.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/sha512.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/threading.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/version.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/version_features.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/maxq_wrap.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/library/maxq_entropy.c

MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/programs/ssl/ssl_test_lib.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/tests/src/helpers.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/tests/src/random.c
MBEDTLS_C_FILES += $(MBEDTLS_LIB_DIR)/tests/src/fake_external_rng_for_test.c

# Where to find header files for this project
MBEDTLS_H_FILES +=  $(shell find $(MBEDTLS_LIB_DIR)/include -wholename 'mbedtls/*.h')
