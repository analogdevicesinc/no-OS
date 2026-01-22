#ifndef __USS_API_H_INCLUDED__
#define __USS_API_H_INCLUDED__

#include "MXQ_API.h"

/**
 * Definition of cryptographic key usage.
 * @ingroup KeyManagement
 */

typedef enum _PACKED
{
    USS_KEYUSE_SECURECHANNEL            =0x00U, /**< Key can be used for secure channel */
    USS_KEYUSE_DATASIGNATURE	        =0x01U, /**< Key/Cert can be used for generic data signature/verification */
    USS_KEYUSE_ATTESTATION              =0x11U, /**< Key/Cert can be used for generic data signature/verification */
    USS_KEYUSE_INTERNALENCRYPTION       =0xA2U, /**< Key can be used for object or key data encryption/decryption */
    USS_KEYUSE_INTERNALMAC		=0xA1U, /**< Key can be used for MAC key derivation only*/
    USS_KEYUSE_ENCRYPTION		=0x02U, /**< Key can be used for generic data encryption/decryption */
    USS_KEYUSE_TLSAUTH			=0x04U, /**< Key can be used in TLS authentication */
    USS_KEYUSE_ADMINAUTH		=0x08U, /**< Key can be used to authenticate the manufacturer */
    USS_KEYUSE_ENDCUSTOMERAUTH	        =0x18U, /**< Key can be used to authenticate the end customer */
    USS_KEYUSE_VERIFY_KEY_CERT	        =0x10U, /**< Key/Cert can be used to verify certificates */
    USS_KEYUSE_VERIFY_FW		=0x20U, /**< Key can be used to verify firmware */
    USS_KEYUSE_RFU   			=0x40U, /**< RFU */
    USS_KEYUSE_KEY_IMPORT		=0x80U, /**< Key can be used to verify import/generate key requests */
    USS_KEYUSE_GPIO_CONTROL 	        =0x8AU, /**< Key can be used to Secure GPIO Control */
    USS_KEYUSE_KEY_EXCHAGE		=0xD1U, /**< Key can be used for key exchange requests */
//#ifdef MXQ_FW_UPDATE_CAP
    USS_KEYUSE_FIRMWARE_UPDATE          =0xA0U, /**< Key can be used to authenticate firmware updates */
//#endif
    USS_KEYUSE_NONE         	        =0xFFU  /**< Key can't be used */

} uss_keyuse_t;

/**
 * Definition of Key Curves.
 * @ingroup KeyManagement
 */
typedef enum _PACKED
{
    /*  For USS_KEYTYPE_ECC: */
    USS_KEYPARAM_EC_P256R1, /**< NIST secP256r1 */
    USS_KEYPARAM_EC_P384R1, /**< NIST secP384r1  */
    USS_KEYPARAM_EC_P521R1, /**< NIST secP521r1 */
    USS_KEYPARAM_EC_BP256R1, /**< Brainpool P256r1 */
    USS_KEYPARAM_EC_BP384R1, /**< Brainpool P384r1  */
    USS_KEYPARAM_EC_BP512R1, /**< Brainpool P521r1 */
    USS_KEYPARAM_EDWARD_TBD,
#ifdef ENABLE_RSA
    USS_KEYPARAM_RSA_PUB3,
    USS_KEYPARAM_RSA_CRT_PUB3,
    USS_KEYPARAM_RSA_PUB65537,
    USS_KEYPARAM_RSA_CRT_PUB65537,
    USS_KEYPARAM_DHE = 0xAA,
#endif
    USS_UNKNOWN_CURVE
} uss_keyparam_id_t;

/**
 * Definition of key types.
 * @ingroup KeyManagement
 */

typedef enum _PACKED
{
    USS_KEYTYPE_AES,     /**< AES Key. Supports AES 128 and 256, not 192 (less secure) */
    USS_KEYTYPE_PSK,     /**< TLS Pre Shared Key */
    USS_KEYTYPE_HMAC,    /**< HMAC key. Key length must be suitable, adjusted to the used SHA algo */
    USS_KEYTYPE_ECC,     /**< ECC key. Key length must fit the curve! */
    USS_KEYTYPE_IKM,     /**< Input Key Material for HKDF! */
    USS_KEYTYPE_RSA,     /**< RSA key */
    USS_KEYTYPE_UNKNOWN, /**< Unknown key */
} uss_keytype_id_t;

