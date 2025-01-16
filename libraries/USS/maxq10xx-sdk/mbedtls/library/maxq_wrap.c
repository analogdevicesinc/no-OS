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
*******************************************************************************
*/

#include "common.h"

#ifdef __MAXQ10XX__

#include "mbedtls/platform.h"
#include "mbedtls/maxq_wrap.h"
#include "mbedtls/ssl.h"
#include "mbedtls/debug.h"
#include "mbedtls/error.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/asn1write.h"

#include "ssl_misc.h"

#ifdef __MAXQ_USS__
#include "maxq10xx-api/include/MXQ_API.h"
#include "maxq10xx-api/include/MXQ_Types.h"
#else
#include "maxq10xx_api/MXQ_API.h"
#include "maxq10xx_api/MXQ_Types.h"
#endif



extern int is_module_init; /* int is_module_init = 0; */

int ssl_calc_verify_tls_sha256(const mbedtls_ssl_context *, unsigned char *, size_t *);
int ssl_calc_finished_tls_sha256(mbedtls_ssl_context *, unsigned char *, int);

int ssl_calc_verify_tls_sha384(const mbedtls_ssl_context *, unsigned char *, size_t *);
int ssl_calc_finished_tls_sha384(mbedtls_ssl_context *, unsigned char *, int);

#ifndef __MAXQ10XX_PERSO__
static void mbedtls_zeroize(void *v, size_t n) {
    volatile unsigned char *p = v; while(n--) *p++ = 0;
}
#endif /* __MAXQ10XX_PERSO__ */

#if defined(MAXQ108x) && defined(MBEDTLS_SSL_PROTO_TLS1_3) && !defined(__MAXQ10XX_PERSO__)
static int get_tls13_server_cert_id(void);
static void set_tls13_server_key_algo_len(int algo, int len);
#endif /* MAXQ108x && MBEDTLS_SSL_PROTO_TLS1_3 && !(__MAXQ10XX_PERSO__) */

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED) && !defined(__MAXQ10XX_PERSO__)
static int char2int(char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  MXQ_PRINTF("Error: Invalid input string"); exit(-1);
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
static void parse_hex(const char* src, unsigned char* target, int* len)
{  *len=0;
  while(*src && src[1])
  {
    *(target++) = char2int(*src)*16 + char2int(src[1]);
    src += 2;
    *len=*len+1;
  }
}

static mxq_u2 getKeyId(mbedtls_ssl_context* ssl)
{
    unsigned char zero_terminated[5];
    unsigned char id[2];
    int           idlen = sizeof(id);

    if(ssl->conf->psk_identity_len!=4) {
        return 0;
    }

    memcpy(zero_terminated,ssl->conf->psk_identity,4);
    zero_terminated[4]=0;

    parse_hex((const char*)zero_terminated, id, &idlen);
    return (id[0]<<8) + id[1];
}
#endif


int maxq_curve_mbedtls_id2mxq_id(mbedtls_ecp_group_id polar_id){

    switch (polar_id){
    case MBEDTLS_ECP_DP_SECP192R1:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_SECP224R1:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_SECP256R1:
        return MXQ_KEYPARAM_EC_P256R1;
    case MBEDTLS_ECP_DP_SECP384R1:
        return MXQ_KEYPARAM_EC_P384R1;
    case MBEDTLS_ECP_DP_SECP521R1:
        return MXQ_KEYPARAM_EC_P521R1;
    case MBEDTLS_ECP_DP_BP256R1:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_BP384R1:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_BP512R1:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_CURVE25519:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_SECP192K1:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_SECP224K1:
        return MXQ_UNKNOWN_CURVE;
    case MBEDTLS_ECP_DP_SECP256K1:
        return MXQ_UNKNOWN_CURVE;
    default:
        return MXQ_UNKNOWN_CURVE;
    }
    return 0;
}


int mxq_get_sign_alg_from_curve(mxq_keyparam_id_t maxq_id){

    switch (maxq_id){
    case MXQ_KEYPARAM_EC_P256R1:
        return ALGO_ECDSA_SHA_256;
    case MXQ_KEYPARAM_EC_P384R1:
        return ALGO_ECDSA_SHA_384;
    case MXQ_KEYPARAM_EC_P521R1:
        return ALGO_ECDSA_SHA_512;
    case MXQ_KEYPARAM_EC_BP256R1:
        return ALGO_ECDSA_SHA_256;
    case MXQ_KEYPARAM_EC_BP384R1:
        return ALGO_ECDSA_SHA_384;
    case MXQ_KEYPARAM_EC_BP512R1:
        return ALGO_ECDSA_SHA_512;
    default:
        break;
    }
    return 0;
}


int mxq_curve_size(mxq_keyparam_id_t maxq_id){

    switch (maxq_id){
    case MXQ_KEYPARAM_EC_P256R1:
        return 32;
    case MXQ_KEYPARAM_EC_P384R1:
        return 48;
    case MXQ_KEYPARAM_EC_P521R1:
        return 66;
    case MXQ_KEYPARAM_EC_BP256R1:
        return 32;
    case MXQ_KEYPARAM_EC_BP384R1:
        return 48;
    case MXQ_KEYPARAM_EC_BP512R1:
        return 64;
    default:
        break;
    }
    return 0;
}


int mxq_x509_cli_cert_read(mbedtls_x509_crt * clicert, int objid)
{
    unsigned char rawclicert[2048];
    size_t len = sizeof(rawclicert);

    mxq_err_t    err = 0;

    err = MXQ_ReadObject(objid, 0, rawclicert,(mxq_length *) &len);
    if(err != MXQ_OK){
        MXQ_PRINTF("Error %s:%d\n", __FILE__, __LINE__);
        return err;
    }

    len = (rawclicert[26] << 8) + rawclicert[27] + 4;
    return mbedtls_x509_crt_parse(clicert, (const unsigned char *) rawclicert + 24, len);
}


int mxq_x509_root_cert_read(mbedtls_x509_crt * rootcert, int objid)
{
    unsigned char rawrootcert[2048];
    size_t len = sizeof(rawrootcert);

    mxq_err_t err = 0;

    err = MXQ_ReadObject(objid, 0, rawrootcert, (mxq_length *) &len);
    if(err != MXQ_OK){
        MXQ_PRINTF("Error %s:%d\n", __FILE__, __LINE__);
        return err;
    }

#if defined(MAXQ1065) || defined(MAXQ108x)
    len = (rawrootcert[17] << 8) + rawrootcert[16];
    return mbedtls_x509_crt_parse(rootcert, (const unsigned char *) rawrootcert + 24, len);
#else
    len = (rawrootcert[13] << 8) + rawrootcert[12];
    return mbedtls_x509_crt_parse(rootcert, (const unsigned char *) rawrootcert + 24, len);
#endif
}


/* Length of the "epoch" field in the record header */
static inline size_t ssl_ep_len(const mbedtls_ssl_context *ssl)
{
#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if(ssl->conf->transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM)
        return 2;
#else
    ((void) ssl);
#endif
    return 0;
}

mbedtls_x509_crt* mxq_cert_chain[5];
int mxq_cert_chain_idx=0;

#ifdef MAXQ108x
static int calculate_modulus_offset(unsigned char * cert_data, int offset)
{
    int l_offset = offset;
    for(int i =0;i<2;i++)
    {
        if ( ( cert_data[l_offset] ) & 0x80 )
        {
            if ( ( cert_data[l_offset] & 0x7f ) == 1 )
            {
                l_offset += 3;
            }
            if ( ( cert_data[l_offset] & 0x7f ) == 2 )
            {
                l_offset += 4;
            }
        }
        else
        {
            l_offset += 2;
        }
    }
    return l_offset;
}
#endif

int mxq_pk_verify_ext(mbedtls_x509_crt *crt, mbedtls_pk_context *pk_parent, int parent_cert_id)
{
    mxq_err_t err = 0;
    mxq_u1 certdata[1024];
    mxq_length certdatalen = sizeof(certdata);
    mxq_algo_id_t sign_algo;
    mxq_keytype_id_t key_type = MXQ_KEYTYPE_ECC;
    mxq_algo_id_t key_algo;
    mxq_keyparam_id_t mxq_keytype;
    int keycomplen;
    int totalkeylen;
    int pk_offset = crt->pk.pk_offset;

#ifdef MAXQ108x
    if( (crt->sig_pk == MBEDTLS_PK_RSA) || (crt->sig_pk == MBEDTLS_PK_RSASSA_PSS) )
    {
        switch (crt->sig_md) {
            case MBEDTLS_MD_SHA256:
                sign_algo = ALGO_RSASSAPSSPKCS1_V1_5_SHA256;
                break;

            case MBEDTLS_MD_SHA384:
                sign_algo = ALGO_RSASSAPSSPKCS1_V1_5_SHA384;
                break;

            case MBEDTLS_MD_SHA512:
                sign_algo = ALGO_RSASSAPSSPKCS1_V1_5_SHA512;
                break;

            default:
                MXQ_PRINTF("Error: Unsupported Signature Algorithm\n");
                exit(-1);
        }
    }
    else
#endif /* MAXQ108x */
    {
        mxq_keyparam_id_t mxq_keytype_parent = maxq_curve_mbedtls_id2mxq_id(((mbedtls_ecp_keypair*)((*pk_parent).pk_ctx))->grp.id);
        sign_algo = mxq_get_sign_alg_from_curve(mxq_keytype_parent);
    }

#ifdef MAXQ108x
    if( mbedtls_ecp_check_pubkey(&((mbedtls_ecp_keypair*)(crt->pk.pk_ctx))->grp, &((mbedtls_ecp_keypair*)(crt->pk.pk_ctx))->Q) )
    {
        keycomplen = mbedtls_pk_rsa( crt->pk )->len;
        mbedtls_mpi_uint modulus = (mbedtls_pk_rsa(crt->pk)->N.p[(keycomplen/sizeof(mbedtls_mpi_uint))-1]);
        unsigned char *modulus_p = (unsigned char *)&modulus;
        pk_offset = calculate_modulus_offset( crt->raw.p, crt->pk.pk_offset );
        if( crt->raw.p[pk_offset] != modulus_p[sizeof(mbedtls_mpi_uint)-1] )
        {
            MXQ_PRINTF("Error: Modulus offset calculation failed\n");
            exit(-1);
        }

        totalkeylen = keycomplen;
        key_type = MXQ_KEYTYPE_RSA;
        key_algo = ALGO_RSASIGN_ANY;
        if( mbedtls_pk_rsa( crt->pk )->E.p[0] == 65537 )
            mxq_keytype = MXQ_KEYPARAM_RSA_PUB65537;
        else if( mbedtls_pk_rsa( crt->pk )->E.p[0] == 3 )
            mxq_keytype = MXQ_KEYPARAM_RSA_PUB3;
        else
        {
            MXQ_PRINTF("Error: Unsupported Public Exponent\n");
            exit (-1);
        }
    }
    else
#endif /* MAXQ108x */
    {
        key_algo = ALGO_ECDSA_SHA_any;
        mxq_keytype = maxq_curve_mbedtls_id2mxq_id(((mbedtls_ecp_keypair*)((crt->pk).pk_ctx))->grp.id);
        keycomplen = mxq_curve_size(mxq_keytype);
        totalkeylen = keycomplen * 2;
    }

#if defined(MAXQ108x) && defined(MBEDTLS_SSL_PROTO_TLS1_3) && !defined(__MAXQ10XX_PERSO__)
    set_tls13_server_key_algo_len((int)key_algo, keycomplen);
#endif /* MAXQ108x && MBEDTLS_SSL_PROTO_TLS1_3 && !(__MAXQ10XX_PERSO__) */

    /* send the certificate to maxq106x */
    err = MXQ_Build_EC_Cert(certdata, &certdatalen, key_type, mxq_keytype,
            keycomplen, totalkeylen, pk_offset,
            0x04,
            crt->tbs.len,
            crt->raw.len, sign_algo, parent_cert_id,
            MXQ_KEYUSE_VERIFY_KEY_CERT, key_algo,
#if SERVCERTOBJID==0
            MXQ_KEYUSE_DATASIGNATURE, key_algo,
#else
            MXQ_KEYUSE_ENCRYPTION, 0,
#endif
            crt->raw.p);
    if(err != MXQ_OK) return err;

#if defined(MAXQ108x) && defined(MBEDTLS_SSL_PROTO_TLS1_3) && !defined(__MAXQ10XX_PERSO__)
    int server_cert_id = get_tls13_server_cert_id();
    if (server_cert_id < 0) {
        MXQ_PRINTF("Error: alloc_temp_key_id() failed\n");
        exit (-1);
    }

    err = MXQ_ImportChildCert(server_cert_id, certdata, certdatalen);
#else
    err = MXQ_ImportChildCert(SERVCERTOBJID, certdata, certdatalen);
#endif /* MAXQ108x && MBEDTLS_SSL_PROTO_TLS1_3 && !(__MAXQ10XX_PERSO__) */
    if(err != MXQ_OK) return err;

    /* Trick cipher init to load cert pub key into cached_key */
    if(SERVCERTOBJID) MXQ_Cipher_Init(1, ALGO_CIPHER_NULL, SERVCERTOBJID, NULL, 0);

    return err;
}


int mxq_pk_sign_ec(mbedtls_md_type_t md_alg,
             unsigned char *hash, size_t hash_len,
             unsigned char *sig, size_t *sig_len, mxq_u2 signingkeyID)

{
    int ret;
    unsigned char buf[256];
    unsigned char insig[256];
    unsigned char r[256];
    unsigned char s[256];
    mxq_err_t err;
    int r_index = 0, r_siglen;
    int s_index = 0, s_siglen;
    size_t len = 0;
    unsigned char *p;


    if(md_alg == MBEDTLS_MD_SHA384){
        hash_len = 48;
        *sig_len = 48 * 2;
    }else if(md_alg == MBEDTLS_MD_SHA256){
        hash_len = 32;
        *sig_len = 64;
    }else if(md_alg == MBEDTLS_MD_SHA512){
        hash_len = 64;
        *sig_len = 66 * 2;
    }else{
        return -1;
    }

    err = MXQ_Sign(ALGO_ECDSA_PLAIN, signingkeyID, hash, hash_len, insig,(mxq_length *) sig_len);
    if(err != MXQ_OK){
        return -1;
    }

    /* Sign extend */

    r_siglen =* sig_len / 2;
    s_siglen =* sig_len / 2;

    if((insig[0] == 0)){
        for(r_index = 0; r_index < r_siglen; r_index++ )
        {
            if(insig[r_index] != 0)
                break;
        }
        r_siglen -= r_index;
    }

    if(insig[r_index] > 0x7f){
        r[0] = 0;
        memcpy(r + 1, insig + r_index, r_siglen);
        r_siglen++;
    }else {
        memcpy(r, insig + r_index, r_siglen);
    }

    if(insig[*sig_len / 2] == 0){
        for(s_index = 0; s_index < s_siglen; s_index++ )
        {
            if(insig[(*sig_len / 2) + s_index] != 0)
                break;
        }
        s_siglen -= s_index;

    }
    s_index += (*sig_len / 2);
    if(insig[s_index] > 0x7f){
        s[0]=0;
        memcpy(s + 1, insig + s_index, s_siglen);
        s_siglen++;
    }else {
        memcpy(s, insig + s_index, s_siglen);
    }

    p = buf + sizeof(buf);

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_longint(&p, buf, s, s_siglen));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_longint(&p, buf, r, r_siglen));

    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_len(&p, buf, len));
    MBEDTLS_ASN1_CHK_ADD(len, mbedtls_asn1_write_tag(&p, buf,
                                       MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));

    memcpy(sig, p, len);
    *sig_len = len;

    return err;
}

