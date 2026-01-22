/*******************************************************************************
* Copyright © 2023 Maxim Integrated Inc. (now owned by Analog Devices, Inc.),
* 2024 Analog Devices, Inc. All Rights Reserved. This software is proprietary
* and confidential to Analog Devices, Inc. and its licensors.
*
* This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*
*
*******************************************************************************
*/

#ifndef MAXQ_WRAP_H
#define MAXQ_WRAP_H

#ifdef __MAXQ10XX__

#ifdef __MAXQ_USS__
#include "maxq10xx-api/include/MXQ_API.h"
#else
#include "maxq10xx_api/MXQ_API.h"
#endif

#include "mbedtls/pk.h"
#include "mbedtls/ssl.h"

/* Definitions */
#if defined (TEST_SETUP)
#define ROOTCACERTOBJID     (0x1006)
#define CLICERTOBJID        (0x1005)
#define CLIKEYOBJID         (0x1007)
#else
#define ROOTCACERTOBJID     (0x1003)
#define CLICERTOBJID        (0x1002)
#define CLIKEYOBJID         (0x1004)
#endif


#define PSKOBJID            (0x3132)

#ifdef __MAXQ106x_C2__
#define SERVCERTOBJID       (3331)
#else
#define SERVCERTOBJID       (0)
#endif

#define MXQ_IS_KEY_SECRET   MXQ_TRUE

/* Structs */
typedef struct {
    mbedtls_ecp_group grp;
    int key_id;
} mbedtls_mxq_context;

/* Externs */
extern mbedtls_x509_crt *mxq_cert_chain[5];
extern int mxq_cert_chain_idx;

/* Functions */
int mbedtls_asn1_write_longint(unsigned char **p, unsigned char *start, const unsigned char *buf, size_t size);

int maxq_curve_mbedtls_id2mxq_id(mbedtls_ecp_group_id polar_id);

int mxq_get_sign_alg_from_curve(mxq_keyparam_id_t maxq_id);
int mxq_curve_size(mxq_keyparam_id_t maxq_id);

int mxq_x509_root_crt_import(unsigned char *buf, size_t buflen, int pk_offset, mbedtls_pk_context *pk);
int mxq_x509_root_cert_read(mbedtls_x509_crt *rootcert, int objid);

int mxq_x509_cli_crt_import(unsigned char *buf, size_t buflen, int pk_offset, int tbs_len, mbedtls_pk_context *pk, mbedtls_pk_context *signing_pk);
int mxq_x509_cli_cert_read(mbedtls_x509_crt *clicert, int objid);
int mxq_x509_cli_privkey_create(int cert_param);

int mxq_authAdmin(void);

int mxq_pk_verify_ext(mbedtls_x509_crt *crt, mbedtls_pk_context *pk_parent, int parent_cert_id);
int mxq_pk_sign_ec(mbedtls_md_type_t md_alg,
            unsigned char *hash, size_t hash_len,
            unsigned char *sig, size_t *sig_len, mxq_u2 signingkeyID);
int mxq_pk_sign_rsa(mbedtls_md_type_t md_alg,
            unsigned char *hash, size_t hash_len,
            unsigned char *sig, size_t *sig_len, mxq_u2 signingkeyID, int padding);

int maxq_init(mbedtls_ssl_context *ssl);
int maxq_NVM_Init(mbedtls_ssl_context *ssl);

int mxq_verifyDHEparamsSignature(
        mxq_algo_id_t algoid, mxq_u2 CSID,
        mxq_u1 *random, mxq_length randomlen,
        mxq_u1 *dheparams, mxq_length dheparamslen,
        mxq_u1 *signature, mxq_length signaturelen);
int mxq_verifyECDHEparamsSignature(
        mbedtls_ecdh_context *ctx,
        mxq_algo_id_t algoid, mxq_u2 CSID,
        mxq_u1 *random, mxq_length randomlen,
        mxq_u1 *ecdheparams, mxq_length ecdheparamslen,
        mxq_u1 *signature, mxq_length signaturelen);

int mxq_ecdh_calc(mbedtls_ecdh_context *ctx, size_t *olen, unsigned char *buf, mxq_u2 CSID);

#ifdef MAXQ108x
int mxq_dh_calc(mbedtls_dhm_context *ctx, size_t *olen, unsigned char *buf, mxq_u2 CSID);
#endif

int mxq_rsaes_pkcs1_v15_encrypt(unsigned char *version, size_t *olen, unsigned char *buf, mxq_u2 CSID);

int maxq_ssl_derive_keys(mbedtls_ssl_context *ssl);

int maxq_cipher_aead_encrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *ad, size_t ad_len,
                    const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t output_len,
                    size_t *olen, size_t tag_len );

int maxq_cipher_cbc_encrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *input, size_t ilen,
                    const unsigned char *ad, size_t ad_len,
                    size_t mac_len,
                    unsigned char *output, size_t *olen );

int maxq_cipher_aead_decrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *ad, size_t ad_len,
                    const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t output_len,
                    size_t *olen, size_t tag_len );

int maxq_cipher_cbc_decrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *input, size_t ilen,
                    const unsigned char *ad, size_t ad_len,
                    size_t mac_len,
                    unsigned char *output, size_t *olen );

#if defined(MAXQ108x) || defined(MBEDTLS_SSL_PROTO_TLS1_3)
int maxq_tls13_ecc_key_gen( mbedtls_ssl_context *ssl,
                    uint16_t named_group,
                    unsigned char *buf, unsigned char *end,
                    size_t *out_len );

int maxq_tls13_ecc_shared_secret( mbedtls_ssl_context *ssl,
                    unsigned char *out, size_t *out_len );

int maxq_tls13_hkdf_extract( unsigned char *prk,
                    const unsigned char *salt, size_t saltLen,
                    unsigned char *ikm, size_t ikmLen,
                    psa_algorithm_t digest );

int maxq_tls13_hkdf_expand( psa_algorithm_t digest,
                    const unsigned char *inKey, size_t inKeySz,
                    const unsigned char *info, size_t infoSz,
                    unsigned char *out, size_t outSz,
                    int side );

void maxq_tls13_hkdf_set_side(int side);
int maxq_tls13_hkdf_get_side(void);

int maxq_tls13_record_processing( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *ad, size_t ad_len,
                    const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t output_len,
                    size_t *olen, size_t tag_len,
                    int is_encrypt );

int maxq_tls13_hmac_set_key(psa_algorithm_t digest);
int maxq_tls13_hmac_update(const unsigned char *msg, size_t length);
int maxq_tls13_hmac_final(unsigned char *hash, size_t *dst_len);
#endif

#endif /* __MAXQ10XX__ */

#endif /* MAXQ_WRAP_H */