/**
 * Definition of key object types.
 * @ingroup ObjectManagement
 */
typedef enum _PACKED
{
    USS_OBJTYPE_SECRETKEY     = 0x01U, /**< Secret Key */
    USS_OBJTYPE_PUBKEY        = 0x02U, /**< Public key */
    USS_OBJTYPE_KEYPAIR       = 0x03U, /**< Key Pair */
    USS_OBJTYPE_TRANSPARENT   = 0x04U, /**< Transparent object */
    USS_OBJTYPE_COUNTER_UP    = 0x05U, /**< Monotonic counter incrementing */
    USS_OBJTYPE_X509          = 0x06U, /**< X.509 certificate */
    USS_OBJTYPE_COUNTER_DOWN  = 0x07U, /**< Monotonic counter decrementing */
    USS_OBJTYPE_UNKNOWN       = 0x80U  /**< Unknown object type */
} uss_objecttype_t;

#define uss_algo_id_t           mxq_algo_id_t

#define uss_object_prop_t       mxq_object_prop_t

#define USS_Module_Init(void) \
        MXQ_Module_Init(void)

#define USS_Module_Deinit(void) \
        MXQ_Module_Deinit(void)

#define USS_Module_SetInterface(i) \
        MXQ_Module_SetInterface(i)

#define USS_ResetFactory(void) \
        MXQ_ResetFactory(void)

#define USS_Reset(void) \
        MXQ_Reset(void)

#define USS_DisplayObjectDetails( pOption, pObjDetails, pDetailsLen) \
        MXQ_DisplayObjectDetails( pOption, pObjDetails, pDetailsLen)

#define USS_ListObject( option,  objectid,  pdest,  pmaxlen_readlen ) \
        MXQ_ListObject( option,  objectid,  pdest,  pmaxlen_readlen )

#define USS_CreateObject( objectid, objectlen, objtype, objprops, accessrules) \
        MXQ_CreateObject( objectid, objectlen, objtype, objprops, accessrules)

#define USS_DeleteObject( objectid)\
        MXQ_DeleteObject( objectid)

#define USS_WriteObject( objectid,  dest_offset,  psrc,  len)\
        MXQ_WriteObject( objectid,  dest_offset,  psrc,  len)

#define USS_WriteObjectV2(objectid,dest_offset,psrc,len,keyid,algoid,pIv,pTag)\
        MXQ_WriteObjectV2(objectid,dest_offset,psrc,len,keyid,algoid,pIv,pTag)


#define USS_UpdateCounter( objectid, countval)\
        MXQ_UpdateCounter( objectid, countval)

#define USS_ReadObject( objectid ,  src_offset,  pdest,  pmaxlen_readlen)\
        MXQ_ReadObject( objectid ,  src_offset,  pdest,  pmaxlen_readlen)

#define USS_ReadCounter( objectid,  countval)\
        MXQ_ReadCounter( objectid,  countval)

#define USS_Ping( l)\
        MXQ_Ping( l)

#define USS_Get_Challenge( p_buff_out,  len)\
        MXQ_Get_Challenge( p_buff_out,  len)


#define USS_Get_Random_Ext( p_buff_out,  len,  option)\
        MXQ_Get_Random_Ext( p_buff_out,  len,  option)

#ifdef MAXQ106x
#define USS_Get_Random(p,l) USS_Get_Challenge(p,l)
#else
#define USS_Get_Random(p,l) USS_Get_Random_Ext(p,l,0)
#endif

#define USS_Attestation( p_buff_out,  len,  option,  key_id,  alg_id,  objid)\
        MXQ_Attestation( p_buff_out,  len,  option,  key_id,  alg_id,  objid)

#define USS_GPIO_Ctrl( pSecGpioId,  pSecGpioMode, pPulseDuration,  pOutputCond, pSignAlgo, pKeyId, pSignature, pSignLen)\
        MXQ_GPIO_Ctrl( pSecGpioId,  pSecGpioMode, pPulseDuration,  pOutputCond, pSignAlgo, pKeyId, pSignature, pSignLen)