#ifdef MAXQ108x
int mxq_pk_sign_rsa(mbedtls_md_type_t md_alg,
             unsigned char *hash, size_t hash_len,
             unsigned char *sig, size_t *sig_len, mxq_u2 signingkeyID, int padding)

{
    mxq_err_t err;
    mxq_algo_id_t algo_id;


    if(padding == MBEDTLS_RSA_PKCS_V15){
        algo_id = ALGO_RSASSAPSSPKCS1_V1_5_PLAIN;
    }else if (padding == MBEDTLS_RSA_PKCS_V21){
        algo_id = ALGO_RSASSAPSSPKCS1_V2_1_PLAIN;
    }else{
        return -1;
    }


    if(md_alg == MBEDTLS_MD_SHA384){
        hash_len = 48;
    }else if(md_alg == MBEDTLS_MD_SHA256){
        hash_len = 32;
    }else if(md_alg == MBEDTLS_MD_SHA512){
        hash_len = 64;
    }else{
        return -1;
    }

    *sig_len = 512;
    err = MXQ_Sign(algo_id, signingkeyID, hash, hash_len, sig,(mxq_length *) sig_len);
    if(err != MXQ_OK){
        return -1;
    }

    return err;
}
#endif

#ifdef __MAXQ10XX_CLI__
#ifdef MAXQ108x
int mxq_rsaes_pkcs1_v15_encrypt(unsigned char *version, size_t *olen, unsigned char *buf, mxq_u2 CSID)
{
    *olen = 2;
    return (int)( MXQ_Encrypt_PMS_with_RSA(version, buf, (mxq_length *)olen, SERVCERTOBJID, CSID) );
}

int mxq_dh_calc(mbedtls_dhm_context *ctx, size_t *olen, unsigned char *buf, mxq_u2 CSID)
{
    int ret = 0 ;
    mxq_u1*   pservkey;
    mxq_u1       dhmparam[3*MAX_DH_KEY_SIZE];
    mxq_u1    client_public_key[MAX_DH_KEY_SIZE];
    mxq_length   key_len = *olen;
    mxq_u2 total_len = 0;
    mxq_u2 p_len, g_len, gy_len;

    if( (CSID == TLS_DHE_PSK_WITH_AES_128_GCM_SHA256) || (CSID == TLS_DHE_PSK_WITH_AES_256_GCM_SHA384))
    {
        if(ctx == NULL)
            return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

        p_len = mbedtls_mpi_size(&ctx->P);
        g_len = mbedtls_mpi_size(&ctx->G);
        gy_len = mbedtls_mpi_size(&ctx->GY);
        dhmparam[total_len] = p_len >> 8;
        dhmparam[total_len+1] = p_len&0xff;
        total_len += 2;
        mbedtls_mpi_write_binary( &ctx->P, (dhmparam + total_len), p_len );
        total_len += p_len;
        dhmparam[total_len] = g_len >> 8;
        dhmparam[total_len+1] = g_len&0xff;
        total_len += 2;
        mbedtls_mpi_write_binary( &ctx->G, (dhmparam + total_len), g_len );
        total_len += g_len;
        dhmparam[total_len] = p_len >> 8;
        dhmparam[total_len+1] = p_len&0xff;
        total_len += 2;
        mbedtls_mpi_write_binary( &ctx->GY, (dhmparam + total_len), gy_len );
        total_len += gy_len;

        key_len = total_len;
        pservkey = dhmparam;
    }
    else
    {
        pservkey = NULL;
    }

    ret = MXQ_Ecdh_Compute_Shared(0xAA, pservkey, client_public_key, key_len, CSID);
    if(ret != 0)
    {
        return ret;
    }

    memcpy(buf, client_public_key, *olen);

    return ret;
}


int mxq_verifyDHEparamsSignature(mxq_algo_id_t algoid,
        mxq_u2 CSID,
        mxq_u1* random, mxq_length randomlen,
        mxq_u1* dheparams, mxq_length dheparamslen,
        mxq_u1* signature, mxq_length signaturelen)
{

    mxq_keyparam_id_t  key_id = 0xaa; // AA is used for DH Key Params

    /*
     * MACQ1061-c2: Trick cipher init to load cert pub key into cached_key
     * It has no effect on C3
     */
    if(SERVCERTOBJID) MXQ_Cipher_Init(1, ALGO_CIPHER_NULL, SERVCERTOBJID, NULL,0);

    return MXQ_SetECDHEKey(
            algoid,    /* this is the algo id used for signing the dheparams */
            key_id,  /* this is the MXQ curve ID of the ecdhe public key */
            CSID,      /* not used */
            random,  randomlen,
            dheparams,  dheparamslen,
            signature,  signaturelen);
}

#endif

int mxq_verifyECDHEparamsSignature(
        mbedtls_ecdh_context *ctx,
        mxq_algo_id_t algoid,
        mxq_u2 CSID,
        mxq_u1* random, mxq_length randomlen,
        mxq_u1* ecdheparams, mxq_length ecdheparamslen,
        mxq_u1* signature, mxq_length signaturelen)
{

    mxq_keyparam_id_t  curve_id;

    curve_id = maxq_curve_mbedtls_id2mxq_id(ctx->grp_id); // ctx->grp.id
    if(curve_id == MXQ_UNKNOWN_CURVE) return -1;

    /*
     * MACQ1061-c2: Trick cipher init to load cert pub key into cached_key
     * It has no effect on C3
     */
    if(SERVCERTOBJID) MXQ_Cipher_Init(1, ALGO_CIPHER_NULL, SERVCERTOBJID, NULL,0);

    return MXQ_SetECDHEKey(
            algoid,    /* this is the algo id used for signing the ecdheparams */
            curve_id,  /* this is the MXQ curve ID of the ecdhe public key */
            CSID,      /* not used */
            random,  randomlen,
            ecdheparams,  ecdheparamslen,
            signature,  signaturelen);
}