#define USS_GPIO_Read(pSecGpioId, pSignAlgo, pKeyId, output,  olen, pChallenge)\
        MXQ_GPIO_Read(pSecGpioId, pSignAlgo, pKeyId, output,  olen, pChallenge)

#define USS_EstablishKey_DH(option_flag, rsa_ssa_algo,\
	moduluslen, modulus, generator, message,\
	server_rand_len, server_rand_buf,\
	keyusage,\
	dst_Aes1_kid, dst1_key_len,\
	dst_Aes2_kid, dst2_key_len,\
	fixed_info_len, fixed_info,\
	sign_verif_key_id, signing_key_pair_id,\
	signature_len, signature,\
	out_buf, out_buf_len)\
        MXQ_EstablishKey_DH(option_flag, rsa_ssa_algo,\
	moduluslen, modulus, generator, message,\
	server_rand_len, server_rand_buf,\
	keyusage,\
	dst_Aes1_kid, dst1_key_len,\
	dst_Aes2_kid, dst2_key_len,\
	fixed_info_len, fixed_info,\
	sign_verif_key_id, signing_key_pair_id,\
	signature_len, signature,\
	out_buf, out_buf_len)

#define USS_EstablishKey(option_flag, ec_type, key_pair_id, pub_key_id, pub_key,\
        server_rand_len, server_rand_buf,\
        dst_Aes1_kid, dst1_key_len,\
        dst_Aes2_kid, dst2_key_len,\
        fixed_info_len, fixed_info,\
        sign_verif_key_id, signing_key_pair_id, signature,\
        out_buf, out_buf_len)\
        MXQ_EstablishKey(option_flag, ec_type, key_pair_id, pub_key_id, pub_key,\
        server_rand_len, server_rand_buf,\
        dst_Aes1_kid, dst1_key_len,\
        dst_Aes2_kid, dst2_key_len,\
        fixed_info_len, fixed_info,\
        sign_verif_key_id, signing_key_pair_id, signature,\
        out_buf, out_buf_len)
        

#define USS_Perform_Key_Exchange( key_Exchange_Session )\
        MXQ_Perform_Key_Exchange( key_Exchange_Session )

#define USS_TLS13_Generate_Key( key,  key_id,  key_type,  key_param_id,  key_size,  p,  g_size,  g)\
        MXQ_TLS13_Generate_Key( key,  key_id,  key_type,  key_param_id,  key_size,  p,  g_size,  g)

#define USS_TLS13_Create_Secret( peer_key,  peer_key_size,  key_id,  key_type,  key_param_id,  CSID,  shared_secret_id,  secret,  secret_size)\
        MXQ_TLS13_Create_Secret( peer_key,  peer_key_size,  key_id,  key_type,  key_param_id,  CSID,  shared_secret_id,  secret,  secret_size)

#define USS_TLS13_Update_IV( key_id,   iv,  iv_len)\
        MXQ_TLS13_Update_IV( key_id,   iv,  iv_len)

#define USS_TLS13_Extract_Secret( digest_id, ret_kid, prk, prk_len, salt_kid, salt,  salt_len, ikm_kid, ikm, ikm_len)\
        MXQ_TLS13_Extract_Secret( digest_id, ret_kid, prk, prk_len, salt_kid, salt,  salt_len, ikm_kid, ikm, ikm_len)
        
#define USS_TLS13_Expand_Secret( digest_id,  ret_kid,  ret_keytype,  ret_isiv,  okm,  okm_len,  prk_kid,   prk,  prk_len,   info,  info_len)\
        MXQ_TLS13_Expand_Secret( digest_id,  ret_kid,  ret_keytype,  ret_isiv,  okm,  okm_len,  prk_kid,   prk,  prk_len,   info,  info_len)

#define USS_Encrypt_PMS_with_RSA( data,  encrypted_pms,  data_len,  cert_id,  CSID)\
        MXQ_Encrypt_PMS_with_RSA( data,  encrypted_pms,  data_len,  cert_id,  CSID)

#define USS_Ecdh_Compute_Shared(curve_id, cserver_public_key, client_public_key, key_len, CSID)\
        MXQ_Ecdh_Compute_Shared(curve_id, cserver_public_key, client_public_key, key_len, CSID)

#define USS_SetECDHEKey(algoid,curveID,na,random,randomlen,ecdheparams,ecdheparamslen,signature,signaturelen)\
        MXQ_SetECDHEKey(algoid,curveID,na,random,randomlen,ecdheparams,ecdheparamslen,signature,signaturelen)

#define USS_tls_prf_sha_256(key_ID, label, llen, rand, rlen, destbuf, dlen)\
        MXQ_tls_prf_sha_256(key_ID, label, llen, rand, rlen, destbuf, dlen)

#define USS_tls_records(direction,key_ID,iv, iv_len, input, ilen, output, olen, hdr_msg, hdr_msg_len)\
        MXQ_tls_records(direction,key_ID,iv, iv_len, input, ilen, output, olen, hdr_msg, hdr_msg_len)

#define USS_cipher_auth_ccm(key_ID,\
	direction,\
	iv, iv_len,\
	ad, ad_len,\
	input,  ilen,\
	output, olen,\
	tag, tag_len)\
        MXQ_cipher_auth_ccm(key_ID\
	direction,\
	iv, iv_len,\
	ad, ad_len,\
	input,  ilen,\
	output, olen,\
	tag, tag_len)

#define USS_tls_cipher_cbc(direction,\
	key_ID,\
	iv, iv_len,\
	input, ilen,\
	output, olen,\
	hdr_msg, hdr_msg_len,\
	maclen)\
        MXQ_tls_cipher_cbc(direction,\
	key_ID,\
	iv, iv_len,\
	input, ilen,\
	output, olen,\
	hdr_msg, hdr_msg_len,\
	maclen)

#define USS_tls_cipher_aead(key_ID,\
	direction,\
	algo,\
	iv, iv_len,\
	aad, aad_len,\
	input, ilen,\
	output, olen,\
	tag, tag_len)\
        MXQ_tls_cipher_aead(key_ID,\
	direction,\
	algo,\
	iv, iv_len,\
	aad, aad_len,\
	input, ilen,\
	output, olen,\
	tag, tag_len)

#define  USS_client_key_exchange(remote_random,\
        srvcert,\
        client_ecdhe_key,\
        client_random,\
        client_signkey,\
        client_ecdsa_sign,\
        aeskey)\
        MXQ_client_key_exchange(remote_random,\
        srvcert,\
        client_ecdhe_key,\
        client_random,\
        client_signkey,\
        client_ecdsa_sign,\
        aeskey)
        
#define USS_client_decrypt( cleartext,   ciphertext,  ciphertext_len,   IV)\
        MXQ_client_decrypt( cleartext,   ciphertext,  ciphertext_len,   IV)


#define USS_client_encrypt( ciphertext,   cleartext,  cleartext_len,   IV)\
        MXQ_client_encrypt( ciphertext,   cleartext,  cleartext_len,   IV)

#define USS_client_key_exchange_psk(remote_ecdhe_pubkey,remote_random,client_ecdhe_key,client_random, curve_id,PSK_ID,aeskey)\
        MXQ_client_key_exchange_psk(remote_ecdhe_pubkey,remote_random,client_ecdhe_key,client_random, curve_id,PSK_ID,aeskey)

#define USS_Cipher_Init(_1encrypt_0decrypt,cipher_algo_id,key_object_id,param,doSwitchAesSpi)\
        MXQ_Cipher_Init(_1encrypt_0decrypt,cipher_algo_id,key_object_id,param,doSwitchAesSpi)

#define USS_Cipher_Update( pdest,   psrc,  len)\
        MXQ_Cipher_Update( pdest,   psrc,  len)

#define USS_Cipher_Finish( additional_data,  additional_data_len)\
        MXQ_Cipher_Finish( additional_data,  additional_data_len)

#define USS_MAC_Init(_1compute_0verify,mac_algo_id,key_object_id,IV,ivlen)\
        MXQ_MAC_Init(_1compute_0verify,mac_algo_id,key_object_id,IV,ivlen)