int mxq_ecdh_calc(mbedtls_ecdh_context *ctx, size_t *olen, unsigned char *buf, mxq_u2 CSID)
{
    int ret = 0 ;
    size_t plen;
    mxq_u1    server_public_key[MAX_ECDH_KEY_SIZE];
    mxq_u1*   pservkey;
    mxq_u1    client_public_key[MAX_ECDH_KEY_SIZE];
    mxq_length   key_len;
    mxq_keyparam_id_t  curve_id;


    if(ctx == NULL)
        return MBEDTLS_ERR_ECP_BAD_INPUT_DATA;

    /* add curve Id identification */
    curve_id = maxq_curve_mbedtls_id2mxq_id(ctx->grp_id); // ctx->grp.id
    if(curve_id == MXQ_UNKNOWN_CURVE) return -1;

    /* get key_len */
    plen = mbedtls_mpi_size(&ctx->ctx.mbed_ecdh.grp.P); // &ctx->grp.P
    key_len = 2 * plen + 1;

    /* prepare server public key */
    pservkey = server_public_key;
    if( (CSID == TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256) || (CSID == TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384)
        || (CSID == TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA) || (CSID == TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA))
    {
        /* Put the public key as is... */
        server_public_key[0] = MBEDTLS_ASN1_OCTET_STRING;
        mbedtls_mpi_write_binary(&ctx->ctx.mbed_ecdh.Qp.X, server_public_key + 1, plen);
        mbedtls_mpi_write_binary(&ctx->ctx.mbed_ecdh.Qp.Y, server_public_key + 1 + plen, plen);
    }
    else
    {
        pservkey = NULL;
    }

    ret = MXQ_Ecdh_Compute_Shared(curve_id, pservkey, client_public_key, key_len, CSID);
    if(ret != 0)
    {
        return ret;
    }

    *olen = key_len;
    buf[0] = (mxq_u1) *olen;
    memcpy(buf + 1, client_public_key, key_len);
    (*olen)++;

    return ret;
}
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
static int maxq_tls_prf(const unsigned char *secret, size_t slen,
         const char *label,
         const unsigned char *random, size_t rlen,
         unsigned char *dstbuf, size_t dlen)
{

    mxq_length llen;
    mxq_u2 ret = 0;

    (void) (secret); /* secret known by maxq */
    (void) (slen);

    llen = strlen(label);

    /* call maxq prf function */
    ret = MXQ_tls_prf_sha_256(0, (mxq_u1*) label, llen, (mxq_u1*) random, rlen, dstbuf, dlen);

     return ret;
 }
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
/**
 * @brief this command call ssl_derive_keys function
 *
 * @param ssl pointer on ssl structure
 * @return
 */
int maxq_ssl_derive_keys(mbedtls_ssl_context *ssl)
{
    int ret = 0;
    unsigned char keyblk[256];
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    unsigned char *key1;
    unsigned char *key2;
    unsigned char *mac_enc;
    unsigned char *mac_dec;
    size_t iv_copy_len;
    size_t keylen;
#endif  /* MBEDTLS_SSL_PROTO_TLS1_2 */
    const mbedtls_ssl_ciphersuite_t *ciphersuite_info;
    const mbedtls_cipher_info_t *cipher_info;
    const mbedtls_md_info_t *md_info;

    mxq_secret_context_api_t derive_key_info;

    mbedtls_ssl_session *session = ssl->session_negotiate;
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    mbedtls_ssl_transform *transform = ssl->transform_negotiate;
#endif
    mbedtls_ssl_handshake_params *handshake = ssl->handshake;

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> derive keys"));
    ciphersuite_info = mbedtls_ssl_ciphersuite_from_id( session->ciphersuite );
    if( ciphersuite_info == NULL )
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "ciphersuite info for %d not found",
                                    session->ciphersuite ) );
        return( MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
    }

    MBEDTLS_SSL_DEBUG_MSG(3, ("=> mbedtls_cipher_info_from_type"));
    cipher_info = mbedtls_cipher_info_from_type(ciphersuite_info->cipher);
    if(cipher_info == NULL)
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "cipher info for %d not found",
                            ciphersuite_info->cipher ) );
        return( MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
    }

    MBEDTLS_SSL_DEBUG_MSG(3, ("=> mbedtls_md_info_from_type"));
    md_info = mbedtls_md_info_from_type(ciphersuite_info->mac);
    if(md_info == NULL)
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "mbedtls_md info for %d not found",
                            ciphersuite_info->mac ) );
        return( MBEDTLS_ERR_SSL_BAD_INPUT_DATA );
    }
    MBEDTLS_SSL_DEBUG_MSG(3, ("=> Set appropriate PRF function and other SSL / TLS / TLS1.2 functions"));

    /*
     * Set appropriate PRF function and other SSL / TLS / TLS1.2 functions
     */

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
#if defined(MBEDTLS_HAS_ALG_SHA_384_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
    if(ciphersuite_info->mac == MBEDTLS_MD_SHA384)
    {
        handshake->tls_prf = maxq_tls_prf;
        handshake->calc_verify = ssl_calc_verify_tls_sha384;
        handshake->calc_finished = ssl_calc_finished_tls_sha384;
    }
    else
#endif /* MBEDTLS_HAS_ALG_SHA_384_VIA_MD_OR_PSA_BASED_ON_USE_PSA */
    {
        handshake->tls_prf = maxq_tls_prf;
        handshake->calc_verify = ssl_calc_verify_tls_sha256;
        handshake->calc_finished = ssl_calc_finished_tls_sha256;
    }