#define USS_MAC_Update( psrc,  inlen)\
        MXQ_MAC_Update( psrc,  inlen)

#define USS_MAC_Finish( pdest,  destlen)\
        MXQ_MAC_Finish( pdest,  destlen)

#define USS_MAC_Verify( pmac,  maclen)\
        MXQ_MAC_Verify( pmac,  maclen)

#define USS_MD_Init( md_algo_id)\
        MXQ_MD_Init( md_algo_id)

#define USS_MD_Update( psrc,  inlen)\
        MXQ_MD_Update( psrc,  inlen)

#define USS_MD_Finish( pdest,  destlen)\
        MXQ_MD_Finish( pdest,  destlen)


#define USS_Get_MD_OutputSize( md_algo_id)\
        MXQ_Get_MD_OutputSize( md_algo_id)

#define USS_get_curve_bytelen( curve_id)\
        MXQ_get_curve_bytelen( curve_id)

#define USS_get_sign_alg_from_curve( maxq_id)\
        MXQ_get_sign_alg_from_curve( maxq_id)

        
#define USS_Sign( signalgo,  privkey_objectid,   pdata,  datalen,  psignout,  signlen)\
        MXQ_Sign( signalgo,  privkey_objectid,   pdata,  datalen,  psignout,  signlen)

#define USS_Verify( signalgo,  pubkey_objectid,   pdata,  datalen,   signature,  signlen)\
        MXQ_Verify( signalgo,  pubkey_objectid,   pdata,  datalen,   signature,  signlen)

#define USS_BuildKey(pdest, destlen,\
	keytype,\
	keyparam,\
	keycomplen, total_keylen,\
	uu1, a1,\
	uu2, a2,\
	psrc )\
        MXQ_BuildKey(pdest, destlen,\
	keytype,\
	keyparam,\
	keycomplen, total_keylen,\
	uu1, a1,\
	uu2, a2,\
	psrc )

#define USS_BuildKey_Ex(pdest, destlen,\
	keytype,\
	keyparam,\
	keycomplen, total_keylen,\
	uu1, a1,\
	uu2, a2,\
	psrc, len )\
        MXQ_BuildKey_Ex(pdest, destlen,\
	keytype,\
	keyparam,\
	keycomplen, total_keylen,\
	uu1, a1,\
	uu2, a2,\
	psrc, len )

#define USS_BuildEK_Material(pdest, destlen, psrc, srclen, kek_id, pIV, pTag)\
        MXQ_BuildEK_Material(pdest, destlen, psrc, srclen, kek_id, pIV, pTag)
        
#define USS_Build_EC_Cert(pdest, destlen,\
	keytype,\
	keyparam,\
	keycomplen,  total_keylen,\
	pubkeyoffset,\
	tbs_start,\
	tbs_len,\
	total_certlen,\
	verifalgo,\
	verifkeyobjid,\
	uu1, a1,\
	uu2, a2,\
	pcertbody )\
        MXQ_Build_EC_Cert(pdest, destlen,\
	keytype,\
	keyparam,\
	keycomplen,  total_keylen,\
	pubkeyoffset,\
	tbs_start,\
	tbs_len,\
	total_certlen,\
	verifalgo,\
	verifkeyobjid,\
	uu1, a1,\
	uu2, a2,\
	pcertbody )

#define USS_ImportKeyCert(destobj_id,\
	verifalgo,\
	verifkey_id,\
	pdata, datalen,\
	sign, signlen,\
	cmd)\
        MXQ_ImportKeyCert(destobj_id,\
	verifalgo,\
	verifkey_id,\
	pdata, datalen,\
	sign, signlen,\
	cmd)
        
#define USS_ImportKey(destid,valg,vkid,data,datalen,sign,signlen)\
        MXQ_ImportKey(destid,valg,vkid,data,datalen,sign,signlen)

#define USS_ImportKeyEnc(destid,valg,vkid,data,datalen,sign,signlen)\
        MXQ_ImportKeyEnc(destid,valg,vkid,data,datalen,sign,signlen)

#define USS_ImportRootCert(destid,valg,vkid,data,datalen,sign,signlen)\
        MXQ_ImportRootCert(destid,valg,vkid,data,datalen,sign,signlen)

#define USS_ImportChildCert( destobj_id, pdata, datalen)\
        MXQ_ImportChildCert( destobj_id, pdata, datalen)

#define USS_AdminAuth(verifkey_id,algo,sign,signlen)\
        MXQ_AdminAuth(verifkey_id,algo,sign,signlen)

#define USS_AdminAuthReset(void)\
        MXQ_AdminAuthReset(void)

#define USS_VerifyBoot(verifkey_id,algo,sign,signlen)\
        MXQ_VerifyBoot(verifkey_id,algo,sign,signlen)

#define USS_VerifyBootReset(void)\
        MXQ_VerifyBootReset(void)

#if defined(MAXQ106x)
#define USS_OpenSecureChannel( channel_key_id, local_key)\
        MXQ_OpenSecureChannel( channel_key_id, local_key)

#define USS_OpenSecureChannel2( channel_key_id, local_key)\
        MXQ_OpenSecureChannel2( channel_key_id, local_key)
#elif defined(MAXQ1065)
#define USS_OpenSecureChannel( pMode, pSCKeyId, pPrivateKey, pHostPubId, pDevicePubKey, local_key)\
        MXQ_OpenSecureChannel( pMode, pSCKeyId, pPrivateKey, pHostPubId, pDevicePubKey, local_key)
#else
#define USS_OpenSecureChannel( pMode, pSCKeyId, pPrivateKey, local_key)\
        MXQ_OpenSecureChannel( pMode, pSCKeyId, pPrivateKey, local_key)
#endif
#define USS_CloseSecureChannel(void)\
        MXQ_CloseSecureChannel(void)

#define USS_SetConfig( s)\
        MXQ_SetConfig( s)
        
#define USS_SetState( newstate)\
        MXQ_SetState( newstate)

#define USS_GetStatus( status)\
        MXQ_GetStatus( status)

#if defined(MAXQ106x) || defined(MAXQ108x)
#define USS_Sleep(void)\
        MXQ_Sleep(void)

#define USS_Wakeup(void)\
        MXQ_Wakeup(void)
#endif

#if defined(MAXQ106x)
#define USS_GCU_Enable(void)\
        MXQ_GCU_Enable(void)

#define USS_GCU_Direct(void)\
        MXQ_GCU_Direct(void)
#endif

#if defined(MAXQ1065)    
#define USS_1065_Sleep( p_buff_out)\
        MXQ_1065_Sleep( p_buff_out)

#define USS_1065_Restore( p_restore_data, data_len)\
        MXQ_1065_Restore( p_restore_data, data_len)
#endif

#define USS_TLS_Enable(void)\
        MXQ_TLS_Enable(void)

#define USS_TLS_Direct(void)\
        MXQ_TLS_Direct(void)

#define USS_TLS_Restore(void)\
        MXQ_TLS_Restore(void)

#define USS_TLS_Save(void)\
        MXQ_TLS_Save(void)

#define USS_DisplayStatus( s)\
        MXQ_DisplayStatus( s)

#define USS_DisplayConfig( c)\
        MXQ_DisplayConfig( c)

#define USS_DisplaySysConfig( c)\
        MXQ_DisplaySysConfig( c)

//#ifdef MXQ_FW_UPDATE_CAP
#if defined(MAXQ108x)
#define USS_FW_Update( fw_img, fw_len, key_ID, signature, algo, keyparam)\
        MAXQ_FW_Update( fw_img, fw_len, key_ID, signature, algo, keyparam)
#elif defined(MAXQ1065)
#define USS_FW_Update( fw_img, fw_len)\
        MAXQ_FW_Update( fw_img, fw_len)
#endif
//#endif


#define USS_Tamper(cmd)\
        MXQ_Tamper(cmd)

#if defined(USS_ST)
#define USS_Bypass( in_out_buf_p, int_out_buf_len)\
        MXQ_Bypass( in_out_buf_p, int_out_buf_len)
#endif 

#define USS_get_str_code(error_code)\
        MXQ_get_str_code(error_code)
#endif