#else
    {
        MBEDTLS_SSL_DEBUG_MSG( 1, ( "should never happen" ) );
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

    /*
     * SSLv3:
     *   master =
     *     MD5(premaster + SHA1('A'   + premaster + randbytes)) +
     *     MD5(premaster + SHA1('BB'  + premaster + randbytes)) +
     *     MD5(premaster + SHA1('CCC' + premaster + randbytes))
     *
     * TLSv1+:
     *   master = PRF(premaster, "master secret", randbytes)[0..47]
     */
    if(handshake->resume == 0)
    {
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
        MBEDTLS_SSL_DEBUG_BUF(3, "premaster secret", handshake->premaster, handshake->pmslen);
#endif
        /* start maxq106x derivation */
        derive_key_info.pass = PSK_FIRST;
        derive_key_info.CSID = ciphersuite_info->id;
        derive_key_info.Random = handshake->randbytes;
        derive_key_info.Random_size = 64;
#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
        derive_key_info.PSK_info.psk_id = getKeyId(ssl);
#endif
        /* TODO add mechanism to change that value MXQ_IS_KEY_SECRET */
        derive_key_info.is_session_key_secret = 0;

        /* output TODO useful? */
        memcpy(derive_key_info.PSK_info.psk_key_bloc, keyblk, 256);


        MBEDTLS_SSL_DEBUG_MSG(1, ("MXQ_Perform_Key_Exchange\n"));
        ret = MXQ_Perform_Key_Exchange(&derive_key_info);
        if(ret != 0)
        {
            MBEDTLS_SSL_DEBUG_MSG(1, ("Error %s:%d\n", __FILE__, __LINE__));
            MBEDTLS_SSL_DEBUG_RET(1, "MXQ_Perform_Key_Exchange", ret);
            return ret;
        }

        memcpy(keyblk, derive_key_info.PSK_info.psk_key_bloc, 256);
    }
    else
        MBEDTLS_SSL_DEBUG_MSG(3, ("no premaster (session resumed)"));

    MBEDTLS_SSL_DEBUG_MSG(3, ("ciphersuite = %s",
                   mbedtls_ssl_get_ciphersuite_name(session->ciphersuite)));
    MBEDTLS_SSL_DEBUG_BUF(3, "master secret", session->master, 48);
    MBEDTLS_SSL_DEBUG_BUF(4, "random bytes", handshake->randbytes, 64);
    MBEDTLS_SSL_DEBUG_BUF(4, "key block", keyblk, 256);

    mbedtls_zeroize( handshake->randbytes,
                              sizeof( handshake->randbytes ) );

    /*
     * Determine the appropriate key, IV and MAC length.
     */
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    keylen = cipher_info->key_bitlen / 8;

    if(cipher_info->mode == MBEDTLS_MODE_GCM || cipher_info->mode == MBEDTLS_MODE_CCM)
    {
        transform->maclen = 0;
        transform->ivlen = 12;
        transform->fixed_ivlen = 4;

        transform->taglen =
            ciphersuite_info->flags & MBEDTLS_CIPHERSUITE_SHORT_TAG ? 8 : 16;

        /* Minimum length is expicit IV + tag */
        transform->minlen = transform->ivlen - transform->fixed_ivlen
                            + (ciphersuite_info->flags &
                                MBEDTLS_CIPHERSUITE_SHORT_TAG ? 8 : 16);
    }
    else
    {
        /* Initialize HMAC contexts */
        if((ret = mbedtls_md_setup(&transform->md_ctx_enc, md_info, 1)) != 0 ||
            (ret = mbedtls_md_setup(&transform->md_ctx_dec, md_info, 1)) != 0)
        {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_md_setup", ret);
            return( ret );
        }

        /* Get MAC length */
        transform->maclen = mbedtls_md_get_size(md_info);

#if defined(MBEDTLS_SSL_TRUNCATED_HMAC)
        /*
         * If HMAC is to be truncated, we shall keep the leftmost bytes,
         * (rfc 6066 page 13 or rfc 2104 section 4),
         * so we only need to adjust the length here.
         */
        if( session->trunc_hmac == MBEDTLS_SSL_TRUNC_HMAC_ENABLED )
        {
            transform->maclen = MBEDTLS_SSL_TRUNCATED_HMAC_LEN;
        }
#endif /* MBEDTLS_SSL_TRUNCATED_HMAC */

        /* IV length */
        transform->ivlen = cipher_info->iv_size;

        /* Minimum length */
        if(cipher_info->mode == MBEDTLS_MODE_STREAM)
            transform->minlen = transform->maclen;
        else
        {
            /*
             * GenericBlockCipher:
             * 1. if EtM is in use: one block plus MAC
             *    otherwise: * first multiple of blocklen greater than maclen
             * 2. IV except for SSL3 and TLS 1.0
             */
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
            if( session->encrypt_then_mac == MBEDTLS_SSL_ETM_ENABLED )
            {
                transform->minlen = transform->maclen
                                  + cipher_info->block_size;
            }
            else
#endif
            {
                transform->minlen = transform->maclen
                                  + cipher_info->block_size
                                  - transform->maclen % cipher_info->block_size;
            }

            if(ssl->tls_version == MBEDTLS_SSL_VERSION_TLS1_2)
            {
                transform->minlen += transform->ivlen;
            }
            else
            {
                MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
                return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
            }
        }
    }

    MBEDTLS_SSL_DEBUG_MSG(3, ("keylen: %u, minlen: %u, ivlen: %u, maclen: %u",
                   (unsigned)keylen,
                   (unsigned)transform->minlen,
                   (unsigned)transform->ivlen,
                   (unsigned)transform->maclen));

    /*
     * Finally setup the cipher contexts, IVs and MAC secrets.
     */
#if defined(MBEDTLS_SSL_CLI_C)
    if(ssl->conf->endpoint == MBEDTLS_SSL_IS_CLIENT)
    {
        key1 = keyblk + transform->maclen * 2;
        key2 = keyblk + transform->maclen * 2 + keylen;

        mac_enc = keyblk;
        mac_dec = keyblk + transform->maclen;

        /*
         * This is not used in TLS v1.1.
         */
        iv_copy_len = (transform->fixed_ivlen) ?
                            transform->fixed_ivlen : transform->ivlen;
        memcpy(transform->iv_enc, key2 + keylen,  iv_copy_len);
        memcpy(transform->iv_dec, key2 + keylen + iv_copy_len,
                iv_copy_len);
    }
    else
#endif /* MBEDTLS_SSL_CLI_C */
#if defined(MBEDTLS_SSL_SRV_C)
    if(ssl->conf->endpoint == MBEDTLS_SSL_IS_SERVER)
    {
        key1 = keyblk + transform->maclen * 2 + keylen;
        key2 = keyblk + transform->maclen * 2;

        mac_enc = keyblk + transform->maclen;
        mac_dec = keyblk;

        /*
         * This is not used in TLS v1.1.
         */
        iv_copy_len = (transform->fixed_ivlen) ?
                            transform->fixed_ivlen : transform->ivlen;
        memcpy(transform->iv_dec, key1 + keylen,  iv_copy_len);
        memcpy(transform->iv_enc, key1 + keylen + iv_copy_len,
                iv_copy_len);
    }
    else
#endif /* MBEDTLS_SSL_SRV_C */
    {
        MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    if(ssl->tls_version >= MBEDTLS_SSL_VERSION_TLS1_2)
    {
        /* For HMAC-based ciphersuites, initialize the HMAC transforms.
           For AEAD-based ciphersuites, there is nothing to do here. */
        if( transform->maclen != 0 )
        {
            mbedtls_md_hmac_starts(&transform->md_ctx_enc, mac_enc, transform->maclen);
            mbedtls_md_hmac_starts(&transform->md_ctx_dec, mac_dec, transform->maclen);
        }
    }
    else
    {
        MBEDTLS_SSL_DEBUG_MSG(1, ("should never happen"));
        return( MBEDTLS_ERR_SSL_INTERNAL_ERROR );
    }

    MBEDTLS_SSL_DEBUG_BUF(4, "key1", key1, keylen);
    MBEDTLS_SSL_DEBUG_BUF(4, "key2", key2, keylen);
    MBEDTLS_SSL_DEBUG_BUF(4, "transform->iv_enc", transform->iv_enc, iv_copy_len);
    MBEDTLS_SSL_DEBUG_BUF(4, "transform->iv_dec", transform->iv_dec, iv_copy_len);
    MBEDTLS_SSL_DEBUG_BUF(4, "mac_enc", mac_enc, transform->maclen);
    MBEDTLS_SSL_DEBUG_BUF(4, "mac_dec", mac_dec, transform->maclen);

#if defined(MBEDTLS_SSL_HW_RECORD_ACCEL)
    if(mbedtls_ssl_hw_record_init != NULL)
    {
        int ret = 0;

        MBEDTLS_SSL_DEBUG_MSG(2, ("going for mbedtls_ssl_hw_record_init()"));

        if((ret = mbedtls_ssl_hw_record_init(ssl, key1, key2, keylen,
                                        transform->iv_enc, transform->iv_dec,
                                        iv_copy_len,
                                        mac_enc, mac_dec,
                                        transform->maclen)) != 0)
        {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_ssl_hw_record_init", ret);
            return( MBEDTLS_ERR_SSL_HW_ACCEL_FAILED );
        }
    }
#endif /* MBEDTLS_SSL_HW_RECORD_ACCEL */


#if defined(MBEDTLS_SSL_EXPORT_KEYS)
    if( ssl->conf->f_export_keys != NULL )
    {
        ssl->conf->f_export_keys( ssl->conf->p_export_keys,
                                  session->master, keyblk,
                                  transform->maclen, keylen,
                                  iv_copy_len );
    }
#endif  /* MBEDTLS_SSL_EXPORT_KEYS */

    if((ret = mbedtls_cipher_setup(&transform->cipher_ctx_enc,
                                 cipher_info)) != 0)
    {
        MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_setup", ret);
        return( ret );
    }

    if((ret = mbedtls_cipher_setup(&transform->cipher_ctx_dec,
                                 cipher_info)) != 0)
    {
        MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_setup", ret);
        return( ret );
    }

    if((ret = mbedtls_cipher_setkey(&transform->cipher_ctx_enc, key1,
                               cipher_info->key_bitlen,
                               MBEDTLS_ENCRYPT)) != 0)
    {
        MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_setkey", ret);
        return( ret );
    }

    if((ret = mbedtls_cipher_setkey(&transform->cipher_ctx_dec, key2,
                               cipher_info->key_bitlen,
                               MBEDTLS_DECRYPT)) != 0)
    {
        MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_setkey", ret);
        return( ret );
    }

#if defined(MBEDTLS_CIPHER_MODE_CBC)
    if(cipher_info->mode == MBEDTLS_MODE_CBC)
    {
        if((ret = mbedtls_cipher_set_padding_mode(&transform->cipher_ctx_enc,
                                             MBEDTLS_PADDING_NONE)) != 0)
        {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_set_padding_mode", ret);
            return( ret );
        }

        if((ret = mbedtls_cipher_set_padding_mode(&transform->cipher_ctx_dec,
                                             MBEDTLS_PADDING_NONE)) != 0)
        {
            MBEDTLS_SSL_DEBUG_RET(1, "mbedtls_cipher_set_padding_mode", ret);
            return( ret );
        }
    }
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

    mbedtls_zeroize(keyblk, sizeof(keyblk));

#if defined(MBEDTLS_ZLIB_SUPPORT)
#error There is no ZLIB Support for MAXQ
#endif



    MBEDTLS_SSL_DEBUG_MSG(2, ("<= derive keys"));

    return( 0 );
}


#if defined(MBEDTLS_CCM_MAXQ) || defined(MBEDTLS_GCM_MAXQ)
int maxq_cipher_aead_decrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *ad, size_t ad_len,
                    const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t output_len,
                    size_t *olen, size_t tag_len )
{
    int ret;
    mxq_u2 algo = ALGO_NONE;

    if( MBEDTLS_MODE_GCM == ctx->cipher_info->mode ) {
        algo = ALGO_CIPHER_AES_GCM;
    }
    else if( MBEDTLS_MODE_CCM == ctx->cipher_info->mode ) {
        algo = ALGO_CIPHER_AES_CCM;
    }

#if defined(MAXQ1065) || defined(MAXQ108x)
    (void)output_len;

    *olen = ilen - tag_len;
    if ((ret = MXQ_tls_cipher_aead(
            0x00,
            MXQ_OP_DECRYPT,
            algo,
            (mxq_u1 *)iv, iv_len,
            (mxq_u1 *)ad, ad_len,
            (mxq_u1 *)input, ilen - tag_len,
            output, (mxq_length *)olen,
            output + ilen - tag_len, tag_len)) != 0)
    {
        if( ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED )
            return( MBEDTLS_ERR_SSL_INVALID_MAC );
        return( ret );
    }
#else
    if (algo == ALGO_CIPHER_AES_GCM) {
        /* Soft GCM */
        if ((ret = mbedtls_cipher_auth_decrypt_ext(
                ctx,
                iv, iv_len,
                ad, ad_len,
                input, ilen,
                output, output_len,
                olen,
                tag_len)) != 0)
        {
            if( ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED )
                return( MBEDTLS_ERR_SSL_INVALID_MAC );
            return( ret );
        }
    }
    else {
        *olen = ilen - tag_len;
        if ((ret = MXQ_cipher_auth_ccm(
                0x00,
                MXQ_OP_DECRYPT,
                (mxq_u1 *)iv, iv_len,
                (mxq_u1 *)ad, ad_len,
                (mxq_u1 *)input, ilen - tag_len,
                output, (mxq_length *)olen,
                output + ilen - tag_len, tag_len)) != 0)
        {
            if( ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED )
                return( MBEDTLS_ERR_SSL_INVALID_MAC );
            return( ret );
        }
    }
#endif

    return ret;
}
#endif

#ifdef MBEDTLS_CIPHER_MODE_CBC
int maxq_cipher_cbc_decrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *input, size_t ilen,
                    const unsigned char *ad, size_t ad_len,
                    size_t mac_len,
                    unsigned char *output, size_t *olen )
{
    int ret;
    mxq_length mxq_olen = 0;

    (void)ctx;

#ifdef MAXQ108x
    ret = MXQ_tls_cipher_cbc(
            MXQ_OP_DECRYPT,
            0x00,
            iv, iv_len,
            input, ilen,
            output, &mxq_olen,
            (mxq_u1 *)ad, ad_len, mac_len);
    *olen = (size_t)mxq_olen;
#else
    (void)mac_len;

    ret = MXQ_tls_records(
            MXQ_OP_DECRYPT,
            0x00,
            iv, iv_len,
            input, ilen,
            output, &mxq_olen,
            (mxq_u1 *)ad, ad_len);
    *olen = (size_t)mxq_olen;
#endif

    return ret;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MBEDTLS_CCM_MAXQ) || defined(MBEDTLS_GCM_MAXQ)
int maxq_cipher_aead_encrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *ad, size_t ad_len,
                    const unsigned char *input, size_t ilen,
                    unsigned char *output, size_t output_len,
                    size_t *olen, size_t tag_len )
{
    int ret;
    mxq_u2 algo = ALGO_NONE;

    if( MBEDTLS_MODE_GCM == ctx->cipher_info->mode ) {
        algo = ALGO_CIPHER_AES_GCM;
    }
    else if( MBEDTLS_MODE_CCM == ctx->cipher_info->mode ) {
        algo = ALGO_CIPHER_AES_CCM;
    }

#if defined(MAXQ1065) || defined(MAXQ108x)
    (void)output_len;

    if ((ret = MXQ_tls_cipher_aead(
            0x00,
            MXQ_OP_ENCRYPT,
            algo,
            (mxq_u1 *)iv, iv_len,
            (mxq_u1 *)ad, ad_len,
            (mxq_u1 *)input, ilen,
            output, (mxq_length *)olen,
            output + ilen, tag_len)) != 0)
    {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    *olen += tag_len;
#else
    if (algo == ALGO_CIPHER_AES_GCM) {
        /* Soft GCM */
        if ((ret = mbedtls_cipher_auth_encrypt_ext(
                ctx,
                iv, iv_len,
                ad, ad_len,
                input, ilen,
                output, output_len,
                olen,
                tag_len)) != 0)
        {
            return( ret );
        }
    }
    else {
        if ((ret = MXQ_cipher_auth_ccm(
                0x00,
                MXQ_OP_ENCRYPT,
                (mxq_u1 *)iv, iv_len,
                (mxq_u1 *)ad, ad_len,
                (mxq_u1 *)input, ilen,
                output, (mxq_length *)olen,
                output + ilen, tag_len)) != 0)
        {
            return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
        }

        *olen += tag_len;
    }
#endif

    return ret;
}
#endif /* MBEDTLS_CCM_MAXQ */

#if defined(MBEDTLS_CIPHER_MODE_CBC)
int maxq_cipher_cbc_encrypt( mbedtls_cipher_context_t *ctx,
                    const unsigned char *iv, size_t iv_len,
                    const unsigned char *input, size_t ilen,
                    const unsigned char *ad, size_t ad_len,
                    size_t mac_len,
                    unsigned char *output, size_t *olen )
{
    int ret;
    mxq_length mxq_olen = 0;

    (void)ctx;

#ifdef MAXQ108x
    ret = MXQ_tls_cipher_cbc(
            MXQ_OP_ENCRYPT,
            0x00,
            iv, iv_len,
            input, ilen,
            output, &mxq_olen,
            (mxq_u1 *)ad, ad_len, mac_len);
    *olen = (size_t)mxq_olen;
#else
    (void)mac_len;

    ret = MXQ_tls_records(
            MXQ_OP_ENCRYPT,
            0x00,
            iv, iv_len,
            input, ilen,
            output, &mxq_olen,
            (mxq_u1 *)ad, ad_len);
    *olen = (size_t)mxq_olen;
#endif

    if (ret)
    {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }
    return ret;
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */

#if defined(MAXQ108x) && defined(MBEDTLS_SSL_PROTO_TLS1_3)
#define PSK_KID               (0x1234)

#define TEMP_KEY_ID_START     (0)
#define TEMP_KEY_ID_MAX_NUM   (16)
#define K_CHUNKSIZE           (2032)
#define K_CIPHER_BLOCKSIZE    (16)

static int temp_key_id_arr[TEMP_KEY_ID_MAX_NUM] = {0};
static int *mac_key_obj_id = NULL;
static int mac_comp_active = 0;

static unsigned char tls13early           = 0;
static int local_is_psk                   = 0;
static int is_hs_key                      = 0;
static int hkdf_expand_side               = MBEDTLS_SSL_IS_CLIENT;
static int tls13_psk_running              = 0;

static int tls13_dh_obj_id                = -1;
static int tls13_ecc_obj_id               = -1;
static int tls13_hs_early_secret_obj_id   = -1;
static int tls13_binder_key_obj_id        = -1;
static int tls13_shared_secret_obj_id     = -1;
static int tls13_early_secret_obj_id      = -1;
static int tls13_derived_secret_obj_id    = -1;
static int tls13_handshake_secret_obj_id  = -1;
static int tls13_master_secret_obj_id     = -1;
static int tls13_client_secret_obj_id     = -1;
static int tls13_client_secret_prv_obj_id = -1;
static int tls13_server_secret_obj_id     = -1;
static int tls13_exp_master_obj_id        = -1;
static int tls13_client_hs_key_obj_id     = -1;
static int tls13_server_hs_key_obj_id     = -1;
static int tls13_client_app_key_obj_id    = -1;
static int tls13_server_app_key_obj_id    = -1;
static int tls13_server_finish_obj_id     = -1;
static int tls13_client_finish_obj_id     = -1;
static int *tls13_server_key_id           = NULL;
static int *tls13_client_key_id           = NULL;
static int tls13_res_master_obj_id        = -1;
static int tls13_server_cert_id           = -1;
static int tls13_server_key_algo          = -1;
static int tls13_server_key_len           = -1;

static const char derivedLabel[]     = "derived";
static const char cHsTrafficLabel[]  = "c hs traffic";
static const char sHsTrafficLabel[]  = "s hs traffic";
static const char cAppTrafficLabel[] = "c ap traffic";
static const char sAppTrafficLabel[] = "s ap traffic";
static const char expMasterLabel[]   = "exp master";
static const char appTrafUpdLabel[]  = "traffic upd";
static const char keyLabel[]         = "key";
static const char ivLabel[]          = "iv";
static const char finishedLabel[]    = "finished";
static const char resMasterLabel[]   = "res master";
static const char extBinderLabel[]   = "ext binder";

static int alloc_temp_key_id(void)
{
    int i;
    for (i = 0; i < TEMP_KEY_ID_MAX_NUM; ++i) {
        if (temp_key_id_arr[i] == 0) {
            temp_key_id_arr[i] = 1;
            break;
        }
    }

    if (i == TEMP_KEY_ID_MAX_NUM) {
        return -1;
    }
    else {
        return i;
    }
}

static void free_temp_key_id(int obj_id)
{
    int idx_for_arr = obj_id - TEMP_KEY_ID_START;

    if (idx_for_arr >=0 && idx_for_arr < TEMP_KEY_ID_MAX_NUM) {
        temp_key_id_arr[idx_for_arr] = 0;
    }
}

static char *strstr_with_size(char *str, const char *substr, size_t n)
{
    char *p = str, *pEnd = str + n;
    size_t substr_len = strlen(substr);

    if (substr_len == 0) {
        return str;
    }

    pEnd -= (substr_len - 1);

    for (;p < pEnd; ++p) {
        if (0 == strncmp(p, substr, substr_len))
            return p;
    }

    return NULL;
}

static int get_tls13_server_cert_id(void)
{
    if (tls13_server_cert_id == -1) {
        tls13_server_cert_id = alloc_temp_key_id();
    }
    return tls13_server_cert_id;
}

static void set_tls13_server_key_algo_len(int algo, int len)
{
    tls13_server_key_algo = algo;
    tls13_server_key_len  = len;
}

static int getMaxqKeyParamFromCurve(int c)
{
    switch(c) {
    case MBEDTLS_ECP_DP_SECP256R1:       return MXQ_KEYPARAM_EC_P256R1;
    case MBEDTLS_ECP_DP_SECP384R1:       return MXQ_KEYPARAM_EC_P384R1;
    case MBEDTLS_ECP_DP_SECP521R1:       return MXQ_KEYPARAM_EC_P521R1;
    case MBEDTLS_ECP_DP_BP256R1:         return MXQ_KEYPARAM_EC_BP256R1;
    case MBEDTLS_ECP_DP_BP384R1:         return MXQ_KEYPARAM_EC_BP384R1;
    case MBEDTLS_ECP_DP_BP512R1:         return MXQ_KEYPARAM_EC_BP512R1;
    }

    return -1;
}

static int maxq_cipher_do(mxq_algo_id_t algo_id, mxq_u1 encrypt,
                          mxq_u2 key_id, mxq_u1* p_in, mxq_u1* p_out,
                          mxq_length data_size, mxq_u1* p_iv,
                          mxq_length iv_len, mxq_u1* p_aad,
                          mxq_length aad_len, mxq_u1* p_tag,
                          mxq_length tag_len)
{
    mxq_err_t mxq_rc;
    ciph_params_t cparams;

    mxq_u1 internal_data[K_CHUNKSIZE + K_CIPHER_BLOCKSIZE];
    mxq_u1 *p_int_data = internal_data;

    mxq_length data_offset = 0;
    mxq_length proc_len = 0, req_len = 0;

    memset(&cparams, 0, sizeof(cparams));

    cparams.data_length  = data_size;
    cparams.p_iv         = p_iv;
    cparams.iv_length    = iv_len;
    cparams.p_aad        = p_aad;
    cparams.aad_length   = aad_len;

    if (encrypt) {
        cparams.aead_tag_len = tag_len;
    }
    else {
        memcpy(cparams.aead_tag, p_tag, tag_len);
        cparams.aead_tag_len = tag_len;
    }

    mxq_rc = MXQ_Cipher_Init(encrypt, algo_id, key_id, &cparams, 0);
    if (mxq_rc) {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    while (data_size) {
        proc_len = (data_size < K_CHUNKSIZE) ? data_size : K_CHUNKSIZE;
        req_len  = proc_len;

        mxq_rc = MXQ_Cipher_Update(&p_out, &p_in[data_offset], &proc_len);
        if (mxq_rc) {
            return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
        }

        data_offset += req_len;
        data_size -= req_len;
    }

    if (encrypt) {
        proc_len = tag_len;
        mxq_rc = MXQ_Cipher_Finish(&p_int_data, &proc_len);
        if (mxq_rc) {
            return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
        }

        if (proc_len > tag_len) {
            memcpy(p_out, internal_data, proc_len - tag_len);
        }

        if ((tag_len != 0) && (proc_len >= tag_len)) {
            memcpy(p_tag, &internal_data[proc_len - tag_len], tag_len);
        }
    }
    else {
        internal_data[0] = 0xDE;
        memcpy(&internal_data[1], p_tag, tag_len);
        proc_len = tag_len;

        mxq_rc = MXQ_Cipher_Finish(&p_int_data, &proc_len);
        if (mxq_rc) {
            return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
        }

        if (proc_len) {
            memcpy(p_out, internal_data, proc_len);
        }
    }

    return 0;
}

int maxq_tls13_ecc_key_gen(mbedtls_ssl_context *ssl,
                    uint16_t named_group,
                    unsigned char *buf, unsigned char *end,
                    size_t *out_len)
{
    unsigned char mxq_key[MAX_EC_KEY_SIZE];
    const mbedtls_ecp_curve_info *curve_info =
                mbedtls_ecp_curve_info_from_tls_id( named_group );

    int mxq_key_param = getMaxqKeyParamFromCurve(curve_info->grp_id);
    if (mxq_key_param == -1) {
        return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }    

    if (tls13_ecc_obj_id == -1) {
        tls13_ecc_obj_id = alloc_temp_key_id();
        if (tls13_ecc_obj_id == -1) {
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }
    }

    MBEDTLS_SSL_DEBUG_MSG( 3, ( "maxq_tls13_ecc_key_gen() -> tls13_ecc_obj_id = %d",
                                    tls13_ecc_obj_id ) );

    int mxq_key_size = mxq_curve_size(mxq_key_param);
    mxq_err_t mxq_rc = MXQ_TLS13_Generate_Key(mxq_key, tls13_ecc_obj_id, MXQ_KEYTYPE_ECC,
                                mxq_key_param, mxq_key_size, NULL, 0, NULL);
    if (mxq_rc) {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    *out_len = (mxq_key_size * 2) + 1;
    if ((size_t)(end - buf) < *out_len) {
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    buf[0] = 0x04;
    memcpy(buf + 1, mxq_key, *out_len - 1);

    return 0;
}

int maxq_tls13_ecc_shared_secret(mbedtls_ssl_context *ssl,
                        unsigned char *out, size_t *out_len)
{
    int rc = 0;
    mbedtls_ssl_handshake_params *handshake = ssl->handshake;
    const mbedtls_ecp_curve_info *curve_info =
                mbedtls_ecp_curve_info_from_tls_id( handshake->offered_group_id );

    int mxq_key_param = getMaxqKeyParamFromCurve(curve_info->grp_id);
    if (mxq_key_param == -1) {
        return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    if (tls13_ecc_obj_id == -1) {
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    if (tls13_shared_secret_obj_id == -1) {
        tls13_shared_secret_obj_id = alloc_temp_key_id();
        if (tls13_shared_secret_obj_id == -1) {
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }
    }

    MBEDTLS_SSL_DEBUG_MSG( 3, ( "maxq_tls13_ecc_shared_secret() -> tls13_shared_secret_obj_id = %d",
                                    tls13_shared_secret_obj_id ) );

    memset(out, 0xFF, (handshake->ecdh_psa_peerkey_len - 1) >> 1); // dummy bytes

    mxq_err_t mxq_rc = MXQ_TLS13_Create_Secret(handshake->ecdh_psa_peerkey + 1,
                                    handshake->ecdh_psa_peerkey_len - 1,
                                    tls13_ecc_obj_id,
                                    MXQ_KEYTYPE_ECC,
                                    mxq_key_param,
                                    handshake->ciphersuite_info->id,
                                    tls13_shared_secret_obj_id,
                                    out, (mxq_length *)out_len);
    if (mxq_rc) {
        rc = MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    *out_len = (handshake->ecdh_psa_peerkey_len - 1) >> 1;

    free_temp_key_id(tls13_dh_obj_id);
    tls13_dh_obj_id = -1;
    free_temp_key_id(tls13_ecc_obj_id);
    tls13_ecc_obj_id = -1;

    return rc;
}

int maxq_tls13_hkdf_extract(unsigned char *prk,
                const unsigned char *salt, size_t saltLen,
                unsigned char *ikm, size_t ikmLen,
                psa_algorithm_t digest)
{
    mxq_err_t mxq_rc;
    mxq_length prk_len = 0;
    mxq_algo_id_t mxq_digest_id = ALGO_INVALID;
    int salt_kid = -1, ikm_kid = -1, ret_kid = -1;

    switch (digest) {
        case PSA_ALG_SHA_256: mxq_digest_id = ALGO_MD_SHA256; break;
        case PSA_ALG_SHA_384: mxq_digest_id = ALGO_MD_SHA384; break;
        case PSA_ALG_SHA_512: mxq_digest_id = ALGO_MD_SHA512; break;
        default:
            return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    prk_len = PSA_HASH_LENGTH( digest );

    /* Prepare key id parameters */
    if (saltLen != 0 && ikmLen != 0) {
        MXQ_PRINTF("maxq_tls13_hkdf_extract() -> handshake_secret\n");
        /* handshake_secret = HKDF-Extract(salt: derived_secret,
         *                        key: shared_secret) */
        if (tls13_handshake_secret_obj_id == -1) {
            tls13_handshake_secret_obj_id = alloc_temp_key_id();
            if (tls13_handshake_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        salt_kid = tls13_derived_secret_obj_id;
        ikm_kid  = tls13_shared_secret_obj_id;
        ret_kid  = tls13_handshake_secret_obj_id;
        free_temp_key_id(tls13_derived_secret_obj_id);
        tls13_derived_secret_obj_id = -1;
    }
    else if (saltLen != 0 && ikmLen == 0) {
        MXQ_PRINTF("maxq_tls13_hkdf_extract() -> master_secret\n");
        /* master_secret = HKDF-Extract(salt: derived_secret,
         *                     key: 00...) */
        if (tls13_master_secret_obj_id == -1) {
            tls13_master_secret_obj_id = alloc_temp_key_id();
            if (tls13_master_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        salt_kid = tls13_derived_secret_obj_id;
        ikm_kid  = -1;
        ret_kid  = tls13_master_secret_obj_id;
        free_temp_key_id(tls13_derived_secret_obj_id);
        tls13_derived_secret_obj_id = -1;
    }
    else if (saltLen == 0 && ikmLen == 0) {
        MXQ_PRINTF("maxq_tls13_hkdf_extract() -> early_secret\n");
        /* early_secret = HKDF-Extract(salt: 00, key: 00...) */
        if (tls13_early_secret_obj_id == -1) {
            tls13_early_secret_obj_id = alloc_temp_key_id();
            if (tls13_early_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        salt_kid = -1;
        ikm_kid  = -1;
        ret_kid  = tls13_early_secret_obj_id;

        tls13early = 1;
    }
    else if (saltLen == 0 && ikmLen != 0) {
        MXQ_PRINTF("maxq_tls13_hkdf_extract() -> hs_early_secret\n");
        /* early_secret = HKDF-Extract(salt: 00, key: 00...) */
        if (tls13_hs_early_secret_obj_id == -1) {
            tls13_hs_early_secret_obj_id = alloc_temp_key_id();
            if (tls13_hs_early_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        salt_kid = -1;
        ikm_kid  = PSK_KID;
        ret_kid  = tls13_hs_early_secret_obj_id;

        tls13early = 1;
    }
    else {
        return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    /* When length is 0 then use zeroed data of digest length. */
    if (ikmLen == 0) {
        ikmLen = prk_len;
        memset(ikm, 0, prk_len);
    }

    if (salt_kid != -1) {
        saltLen = 0;
    }

    if (ikm_kid != -1) {
        ikmLen = 0;
    }

    if (ret_kid != -1) {
        memset(prk, 0xFF, prk_len); // dummy bytes
        prk_len = 0;
    }

    MXQ_PRINTF("maxq_tls13_hkdf_extract() -> salt_kid = %d, ikm_kid = %d, ret_kid = %d\n",
                    (int)salt_kid, (int)ikm_kid, (int)ret_kid);

    mxq_rc = MXQ_TLS13_Extract_Secret(mxq_digest_id, (mxq_u2)ret_kid,
                                      prk, &prk_len, (mxq_u2)salt_kid,
                                      salt, (mxq_u2)saltLen,
                                      (mxq_u2)ikm_kid, ikm, (mxq_u2)ikmLen);
    if (mxq_rc) {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    return 0;
}

void maxq_tls13_hkdf_set_side(int side)
{
    // MBEDTLS_SSL_IS_CLIENT -> 0, MBEDTLS_SSL_IS_SERVER -> 1
    hkdf_expand_side = side;
}

int maxq_tls13_hkdf_get_side(void)
{
    return hkdf_expand_side;
}

int maxq_tls13_hkdf_expand(psa_algorithm_t digest,
                    const unsigned char *inKey, size_t inKeySz,
                    const unsigned char *info, size_t infoSz,
                    unsigned char *out, size_t outSz,
                    int side)
{
    mxq_err_t mxq_rc;
    mxq_algo_id_t mxq_digest_id = ALGO_INVALID;
    mxq_keytype_id_t ret_keytype = MXQ_KEYTYPE_IKM;
    int prk_kid = -1, ret_kid = -1;
    int ret_isiv = 0;

    switch (digest) {
        case PSA_ALG_SHA_256: mxq_digest_id = ALGO_MD_SHA256; break;
        case PSA_ALG_SHA_384: mxq_digest_id = ALGO_MD_SHA384; break;
        case PSA_ALG_SHA_512: mxq_digest_id = ALGO_MD_SHA512; break;
        default:
            return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    /* Prepare key id parameters */
    if (strstr_with_size((char *)info, derivedLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_derived_secret_obj_id\n");
        if (tls13early) {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13early\n");
            if (local_is_psk) {
                MXQ_PRINTF("maxq_tls13_hkdf_expand() -> local_is_psk\n");
                if (tls13_hs_early_secret_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }

                tls13_derived_secret_obj_id = alloc_temp_key_id();
                if (tls13_derived_secret_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }

                prk_kid = tls13_hs_early_secret_obj_id;
                ret_kid = tls13_derived_secret_obj_id;
                ret_keytype = MXQ_KEYTYPE_IKM;
                free_temp_key_id(tls13_hs_early_secret_obj_id);
                tls13_hs_early_secret_obj_id = -1;
            }
            else {
                /* derived_secret = HKDF-Expand-Label(key: early_secret,
                 *                      label: "derived", ctx: empty_hash) */
                tls13_derived_secret_obj_id = alloc_temp_key_id();
                if (tls13_derived_secret_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }

                prk_kid = tls13_early_secret_obj_id;
                ret_kid = tls13_derived_secret_obj_id;
                ret_keytype = MXQ_KEYTYPE_IKM;
                free_temp_key_id(tls13_early_secret_obj_id);
                tls13_early_secret_obj_id = -1;
            }

            ret_isiv = 0;
            tls13early = 0;
        }
        else {
            /* derived_secret = HKDF-Expand-Label(key: handshake_secret,
             *                      label: "derived", ctx: empty_hash) */
            tls13_derived_secret_obj_id = alloc_temp_key_id();
            if (tls13_derived_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }

            if (tls13_psk_running) {
                MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_psk_running\n");
                prk_kid = tls13_master_secret_obj_id;
            }
            else {
                prk_kid = tls13_handshake_secret_obj_id;
            }
            ret_kid = tls13_derived_secret_obj_id;
            ret_keytype = MXQ_KEYTYPE_IKM;
            free_temp_key_id(tls13_handshake_secret_obj_id);
            tls13_handshake_secret_obj_id = -1;
            ret_isiv = 0;
        }
    }
    else if (strstr_with_size((char *)info, cHsTrafficLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_client_secret_obj_id\n");
        /* client_secret = HKDF-Expand-Label(key: handshake_secret,
         *                     label: "c hs traffic", ctx: hello_hash) */
        is_hs_key = 1;
        if (tls13_client_secret_obj_id == -1) {
            tls13_client_secret_obj_id = alloc_temp_key_id();
            if (tls13_client_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        if (tls13_psk_running) {
            prk_kid = tls13_master_secret_obj_id;
        }
        else {
            prk_kid = tls13_handshake_secret_obj_id;
        }
        ret_kid = tls13_client_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, sHsTrafficLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_server_secret_obj_id\n");
        /* client_secret = HKDF-Expand-Label(key: handshake_secret,
         *                     label: "s hs traffic", ctx: hello_hash) */
        if (tls13_server_secret_obj_id == -1) {
            tls13_server_secret_obj_id = alloc_temp_key_id();
            if (tls13_server_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        if (tls13_psk_running) {
            prk_kid = tls13_master_secret_obj_id;
        }
        else {
            prk_kid = tls13_handshake_secret_obj_id;
        }
        ret_kid = tls13_server_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, cAppTrafficLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_client_secret_obj_id\n");
        /* client_secret = HKDF-Expand-Label(key: master_secret,
         *                     label: "c ap traffic", ctx: handshake_hash) */
        is_hs_key = 0;
        if (tls13_client_secret_obj_id == -1) {
            tls13_client_secret_obj_id = alloc_temp_key_id();
            if (tls13_client_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }
        else {
            tls13_client_secret_prv_obj_id = tls13_client_secret_obj_id;
            tls13_client_secret_obj_id = alloc_temp_key_id();
            if (tls13_client_secret_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }
        }

        prk_kid = tls13_master_secret_obj_id;
        ret_kid = tls13_client_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, sAppTrafficLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_server_secret_obj_id\n");
        /* server_secret = HKDF-Expand-Label(key: master_secret,
         *                     label: "s ap traffic", ctx: handshake_hash) */
        tls13_server_secret_obj_id = alloc_temp_key_id();
        if (tls13_server_secret_obj_id == -1) {
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }

        prk_kid = tls13_master_secret_obj_id;
        ret_kid = tls13_server_secret_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, expMasterLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_exp_master_obj_id\n");
        /* label: "exp master" */
        tls13_exp_master_obj_id = alloc_temp_key_id();
        if (tls13_exp_master_obj_id == -1) {
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }

        prk_kid = tls13_master_secret_obj_id;
        ret_kid = tls13_exp_master_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
    }
    else if (strstr_with_size((char *)info, keyLabel, infoSz) != NULL) {
        /* first client key then server */
        if (side == MBEDTLS_SSL_IS_CLIENT) {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_client_key_obj_id\n");
            /* client_handshake_key = HKDF-Expand-Label(key: client_secret,
             *                            label: "key", ctx: "")
             * client_application_key = HKDF-Expand-Label(key: client_secret,
             *                              label: "key", ctx: "") */
            int tls13_client_key_obj_id = -1;
            if (is_hs_key) {
                if (tls13_client_hs_key_obj_id == -1) {
                    tls13_client_hs_key_obj_id = alloc_temp_key_id();
                    if (tls13_client_hs_key_obj_id == -1) {
                        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                    }
                }
                tls13_client_key_obj_id = tls13_client_hs_key_obj_id;
            }
            else {
                if (tls13_client_app_key_obj_id == -1) {
                    tls13_client_app_key_obj_id = alloc_temp_key_id();
                    if (tls13_client_app_key_obj_id == -1) {
                        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                    }
                }
                tls13_client_key_obj_id = tls13_client_app_key_obj_id;
            }

            prk_kid = tls13_client_secret_obj_id;
            ret_kid = tls13_client_key_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 0;
        }
        else {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_server_key_obj_id\n");
            /* server_handshake_key = HKDF-Expand-Label(key: server_secret,
             *                            label: "key", ctx: "")
             * server_application_key = HKDF-Expand-Label(key: server_secret,
                                            label: "key", ctx: "") */
            int tls13_server_key_obj_id = -1;
            if (is_hs_key) {
                if (tls13_server_hs_key_obj_id == -1) {
                    tls13_server_hs_key_obj_id = alloc_temp_key_id();
                    if (tls13_server_hs_key_obj_id == -1) {
                        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                    }
                }
                tls13_server_key_obj_id = tls13_server_hs_key_obj_id;
            }
            else {
                if (tls13_server_app_key_obj_id == -1) {
                    tls13_server_app_key_obj_id = alloc_temp_key_id();
                    if (tls13_server_app_key_obj_id == -1) {
                        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                    }
                }
                tls13_server_key_obj_id = tls13_server_app_key_obj_id;
            }

            prk_kid = tls13_server_secret_obj_id;
            ret_kid = tls13_server_key_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 0;
        }
    }
    else if (strstr_with_size((char *)info, ivLabel, infoSz) != NULL) {
        /* first client key then server */
        if (side == MBEDTLS_SSL_IS_CLIENT) {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_client_iv_obj_id\n");
            /* client_handshake_iv = HKDF-Expand-Label(key: client_secret,
             *                           label: "iv", ctx: "")
             * cient_application_iv = HKDF-Expand-Label(key: client_secret,
             *                            label: "iv", ctx: "") */
            int tls13_client_iv_obj_id = -1;
            if (is_hs_key) {
                if (tls13_client_hs_key_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }
                tls13_client_iv_obj_id = tls13_client_hs_key_obj_id;
            }
            else {
                if (tls13_client_app_key_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }
                tls13_client_iv_obj_id = tls13_client_app_key_obj_id;
            }

            prk_kid = tls13_client_secret_obj_id;
            ret_kid = tls13_client_iv_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 1;
        }
        else {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_server_iv_obj_id\n");
            /* server_handshake_iv = HKDF-Expand-Label(key: server_secret,
             *                           label: "iv", ctx: "")
             * server_application_iv = HKDF-Expand-Label(key: server_secret,
             * label: "iv", ctx: "") */
            int tls13_server_iv_obj_id = -1;
            if (is_hs_key) {
                if (tls13_server_hs_key_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }
                tls13_server_iv_obj_id = tls13_server_hs_key_obj_id;
            }
            else {
                if (tls13_server_app_key_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }
                tls13_server_iv_obj_id = tls13_server_app_key_obj_id;
            }

            prk_kid = tls13_server_secret_obj_id;
            ret_kid = tls13_server_iv_obj_id;
            ret_keytype = MXQ_KEYTYPE_AES;
            ret_isiv = 1;
            local_is_psk = 0;
        }
    }
    else if (strstr_with_size((char *)info, finishedLabel, infoSz) != NULL) {
        if (local_is_psk) {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_client_finish_obj_id\n");
            if (tls13_client_finish_obj_id == -1) {
                tls13_client_finish_obj_id = alloc_temp_key_id();
                if (tls13_client_finish_obj_id == -1) {
                    return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                }
            }
            if (tls13_binder_key_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }

            prk_kid = tls13_binder_key_obj_id;
            ret_kid = tls13_client_finish_obj_id;
            tls13_client_key_id = &tls13_binder_key_obj_id;
            ret_keytype = MXQ_KEYTYPE_HMAC;
            ret_isiv = 0;
        }
        else {
            /* first client key then server */
            if (side == MBEDTLS_SSL_IS_CLIENT) {
                MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_client_finish_obj_id (is_hs_key = %d)\n", is_hs_key);
                /* finished_key = HKDF-Expand-Label(key: client_secret,
                 * label: "finished", ctx: "") */
                if (is_hs_key) {
                    tls13_client_key_id = &tls13_client_hs_key_obj_id;
                }
                else {
                    tls13_client_key_id = &tls13_client_app_key_obj_id;
                }

                if (tls13_client_finish_obj_id == -1) {
                    tls13_client_finish_obj_id = alloc_temp_key_id();
                    if (tls13_client_finish_obj_id == -1) {
                        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                    }
                }

                if (tls13_client_secret_prv_obj_id != -1) {
                    tls13_client_key_id = &tls13_client_hs_key_obj_id; // override
                    prk_kid = tls13_client_secret_prv_obj_id;
                }
                else {
                    prk_kid = tls13_client_secret_obj_id;
                }
                ret_kid = tls13_client_finish_obj_id;

                ret_keytype = MXQ_KEYTYPE_HMAC;
                ret_isiv = 0;
                free_temp_key_id(tls13_client_secret_obj_id);
                tls13_client_secret_obj_id = -1;

                if (tls13_client_secret_prv_obj_id != -1) {
                    free_temp_key_id(tls13_client_secret_prv_obj_id);
                    tls13_client_secret_prv_obj_id = -1;
                }
            }
            else {
                MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_server_finish_obj_id (is_hs_key = %d)\n", is_hs_key);
                /* finished_key = HKDF-Expand-Label(key: server_secret,
                 *                    label: "finished", ctx: "") */
                if (is_hs_key) {
                    tls13_server_key_id = &tls13_server_hs_key_obj_id;
                }
                else {
                    tls13_server_key_id = &tls13_server_app_key_obj_id;
                }

                if (tls13_server_finish_obj_id == -1) {
                    tls13_server_finish_obj_id = alloc_temp_key_id();
                    if (tls13_server_finish_obj_id == -1) {
                        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
                    }
                }

                prk_kid = tls13_server_secret_obj_id;
                ret_kid = tls13_server_finish_obj_id;
                ret_keytype = MXQ_KEYTYPE_HMAC;
                ret_isiv = 0;
                free_temp_key_id(tls13_server_secret_obj_id);
                tls13_server_secret_obj_id = -1;
            }
        }
    }
    else if (strstr_with_size((char *)info, extBinderLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_binder_key_obj_id\n");
        /* binder_key = HKDF-Expand-Label(key: hs_early_secret,
            *                  label: "ext binder", ctx: empty_hash) */
        tls13_binder_key_obj_id = alloc_temp_key_id();
        if (tls13_binder_key_obj_id == -1) {
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }

        prk_kid = tls13_hs_early_secret_obj_id;
        ret_kid = tls13_binder_key_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
        local_is_psk = 1;
        tls13_psk_running = 1;
    }
    else if (strstr_with_size((char *)info, resMasterLabel, infoSz) != NULL) {
        MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_res_master_obj_id\n");
        tls13_res_master_obj_id = alloc_temp_key_id();
        if (tls13_res_master_obj_id == -1) {
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        }

        prk_kid = tls13_master_secret_obj_id;
        ret_kid = tls13_res_master_obj_id;
        ret_keytype = MXQ_KEYTYPE_IKM;
        ret_isiv = 0;
        free_temp_key_id(*tls13_client_key_id);
        *tls13_client_key_id = -1;
    }
    else if (strstr_with_size((char *)info, appTrafUpdLabel, infoSz) != NULL) {
        if (side == MBEDTLS_SSL_IS_CLIENT) {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_client_secret_obj_id\n");
            /* updated_client_secret = HKDF-Expand-Label(key: client_secret,
             *                             label: "traffic upd", ctx: "") */
            if (tls13_client_app_key_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }

            prk_kid = tls13_client_secret_obj_id;
            ret_kid = tls13_client_secret_obj_id;
            ret_keytype = MXQ_KEYTYPE_IKM;
            ret_isiv = 0;
        }
        else {
            MXQ_PRINTF("maxq_tls13_hkdf_expand() -> tls13_server_secret_obj_id\n");
            /* updated_server_secret = HKDF-Expand-Label(key: server_secret,
             *                             label: "traffic upd", ctx: "") */
            if (tls13_server_app_key_obj_id == -1) {
                return MBEDTLS_ERR_SSL_ALLOC_FAILED;
            }

            prk_kid = tls13_server_secret_obj_id;
            ret_kid = tls13_server_secret_obj_id;
            ret_keytype = MXQ_KEYTYPE_IKM;
            ret_isiv = 0;
        }
    }
    else {
        return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    if (prk_kid != -1) {
        inKeySz = 0;
    }

    if (ret_kid != -1) {
        memset(out, 0xFF, outSz); // dummy bytes
    }

    MXQ_PRINTF("maxq_tls13_hkdf_expand() -> prk_kid = %d, ret_kid = %d\n",
                    (int)prk_kid, (int)ret_kid);

    mxq_rc = MXQ_TLS13_Expand_Secret(mxq_digest_id, (mxq_u2)ret_kid,
                                     ret_keytype, ret_isiv, out, (mxq_length *)&outSz,
                                     (mxq_u2)prk_kid, inKey, inKeySz,
                                     info, infoSz );
    if (mxq_rc) {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    return 0;
}

int maxq_tls13_record_processing(mbedtls_cipher_context_t *ctx,
            const unsigned char *iv, size_t iv_len,
            const unsigned char *ad, size_t ad_len,
            const unsigned char *input, size_t ilen,
            unsigned char *output, size_t output_len,
            size_t *olen, size_t tag_len,
            int is_encrypt)
{
    (void)output_len;

    mxq_u2 key_id = 0;
    if (is_encrypt) {
        if (tls13_client_hs_key_obj_id != -1) {
            key_id = tls13_client_hs_key_obj_id;
            MXQ_PRINTF("maxq_tls13_record_processing() -> tls13_client_hs_key_obj_id\n");
        }
        else if (tls13_client_app_key_obj_id != -1) {
            key_id = tls13_client_app_key_obj_id;
            MXQ_PRINTF("maxq_tls13_record_processing() -> tls13_client_app_key_obj_id\n");
        }
        else {
            MXQ_PRINTF("maxq_tls13_record_processing() -> encrypt key not selected\n");
        }
    }
    else {
        if (tls13_server_hs_key_obj_id != -1) {
            key_id = tls13_server_hs_key_obj_id;
            MXQ_PRINTF("maxq_tls13_record_processing() -> tls13_server_hs_key_obj_id\n");
        }
        else if (tls13_server_app_key_obj_id != -1) {
            key_id = tls13_server_app_key_obj_id;
            MXQ_PRINTF("maxq_tls13_record_processing() -> tls13_server_app_key_obj_id\n");
        }
        else {
            MXQ_PRINTF("maxq_tls13_record_processing() -> decrypt key not selected\n");
        }
    }

    mxq_algo_id_t algo_id = ALGO_NONE;
    if ( MBEDTLS_MODE_GCM == ctx->cipher_info->mode ) {
        algo_id = ALGO_CIPHER_AES_GCM;
    }
    else if ( MBEDTLS_MODE_CCM == ctx->cipher_info->mode ) {
        algo_id = ALGO_CIPHER_AES_CCM;
    }

    mxq_err_t mxq_rc = MXQ_TLS13_Update_IV(key_id, (mxq_u1 *)iv, iv_len);
    if (mxq_rc) {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    if (is_encrypt) {
        ilen += tag_len;
    }
    else {
        *olen = ilen - tag_len;
    }

    mxq_rc = maxq_cipher_do(algo_id, is_encrypt, key_id,
                            (mxq_u1 *)input, output, ilen - tag_len,
                            (mxq_u1 *)iv, iv_len,
                            (mxq_u1 *)ad, ad_len,
                            output + ilen - tag_len, tag_len);
    if (mxq_rc) {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    if (is_encrypt) {
        *olen += tag_len;
    }

    return 0;
}

int maxq_tls13_hmac_set_key(psa_algorithm_t digest)
{
    mxq_algo_id_t algo = ALGO_NONE;
    switch (digest) {
        case PSA_ALG_SHA_256: algo = ALGO_HMAC_SHA256; break;
        case PSA_ALG_SHA_384: algo = ALGO_HMAC_SHA384; break;
        default:
            return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    if (tls13_server_finish_obj_id != -1) {
        free_temp_key_id(*tls13_server_key_id);
        *tls13_server_key_id = -1;
        mac_key_obj_id = &tls13_server_finish_obj_id;
    }
    else if (tls13_client_finish_obj_id != -1) {
        mac_key_obj_id = &tls13_client_finish_obj_id;
    }

    if (mac_key_obj_id == NULL) {
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }

    mxq_err_t mxq_rc = MXQ_MAC_Init(0x02, algo, *mac_key_obj_id, NULL, 0);
    if (mxq_rc == 0) {
        mac_comp_active = 1;
    }
    else {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    return 0;
}

int maxq_tls13_hmac_update(const unsigned char *msg, size_t length)
{
    if (!mac_comp_active) {
        return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    mxq_err_t mxq_rc = MXQ_MAC_Update((unsigned char *)msg, length);
    if (mxq_rc) {
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    return 0;
}

int maxq_tls13_hmac_final(unsigned char *hash, size_t *dst_len)
{
    int rc = 0;
    mxq_length maclen = 64;

    if (!mac_comp_active) {
        return MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE;
    }

    mxq_err_t mxq_rc = MXQ_MAC_Finish(hash, &maclen);
    if (mxq_rc) {
        rc = MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }
    else {
        *dst_len = maclen;
    }

    free_temp_key_id(*mac_key_obj_id);
    *mac_key_obj_id = -1;
    mac_key_obj_id = NULL;
    mac_comp_active = 0;

    return rc;
}
#endif /* MAXQ108x && MBEDTLS_SSL_PROTO_TLS1_3 */

 /*---------------------------------------------------------------------------*/
 /* not modified functions */
#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
int ssl_calc_finished_tls_sha256(
    mbedtls_ssl_context *ssl, unsigned char *buf, int from)
{
    int len = 12;
    const char *sender;
    unsigned char padbuf[32];
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    size_t hash_size;
    psa_hash_operation_t sha256_psa = PSA_HASH_OPERATION_INIT;
    psa_status_t status;
#else
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_md_context_t sha256;
#endif

    mbedtls_ssl_session *session = ssl->session_negotiate;
    if (!session) {
        session = ssl->session;
    }

    sender = (from == MBEDTLS_SSL_IS_CLIENT)
             ? "client finished"
             : "server finished";

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    sha256_psa = psa_hash_operation_init();

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> calc PSA finished tls sha256"));

    status = psa_hash_clone(&ssl->handshake->fin_sha256_psa, &sha256_psa);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = psa_hash_finish(&sha256_psa, padbuf, sizeof(padbuf), &hash_size);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    MBEDTLS_SSL_DEBUG_BUF(3, "PSA calculated padbuf", padbuf, 32);
#else

    mbedtls_md_init(&sha256);

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> calc  finished tls sha256"));

    ret = mbedtls_md_setup(&sha256, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    if (ret != 0) {
        goto exit;
    }
    ret = mbedtls_md_clone(&sha256, &ssl->handshake->fin_sha256);
    if (ret != 0) {
        goto exit;
    }

    /*
     * TLSv1.2:
     *   hash = PRF( master, finished_label,
     *               Hash( handshake ) )[0.11]
     */

    ret = mbedtls_md_finish(&sha256, padbuf);
    if (ret != 0) {
        goto exit;
    }
#endif /* MBEDTLS_USE_PSA_CRYPTO */

    MBEDTLS_SSL_DEBUG_BUF(4, "finished sha256 output", padbuf, 32);

    ssl->handshake->tls_prf(session->master, 48, sender,
                            padbuf, 32, buf, len);

    MBEDTLS_SSL_DEBUG_BUF(3, "calc finished result", buf, len);

    mbedtls_platform_zeroize(padbuf, sizeof(padbuf));

    MBEDTLS_SSL_DEBUG_MSG(2, ("<= calc  finished"));

exit:
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    psa_hash_abort(&sha256_psa);
    return PSA_TO_MD_ERR(status);
#else
    mbedtls_md_free(&sha256);
    return ret;
#endif /* MBEDTLS_USE_PSA_CRYPTO */
}

int ssl_calc_verify_tls_sha256(const mbedtls_ssl_context *ssl,
                               unsigned char *hash,
                               size_t *hlen)
{
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    size_t hash_size;
    psa_status_t status;
    psa_hash_operation_t sha256_psa = psa_hash_operation_init();

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> PSA calc verify sha256"));
    status = psa_hash_clone(&ssl->handshake->fin_sha256_psa, &sha256_psa);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = psa_hash_finish(&sha256_psa, hash, 32, &hash_size);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    *hlen = 32;
    MBEDTLS_SSL_DEBUG_BUF(3, "PSA calculated verify result", hash, *hlen);
    MBEDTLS_SSL_DEBUG_MSG(2, ("<= PSA calc verify"));

exit:
    psa_hash_abort(&sha256_psa);
    return PSA_TO_MD_ERR(status);
#else
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_md_context_t sha256;

    mbedtls_md_init(&sha256);

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> calc verify sha256"));

    ret = mbedtls_md_setup(&sha256, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
    if (ret != 0) {
        goto exit;
    }
    ret = mbedtls_md_clone(&sha256, &ssl->handshake->fin_sha256);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_md_finish(&sha256, hash);
    if (ret != 0) {
        goto exit;
    }

    *hlen = 32;

    MBEDTLS_SSL_DEBUG_BUF(3, "calculated verify result", hash, *hlen);
    MBEDTLS_SSL_DEBUG_MSG(2, ("<= calc verify"));

exit:
    mbedtls_md_free(&sha256);
    return ret;
#endif /* MBEDTLS_USE_PSA_CRYPTO */
}

#if defined(MBEDTLS_HAS_ALG_SHA_384_VIA_MD_OR_PSA_BASED_ON_USE_PSA)
int ssl_calc_finished_tls_sha384(
    mbedtls_ssl_context *ssl, unsigned char *buf, int from)
{
    int len = 12;
    const char *sender;
    unsigned char padbuf[48];
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    size_t hash_size;
    psa_hash_operation_t sha384_psa = PSA_HASH_OPERATION_INIT;
    psa_status_t status;
#else
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_md_context_t sha384;
#endif

    mbedtls_ssl_session *session = ssl->session_negotiate;
    if (!session) {
        session = ssl->session;
    }

    sender = (from == MBEDTLS_SSL_IS_CLIENT)
                ? "client finished"
                : "server finished";

#if defined(MBEDTLS_USE_PSA_CRYPTO)
    sha384_psa = psa_hash_operation_init();

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> calc PSA finished tls sha384"));

    status = psa_hash_clone(&ssl->handshake->fin_sha384_psa, &sha384_psa);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = psa_hash_finish(&sha384_psa, padbuf, sizeof(padbuf), &hash_size);
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    MBEDTLS_SSL_DEBUG_BUF(3, "PSA calculated padbuf", padbuf, 48);
#else
    mbedtls_md_init(&sha384);

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> calc  finished tls sha384"));

    ret = mbedtls_md_setup(&sha384, mbedtls_md_info_from_type(MBEDTLS_MD_SHA384), 0);
    if (ret != 0) {
        goto exit;
    }
    ret = mbedtls_md_clone(&sha384, &ssl->handshake->fin_sha384);
    if (ret != 0) {
        goto exit;
    }

    /*
     * TLSv1.2:
     *   hash = PRF( master, finished_label,
     *               Hash( handshake ) )[0.11]
     */

    ret = mbedtls_md_finish(&sha384, padbuf);
    if (ret != 0) {
        goto exit;
    }
#endif

    MBEDTLS_SSL_DEBUG_BUF(4, "finished sha384 output", padbuf, 48);

    ssl->handshake->tls_prf(session->master, 48, sender,
                            padbuf, 48, buf, len);

    MBEDTLS_SSL_DEBUG_BUF(3, "calc finished result", buf, len);

    mbedtls_platform_zeroize(padbuf, sizeof(padbuf));

    MBEDTLS_SSL_DEBUG_MSG(2, ("<= calc  finished"));

exit:
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    psa_hash_abort(&sha384_psa);
    return PSA_TO_MD_ERR(status);
#else
    mbedtls_md_free(&sha384);
    return ret;
#endif /* MBEDTLS_USE_PSA_CRYPTO */
}

int ssl_calc_verify_tls_sha384(const mbedtls_ssl_context *ssl,
                               unsigned char *hash,
                               size_t *hlen)
{
#if defined(MBEDTLS_USE_PSA_CRYPTO)
    size_t hash_size;
    psa_status_t status;
    psa_hash_operation_t sha384_psa = psa_hash_operation_init();

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> PSA calc verify sha384"));
    status = psa_hash_clone(&ssl->handshake->fin_sha384_psa, &sha384_psa);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    status = psa_hash_finish(&sha384_psa, hash, 48, &hash_size);
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    *hlen = 48;
    MBEDTLS_SSL_DEBUG_BUF(3, "PSA calculated verify result", hash, *hlen);
    MBEDTLS_SSL_DEBUG_MSG(2, ("<= PSA calc verify"));

exit:
    psa_hash_abort(&sha384_psa);
    return PSA_TO_MD_ERR(status);
#else
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_md_context_t sha384;

    mbedtls_md_init(&sha384);

    MBEDTLS_SSL_DEBUG_MSG(2, ("=> calc verify sha384"));

    ret = mbedtls_md_setup(&sha384, mbedtls_md_info_from_type(MBEDTLS_MD_SHA384), 0);
    if (ret != 0) {
        goto exit;
    }
    ret = mbedtls_md_clone(&sha384, &ssl->handshake->fin_sha384);
    if (ret != 0) {
        goto exit;
    }

    ret = mbedtls_md_finish(&sha384, hash);
    if (ret != 0) {
        goto exit;
    }

    *hlen = 48;

    MBEDTLS_SSL_DEBUG_BUF(3, "calculated verify result", hash, *hlen);
    MBEDTLS_SSL_DEBUG_MSG(2, ("<= calc verify"));

exit:
    mbedtls_md_free(&sha384);
    return ret;
#endif /* MBEDTLS_USE_PSA_CRYPTO */
}
#endif /* MBEDTLS_HAS_ALG_SHA_384_VIA_MD_OR_PSA_BASED_ON_USE_PSA */
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */
 /*---------------------------------------------------------------------------*/

#endif /* __MAXQ10XX_CLI__ */

#endif /* __MAXQ10XX__ */
