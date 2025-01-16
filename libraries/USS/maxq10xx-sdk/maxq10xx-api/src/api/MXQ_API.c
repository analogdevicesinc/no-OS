/*******************************************************************************
* Copyright (C) 2018-2024 Maxim Integrated Products, Inc. (now owned by Analog
* Devices, Inc.), 2024 Analog Devices, Inc., All Rights Reserved. This software
* is proprietary and confidential to Analog Devices, Inc. and its licensors.
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
******************************************************************************/

#define _MXQ_API_C

#include <stdint.h>
#include "MXQ_Internal.h"
#include "MXQ_Config.h"
#include "MXQ_Types.h"
#include "MXQ_Error.h"
#include "MXQ_API.h"
#include "MXQ_CMD.h"
#include "MXQ_Transport.h"
#include "MXQ_Host.h"
#include "MXQ_Print.h"
#include "MXQ_GCU.h"

static gcu_context_t 	g_gcucontext;
mxq_mode_t 				g_selectedmode;

#if defined(PSA_ABSTRACTION)
extern void set_backup_tag_info(uint8_t* tag, uint8_t tagLen);
#endif
#define MAXQ_MAX_NONCE_SIZE 13u
#define MAXQ_MAX_TAG_LEN 16u

#if (MXQ_DUMP_DEBUG!=(-1))
void MXQ_PRINTBUFFER(const unsigned char * buffer,unsigned int len)
{
	if(!g_debugoutputlevel) return;

    unsigned int i;

    for(i=0;i<len;i++){
        MXQ_PRINTF("%02x ",buffer[i]);
    }
}
#else
void MXQ_PRINTBUFFER(const unsigned char * buffer,unsigned int len)
{
    (void) buffer;
    (void) len;
}
#endif

void put_u2(mxq_u2 u2val, mxq_u1* pdest, mxq_length* offset)
{
	pdest[*offset] = u2val >> 8;
	pdest[*offset + 1] = u2val;
	*offset += 2;
}


void put_u1(mxq_u1 u1val, mxq_u1* pdest, mxq_length* offset)
{
	pdest[*offset] = u1val;
	*offset += 1;
}


void put_buffer(mxq_u1* pdest, mxq_u1* psrc, mxq_u2 len, mxq_length* offset)
{
	while(len)
	{
		pdest[*offset] = *psrc;
		*offset = *offset + 1;
		psrc++;
		len--;
	}
}

void get_u4(mxq_u1* src, mxq_pu4 u4val)
{
	*u4val = (src[0] << 24);
	*u4val |= (src[1] << 16);
	*u4val |= (src[2] << 8);
	*u4val |= src[3];
}

mxq_u2 get_u2(mxq_u1* psrc, mxq_length* offset)
{
    mxq_u2 u2val = (psrc[*offset]<<8)|psrc[*offset+1];
    *offset+=2;
    return u2val;
}

mxq_u1 get_u1(mxq_u1* psrc, mxq_length* offset)
{
    mxq_u1 u1val=   psrc[*offset];
    *offset+=1;
    return u1val;
}

void get_buffer(mxq_u1* pdest, mxq_u1* psrc, mxq_u2 len, mxq_length* offset)
{
    while(len)
    {
        *pdest=psrc[*offset];
        *offset=*offset+1;
        pdest++;
        len--;
    }
}


mxq_algo_id_t __API__ MXQ_get_sign_alg_from_curve(mxq_keyparam_id_t curve_id)
{

	switch (curve_id){
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
	return ALGO_INVALID;
}

int __API__ MXQ_get_curve_bytelen(mxq_keyparam_id_t curve_id)
{

	switch (curve_id){
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


int __API__ MXQ_Get_MD_OutputSize(mxq_algo_id_t a)
{

	switch(a) {

		case ALGO_MD_SHA256: return 32;
		case ALGO_MD_SHA384: return 48;
		case ALGO_MD_SHA512: return 64;
		default:
			DBG_PRINT("MXQ_Get_MD_OutputSize: error unknown SHA algo '%d'\n", a);
			return -1;
	}

}


mxq_err_t __API__ MXQ_Module_Init(void)
{
	mxq_err_t ret;

#if defined(MAXQ1065)
	DBG_PRINT("MXQ>> Initializing communication with MAXQ1065\n");
#elif defined(MAXQ108x)
    DBG_PRINT("MXQ>> Initializing communication with MAXQ1080\n");
#else
    DBG_PRINT("MXQ>> Initializing communication with MAXQ1061\n");
#endif
	DBG_PRINT("timestamp: '%s'\n", __TIMESTAMP__);

	ret = init_interface();

	return ret;
}

mxq_err_t __API__ MXQ_Module_Deinit(void)
{
	mxq_err_t ret;
#if defined(MAXQ1065)
	DBG_PRINT("MXQ>> Deinitializing communication with MAXQ1065\n");
#elif defined(MAXQ108x)
    DBG_PRINT("MXQ>> Deinitializing communication with MAXQ1080\n");
#else
    DBG_PRINT("MXQ>> Deinitializing communication with MAXQ1061\n");
#endif
	DBG_PRINT("timestamp: '%s'\n", __TIMESTAMP__);

	ret = deinit_interface();
	return ret;
}

void* __API__ MXQ_Module_Interface(void) 
{
	return HOST_get_interface();
}

mxq_err_t __API__ MXQ_Module_SetInterface(void* i) 
{
	HOST_set_interface(i);
	return MXQ_OK;
}


/**
[xahs]=[rwdgx](,[xahs]=[rwdgx])*:[xahs]=[rwdgx](,[xahs]=[rwdgx])*:[xahs]=[rwdgx](,[xahs]=[rwdgx])* 
Delivery                      :Initialized                   :Operational 
- role:   a: admin, h:host, s:secureboot x:everyone
- access: r:read, w:write, d: delete, g: generate, x:execute

e.g. ah=r,s=w:a=d means 
Admin and Host can read and SecureBoot can write in Delivery state,
and admin can delete in Initialized state.
*/

static mxq_err_t __API__  MXQ_Parse_Access_Rules(mxq_u1 encoded[9], char* p) 
{
    
    memset(encoded,0,9);
   
    int state=0;
    int i = 0;
    char mask = 0 , mode = 0;

    while(p[i]) {
        if(state>=3) return MXQ_ERR_WRONG_PARAMETER;

        if (mode == 0) // user
        {
            switch(p[i++]) {
            case 'a': mask |= COND_ADMIN; DBG_PRINT("a"); break;
            case 'h': mask |= COND_HOST; DBG_PRINT("h"); break;
            case 's': mask |= COND_SECUREBOOT; DBG_PRINT("s");  break;
    #if !defined(MAXQ106x)
            case 'e': mask |= COND_ENDUSER; DBG_PRINT("e");  break;
    #endif
            case 'x': mask |= COND_NONE; DBG_PRINT("everyone"); break;
            case '=': mode = 1; DBG_PRINT("=");  break;
            default:
                DBG_PRINT("\nError: wrong access rules %s\n",p);
                return MXQ_ERR_WRONG_PARAMETER;
            }
        }
	    // Note: 'ahs' means COND_NONE;
        if (mode == 1) // access
        {

            switch(p[i++]) {
            case 'r': encoded[0+state*3] |= mask; DBG_PRINT("r");  break; // Read
            case 'w': encoded[0+state*3] |= (mask<<4); DBG_PRINT("w"); break; // Write/Import
            case 'd': encoded[1+state*3] |= mask; DBG_PRINT("d"); break; // Delete
            case 'g': encoded[1+state*3] |= (mask<<4); DBG_PRINT("g"); break; // Generate
            case 'x': encoded[2+state*3] |= mask; DBG_PRINT("x"); break; // eXecute
            case ',': mask = 0; mode = 0; DBG_PRINT(","); break;
            case ':': state++; mask = 0; mode = 0; DBG_PRINT(":"); break;
            default :
                DBG_PRINT("\nError: wrong access rules %s\n",p);
                return MXQ_ERR_WRONG_PARAMETER;
            }
        }
   }
   DBG_PRINT("\n");
   return MXQ_OK;

}


mxq_err_t __API__ MXQ_DisplayObjectDetails(mxq_u1 pOption, mxq_u1* pObjDetails, mxq_length pDetailsLen)
{
    mxq_length          lOffset     = 0;
    mxq_u2              lObjId;
    mxq_objecttype_t    lObjType;
    mxq_u1              lAccess[9];
    mxq_length          lObjLen;
    mxq_u2              lObjHdrExt;
    mxq_u1              lKeyType;
    mxq_u1              lKeyUsage;
    mxq_u1              lKeyAlgo;
    mxq_u4              lAlgoUsageCounter;
    mxq_u4              lObjCounter;
    mxq_err_t           lRet = MXQ_OK;

    /* Silence unused variable warnings when MXQ_PRINT is macro defined out */
    (void)lKeyAlgo;
    (void)lKeyType;
    (void)lKeyUsage;
    (void)lObjId;
    (void)lObjType;
    (void)lObjHdrExt;
    (void)lObjLen;

    switch ( pOption )
    {
        case 0:
            MXQ_PRINTF("\n\nDisplay objects details - Details Len : %2d...\n", pDetailsLen);

            lObjId      = get_u2( pObjDetails, &lOffset );
            lObjType    = (mxq_objecttype_t)get_u1( pObjDetails, &lOffset );
            lObjHdrExt  = get_u2( pObjDetails, &lOffset );
            get_buffer( (mxq_u1*)lAccess, pObjDetails, sizeof( lAccess ), &lOffset );
            lObjLen     = get_u2( pObjDetails, &lOffset );
            MXQ_PRINTF("\n\nDisplay objects details...\n");

            MXQ_PRINTF("\tlObjId\t\t: 0x%04x\n", lObjId);
            MXQ_PRINTF( "\tlObjType\t: 0x%04x\n", lObjType );
            MXQ_PRINTF("\tlObjHdrExt\t: 0x%04x\n", lObjHdrExt);
            MXQ_PRINTF("\tlObjLen\t\t: 0x%04x\n", lObjLen);
            switch( lObjType )
            {
                case MXQ_OBJTYPE_SECRETKEY:
                case MXQ_OBJTYPE_PUBKEY:
                case MXQ_OBJTYPE_KEYPAIR:
                case MXQ_OBJTYPE_X509:
                    lKeyType    = get_u1( pObjDetails, &lOffset );
                    MXQ_PRINTF("\tlKeyType\t: 0x%04x\n", lKeyType);
                    for( lAlgoUsageCounter = 0; lAlgoUsageCounter < 2/* CONFIG_NUM_USAGE_ALGO_PER_KEY */; lAlgoUsageCounter++ )
                    {
                        lKeyUsage   = get_u1( pObjDetails, &lOffset );
                        lKeyAlgo    = get_u1( pObjDetails, &lOffset );
                        MXQ_PRINTF("\tlKeyUsage[%lu]\t: 0x%04x\n", lAlgoUsageCounter, lKeyUsage);
                        MXQ_PRINTF("\tlKeyAlgo[%lu]\t: 0x%04x\n", lAlgoUsageCounter, lKeyAlgo);

                    }
                    break;

                default:
                    lRet = MXQ_ERR_INTERNAL_ERROR;
                    break;
            }
            break;

        case 1:
            MXQ_PRINTF("\n\nDisplay Object ID List\n");
            for( lObjCounter = 0; pDetailsLen; pDetailsLen -= sizeof( lObjId ), lObjCounter++ )
            {
                lObjId  = get_u2(pObjDetails, &lOffset);
                MXQ_PRINTF("\tOBJECT ID: 0x%04X\r\n", lObjId);
            }
            break;

        default:
            lRet = MXQ_ERR_INTERNAL_ERROR;
            break;
    }

    return lRet;
}

mxq_err_t __API__ MXQ_ListObject(mxq_u1 option, mxq_u2 objectid, mxq_u1* pdest, mxq_length* pmaxlen_readlen)
{
    mxq_err_t       lRet;
    cmd_response_t  lResponse;

    cmd_reset_offset();

    // Store parameters in command data buffer
    cmd_put_u1( option );
    if( option == 0 )
    {
        cmd_put_u2( objectid );
    }

    // Direct the response buffer into the destination
    lResponse.p_data       = pdest;
    lResponse.pdata_length = pmaxlen_readlen;

    lRet = CMD_SendCommand( CMDNUM_LIST_OBJECTS, &lResponse);
    if ( lRet == MXQ_OK )
    {
        lRet = (mxq_err_t)lResponse.status;
    }

    return lRet;
}

mxq_err_t __API__ MXQ_CreateObject(mxq_u2 objectid, mxq_length objectlen, mxq_objecttype_t objtype, mxq_object_prop_t objprops, char* accessrules)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> Create Object %04x, type=%02x, accessrules=%s\n",objectid, objtype, accessrules);

	cmd_reset_offset();
	mxq_u1 encoded[9];

	if(MXQ_OK!=MXQ_Parse_Access_Rules(encoded,accessrules)) return MXQ_ERR_WRONG_PARAMETER;

	cmd_put_u2(objectid);			/* Object ID */
	cmd_put_u1(objtype);			/* Object Type */
	cmd_put_u1(objprops);			/* Object Properties */
	cmd_put_buffer(encoded, 9);		/* Object Access Conditions */
	cmd_put_u2(objectlen);			/* Object Length */
	cmd_put_u4(0);				/* Unused */

	response.p_data       = NULL;
	response.pdata_length = NULL;

	ret = CMD_SendCommand(CMDNUM_CREATE_OBJECT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_CreateCounter(mxq_u2 objectid, mxq_objecttype_t objtype, mxq_object_prop_t objprops, char* accessrules,
								  mxq_u4 init_max_val)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> Create Counter %04x\n",objectid);

	cmd_reset_offset();

	mxq_u1 encoded[9];
	if(MXQ_OK!=MXQ_Parse_Access_Rules(encoded,accessrules)) return MXQ_ERR_WRONG_PARAMETER;

	cmd_put_u2(objectid);			/* Object ID */
	cmd_put_u1(objtype);			/* Object Type */
	cmd_put_u1(objprops);			/* Object Properties */
	cmd_put_buffer(encoded, 9);	/* Object Access Conditions */
	cmd_put_u2(0);				/* Unused */
	cmd_put_u4(init_max_val);		/* Initial or Maximal counter value */

	response.p_data       = NULL;
	response.pdata_length = NULL;

	ret = CMD_SendCommand(CMDNUM_CREATE_OBJECT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;;
}


mxq_err_t __API__ MXQ_ReadObject(mxq_u2 objectid, mxq_u2 src_offset, mxq_u1 *pdest, mxq_length *pmaxlen_readlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> Read Object %04x\n",objectid);

	cmd_reset_offset();
	cmd_put_u2(objectid);						/* Object ID */
	cmd_put_u2(src_offset);						/* Offset */
	cmd_put_u2(*pmaxlen_readlen);				/* Length to read */

	/*  Direct the response buffer into the destination */
	response.p_data = pdest;
	response.pdata_length = pmaxlen_readlen;

	ret = CMD_SendCommand(CMDNUM_READ_OBJECT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_ReadCertificate(mxq_u2 objectid, mxq_u1 *pdest, mxq_length *pmaxlen_readlen)
{
	mxq_err_t ret;

	ret = MXQ_ReadObject(objectid,0,pdest,pmaxlen_readlen);
	if(ret!=MXQ_OK) return ret;

	return ret;
}



mxq_err_t __API__ MXQ_WriteObject(mxq_u2 objectid, mxq_u2 dest_offset, mxq_u1 *psrc, mxq_u2 len)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Write Object %04x\n",objectid);

	cmd_reset_offset();
	cmd_put_u2(objectid);						/* Destination Object ID */
	cmd_put_u2(dest_offset);					/* Offset */
#if !defined(MAXQ106x)
    cmd_put_u2(len);					        /* Length */
#endif
	cmd_put_buffer(psrc,len);					/* Data */

	response.p_data = NULL; /*  Null as data in to say that our data is already in place */
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_WRITE_OBJECT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_WriteObjectV2(
                                    mxq_u2 objectid,
                                    mxq_u2 dest_offset,
                                    mxq_u1* psrc,
                                    mxq_u2 len,
                                    mxq_u2 keyid,
                                    mxq_algo_id_t algoid,
                                    mxq_u1* pIv,
                                    mxq_u1* pTag)
{
    cmd_response_t  lResponse;
    mxq_length      lAlgoIvLen  = 0;
    mxq_length      lAlgoTagLen = 0;
    mxq_length      UCL_AES_BLOCKSIZE = 16;
    mxq_err_t       lRet;
    mxq_length      lRespLen = 0;

    if ( g_selectedmode == MXQ_MODE_GCU )
    {
        return MXQ_ERR_MODE;
    }

    DBG_PRINT( "MXQ>> Write Object %04x\n", objectid );
    cmd_reset_offset();
    cmd_put_u2( objectid );         /* Destination Object ID */
    cmd_put_u2( dest_offset );      /* Offset */
    cmd_put_u2( len );
    cmd_put_buffer( psrc, len );    /* Data */
    if( algoid != ALGO_NONE )
    {
        cmd_put_u2( keyid );
        cmd_put_u1( algoid );
    }

    switch( algoid )
    {
        case ALGO_CIPHER_AES_GCM:
            lAlgoIvLen  = 12;
            lAlgoTagLen = UCL_AES_BLOCKSIZE;
            break;

        case ALGO_NONE:
            break;

        default:
            /* TODO: Return ERR */
            break;
    }

    if( lAlgoIvLen )
    {
        cmd_put_buffer( pIv, lAlgoIvLen );
    }
    if( lAlgoTagLen )
    {
        cmd_put_buffer( pTag, lAlgoTagLen );
    }

    lResponse.p_data        = NULL; /*  Null as data in to say that our data is already in place */
    lResponse.pdata_length  = &lRespLen;

    lRet = CMD_SendCommand( CMDNUM_WRITE_OBJECT, &lResponse );
    if ( lRet == MXQ_OK )
    {
        lRet = (mxq_err_t)lResponse.status;
    }

    return lRet;
}

mxq_err_t __API__ MXQ_UpdateCounter(mxq_u2 objectid, mxq_u4 *countval)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	mxq_err_t ret;
	cmd_response_t response;
	counter_t c;
	mxq_length resp_len = sizeof(c);

	DBG_PRINT("MXQ>> Update Counter %04x\n",objectid);

	cmd_reset_offset();
	cmd_put_u2(objectid);				/* Counter Object ID */

	response.p_data = (mxq_u1 *)&c;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_UPDATE_COUNTER, &response);
	if(ret != MXQ_OK) return ret;

	*countval = c.countval;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_ReadCounter(mxq_u2 objectid, mxq_u4 *countval)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	mxq_err_t ret;
	counter_t c;
	mxq_length len = sizeof(c);

	DBG_PRINT("MXQ>> Read Counter %04x\n",objectid);

	ret = MXQ_ReadObject(objectid, 0, (mxq_u1 *)&c, &len);
	if(ret != MXQ_OK) return ret;

	*countval = c.countval;

	return ret;
}


mxq_err_t __API__ MXQ_DeleteObject(mxq_u2 objectid)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> Delete Object %04x\n",objectid);

	cmd_reset_offset();
	cmd_put_u2(objectid);		/* Object ID */

	response.p_data = NULL;
	response.pdata_length = NULL;

	ret = CMD_SendCommand(CMDNUM_DELETE_OBJECT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_MD_Init(mxq_algo_id_t md_algo_id)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> MD Init\n");

	cmd_reset_offset();
	cmd_put_u1(PASS_NB_1);
	cmd_put_u1(md_algo_id);

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_COMPUTE_MESSAGE_DIGEST, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_MD_Update(const mxq_u1 *psrc, mxq_length inlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	mxq_length part_len = 0;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> MD Update\n");

	while(inlen) {
		if(inlen >= (MAX_COMM_BUFFER )) {
			part_len = MAX_COMM_BUFFER -1;
		} else {
			part_len = inlen;
		}


		cmd_reset_offset();
		cmd_put_u1(PASS_NB_2);
		cmd_put_buffer(psrc, part_len);

		resp_len = 0;
		response.p_data = NULL;
		response.pdata_length = &resp_len;

		ret = CMD_SendCommand(CMDNUM_COMPUTE_MESSAGE_DIGEST, &response);
		if(ret != MXQ_OK) return ret;

		if(response.status != MXQ_OK) return (mxq_err_t)response.status;

		inlen -= part_len;
		psrc += part_len;
	}

	return MXQ_OK;
}


/*  if destlen < maclen, truncate */
/*  if >, destlen adjusted to actual len */
mxq_err_t __API__ MXQ_MD_Finish(mxq_u1 *pdest, mxq_length *destlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> MD Finish\n");

	cmd_reset_offset();

	cmd_put_u1(PASS_NB_3);

	response.p_data = pdest;
	response.pdata_length = destlen;

	ret = CMD_SendCommand(CMDNUM_COMPUTE_MESSAGE_DIGEST, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_MAC_Init(mxq_u1 _1compute_0verify, mxq_algo_id_t mac_algo_id, mxq_u2 key_object_id, mxq_u1 *IV,
							 mxq_length ivlen)
{

	mxq_err_t ret;
	cmd_response_t response;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> MAC Init %04x\n", key_object_id);

	cmd_reset_offset();

	cmd_put_u1(PASS_NB_1);
	cmd_put_u1(_1compute_0verify);
	cmd_put_u1(mac_algo_id);
	cmd_put_u2(key_object_id);
	cmd_put_u2(ivlen);
	cmd_put_buffer(IV, ivlen);

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_COMPUTE_MAC, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_MAC_Update(mxq_u1 *psrc, mxq_length inlen)
{
	cmd_response_t response;
	mxq_err_t ret;

	mxq_length resp_len = 0;
	mxq_length part_len = 0;

	DBG_PRINT("MXQ>> MAC Update\n");

	while(inlen) {
		if(inlen >= (MAX_COMM_BUFFER )) {
			part_len = MAX_COMM_BUFFER - 1;
		} else {
			part_len = inlen;
		}

		cmd_reset_offset();
		cmd_put_u1(PASS_NB_2);
		cmd_put_buffer(psrc, part_len);

		resp_len = 0;
		response.p_data = NULL;
		response.pdata_length = &resp_len;

		ret = CMD_SendCommand(CMDNUM_COMPUTE_MAC, &response);
		if(ret != MXQ_OK) return ret;

		if(response.status != MXQ_OK) return (mxq_err_t)response.status;

		inlen -= part_len;
		psrc += part_len;
	}

	return MXQ_OK;
}


/*  if destlen < maclen, truncate */
/*  if >, destlen adjusted to actual len */
mxq_err_t __API__ MXQ_MAC_Finish(mxq_u1 *pdest, mxq_length *destlen)
{
	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> MAC Finish (Compute)\n");

	cmd_reset_offset();
	cmd_put_u1(PASS_NB_3);

	response.p_data = pdest;
	response.pdata_length = destlen;

	ret = CMD_SendCommand(CMDNUM_COMPUTE_MAC, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_MAC_Verify(mxq_u1 *pmac, mxq_length maclen)
{
	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> MAC Finish (Verify)\n");

	cmd_reset_offset();
	cmd_put_u1(PASS_NB_3);
	cmd_put_buffer(pmac, maclen);

	response.p_data = NULL;
	response.pdata_length = 0;

	ret = CMD_SendCommand(CMDNUM_COMPUTE_MAC, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_Cipher_Init(mxq_u1 _1encrypt_0decrypt, mxq_algo_id_t cipher_algo_id, mxq_u2 key_object_id,
								ciph_params_t *param, int useAESSPI)
{
	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;
	DBG_PRINT("MXQ>> Cipher Init with key %04x\n", key_object_id);

    while(1)
    {
        switch(g_selectedmode) {

            case MXQ_MODE_TLS:

                cmd_reset_offset();
                cmd_put_u1(PASS_NB_1);
                cmd_put_u1(_1encrypt_0decrypt ? 1 : 2);
                cmd_put_u1(useAESSPI?ALGO_CIPHER_AES_GCU:cipher_algo_id);
                cmd_put_u2(key_object_id);

                if(param != NULL) {
                    if(cipher_algo_id==ALGO_CIPHER_AES_CCM) {
                        cmd_put_u2( +4
                                    +13 //param->iv_length
                                    +1
                                    +(_1encrypt_0decrypt ? 0 : MAXQ_MAX_TAG_LEN /*param->aead_tag_len*/)
                                    +2 + param->aad_length); // length of params
                        cmd_put_u1(param->aead_tag_len);    // M
                        cmd_put_u1(15-param->iv_length);   // L length of message length field
                        cmd_put_u1(param->data_length);    // Length of payload
                        cmd_put_u1(param->data_length>>8); // Length of payload
                        cmd_put_buffer(param->p_iv, param->iv_length); // nonce
                        cmd_put_buffer(param->p_iv, MAXQ_MAX_NONCE_SIZE - param->iv_length); // unused nonce bytes up to 13
                        cmd_put_u1(0); // Unused
                        if(_1encrypt_0decrypt==0) cmd_put_buffer(param->aead_tag, MAXQ_MAX_TAG_LEN /*param->aead_tag_len*/);
                        cmd_put_u1(param->aad_length);
                        cmd_put_u1(param->aad_length>>8);
                        cmd_put_buffer(param->p_aad, param->aad_length);
                    }
                    else if(cipher_algo_id==ALGO_CIPHER_AES_GCM) {
                        cmd_put_u2( +5
                                    +(_1encrypt_0decrypt ? 0 : MAXQ_MAX_TAG_LEN)
                                    + param->iv_length
                                    + param->aad_length); // length of params
                        cmd_put_u1(param->iv_length);
                        cmd_put_u1(param->iv_length>>8);
                        cmd_put_u1(param->aad_length);
                        cmd_put_u1(param->aad_length>>8);
                        cmd_put_u1(param->aead_tag_len);
                        if(_1encrypt_0decrypt==0) cmd_put_buffer(param->aead_tag, MAXQ_MAX_TAG_LEN); // TAG (decryption only)
                        cmd_put_buffer(param->p_iv, param->iv_length); // IV
                        cmd_put_buffer(param->p_aad, param->aad_length); // AAD
                    }
                    else if(param->iv_length) {
                        cmd_put_u2(param->iv_length);
                        cmd_put_buffer(param->p_iv, param->iv_length);
                    }
                    else {
                        if(param->ecies_cipher_algo==ALGO_CIPHER_AES_ECB) {
                            if(_1encrypt_0decrypt){
                                cmd_put_u2(5);
                                cmd_put_u1(param->ecies_cipher_algo);
                                cmd_put_u1(param->ecies_keylen);
                                cmd_put_u1(param->ecies_hmac_algo);
                                cmd_put_u1(0);
                                cmd_put_u1(0);
                            }
                            else {
                                cmd_put_u2(5+param->Rlen);
                                cmd_put_u1(param->ecies_cipher_algo);
                                cmd_put_u1(param->ecies_keylen);
                                cmd_put_u1(param->ecies_hmac_algo);
                                cmd_put_u1(0);
                                cmd_put_u1(param->Rlen);
                                cmd_put_buffer(param->R,param->Rlen);

                            }
                        }
                        else {
                            cmd_put_u2(0); /* No Algorithm parameters */
                        }
                    }

                } else {
                    cmd_put_u2(0); /* No Algorithm parameters */
                }


                if(param==NULL) {
                    response.p_data       = NULL;
                    response.pdata_length = &resp_len;
                }
                else {
                    response.p_data       = param->R;
                    response.pdata_length = &(param->Rlen);
                }
    #if defined(MAXQ106x)
                if(useAESSPI) {
                    MXQ_GCU_Direct(); // prepare switching to AES-SPI mode
                }
    #endif
                ret = CMD_SendCommand(CMDNUM_ENCRYPT_DECRYPT, &response);
                if(ret != MXQ_OK) return ret;

                if(useAESSPI) {
                    DBG_PRINT("Hot switching to AES-SPI mode\n");
                    g_selectedmode = MXQ_MODE_GCU; // Now we are in AES-SPI mode!
                    useAESSPI=0;
                    continue;
                }
                return (mxq_err_t)response.status;
                break;
    #if defined(MAXQ106x)
            case MXQ_MODE_GCU:

                g_gcucontext.algo       = cipher_algo_id;
                g_gcucontext.encdec = _1encrypt_0decrypt;
                mxq_length part_len = 0;

                if((param->p_iv) && (param->iv_length != 0)) {
                    ret = GCU_LoadIV(param->p_iv);
                    if(ret != MXQ_OK) return ret;
                }

                if((param->p_key) && (param->key_length != 0)) {
                    ret = GCU_LoadKey(param->p_key);
                    if(ret != MXQ_OK) return ret;
                }

                if(cipher_algo_id == ALGO_CIPHER_AES_GCM) {

                    g_gcucontext.auth = 1;

                    if((param->p_aad) &&(param->aad_length != 0)) {

                        g_gcucontext.initial = 0;

                        ret = GCU_GCMAuthEncDec(_1encrypt_0decrypt, param->aad_length, param->data_length, param->p_aad);
                        if(ret != MXQ_OK) return ret;

                        if(param->aad_length > MXQ_GCU_BLOCK_SIZE) {

                            param->aad_length -= MXQ_GCU_BLOCK_SIZE;

                            while(param->aad_length) {
                                if(param->aad_length > MXQ_GCU_BLOCK_SIZE) {
                                    part_len = MXQ_GCU_BLOCK_SIZE;
                                } else {
                                    part_len = param->aad_length;
                                }

                                ret = GCU_SendBlock(param->p_aad + MXQ_GCU_BLOCK_SIZE);
                                if(ret != MXQ_OK) return ret;

                                param->aad_length -= part_len;
                                param->p_aad += part_len;
                            }
                        }
                    } else {
                        g_gcucontext.initial = 1;
                        g_gcucontext.auth = 0;
                        g_gcucontext.pdata_length = param->data_length;
                    }
                }

                return MXQ_OK;
                break;
    #endif
            default:
                return MXQ_ERR_INTERNAL_ERROR;
        }
    }
}


mxq_err_t __API__ MXQ_Cipher_Update(mxq_u1 **pdest, const mxq_u1 *psrc, mxq_length *in_outlen)
{
	cmd_response_t response;
	mxq_length     resp_len;
	mxq_err_t      ret;
	mxq_length     part_len = 0;
	mxq_length     out_len  = 0;
	mxq_length     in_len   = *in_outlen;

	DBG_PRINT("MXQ>> Cipher Update %d\n",in_len);

	switch(g_selectedmode) {

		case MXQ_MODE_TLS:
			while(in_len) {

				if(in_len > (MAX_COMM_BUFFER -16 )) {
					part_len = MAX_COMM_BUFFER -16;
				} else {
					part_len = in_len;
				}

				cmd_reset_offset();
				cmd_put_u1(PASS_NB_2);
				cmd_put_buffer(psrc, part_len);

				resp_len              = part_len + 16; /* Expected return length is at least the input data plus a block size */
				response.p_data       = *pdest;
				response.pdata_length = &resp_len;

				ret = CMD_SendCommand(CMDNUM_ENCRYPT_DECRYPT, &response);
				if(ret != MXQ_OK) return ret;

				if(response.status != MXQ_OK) return (mxq_err_t)response.status;

				in_len -= part_len;
				out_len += resp_len;
				*pdest += *response.pdata_length;
				psrc += part_len;

				*in_outlen = out_len;
			}
			break;

		case MXQ_MODE_GCU:
			while(in_len) {

				if(in_len > MXQ_GCU_BLOCK_SIZE) {
					part_len = MXQ_GCU_BLOCK_SIZE;
				} else {
					part_len = in_len;
				}

				if(g_gcucontext.algo == ALGO_CIPHER_AES_ECB) {
					GCU_ECBEncDec(g_gcucontext.encdec, psrc);
				} else {

					if(g_gcucontext.initial) {
						g_gcucontext.initial = 0;
						GCU_GCMEncDec(g_gcucontext.pdata_length, psrc);
					} else {
						GCU_SendBlock(psrc);
					}
				}

				GCU_RequestBlock(*pdest);
				in_len -= part_len;
				*pdest += MXQ_GCU_BLOCK_SIZE;
				psrc += part_len;
			}
			break;

		default:
			return MXQ_ERR_INTERNAL_ERROR;
	}

	return MXQ_OK;
}

mxq_err_t __API__ MXQ_Cipher_Finish(mxq_u1 **tag, mxq_length *tag_len)
{
	cmd_response_t response;
	mxq_err_t ret;
	mxq_length reslen = MAX_AAD_CCM_LENGTH;

	DBG_PRINT("MXQ>> Cipher Finish\n");

	switch(g_selectedmode) {

		case MXQ_MODE_TLS:

			cmd_reset_offset();
			cmd_put_u1(PASS_NB_3);
			
			if((tag == NULL) ||(*tag == NULL)) {
				response.p_data = NULL;
				response.pdata_length = NULL;
			} else {
				if(*tag[0]==0xDE) {
					cmd_put_buffer(*tag+1, *tag_len);
					#if defined(PSA_ABSTRACTION)
					set_backup_tag_info(*tag+1, *tag_len);
					#endif
				}
				response.p_data = *tag;
				response.pdata_length = &reslen;
			}


			ret = CMD_SendCommand(CMDNUM_ENCRYPT_DECRYPT, &response);
			if(ret != MXQ_OK) return ret;

			if(tag_len != NULL) *tag_len = reslen;

			if(tag != NULL) *tag += reslen;

			return (mxq_err_t)response.status;

		case MXQ_MODE_GCU:

			if(g_gcucontext.algo == ALGO_CIPHER_AES_GCM && g_gcucontext.auth) {
				if(tag_len != NULL) *tag_len = 16;
				if(tag != NULL) GCU_RequestTag(*tag);
			}

			break;

		default:
			return MXQ_ERR_INTERNAL_ERROR;
	}

	return MXQ_OK;
}


mxq_err_t __API__ MXQ_Verify(mxq_algo_id_t signalgo, mxq_u2 pubkey_objectid, const mxq_u1 *pdata, mxq_u2 datalen,
						   const mxq_u1 *signature, mxq_u2 signlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> Verify Signature with key %04x\n",pubkey_objectid);

	cmd_reset_offset();
	cmd_put_u1(PASS_VERIFY_SIGN);	/*  1: verify */
	cmd_put_u1(signalgo);			/*  AA */
	cmd_put_u2(pubkey_objectid); 	/*  KID */
	cmd_put_u2(0);					/*  Unused */

	if(pdata == NULL) {
		cmd_put_u2(0); /*  LLLL */
	} else {
		/*  Length of data / Data */
		cmd_put_u2(datalen); /*  LLLL */
		cmd_put_buffer(pdata, datalen);
	}

	/*  Length of signature to verify */
	cmd_put_u2(signlen); /*  LLLL */
	cmd_put_buffer(signature, signlen);

	response.p_data = NULL;
	response.pdata_length = NULL;

	ret = CMD_SendCommand(CMDNUM_COMPUTE_VERIFY_DIGITAL_SIGNATURE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_Sign(mxq_algo_id_t signalgo, mxq_u2 privkey_objectid, const mxq_u1 *pdata, mxq_u2 datalen, mxq_u1* psignout,
						 mxq_length *signlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> Sign with key %04x\n",privkey_objectid);

	cmd_reset_offset();
	cmd_put_u1(PASS_COMPUTE_SIGN);		/*  2: sign */
	cmd_put_u1(signalgo);				/*  AA */
	cmd_put_u2(privkey_objectid);		/*  KID */
	cmd_put_u2(0);						/*  Unused */

	if(pdata == NULL) /* If data previously hashed using COMPUTE MESSAGE DIGEST */
	{
		cmd_put_u2(0); /*  LLLL */
	} else {
		/*  Length of data / Data */
		cmd_put_u2(datalen); /*  LLLL */
		cmd_put_buffer(pdata, datalen);
	}

	/*  Length of signature/signature to verify (does not apply, so put 0000) */
	cmd_put_u2(0); /*  LLLL */

	response.p_data = psignout;
	response.pdata_length = signlen;

	ret = CMD_SendCommand(CMDNUM_COMPUTE_VERIFY_DIGITAL_SIGNATURE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_BuildKey(mxq_u1 *pdest, mxq_length *destlen, mxq_keytype_id_t keytype,
							 mxq_keyparam_id_t keyparam, /* e.g. curve ID */
							 mxq_length keycomplen, mxq_length total_keylen, mxq_keyuse_t uu1, mxq_algo_id_t a1,
							 mxq_keyuse_t uu2, mxq_algo_id_t a2, mxq_u1 *psrc)
{
	return MXQ_BuildKey_Ex(pdest, destlen, keytype, keyparam, keycomplen, total_keylen, uu1, a1, uu2, a2, psrc, total_keylen);
}

mxq_err_t __API__ MXQ_BuildKey_Ex(mxq_u1 *pdest, mxq_length *destlen, mxq_keytype_id_t keytype,
							 mxq_keyparam_id_t keyparam, /* e.g. curve ID */
							 mxq_length keycomplen, mxq_length total_keylen, mxq_keyuse_t uu1, mxq_algo_id_t a1,
							 mxq_keyuse_t uu2, mxq_algo_id_t a2, mxq_u1 *psrc, mxq_length material_len)
{

	mxq_length offset = 0;

	put_u1(keytype,  pdest, &offset);			/*  TT */
	put_u1(keyparam, pdest, &offset);			/*  PP */
	put_u2_lsb(keycomplen, pdest, &offset);		/*  Total key len */
	put_u2_lsb(total_keylen, pdest, &offset);	/*  Total key len */
	put_u1(uu1,      pdest, &offset);			/*  UU1 */
	put_u1(a1,       pdest, &offset);			/*  Algo1 */
	put_u1(uu2,      pdest, &offset);			/*  UU2 */
	put_u1(a2,       pdest, &offset);			/*  Algo2 */

	if(psrc != NULL) {
		put_buffer(pdest, psrc, material_len, &offset);
	} else {
		if(total_keylen != 0) return MXQ_ERR_BAD_LENGTH;
	}

	*destlen = offset;

	return MXQ_OK;
}


mxq_err_t __API__ MXQ_BuildEK_Material(mxq_u1 *pdest, mxq_length *destlen, mxq_u1 *psrc, mxq_length srclen, 
 									   mxq_u2 kek_id, mxq_u1 *pIV, mxq_u1 *pTag)

{
	mxq_length offset = 0;

	put_u2(srclen,  pdest, &offset);			
	put_buffer(pdest, psrc, srclen, &offset);
	put_u2(kek_id,   pdest, &offset);			
	put_u1(ALGO_CIPHER_AES_GCM, pdest, &offset);			
	put_buffer(pdest, pIV, 12, &offset);
	put_buffer(pdest, pTag, 16, &offset);

	*destlen = offset;

	return MXQ_OK;
}


mxq_err_t __API__ MXQ_Build_EC_Cert(mxq_u1 *pdest, mxq_length *destlen, mxq_keytype_id_t keytype,
								  mxq_keyparam_id_t keyparam, /* e.g. curve ID */
								  mxq_length keycomplen, mxq_length total_keylen, mxq_u2 pubkeyoffset,
								  mxq_length tbs_start, mxq_length tbs_len, mxq_length total_certlen,
								  mxq_algo_id_t verifalgo, mxq_u2 verifkeyobjid, mxq_keyuse_t uu1,
								  mxq_algo_id_t a1, mxq_keyuse_t uu2, mxq_algo_id_t a2, mxq_u1 *pcertbody)
{

	DBG_PRINT("Building child EC Certificate import request\n");
	DBG_PRINT("t=%d,p=%d,l=%d,pko=%d,ss=%d,sl=%d,tl=%d,vo=%04x,va=%d\n",
		keytype,keyparam,keycomplen,pubkeyoffset,tbs_start,tbs_len,total_certlen,verifkeyobjid,verifalgo);

	mxq_length offset = 0;

	put_u1(keytype, pdest, &offset);  			/*  TT - Imported Key type*/
	put_u1(keyparam, pdest, &offset);			/*  PP - Key parameter*/
	put_u2_lsb(keycomplen, pdest, &offset);		/*  LLLL - Public key Component Length */
	put_u2_lsb(total_keylen, pdest, &offset);	/*  LLLL - Total public key len */
#if defined(MAXQ1065) || defined(MAXQ108x)
	put_u1(uu1, pdest, &offset);				/*  UU1 - Imported certificate Usage 1 */
	put_u1(a1, pdest, &offset);					/*  AA1 - Algorithm for Usage 1 */
	put_u1(uu2, pdest, &offset);				/*  UU2 - Imported certificate Usage 2 */
	put_u1(a2, pdest, &offset);					/*  AA2 - Algorithm for Usage 2 */
#endif
	put_u2_lsb(pubkeyoffset, pdest, &offset);	/*  LLLL - Public key offset in certificate */
	put_u2_lsb(tbs_start, pdest, &offset);		/*  LLLL - To-be-signed start offset */
	put_u2_lsb(tbs_len, pdest, &offset);		/*  LLLL - To-be-signed length */
	put_u2_lsb(total_certlen, pdest, &offset);	/*  LLLL - Total certificate length */
	put_u2_lsb(verifkeyobjid, pdest, &offset);	/*  PID - Parent Certificate Object ID */
	put_u2_lsb(0x00, pdest, &offset);			/*  Unused */
	put_u1(verifalgo, pdest, &offset);			/*  AA - Algorithm for certificate signature verification */
	put_u1(0x00, pdest, &offset);				/*  Unused */
#ifdef MAXQ106x
	put_u1(uu1, pdest, &offset);				/*  UU1 - Imported certificate Usage 1 */
	put_u1(a1, pdest, &offset);					/*  AA1 - Algorithm for Usage 1 */
	put_u1(uu2, pdest, &offset);				/*  UU2 - Imported certificate Usage 2 */
	put_u1(a2, pdest, &offset);					/*  AA2 - Algorithm for Usage 2 */
#endif

	put_buffer(pdest, pcertbody, total_certlen, &offset); /* x.509 DER certificate material */

	*destlen = offset;

	return MXQ_OK;
}


mxq_err_t __API__ MXQ_ImportKeyCert(mxq_u2 destobj_id, mxq_algo_id_t verifalgo, mxq_u2 verifkey_id, mxq_u1 *pdata,
								  mxq_length datalen, mxq_u1 *sign, mxq_length signlen, cmd_num_t cmd)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Import key or root certificate %04x\n",destobj_id);

	cmd_reset_offset();
	if(cmd==CMDNUM_IMPORT_KEY_ENC)
	{ 
		cmd_put_u1(PASS_KEY_IMPORT_ENC);		/*  Import mode */
		cmd=CMDNUM_IMPORT_KEY;
	}
	else
		cmd_put_u1(PASS_KEY_IMPORT_GEN);		/*  Import mode */
	cmd_put_u1(verifalgo);					/*  AA */
	cmd_put_u2(verifkey_id);				/*  VKID */

	cmd_put_u2(destobj_id);					/*  VKID */
	cmd_put_u2(datalen);					/*  LLLL */
	cmd_put_buffer(pdata, datalen);			/*  Key material */

	/*  Signature */
	cmd_put_u2(signlen); /*  LLLL */
	if(sign != NULL)	 /*  Signature (certificates don't need an extra signature if they are not root...) */
		cmd_put_buffer(sign, signlen); /*  Signature of key material (Data) */


	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(cmd, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_ImportChildCert(mxq_u2 destobj_id, mxq_u1 *pdata, mxq_length datalen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Import child certificate %04x\n",destobj_id);

	cmd_reset_offset();

	cmd_put_u2(destobj_id);			/*  VKID */
	cmd_put_u2(datalen);			/*  LLLL */
	cmd_put_buffer(pdata, datalen);	/*  Key material */


	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_IMPORT_CHILDCERT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

#if defined(MAXQ106x)
mxq_err_t __API__ MXQ_AdminAuth(mxq_u2 verifkey_id, mxq_algo_id_t algo, mxq_u1 *sign, mxq_length signlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Admin authenticate with key %04x\n",verifkey_id);

	cmd_reset_offset();
	cmd_put_u1(PASS_OPEN_ADMIN);	/*  1: Open Administrator Channel */
	cmd_put_u1(algo);				/*  AA */
	cmd_put_u2(verifkey_id);		/*  VKID */
	cmd_put_u2(0);					/*  Unused */
	cmd_put_u2(0);					/*  Unused */

	cmd_put_u2(signlen); /*  LLLL */
	cmd_put_buffer(sign, signlen);

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_ADMIN_AUTHENTICATE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}
#else
mxq_err_t __API__ MXQ_AdminAuth(mxq_u2 verifkey_id, mxq_algo_id_t algo, /*mxq_u1 pSecChannel,*/mxq_u1 *sign, mxq_length signlen /*, mxq_u2 pDeviceScKeyId, mxq_u2 pHostScKeyId*/)
{
	mxq_u1 pSecChannel = 0; // Todo add Secure Channel
	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;


	DBG_PRINT("MXQ>> Admin authenticate with key %04x\n",verifkey_id);

	cmd_reset_offset();
	cmd_put_u1(PASS_OPEN_ADMIN);	/*  1: Open Administrator Channel */
	cmd_put_u1(algo);				/*  AA */
	cmd_put_u2(verifkey_id);		/*  VKID */
	cmd_put_u1(pSecChannel);        /*  Secure channel */
	cmd_put_u1(0);					/*  Unused */
/* 	if(pSecChannel)
	{
	#if defined(MAXQ1065)
		mxq_u2 pDeviceScKeyId =0;
		mxq_u2 pHostScKeyId = 0;
		cmd_put_u2(pDeviceScKeyId);
        cmd_put_u2(pHostScKeyId);
	#else
		cmd_put_u2(0x0040);         //  ECDHE Public Key Length
		//cmd_put_buffer(gScEcdheKeyPair, 0x0040);
	#endif
	}
	else
	{ */
		cmd_put_u2(0);   			// LLLL
	#if defined(MAXQ1065) // Keep for MAXQ1080
		cmd_put_u2(0);   			// LLLL
	#endif
	//}

	cmd_put_u2(signlen); /*  LLLL */
	cmd_put_buffer(sign, signlen);

	/*if ( (pSecChannel) && (gSecChannelMode != eSCEcdheMode1065) )
		{
			gSecChannelMode = eSCEcdhe;
			resp_len = KEYCOMPLEN256*2;
			response.p_data       = KeyPairObj;
		}
	else
		response.p_data       = NULL;
	response.pdata_length = &resp_len;*/

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_ADMIN_AUTHENTICATE, &response);
	if(ret != MXQ_OK) return ret;

/* 	if(pSecChannel)
	{
		ret = CMD_InitSecureChannel(NULL, KeyPairObj,CMDNUM_ADMIN_AUTHENTICATE);
		if(ret != 0) return ret;
	} */

	return (mxq_err_t)response.status;
}
#endif

mxq_err_t __API__ MXQ_AdminAuthReset(void)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Cancel Admin authenticate\n");

	cmd_reset_offset();
	cmd_put_u1(PASS_CLOSE_ADMIN); /*  1: Close Administrator Channel */

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_ADMIN_AUTHENTICATE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

#ifndef NO_SECURE_CHANNEL
#if defined(MAXQ106x)
mxq_err_t __API__ MXQ_OpenSecureChannel(mxq_u2 verifkey_id, mxq_u1 *local_key)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret = MXQ_ERR_INTERNAL_ERROR;
	mxq_u1 rnd[CHALLENGE_SIZE];
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Open Secure Channel (mode 1) with key %04x\n",verifkey_id);

	ret = CMD_GetScChallengeWithMode(eSCInit, rnd, sizeof(rnd));
	if(ret != MXQ_OK) return ret;

	cmd_reset_offset();
	cmd_put_u1(0x01);		/*  1 : Open Secure Channel mode 1 */
	cmd_put_u2(verifkey_id);			/*  KID */

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_MANAGE_SECURE_CHANNEL, &response);
	if(ret != MXQ_OK) return ret;

	if(response.status != MXQ_OK) return (mxq_err_t)response.status;

	ret = CMD_InitSecureChannel(rnd, local_key, 1);

	return ret;
}


mxq_err_t __API__ MXQ_OpenSecureChannel2(mxq_u2 verifkey_id, mxq_u1 *local_key)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_u1 rnd[CHALLENGE_SIZE+16];
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Open Secure Channel (mode 2) with key %04x\n",verifkey_id);

	ret = CMD_GetScChallengeWithMode(eSCInit, rnd, sizeof(rnd));
	if(ret != MXQ_OK) return ret;

	cmd_reset_offset();
	cmd_put_u1(0x02);		/*  1 : Open Secure Channel mode 2*/
	cmd_put_u2(verifkey_id);			/*  KID */

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_MANAGE_SECURE_CHANNEL, &response);
	if(ret != MXQ_OK) return ret;

	if(response.status != MXQ_OK) return (mxq_err_t)response.status;

	ret = CMD_InitSecureChannel(rnd, local_key, 2);

	return ret;
}


mxq_err_t __API__ MXQ_CloseSecureChannel(void)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Close secure channel\n");

	cmd_reset_offset();
	cmd_put_u1(PASS_CLOSE_CHANNEL); /* 3: Close Channel */

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_MANAGE_SECURE_CHANNEL, &response);
	if(ret != MXQ_OK) return ret;

	CMD_CloseSecureChannel(CMDNUM_MANAGE_SECURE_CHANNEL);

	return (mxq_err_t)response.status;
}
#elif defined(MAXQ1065)
mxq_err_t __API__ MXQ_OpenSecureChannel(mxq_u1 pMode, mxq_u2 pSCKeyId, mxq_pu1 pPrivateKey, mxq_u2 pHostPubId, mxq_pu1 pDevicePubKey, mxq_u1* local_key)
{
	mxq_err_t  	ret;
    //mxq_u1 lSignature[64];

	CMD_GetScChallenge(pMode);
    cmd_reset_offset();
	if((pMode == eSCAesGcmInit) || (pMode == eSCPreSharedKeyMode))
	{
		cmd_put_u1(pMode); // 1
		cmd_put_u2(pSCKeyId); // KID
	}
	else if (pMode == eSCEcdheMode1065)
	{
	    cmd_put_u1(pMode);      // Mode
	    cmd_put_u2(pSCKeyId);   // MAXQ Private KID
	    cmd_put_u2(pHostPubId); // Host Public Key or Certificate ID

        CMD_PrepareEcdheMode1065Params(pPrivateKey, pDevicePubKey);
	}


	{
		mxq_u1 KeyPairObj[KEYCOMPLEN256*2];
		cmd_response_t response;
		mxq_length	 resp_len = 0;
		response.status       = MXQ_RSP_ERROR_UNKNOWN;
		if ( (pMode == eSCEcdhe)  || (pMode == eSCAuthEcdhe) )
		{
			resp_len = KEYCOMPLEN256*2;
			response.p_data       = KeyPairObj;
		}
		else
		{
			response.p_data       = NULL;
		}
		response.pdata_length = &resp_len;
		ret = CMD_SendCommand( CMDNUM_MANAGE_SECURE_CHANNEL, &response);
		if(ret != 0) return ret;
		ret = CMD_InitSecureChannel(local_key, KeyPairObj,CMDNUM_MANAGE_SECURE_CHANNEL);
		if(ret != 0) return ret;
		return (mxq_err_t)response.status;
	}

}

mxq_err_t __API__ MXQ_CloseSecureChannel(void)
{
	mxq_err_t  	ret;

    cmd_reset_offset();
	cmd_put_u1(PASS_CLOSE_CHANNEL);

    cmd_response_t response;
    mxq_length	 resp_len = 0;
    response.status       = MXQ_RSP_ERROR_UNKNOWN;
    response.p_data       = NULL;
    response.pdata_length = &resp_len;

    ret = CMD_SendCommand( CMDNUM_MANAGE_SECURE_CHANNEL, &response);
    if(ret != 0) return ret;

    CMD_CloseSecureChannel(CMDNUM_MANAGE_SECURE_CHANNEL);

    return (mxq_err_t)response.status;
}

#else

mxq_err_t __API__ MXQ_OpenSecureChannel(mxq_u1 pMode, mxq_u2 pSCKeyId, mxq_pu1 pPrivateKey, mxq_u1* local_key)
{
	mxq_err_t  	ret;
    mxq_u1 lSignature[KEYCOMPLEN521*2];

	CMD_GetScChallenge(pMode);
    cmd_reset_offset();
    
    cmd_put_u1(pMode); // 1
	cmd_put_u2(pSCKeyId); // KID
    if((pMode == eSCEcdhe)||(pMode == eSCAuthEcdhe))
	{
        mxq_u2  lKeyLen = KEYCOMPLEN256; // TODO: support for other curves
		mxq_u1 KeyPairObj[KEYCOMPLEN256*2];
        cmd_put_u1(MXQ_KEYPARAM_EC_P256R1);
		CMD_PrepareEcdheKeyPair(KeyPairObj);
        cmd_put_u2((2*lKeyLen));
		cmd_put_buffer(KeyPairObj, (2*lKeyLen));
		if(pMode == eSCAuthEcdhe)
		{
            mxq_u2  lSignLen = KEYCOMPLEN256; // TODO: support for other curves
			CMD_SignSCParams(lSignature, pPrivateKey);
			cmd_put_buffer(lSignature, (2*lSignLen));
		}
	}


	{
		mxq_u1 KeyPairObj[KEYCOMPLEN256*2];
		cmd_response_t response;
		mxq_length	 resp_len = 0;
		response.status       = MXQ_RSP_ERROR_UNKNOWN;
		if ( (pMode == eSCEcdhe)  || (pMode == eSCAuthEcdhe) )
		{
			resp_len = KEYCOMPLEN256*2;
			response.p_data       = KeyPairObj;
		}
		else
		{
			response.p_data       = NULL;
		}
		response.pdata_length = &resp_len;
		ret = CMD_SendCommand( CMDNUM_MANAGE_SECURE_CHANNEL, &response);
		if(ret != 0) return ret;
		ret = CMD_InitSecureChannel(local_key, KeyPairObj,CMDNUM_MANAGE_SECURE_CHANNEL);
		if(ret != 0) return ret;
		return (mxq_err_t)response.status;
	}

}

mxq_err_t __API__ MXQ_CloseSecureChannel(void)
{
	mxq_err_t  	ret;

    cmd_reset_offset();
	cmd_put_u1(PASS_CLOSE_CHANNEL);

    cmd_response_t response;
    mxq_length	 resp_len = 0;
    response.status       = MXQ_RSP_ERROR_UNKNOWN;
    response.p_data       = NULL;
    response.pdata_length = &resp_len;

    ret = CMD_SendCommand( CMDNUM_MANAGE_SECURE_CHANNEL, &response);
    if(ret != 0) return ret;

    CMD_CloseSecureChannel(CMDNUM_MANAGE_SECURE_CHANNEL);

    return (mxq_err_t)response.status;
}
#endif
#endif /* NO_SECURE_CHANNEL */

mxq_err_t __API__ MXQ_VerifyBoot(mxq_u2 verifkey_id, mxq_algo_id_t algo, mxq_u1 *sign, mxq_length signlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Verify Boot with key %04x\n",verifkey_id);

	cmd_reset_offset();
	cmd_put_u1(PASS_ENABLE_SECUREBOOT);		/* 1: verify Boot */
	cmd_put_u1(algo);						/*  AA */
	cmd_put_u2(verifkey_id);				/*  VKID */
	cmd_put_u2(0);							/*  Unused */
	cmd_put_u2(0);							/*  Unused */

	cmd_put_u2(signlen);					/*  LLLL */
	cmd_put_buffer(sign, signlen);

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_VERIFY_BOOT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_VerifyBootReset()
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Cancel secure boot\n");

	cmd_reset_offset();
	cmd_put_u1(PASS_CANCEL_SECUREBOOT); /* 3: Cancel SecureBoot */

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_VERIFY_BOOT, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_GetStatus(mxq_status_t *s)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_status_raw_t rs;
	mxq_err_t ret;


	mxq_length len = sizeof(mxq_status_t);
	response.p_data = (mxq_u1*)&rs;
	response.pdata_length = &len;

	DBG_PRINT("MXQ>> Get Status\n");

	cmd_reset_offset();

	ret = CMD_SendCommand(CMDNUM_GETSTATUS, &response);
	if(ret != MXQ_OK) return ret;

	memcpy(s->product_name,rs.product_name,MXQ_MAXQ106x_CHIP_NAME_LENGTH);
	memcpy(s->serial_num, rs.serial_num,MXQ_MAXQ106x_USN_LENGTH);
	s->v_fw_maj = rs.v_fw_maj;
	s->v_fw_min = rs.v_fw_min;
	s->v_fw_rev = rs.v_fw_rev;			
	s->v_hw_cod = rs.v_hw_cod;
#if defined(MAXQ106x)
	s->freemem  = rs.freemem.L + (rs.freemem.H<<8);
	s->totalmem = rs.totalmem.L + (rs.totalmem.H<<8);
#else
    s->freemem  = rs.freemem.mem_byte0 + (rs.freemem.mem_byte1<<8) + (rs.freemem.mem_byte2<<16) + (rs.freemem.mem_byte3<<24);
	s->totalmem = rs.totalmem.mem_byte0 + (rs.totalmem.mem_byte1<<8) + (rs.totalmem.mem_byte2<<16) + (rs.totalmem.mem_byte3<<24);
#endif
	s->lifecycle_state = rs.lifecycle_state;
	memcpy(&s->config, &rs.config, sizeof(rs.config));
	memcpy(&s->sysconfig, &rs.sysconfig, sizeof(rs.sysconfig));


	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_SetState(mxq_state_t s)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Set new life cycle state: %02x\n",s);

	cmd_reset_offset();
	cmd_put_u1(s);

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_SET_LIFE_CYCLE_STATE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_SetConfig(mxq_config_t *s)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> Set config\n");

	cmd_reset_offset();
	cmd_put_buffer((mxq_u1 *)s, sizeof(mxq_config_t));

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_SETCONFIG, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_ResetFactory(void)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length len = 0;

	response.p_data = NULL;
	response.pdata_length = &len;

	DBG_PRINT("MXQ>> Factory Reset\n");

	cmd_reset_offset();

	ret = CMD_SendCommand(CMDNUM_RESET_FACTORY, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}
#ifdef LCOV_REPORT
mxq_err_t __API__ MXQ_Lcov(void)
{
	cmd_response_t response;
	mxq_err_t ret;
	mxq_length len = 0;

	DBG_PRINT("MXQ>> LCOV\n");


	cmd_reset_offset();

	response.p_data = NULL;
	response.pdata_length = &len;

	switch(g_selectedmode) {

		case MXQ_MODE_TLS:
			ret = CMD_SendCommand(CMDNUM_LCOV, &response);
			return MXQ_OK;
			break;

		default:
			return MXQ_ERR_INTERNAL_ERROR;
	}

	return ret;
}
#endif
mxq_err_t __API__ MXQ_Ping(mxq_length length)
{
	cmd_response_t response;
	mxq_err_t ret;

	static mxq_u1 cmdbuf[CONFIG_MAX_PING_SIZE];
	static mxq_u1 respbuf[CONFIG_MAX_PING_SIZE];
	mxq_u4 i;

	DBG_PRINT("MXQ>> Ping\n");

	if(length > CONFIG_MAX_PING_SIZE) return MXQ_ERR_WRONG_PARAMETER;

	cmd_reset_offset();
	response.p_data = respbuf;
	response.pdata_length = &length;

	for(i = 0; i < length; i++) {
		cmdbuf[i] = i;
		cmd_put_u1(i);
	}

	switch(g_selectedmode) {

		case MXQ_MODE_TLS:
			ret = CMD_SendCommand(CMDNUM_PING, &response);
			if(ret != MXQ_OK) return ret;

			if(MXQ_MEMCMP(cmdbuf, respbuf, length) == 0) return MXQ_OK;

			ret=MXQ_ERR_TRANS_ERROR;

			break;

		case MXQ_MODE_GCU:
			ret = GCU_LinkTest();
			return ret;
			break;

		default:
			return MXQ_ERR_INTERNAL_ERROR;
	}

	return ret;
}


mxq_err_t __API__ MXQ_Wakeup(void) 
{
#ifdef MAXQ108x
	DBG_PRINT("MXQ>> Wakeup\n");

	cmd_reset_offset();
	mxq_err_t err = CMD_SendRawCommand(CMDNUM_PING); // random command
    (void) err;

	DBG_PRINT("MXQ>> %d\n", err);
	return MXQ_OK;
#else
	return HOST_wakeup_maxq106x();
#endif
}

mxq_err_t __API__ MXQ_Reset(void) 
{
	return HOST_reset_maxq106x();
}

mxq_err_t __API__ MXQ_Sleep(void)
{
	cmd_response_t response;
	mxq_err_t ret;
	mxq_length len = 0;

	DBG_PRINT("MXQ>> Sleep\n");

#ifdef MAXQ108x
	cmd_reset_offset();

	response.p_data = NULL;
	response.pdata_length = &len;

	ret = CMD_SendCommand(CMDNUM_SLEEP, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
#else
	switch(g_selectedmode) {

		case MXQ_MODE_TLS:
			cmd_reset_offset();

			response.p_data = NULL;
			response.pdata_length = &len;

			ret = CMD_SendCommand(CMDNUM_SLEEP, &response);
			if(ret != MXQ_OK) return ret;

			return (mxq_err_t)response.status;
			break;

		case MXQ_MODE_GCU:
			ret = GCU_Shutdown();
			return ret;
			break;

		default:
			return MXQ_ERR_INTERNAL_ERROR;
	}

	return ret;
#endif
}

mxq_err_t __API__ MXQ_Attestation(mxq_u1* p_buff_out, mxq_length len, mxq_u1 option, mxq_u2 key_id, mxq_algo_id_t alg_id, mxq_u2 objid)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_u1 challenge[16];

	memset(challenge,0xcd,sizeof(challenge));

	DBG_PRINT("MXQ>> Attestation\n");

	cmd_reset_offset();
	cmd_put_u1(alg_id);
	cmd_put_u2(key_id);
	cmd_put_u1(option);
	cmd_put_u2(objid);
	cmd_put_buffer(challenge,sizeof(challenge));

	/* set information for reception */
	response.p_data = p_buff_out;
	response.pdata_length = &len;

	ret = CMD_SendCommand(CMDNUM_ATTESTATION, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_EstablishKey(
		mxq_u1 option_flag, mxq_keyparam_id_t ec_type, mxq_u2 key_pair_id, mxq_u2 pub_key_id, mxq_u1* pub_key,
		mxq_u1 server_rand_len, mxq_pu1 server_rand_buf,
		mxq_u2 dst_Aes1_kid, mxq_u1 dst1_key_len,
		mxq_u2 dst_Aes2_kid, mxq_u1 dst2_key_len,
		mxq_u1 fixed_info_len, mxq_pu1 fixed_info,
		mxq_u2 sign_verif_key_id, mxq_u2 signing_key_pair_id, mxq_u1* signature,
		mxq_pu1 out_buf, mxq_length* out_buf_len)
{
    cmd_response_t response;
	mxq_err_t	ret;
	mxq_u1	keyLen    = 0;

	switch ( ec_type )
	{
		case MXQ_KEYPARAM_EC_P256R1:
		case MXQ_KEYPARAM_EC_BP256R1:
			keyLen = KEYCOMPLEN256;
			break;

		case MXQ_KEYPARAM_EC_P384R1:
		case MXQ_KEYPARAM_EC_BP384R1:
			keyLen = KEYCOMPLEN384;
			break;

		case MXQ_KEYPARAM_EC_P521R1:
			keyLen = KEYCOMPLEN521;
			break;
		case MXQ_KEYPARAM_EC_BP512R1:
			keyLen = 64;
			break;
		default:
			return MXQ_ERR_WRONG_PARAMETER;
	}


	cmd_reset_offset();
    cmd_put_u1(1);              // Mode
	cmd_put_u1(option_flag);    // Option Flag
	cmd_put_u1(ec_type);        // Eliptic Curve ID
	cmd_put_u2(key_pair_id);    // ECDH Local Key pair id
	cmd_put_u2(pub_key_id);      // Server ECDH Pubkey ID
	if(pub_key_id == 0xffff)
	{
		cmd_put_buffer(pub_key,2*keyLen);   // Explicit pub key value
	}
    cmd_put_u1(server_rand_len);      // 1-byte Server Random length
    cmd_put_buffer(server_rand_buf,server_rand_len);   // Server random value

	cmd_put_u2(dst_Aes1_kid);      // Derived AES Key 1 Dest ID
	cmd_put_u1(dst1_key_len);      // Derived AES Key 1 Length
	cmd_put_u2(dst_Aes2_kid);      // Derived AES Key 2 Dest ID
	cmd_put_u1(dst2_key_len);      // Derived AES Key 2 Length
	cmd_put_u1(fixed_info_len);      // Fixed info len
	cmd_put_buffer(fixed_info,fixed_info_len);   // Fixed info
	if(option_flag&IS_AUTH_ENABLED)
	{
		cmd_put_u2(sign_verif_key_id); //Signature Verification key pair ID
		cmd_put_u2(signing_key_pair_id);     // Signing key pair ID
		cmd_put_buffer(signature,2*KEYCOMPLEN256);   // Signature

	}



    response.p_data       = out_buf;
    response.pdata_length = out_buf_len;
    ret = CMD_SendCommand( CMDNUM_ESTABLISH_KEY, &response);
    if(ret != 0) return ret;


    return (mxq_err_t)response.status;

}

#if defined(MAXQ108x)
mxq_err_t __API__ MXQ_EstablishKey_DH(
		mxq_u1 option_flag, mxq_algo_id_t rsa_ssa_algo,
		mxq_u2 modulus_len, mxq_u1* modulus, mxq_u1* generator, mxq_u1* message,
		mxq_u1 server_rand_len, mxq_pu1 server_rand_buf,
		mxq_u1 key_usage,
		mxq_u2 dst_Aes1_kid, mxq_u1 dst1_key_len,
		mxq_u2 dst_Aes2_kid, mxq_u1 dst2_key_len,
		mxq_u1 fixed_info_len, mxq_u1* fixed_info,
		mxq_u2 sign_verif_key_id, mxq_u2 signing_key_pair_id,
		mxq_u2 signature_len, mxq_u1* signature,
		mxq_pu1 out_buf, mxq_length* out_buf_len)

{
	cmd_response_t response;
	mxq_err_t	ret;

	cmd_reset_offset();
	cmd_put_u1(1);        // Mode
	cmd_put_u1(option_flag);             // flag
	cmd_put_u1(rsa_ssa_algo);     // RSA algo
	cmd_put_u2(modulus_len);
	cmd_put_buffer(modulus,modulus_len);
	cmd_put_buffer(generator,modulus_len);
	cmd_put_buffer(message,modulus_len);

	cmd_put_u1(server_rand_len);      // random Len
	cmd_put_buffer(server_rand_buf,server_rand_len);   // Random dat
	cmd_put_u1(key_usage);      // Key usage
	cmd_put_u2(dst_Aes1_kid);      // AES Key id
	cmd_put_u1(dst1_key_len);      // AES Key LEn
	cmd_put_u2(dst_Aes2_kid);      // AES Key id
	cmd_put_u1(dst2_key_len);      // AES Key LEn
	cmd_put_u1(fixed_info_len);      // random Len
	cmd_put_buffer(fixed_info,fixed_info_len);   // Random data
	if(option_flag & IS_AUTH_ENABLED)
	{
		cmd_put_u1(0);
		cmd_put_u2(sign_verif_key_id);
		cmd_put_u2(signing_key_pair_id);
		cmd_put_buffer(signature,signature_len);   // Signature

	}

	response.p_data       = out_buf;
	response.pdata_length = out_buf_len;
	ret = CMD_SendCommand( CMDNUM_ESTABLISH_KEY_DH, &response);
	if(ret != 0) return ret;


	return (mxq_err_t)response.status;
}
#endif

mxq_err_t __API__ MXQ_Get_Challenge(mxq_u1* p_buff_out, mxq_length len)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
#ifdef MAXQ106x
    DBG_PRINT("MXQ>> Get Random\n");
#else
    DBG_PRINT("MXQ>> Get Challenge\n");
#endif

	cmd_reset_offset();
	cmd_put_u2(len);

	/* set information for reception */
	response.p_data = p_buff_out;
	response.pdata_length = &len;

	ret = CMD_SendCommand(CMDNUM_GET_CHALLENGE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

#ifdef MAXQ108x
mxq_err_t __API__ MXQ_GPIO_Ctrl(mxq_u1 pSecGpioId, mxq_u1 pSecGpioMode,
								mxq_u2 pPulseDuration, mxq_u2 pOutputCond,
								mxq_algo_id_t pSignAlgo, mxq_u2 pKeyId,
								mxq_u1* pSignature, mxq_u2 pSignLen)
{
	mxq_err_t ret;
	cmd_response_t response;
	mxq_length len = 0;

	DBG_PRINT("MXQ>> Secure GPIO Control\n");

	cmd_reset_offset();

	cmd_put_u1(1);                // FN   : (01)
	cmd_put_u1(pSignAlgo);        // AA   : signature algorithm
	cmd_put_u2(pKeyId);           // VKID : key id for verification
	cmd_put_u2(0);                // 0000 : unused
	cmd_put_u2(6);                // LLLL : length of gpio data (0006)

	cmd_put_u1(pSecGpioId);       // gpio id
	cmd_put_u1(pSecGpioMode);     // mode
	cmd_put_u2(pPulseDuration);   // pulse duration
	cmd_put_u2(pOutputCond);      // output condition

	cmd_put_u2(pSignLen);         // LLLL : length of signature

	cmd_put_buffer(pSignature, pSignLen); // signature

	response.p_data       = NULL;
	response.pdata_length = &len;
	ret = CMD_SendCommand(CMDNUM_GPIO_CONTROL, &response);
	if (ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_GPIO_Read(mxq_u1 pSecGpioId, mxq_algo_id_t pSignAlgo, 
								mxq_u2 pKeyId, mxq_u1* output, mxq_length* olen,
								mxq_u1* pChallenge)
{
	mxq_err_t ret;
	cmd_response_t response;

	DBG_PRINT("MXQ>> Secure GPIO Read\n");

	cmd_reset_offset();

	cmd_put_u1(pSignAlgo);          // AA   : signature algorithm
	cmd_put_u2(pKeyId);             // VKID : key id for verification
	cmd_put_u1(pSecGpioId);         // gpio id
	cmd_put_buffer(pChallenge, 16); // 16-byte random number

	response.p_data       = output;
	response.pdata_length = olen;
	ret = CMD_SendCommand(CMDNUM_GPIO_READ, &response);
	if (ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}
#endif

#if defined(MAXQ1065)
mxq_err_t __API__ MXQ_1065_Sleep(mxq_pu1 p_buff_out)
{
    mxq_err_t ret;
    cmd_response_t response;
    mxq_length len = 2048;


    response.p_data       = p_buff_out;
    response.pdata_length = &len;

    ret=CMD_SendCommand(CMDNUM_SLEEP, &response);
    if(ret!=MXQ_OK) return ret;
    HOST_sleep_maxq10xx();
    return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_1065_Restore(mxq_pu1 p_restore_data, mxq_length data_len)
{
    mxq_err_t      ret;
    cmd_response_t response;
    mxq_length   len = 0;
    HOST_wakeup_maxq106x();
    HOST_wait_usec(200000);
    if (data_len != MAX_COMM_BUFFER) {
        return MXQ_RSP_WRONG_SIZE;
    }

    cmd_put_buffer((mxq_u1*)p_restore_data, data_len);

    response.p_data       = NULL;
    response.pdata_length = &len;

    ret=CMD_SendCommand(CMDNUM_RESTORE,  &response);
    if(ret!=MXQ_OK) return ret;

    return (mxq_err_t)response.status;
}
#endif

mxq_err_t __API__ MXQ_Get_Random_Ext(mxq_u1* p_buff_out, mxq_length len, mxq_u1 option)
{
#ifndef MAXQ1065
    (void) option;
#endif

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	cmd_reset_offset();
#ifdef MAXQ1065
    DBG_PRINT("MXQ>> Get Random Ext, Option=%d\n", option);
	if (option) cmd_put_u1(option-1); // MAXQ looks at length of command to decide if option byte is here or not
#else
    DBG_PRINT("MXQ>> Get Random Ext\n");
#endif
	cmd_put_u2(len);

	/* set information for reception */
	response.p_data = p_buff_out;
	response.pdata_length = &len;

	ret = CMD_SendCommand(CMDNUM_GET_RANDOM, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_Perform_Key_Exchange(mxq_secret_context_api_t *configuration)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = sizeof(configuration->PSK_info.psk_key_bloc);

	DBG_PRINT("MXQ>> MXQ_Perform_Key_Exchange\n");

	/* object creation */
	cmd_reset_offset();

	cmd_put_u1(configuration->pass);					/* Pass */
	cmd_put_u2(configuration->CSID);					/* CSID */
	cmd_put_u2(configuration->Random_size);				/* Random */
	cmd_put_buffer(configuration->Random,configuration->Random_size);

	cmd_put_u2(configuration->PSK_info.psk_id);			/* PreShared Key ID */
	cmd_put_u2(configuration->is_session_key_secret);	/* keep session key secret */
	//cmd_put_u2(1);	/* keep session key secret */

	response.pdata_length = &resp_len;
	response.p_data = configuration->PSK_info.psk_key_bloc;

	ret = CMD_SendCommand(CMDNUM_KEY_EXCHANGE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

#ifdef MAXQ108x
mxq_err_t __API__ MXQ_TLS13_Generate_Key(mxq_u1* key, mxq_u2 key_id, mxq_keytype_id_t key_type, mxq_keyparam_id_t key_param_id, mxq_u2 key_size, mxq_u1* p, mxq_u2 g_size, mxq_u1* g)
{
    if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

    cmd_response_t response;
    mxq_err_t ret;
    mxq_length data_len;

    DBG_PRINT("MXQ>> MXQ_TLS13_Generate_Key\n");

    cmd_reset_offset();
    cmd_put_u1(GEN_KEY);		/* Pass */
    cmd_put_u2(key_id);		    /* Key  ID */
    cmd_put_u1(key_type);		/* Key Type */
    cmd_put_u1(key_param_id);	/* Key Param ID */
    cmd_put_u2(key_size);		/* KEy Size */
    if(key_param_id == MXQ_KEYPARAM_DHE) {
        cmd_put_buffer(p, key_size);
        cmd_put_u2(g_size);		/* KEy Size */
        cmd_put_buffer(g, g_size);
    }

    data_len = 2048;
    response.pdata_length = &data_len;
    response.p_data = key;

    ret = CMD_SendCommand(CMDNUM_TLS13_KEY_EXCHANGE, &response);
    if(ret != MXQ_OK) return ret;

    return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_TLS13_Create_Secret(mxq_u1* peer_key, mxq_u2 peer_key_size, mxq_u2 key_id, mxq_keytype_id_t key_type, mxq_keyparam_id_t key_param_id, mxq_u2 CSID, mxq_u2 shared_secret_id, mxq_u1* secret, mxq_length *secret_size)
{
    if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

    cmd_response_t response;
    mxq_err_t ret;

    DBG_PRINT("MXQ>> MXQ_TLS13_Create_Secret\n");

    cmd_reset_offset();
    cmd_put_u1(CREATE_SECRET);  /* Pass */
    cmd_put_u2(key_id);		    /* Key  ID */
    cmd_put_u1(key_type);		/* Key Type */
    cmd_put_u1(key_param_id);	/* Key Param ID */
    cmd_put_u2(CSID);	        /* Ciphersuite ID*/
    cmd_put_u2(shared_secret_id);	/* shared secret object id */
    cmd_put_u2(peer_key_size);	/* Key Size */
    cmd_put_buffer(peer_key, peer_key_size);

    *secret_size = 2048;
    response.pdata_length = secret_size;
    response.p_data = secret;

    ret = CMD_SendCommand(CMDNUM_TLS13_KEY_EXCHANGE, &response);
    if(ret != MXQ_OK) return ret;

    return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_TLS13_Extract_Secret(mxq_algo_id_t digest_id, mxq_u2 ret_kid, mxq_u1* prk, mxq_length* prk_len, mxq_u2 salt_kid, const mxq_u1* salt, mxq_u2 salt_len, mxq_u2 ikm_kid, const mxq_u1* ikm, mxq_u2 ikm_len)
{
    if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

    cmd_response_t response;
    mxq_err_t ret;

    DBG_PRINT("MXQ>> MXQ_TLS13_Extract_Secret\n");

    cmd_reset_offset();
    cmd_put_u1(EXTRACT_SECRET);       /* Pass */
    cmd_put_u1(digest_id);            /* Digest */
    cmd_put_u2(ret_kid);              /* Result Key ID */
    cmd_put_u2(salt_kid);             /* Salt Key ID */
    cmd_put_u2(salt_len);             /* Length of Salt */
    cmd_put_buffer(salt, salt_len);   /* Salt */
    cmd_put_u2(ikm_kid);              /* IKM Key ID */
    cmd_put_u2(ikm_len);              /* Length of Input Keying Material */
    cmd_put_buffer(ikm, ikm_len);     /* Input Keying Material */

    response.pdata_length = prk_len;
    response.p_data = prk;

    ret = CMD_SendCommand(CMDNUM_TLS13_KEY_EXCHANGE, &response);
    if(ret != MXQ_OK) return ret;

    return (mxq_err_t)response.status;
}
mxq_err_t __API__ MXQ_TLS13_Update_IV(mxq_u2 key_id, const mxq_u1* iv, mxq_u2 iv_len)
{
    mxq_length resp_len = 0;
    if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

    cmd_response_t response;
    mxq_err_t ret;

    DBG_PRINT("MXQ>> MXQ_TLS13_Update_IV\n");

    cmd_reset_offset();
    cmd_put_u1(UPDATE_IV);       /* Pass */
    cmd_put_u2(key_id);         /* Key ID */
    cmd_put_u2(iv_len);         /* IV Length */
    cmd_put_buffer(iv, iv_len);  /* IV */


	response.pdata_length = &resp_len;
    response.p_data = NULL;

    ret = CMD_SendCommand(CMDNUM_TLS13_KEY_EXCHANGE, &response);
    if(ret != MXQ_OK) return ret;

    return (mxq_err_t)response.status;
}

mxq_err_t __API__ MXQ_TLS13_Expand_Secret(mxq_algo_id_t digest_id, mxq_u2 ret_kid, mxq_keytype_id_t ret_keytype, mxq_u1 ret_isiv, mxq_u1* okm, mxq_length* okm_len, mxq_u2 prk_kid, const mxq_u1* prk, mxq_u2 prk_len, const mxq_u1* info, mxq_u2 info_len)
{
    if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

    cmd_response_t response;
    mxq_err_t ret;

    DBG_PRINT("MXQ>> MXQ_TLS13_Expand_Secret\n");

    cmd_reset_offset();
    cmd_put_u1(EXPAND_SECRET);       /* Pass */
    cmd_put_u1(digest_id);           /* Digest */
    cmd_put_u2(ret_kid);             /* Result Key ID */
    cmd_put_u1(ret_keytype);         /* Result Key Type */
    cmd_put_u1(ret_isiv);            /* Result Is IV Flag */
    cmd_put_u2(prk_kid);             /* PRK Key ID */
    cmd_put_u2(prk_len);             /* Length of PRK */
    cmd_put_buffer(prk, prk_len);    /* PRK */
    cmd_put_u2(info_len);            /* Length of Information */
    cmd_put_buffer(info, info_len);  /* Information */
    cmd_put_u2(*okm_len);            /* Length of Output Keying Material */

    response.pdata_length = okm_len;
    response.p_data = okm;

    ret = CMD_SendCommand(CMDNUM_TLS13_KEY_EXCHANGE, &response);
    if(ret != MXQ_OK) return ret;

    return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_Encrypt_PMS_with_RSA(mxq_u1* data,
										mxq_u1* encrypted_pms, mxq_length* data_len, mxq_u2 cert_id, mxq_u2 CSID)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> MXQ_Ecdh_Compute_Shared\n");

	cmd_reset_offset();
	cmd_put_u1(RSA_PMS);		/* Pass */
    cmd_put_u2(CSID);			/* Cipher suite ID (CSID) */
	cmd_put_u2(cert_id);		/* Certificate ID */

	/*  Public key (optional) */
    cmd_put_u2(*data_len);
    cmd_put_buffer(data, *data_len);

    *data_len = 2048;
	response.pdata_length = data_len;
	response.p_data = encrypted_pms;

	ret = CMD_SendCommand(CMDNUM_KEY_EXCHANGE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}
#endif


mxq_err_t __API__ MXQ_Ecdh_Compute_Shared(mxq_keyparam_id_t curve_id,	mxq_u1* server_public_key,
										mxq_u1* client_public_key, mxq_length key_len, mxq_u2 CSID)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	DBG_PRINT("MXQ>> MXQ_Ecdh_Compute_Shared\n");

	cmd_reset_offset();
	cmd_put_u1(ECDHE_SECRET);		/* Pass */
	cmd_put_u2(curve_id);			/* Curve ID */
	cmd_put_u2(CSID);				/* Cipher suite ID (CSID) */

	/*  Public key (optional) */
	if(server_public_key != NULL) {
		cmd_put_u2(key_len);
		cmd_put_buffer(server_public_key, key_len);
	} else {
		cmd_put_u2(0);
	}

	/* return length intended */
	resp_len = key_len;

	response.pdata_length = &resp_len;
	response.p_data = client_public_key;

	ret = CMD_SendCommand(CMDNUM_KEY_EXCHANGE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


// Supports the ServerKeyExchange command
mxq_err_t __API__ MXQ_SetECDHEKey(
			mxq_algo_id_t  algoid, mxq_keyparam_id_t curveID, mxq_u2 na,
			mxq_u1* random, mxq_length randomlen,
			mxq_u1* ecdheparams, mxq_length ecdheparamslen,
			mxq_u1* signature, mxq_length signaturelen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;
	(void)na;

	DBG_PRINT("MXQ>> MXQ_SetECDHEKey\n");

	cmd_reset_offset();
	cmd_put_u1(ECDHE_IMPORT);			/* Pass */
	cmd_put_u1(algoid);					/* Algorithm ID */
	cmd_put_u1(curveID);				/* Curve ID */
	cmd_put_u1(0);						/* Discarded by MAXQ106x */
	cmd_put_u2(randomlen); 				/* Random */
	cmd_put_buffer(random, randomlen);

	cmd_put_u2(ecdheparamslen);			/* ECDHE parameters */
	cmd_put_buffer(ecdheparams, ecdheparamslen);


	cmd_put_u2(signaturelen);			/* Signature */
	cmd_put_buffer(signature, signaturelen);

	/* return length intended */
	resp_len = 0;
	response.pdata_length = &resp_len;
	response.p_data = NULL;

	ret = CMD_SendCommand(CMDNUM_KEY_EXCHANGE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_tls_prf_sha_256(
		mxq_u2 key_ID,
		const mxq_u1* label,mxq_length llen,
		const mxq_u1* rand, mxq_length rlen,
		mxq_u1* destbuf, mxq_length dlen)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	DBG_PRINT("MXQ>> MXQ_tls_prf_sha_256\n");

	cmd_reset_offset();
	cmd_put_u1(PSK_LAST);			/* obj 1 : pass# */
	cmd_put_u2(key_ID);				/* obj 2 : PSK ID */
	cmd_put_u2(llen);				/* obj 3 : label */
	cmd_put_buffer(label, llen);

	cmd_put_u2(rlen);				/* obj 4 : random */
	cmd_put_buffer(rand, rlen);

	cmd_put_u2(dlen);				/* obj 5 : return length */

	response.pdata_length = &dlen;
	response.p_data = destbuf;

	ret = CMD_SendCommand(CMDNUM_KEY_EXCHANGE, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

#ifdef MAXQ108x
static mxq_err_t MXQ_tls_cbc_hmac_helper(mxq_operation_t direction,
						mxq_algo_id_t mac_algo_id, mxq_length maclen,
						mxq_u1* hdr_msg_in, mxq_length hdr_msg_len,
						mxq_u1* input, mxq_length msglen)
{
	mxq_err_t err;
	const int BLOCKSIZE = 2032;
	static unsigned char mac_buffer[64];

	int remaininglen = msglen;
	int donelength = 0;
	int processedlen = 0;

	unsigned char hdr_msg[hdr_msg_len];
	memcpy(hdr_msg, hdr_msg_in, hdr_msg_len);

	hdr_msg[ 8 + 3 ] = (mxq_u1)(msglen >> 8);
	hdr_msg[ 8 + 4 ] = (mxq_u1)(msglen);

	err = MXQ_MAC_Init((direction==MXQ_OP_DECRYPT)?1:2, mac_algo_id, (direction==MXQ_OP_ENCRYPT)?3:2, NULL, 0);
	if(err != MXQ_OK) {
		return err;
	}

	err = MXQ_MAC_Update(hdr_msg, hdr_msg_len);
	if(err != MXQ_OK) {
		return err;
	}

	while (remaininglen >= BLOCKSIZE) {
		processedlen = BLOCKSIZE;
		remaininglen -= BLOCKSIZE;

		DBG_PRINT("Processing %d bytes\n", processedlen);

		err = MXQ_MAC_Update((mxq_u1 *)&input[donelength], processedlen);
		if(err != MXQ_OK) {
			return err;
		}

		donelength += processedlen;
	}

	// Process remaining data and padding
	DBG_PRINT("Remaining %d bytes to process in buffer data\n", remaininglen);

	processedlen = remaininglen;
	remaininglen = 0;

	if (processedlen) {
		err = MXQ_MAC_Update((mxq_u1 *)&input[donelength], processedlen);
		if(err != MXQ_OK) {
			return err;
		}
	}

	if (direction==MXQ_OP_DECRYPT) {
		err = MXQ_MAC_Verify((mxq_u1 *)&input[msglen], maclen);
		if(err != MXQ_OK) {
			return err;
		}
	}
	else {
		err = MXQ_MAC_Finish(mac_buffer, &maclen);
		if(err != MXQ_OK) {
			return err;
		}

		memcpy((mxq_u1 *)&input[msglen], mac_buffer, maclen);
	}

	return MXQ_OK;
}


mxq_err_t __API__ MXQ_tls_cipher_cbc(mxq_operation_t direction,
		mxq_u2 key_ID,
		const unsigned char *iv, size_t iv_len,
		const mxq_u1* input, mxq_length ilen,
		mxq_u1* output, mxq_length *olen,
		mxq_u1* hdr_msg, mxq_length hdr_msg_len,
		mxq_length maclen)
{
	(void) key_ID;
	mxq_err_t err;
	const int BLOCKSIZE = 2032;

	/***** MAC *****/
	mxq_algo_id_t mac_algo_id = ALGO_HMAC_SHA256;
	switch (maclen)
	{
		case 32: mac_algo_id = ALGO_HMAC_SHA256; break;
		case 48: mac_algo_id = ALGO_HMAC_SHA384; break;
		case 64: mac_algo_id = ALGO_HMAC_SHA512; break;
		default: return MXQ_ERR_WRONG_PARAMETER;
	}

	int msglen;
	if (direction==MXQ_OP_ENCRYPT) {
		int pad_len = 1 + input[ilen - 1];
		msglen = ilen - maclen - pad_len;

		err = MXQ_tls_cbc_hmac_helper( direction,
								mac_algo_id, maclen,
								hdr_msg, hdr_msg_len,
								(mxq_u1 *)input, msglen);
		if(err != MXQ_OK) {
			return err;
		}
	}

	/***** CIPHER *****/
	int remaininglen = ilen;
	int donelength = 0;
	int processedlen = 0;
	mxq_u1* output_ptr = output;

	ciph_params_t param;
	memset((void * )&param, 0, sizeof(param));

	param.iv_length = iv_len;
	param.p_iv = (mxq_u1 *)iv;
	param.data_length = ilen;

	err = MXQ_Cipher_Init((direction==MXQ_OP_DECRYPT)?0:1, ALGO_CIPHER_AES_CBC, (direction==MXQ_OP_ENCRYPT)?1:0, &param, 0);
	if(err != MXQ_OK) {
		return err;
	}

	while (remaininglen >= BLOCKSIZE) {
		processedlen = BLOCKSIZE;
		remaininglen -= BLOCKSIZE;

		DBG_PRINT("Processing %d bytes\n", processedlen);

		// Encrypt or decrypt
		if ((err = MXQ_Cipher_Update( &output, &input[donelength], (unsigned int *)&processedlen ))) { return err; }

		// Save total length already processed
		donelength += processedlen;
	}

	// Process remaining data and padding
	DBG_PRINT("Remaining %d bytes to process in buffer data\n", remaininglen);

	processedlen = remaininglen;
	remaininglen = 0;

	if (processedlen) {
		if ((err = MXQ_Cipher_Update( &output, &input[donelength], (unsigned int *)&processedlen ))) { return err; }
	}

	if ((err = MXQ_Cipher_Finish(NULL, NULL))) { return err; }

	*olen = ilen;

	/***** MAC *****/
	if (direction==MXQ_OP_DECRYPT) {
		int pad_len = 1 + output_ptr[*olen - 1];
		msglen = *olen - maclen - pad_len;

		err = MXQ_tls_cbc_hmac_helper( direction,
								mac_algo_id, maclen,
								hdr_msg, hdr_msg_len,
								output_ptr, msglen);
		if(err != MXQ_OK) {
			return err;
		}
	}

	return MXQ_OK;
}
#else
PRIVATE mxq_err_t MXQ_tls_records_initial(
		mxq_u1 direction,
		mxq_u2 key_ID,
		const unsigned char *iv, mxq_length iv_len,
		const mxq_u1* input, mxq_length ilen,
		mxq_u1* output, mxq_length *olen,
		mxq_u1* hdr_msg, mxq_length hdr_msg_len,
		send_flag_t send_flag)
{

	DBG_PRINT("MXQ>> MXQ_tls_records_initial\n");

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	cmd_reset_offset();

	cmd_put_u2(key_ID);			/* key_ID (~BID) */
	cmd_put_u1(PASS_NB_1);		/* pass 1 : First packet part*/
	cmd_put_u2(send_flag);		/* first send flag */
	cmd_put_u2(ilen);			/* message to process */
	cmd_put_buffer(input, ilen);

	cmd_put_u1(direction);		/* Direction */
	cmd_put_u2(iv_len);			/* IV value */
	cmd_put_buffer((mxq_u1*)iv, iv_len);

	cmd_put_u2(hdr_msg_len);	/* Header message for hmac calc */
	cmd_put_buffer(hdr_msg, hdr_msg_len);

	/* return length intended */
	//if(send_flag == IS_FIRST_SEND) ilen = 1;

	response.pdata_length = olen;
	response.p_data = output;

	ret = CMD_SendCommand(CMDNUM_TLS_RECORD, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


PRIVATE mxq_err_t MXQ_tls_records_update(
		mxq_u2 key_ID,
		const mxq_u1* input, mxq_length ilen,
		mxq_u1* output, mxq_length *olen, send_flag_t send_flag)
{

	DBG_PRINT("MXQ>> MXQ_tls_records_update\n");

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	cmd_reset_offset();

	cmd_put_u2(key_ID);			/* key_ID (~BID) */
	cmd_put_u1(PASS_NB_2);		/* pass 2 : subsequent packet parts */
	cmd_put_u2(send_flag);		/* First send flag */
	cmd_put_u2(ilen);			/* Message to process */
	cmd_put_buffer(input, ilen);

	/* return length intended */
	response.pdata_length = olen;
	response.p_data = output;

	ret = CMD_SendCommand(CMDNUM_TLS_RECORD, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}

PRIVATE mxq_err_t  MXQ_tls_records_last(
		mxq_u2 key_ID,
		const mxq_u1* input, mxq_length ilen,
		mxq_u1* output, mxq_length *olen,
		send_flag_t send_flag)
{

	DBG_PRINT("MXQ>> MXQ_tls_records_last\n");

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	cmd_reset_offset();

	cmd_put_u2(key_ID);				/* key_ID (~BID) */
	cmd_put_u1(PASS_NB_3);			/* Pass 3: Last packet part */
	cmd_put_u2(send_flag);			/* First send flag */
	cmd_put_u2(ilen);				/* Message to process */
	cmd_put_buffer(input, ilen);


	response.pdata_length = olen;
	response.p_data = output;

	ret = CMD_SendCommand(CMDNUM_TLS_RECORD, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_tls_records(mxq_operation_t direction,
		mxq_u2 key_ID,
		const unsigned char *iv, size_t iv_len,
		const mxq_u1* input, mxq_length ilen,
		mxq_u1* output, mxq_length *olen,
		mxq_u1* hdr_msg, mxq_length hdr_msg_len)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	mxq_err_t ret = MXQ_ERR_INTERNAL_ERROR;
	send_flag_t send_flag = IS_FIRST_SEND;
	mxq_u1 i = 0;
	mxq_length ilen_cpy = ilen;
	mxq_length send_len, receiv_len = 0;

	mxq_length input_send = 0, output_receiv = 0;
	static mxq_u1 temp_output[MAX_TLS_PACKET_SIZE];


	if( ilen < MAX_COMM_DATA_LEN)
	{
		*olen = MAX_COMM_DATA_LEN;
		return MXQ_tls_records_initial(direction, key_ID,
				iv, iv_len, input, ilen, output, olen,
				hdr_msg, hdr_msg_len,
				IN_ONE_SEND);
	}

	/* 2 send needed :
	 * 	first to determine msg len
	 * 	second to process hmac verification
	 */
	for(i = 0; i < 2; i++) {
		if((ilen_cpy >= MAX_COMM_DATA_LEN) && ilen_cpy <= (MAX_COMM_DATA_LEN + MAX_PAD_LEN + MAC_LEN_SHA256)) {
			/* final message length can't be 0. if last */
			input_send += MAX_COMM_DATA_LEN / 2;
			send_len = MAX_COMM_DATA_LEN / 2;

			receiv_len = MAX_COMM_DATA_LEN;
			ret = MXQ_tls_records_initial(direction, key_ID, iv, iv_len, input, send_len, temp_output, &receiv_len,
										  hdr_msg, hdr_msg_len, send_flag);
			if(ret != MXQ_OK) return ret;

			ilen_cpy -= MAX_COMM_DATA_LEN / 2;
			output_receiv += receiv_len;
		} else {
			input_send += MAX_COMM_DATA_LEN;
			send_len = MAX_COMM_DATA_LEN;

			receiv_len = MAX_COMM_DATA_LEN;
			ret = MXQ_tls_records_initial(direction, key_ID, iv, iv_len, input, send_len, temp_output, &receiv_len,
										  hdr_msg, hdr_msg_len, send_flag);
			if(ret != MXQ_OK) return ret;

			ilen_cpy -= MAX_COMM_DATA_LEN;
			output_receiv += receiv_len;
		}

		/*split the message until ilen cpy correspond to the max buffer len with
		 * mac length and maximum padding
		 *
		 **/
		while(ilen_cpy > (MAX_COMM_DATA_LEN + MAX_PAD_LEN + MAC_LEN_SHA256)) {
			receiv_len = MAX_COMM_DATA_LEN;
			ret = MXQ_tls_records_update(key_ID, (mxq_u1*)(input + input_send), MAX_COMM_DATA_LEN,
										 (mxq_u1*)(temp_output + output_receiv), &receiv_len, send_flag);
			if(ret != MXQ_OK) return ret;

			input_send += MAX_COMM_DATA_LEN;
			output_receiv += receiv_len;
			ilen_cpy -= MAX_COMM_DATA_LEN;
		}


		if(ilen_cpy >= MAX_COMM_DATA_LEN) {
			/* final message length can't be inferior of MAX_PAD_LEN + MAC_LEN_SHA.
			 * if it happened, split the message in half
			 */
			receiv_len = MAX_COMM_DATA_LEN;
			ret = MXQ_tls_records_update(key_ID, (mxq_u1*)(input + input_send), MAX_COMM_DATA_LEN / 2,
										 (mxq_u1*)(temp_output + output_receiv), &receiv_len, send_flag);
			if(ret != MXQ_OK) return ret;

			input_send += MAX_COMM_DATA_LEN / 2;
			output_receiv += receiv_len;
			ilen_cpy -= MAX_COMM_DATA_LEN / 2;
		}

		receiv_len = MAX_COMM_DATA_LEN;
		ret = MXQ_tls_records_last(key_ID, (mxq_u1*)(input + input_send), ilen_cpy,
								   (mxq_u1*)(temp_output + output_receiv), &receiv_len, send_flag);
		if(ret != MXQ_OK) return ret;

		if(send_flag == IS_NOT_FIRST_SEND) {
			MXQ_MEMCPY(output, temp_output, ilen);
		}

		send_flag = IS_NOT_FIRST_SEND;
		ilen_cpy = ilen;
		input_send = 0;
		receiv_len = 0;
		output_receiv = 0;
	}

	*olen = ilen_cpy;
	return ret;
}
#endif

PRIVATE mxq_err_t MXQ_tls_cipher_auth_ccm_initial(
		mxq_u2 key_ID,
		mxq_u1 direction,
		mxq_u1 *iv, mxq_length iv_len,
		mxq_u1 *ad, mxq_length ad_len,
		mxq_u1 *input, mxq_length ilen, mxq_length total_msg_len,
		mxq_u1 *output, mxq_length *olen,
		mxq_u1 *tag, mxq_length tag_len)
{

	DBG_PRINT("MXQ>> MXQ_tls_cipher_auth_ccm_initial\n");

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	cmd_reset_offset();

	cmd_put_u2(key_ID);				/* key_ID (~BID) */
	cmd_put_u1(PASS_NB_1);			/* Pass 1 */
	cmd_put_u1(direction);			/* Direction */
	
	cmd_put_u2(ilen);				/* Input message */
	cmd_put_buffer(input, ilen);

	cmd_put_u2(total_msg_len);		/* Message total size */

	cmd_put_u2(iv_len);				/* IV = explicit nonce */
	cmd_put_buffer(iv, iv_len);

	cmd_put_u2(ad_len);				/* AAD */
	cmd_put_buffer(ad, ad_len);

	cmd_put_u2(tag_len);			/* tag */
	cmd_put_buffer(tag, tag_len);

	response.pdata_length = olen;
	response.p_data = output;

	ret = CMD_SendCommand(CMDNUM_TLS_RECORD, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


PRIVATE mxq_err_t MXQ_tls_cipher_auth_ccm_update(mxq_u2 key_ID,
		mxq_u1 direction,
		mxq_u1 *input, mxq_length ilen,
		mxq_u1 *output, mxq_length *olen)
{

	DBG_PRINT("MXQ>> MXQ_tls_cipher_auth_ccm_update\n");

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	cmd_reset_offset();

	cmd_put_u2(key_ID);				/* key_ID (~BID) */
	cmd_put_u1(PASS_NB_2);			/* Pass 2 */
	cmd_put_u1(direction);			/* Direction */
	cmd_put_u2(ilen);				/* Input message */
	cmd_put_buffer(input, ilen);

	response.pdata_length = olen;
	response.p_data = output;

	ret = CMD_SendCommand(CMDNUM_TLS_RECORD, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


PRIVATE mxq_err_t MXQ_tls_cipher_auth_ccm_last(mxq_u2 key_ID,
		mxq_u1 direction,
		mxq_u1 *input, mxq_length ilen,
		mxq_u1 *output, mxq_length *olen) {

	DBG_PRINT("MXQ>> MXQ_tls_cipher_auth_ccm_last\n");

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;

	cmd_reset_offset();

	cmd_put_u2(key_ID);				/* key_ID (~BID) */
	cmd_put_u1(PASS_NB_3);			/* Pass 3 */
	cmd_put_u1(direction);			/* Direction */
	cmd_put_u2(ilen);				/* Input message */
	cmd_put_buffer(input, ilen);

	response.pdata_length = olen;
	response.p_data = output;

	ret = CMD_SendCommand(CMDNUM_TLS_RECORD, &response);
	if(ret != MXQ_OK) return ret;

	return (mxq_err_t)response.status;
}


mxq_err_t __API__ MXQ_TLS_Save(void)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	cmd_reset_offset();
	cmd_put_u1(PASS_SAVE_TLS);		/* Option 4 : Save TLS Context */

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_KEY_EXCHANGE, &response);

	return ret;
}


mxq_err_t __API__ MXQ_TLS_Restore(void)
{
	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	cmd_response_t response;
	mxq_err_t ret;
	mxq_length resp_len = 0;

	cmd_reset_offset();
	cmd_put_u1(PASS_RESTORE_TLS);		/* Option 5 : Restore TLS Context */

	response.p_data = NULL;
	response.pdata_length = &resp_len;

	ret = CMD_SendCommand(CMDNUM_KEY_EXCHANGE, &response);

	return ret;
}

mxq_err_t __API__  MXQ_tls_cipher_aead(
		mxq_u2 key_ID,
		mxq_operation_t direction, // 1: encrypt (TX packets, outbound), 0: decrypt (RX packets, inbound)
		mxq_u2 algo,
		mxq_u1 *iv, mxq_length iv_len,    // IV/nonce
		mxq_u1 *aad, mxq_length aad_len,  // AAD
		mxq_u1 *input, mxq_length ilen,   // Payload
		mxq_u1 *output, mxq_length *olen, // output
		mxq_u1 *tag, mxq_length tag_len)  // input or output tag (depends on direction)
{

    (void) key_ID;
    (void) olen;
    mxq_err_t err;

    const int BLOCKSIZE        = 2032; // avoid chaining by API
    signed int remaininglen    = ilen;
    signed int donelength      = 0;
    signed int processedlen;

	ciph_params_t param;
	memset((void * )&param, 0, sizeof(param));

	int isCCM=(algo==ALGO_CIPHER_AES_CCM);

    (void)isCCM; // Silence warning due to unused variable when MXQ_PRINTF is not used

	MXQ_PRINTF("Algo %s %scryption\n",isCCM?"AES-CCM":"AES-GCM",(direction==MXQ_OP_DECRYPT)?"de":"en");

	// Tag
    MXQ_PRINTF("Tag length=%d\n",tag_len);
    param.aead_tag_len = tag_len;

    if (direction == MXQ_OP_DECRYPT) {
    	//TODO: problem of hard coded ccm tag length of 16 bytes vs CCM_8 suites
        if(tag_len> sizeof(param.aead_tag)) {MXQ_PRINTF("MXQ_tls_cipher_aead error: %d tag length",tag_len); return MXQ_ERR_INTERNAL_ERROR;}
        memcpy(param.aead_tag,tag,tag_len);
        MXQ_PRINTF("TAG: ");
        MXQ_PRINTBUFFER(param.aead_tag, param.aead_tag_len);
        MXQ_PRINTF("\n");
    }

    // AAD
    MXQ_PRINTF("AAD length=%d\n",aad_len);
    if(aad_len>65535UL) {MXQ_PRINTF("MXQ_tls_cipher_aead error: aad_length"); return MXQ_ERR_INTERNAL_ERROR;}
    param.p_aad      = aad;
    param.aad_length = aad_len;
    MXQ_PRINTF("AAD: ");
    MXQ_PRINTBUFFER(param.p_aad, param.aad_length);
    MXQ_PRINTF("\n");

    // Nonce
    MXQ_PRINTF("Nonce/IV length=%d\n",iv_len);
    if(iv_len!=12UL) {MXQ_PRINTF("MXQ_tls_cipher_aead error: iv_length"); return MXQ_ERR_INTERNAL_ERROR;}
    param.p_iv   = iv;
    param.iv_length = iv_len;
    MXQ_PRINTF("Nonce/IV: ");
    MXQ_PRINTBUFFER(param.p_iv, param.iv_length);
    MXQ_PRINTF("\n");

    // Payload
    param.data_length = ilen;
    MXQ_PRINTF("Input length=%d\n", param.data_length);
    // Decipher with key1, cipher with key0
    err=MXQ_Cipher_Init((direction==MXQ_OP_DECRYPT)?0:1, algo, (direction==MXQ_OP_ENCRYPT)?1:0, &param, 0);
    if(err!=MXQ_OK)
        return err;
    // TODO:special mode where nonce is partially overridden by maxq with the implicit nonce

    while (remaininglen) {

    	if(remaininglen>BLOCKSIZE) {
			processedlen = BLOCKSIZE;
			remaininglen-=BLOCKSIZE;
		}
		else {
			processedlen = remaininglen;
			remaininglen = 0;		
		}

        MXQ_PRINTF("Processing %d bytes\n", processedlen);

      
        // Encrypt or decrypt
        if((err=MXQ_Cipher_Update( &output, &input[donelength], (unsigned int *) &processedlen))) return err;

		// Save total length already processed
        donelength += processedlen;

    } //while


    if((err=MXQ_Cipher_Finish(&output, &tag_len))) return err;

    return MXQ_OK;
}


mxq_err_t __API__  MXQ_cipher_auth_ccm(mxq_u2 key_ID,
		mxq_u1 direction,
		mxq_u1 *iv, mxq_length iv_len,
		mxq_u1 *ad, mxq_length ad_len,
		mxq_u1 *input, mxq_length ilen,
		mxq_u1 *output, mxq_length *olen,
		mxq_u1 *tag, mxq_length tag_len)
{

	if(g_selectedmode==MXQ_MODE_GCU) {return MXQ_ERR_MODE;}

	mxq_err_t ret;
	mxq_length ilen_cpy = ilen;
	mxq_length olen_cpy = *olen;
	mxq_length send_len, receiv_len = 0;
	mxq_length input_send = 0, output_receiv = 0;


	if(ilen < MAX_CCM_PACKET_LENGTH) {

		if(direction == 0) {
			*olen += tag_len;
		}

		ret = MXQ_tls_cipher_auth_ccm_initial(key_ID, direction, iv, iv_len, ad, ad_len, input, ilen, ilen, output,
											  olen, tag, tag_len);

		if(direction == 0) {
			*olen -= tag_len; /* remove tag len from received message length */
		}
		return ret;
	}

	/* when message is too long, process it with several call to maxq */
	input_send += MAX_CCM_PACKET_LENGTH;
	send_len = MAX_CCM_PACKET_LENGTH;

	ret = MXQ_tls_cipher_auth_ccm_initial(key_ID, direction, iv, iv_len, ad, ad_len, input, send_len, ilen, output,
										  olen, tag, tag_len);

	if(ret != MXQ_OK) return ret;


	ilen_cpy -= MAX_CCM_PACKET_LENGTH;
	output_receiv += *olen;
	receiv_len = MAX_CCM_PACKET_LENGTH;

	while(ilen_cpy > MAX_CCM_PACKET_LENGTH) {
		ret = MXQ_tls_cipher_auth_ccm_update(key_ID, direction, (mxq_u1*)(input + input_send), MAX_CCM_PACKET_LENGTH,
											 (mxq_u1*)(output + output_receiv), &receiv_len);
		if(ret != MXQ_OK) return ret;

		input_send += MAX_CCM_PACKET_LENGTH;
		output_receiv += receiv_len;
		ilen_cpy -= MAX_CCM_PACKET_LENGTH;
	}

	if(direction == 0) {
		receiv_len = olen_cpy - output_receiv + tag_len;
	}

	ret = MXQ_tls_cipher_auth_ccm_last(key_ID, direction, (mxq_u1*)(input + input_send), ilen_cpy,
									   (mxq_u1*)(output + output_receiv), &receiv_len);

	if(ret != MXQ_OK) {
		return ret; /* bad reception in MXQ_cipher_auth_ccm */
	}

	*olen = output_receiv + ilen_cpy - tag_len;

	return ret;
}

#ifdef MAXQ106x
#define DEBUG_KEY 0
mxq_err_t MXQ_client_key_exchange(
        const mxq_u1* remote_random, //in
        mxq_cert* srvcert,
        mxq_u1* client_ecdhe_key, // out. format 0x04 X Y
        mxq_u1* client_random, // out
        mxq_u2 client_signkey,
        mxq_u1* client_ecdsa_sign,
        mxq_u1* aeskey[] //out
        ) 
{
    mxq_err_t r;

    static mxq_u1 srvcert_importreq[4096];
    mxq_length req_len = sizeof(srvcert_importreq);
    mxq_keyuse_t  uu1  = MXQ_KEYUSE_DATASIGNATURE;
    mxq_algo_id_t aa1  = MXQ_get_sign_alg_from_curve(srvcert->curve);
    mxq_keyuse_t  uu2  = MXQ_KEYUSE_NONE;
    mxq_algo_id_t aa2  = ALGO_INVALID;
    int keycomplen     = MXQ_get_curve_bytelen(srvcert->curve);

    if (srvcert->len > 2008) {
        MXQ_PRINTF("Error: Certificate too large.\n");
        r = MXQ_ERR_INTERNAL_ERROR;
        return(r);
    }

    if((r=(MXQ_Build_EC_Cert(srvcert_importreq, &req_len, MXQ_KEYTYPE_ECC, srvcert->curve, keycomplen, keycomplen * 2, srvcert->pk_offset,
                                    0x04, srvcert->tbslen, srvcert->len, MXQ_get_sign_alg_from_curve(srvcert->parent_curve), srvcert->parent_id, 
                                    uu1, aa1, uu2, aa2,
                                    srvcert->der)))!=MXQ_OK) return r;


    /* Import server certificate/pubkey */
    if((r=(MXQ_ImportChildCert(0, srvcert_importreq, req_len)))!=MXQ_OK) return r;


    // Derives Key using this algorithm:
    // Let Z=shared ECDH secret
    // Let PMK=Z
    // Let MK=PRF(PMK, "master secret", client_random|remote_random)[0..16]
    // Shared AES key=PRF(MK, "key expansion", remote_random|client_random)[0..16]
    // PRF: see tls 1.2 specification
    /* Read back the public key */
    if((r=(MXQ_Ecdh_Compute_Shared(srvcert->curve,NULL,client_ecdhe_key,2*keycomplen+1,0xc026)))!=MXQ_OK) return r;

    // Generate client random
    unsigned char rnd[SIZE_RND_BYTES*2+2*keycomplen+1];
    if((r=(MXQ_Get_Random(client_random,SIZE_RND_BYTES)))!=MXQ_OK) return r;
    memcpy(rnd,client_random,SIZE_RND_BYTES); // client
    memcpy(rnd+SIZE_RND_BYTES,remote_random,SIZE_RND_BYTES); //server

    mxq_secret_context_api_t derive_key_info;
    derive_key_info.pass                  = PSK_FIRST;
    derive_key_info.CSID                  = 0xc026;
    derive_key_info.Random                = rnd; // client_random || remote_random
    derive_key_info.Random_size           = SIZE_RND_BYTES*2;
	derive_key_info.PSK_info.psk_id       = 0;
    derive_key_info.is_session_key_secret = DEBUG_KEY?0:1;

    if((r=(MXQ_Perform_Key_Exchange(&derive_key_info)))!=MXQ_OK) return r;

#if DEBUG_KEY
    #warning KEY OUTPUT
    MXQ_PRINTF("\n\n>> Client key block:\n");
    MXQ_PRINTBUFFER(derive_key_info.PSK_info.psk_key_bloc,256);
    MXQ_PRINTF("\n\n");

    memcpy(aeskey, derive_key_info.PSK_info.psk_key_bloc+64, 2*AES256_KEYSIZE_BYTES);

    MXQ_PRINTF("\n\n>> Client AES KEY\n");
    print_buffer((unsigned char*)aeskey,2*AES256_KEYSIZE_BYTES);
    MXQ_PRINTF("\n\n");
#else
    (void) aeskey;
#endif

    derive_key_info.pass                  = PSK_LAST;
    derive_key_info.CSID                  = 0xc026;
    derive_key_info.Random                = rnd; // client_random || remote_random
    derive_key_info.Random_size           = 64;
    derive_key_info.PSK_info.psk_id       = 0;
    derive_key_info.is_session_key_secret = DEBUG_KEY?0:1;

    if((r=(MXQ_Perform_Key_Exchange(&derive_key_info)))!=MXQ_OK) return r;

    memcpy(rnd+SIZE_RND_BYTES*2,client_ecdhe_key,2*keycomplen+1);

    mxq_length siglen=128;
    if(client_signkey)
        if((r=(MXQ_Sign(aa1, client_signkey, rnd, sizeof(rnd), client_ecdsa_sign, &siglen)))) return r;

    return MXQ_OK;

}


// Supports only AES-CBC
mxq_err_t MXQ_client_decrypt(mxq_u1* cleartext, const mxq_u1* ciphertext, mxq_length ciphertext_len, const mxq_u1 IV[16])
{
    // decrypt server originated data locally, using the MAXQ
    mxq_length outlen;
    mxq_err_t r;

    mxq_u1 dummy[13];
    memset(dummy,0,sizeof(dummy));
    dummy[6]=(unsigned char)(ciphertext_len>>8);
    dummy[7]=(unsigned char)ciphertext_len;

    // workaround call
    outlen=0;
    if((r=MXQ_tls_records_initial(MXQ_OP_DECRYPT,
        0xdddd, 
        IV,0,
        NULL,0,
        NULL,&outlen,
        dummy,13,IS_FIRST_SEND))!=0) return r;
    
    outlen=0;
    if((r=MXQ_tls_records_initial(MXQ_OP_DECRYPT,
        0xdddd, 
        IV,16,
        NULL,0,
        NULL,&outlen,
        dummy,13,IS_FIRST_SEND))!=0) return r;
    
    outlen=ciphertext_len;
    return MXQ_tls_records_last(
        0xdddd,
        ciphertext,ciphertext_len,
        cleartext,&outlen,IN_ONE_SEND);
}

// Supports only AES-CBC
mxq_err_t MXQ_client_encrypt(mxq_u1* ciphertext, const mxq_u1* cleartext, mxq_length cleartext_len, const mxq_u1 IV[16])
{
    mxq_length outlen;
    mxq_err_t r;

    mxq_u1 dummy[13];
    memset(dummy,0,sizeof(dummy));
    dummy[6]=(unsigned char)(cleartext_len>>8);
    dummy[7]=(unsigned char)cleartext_len;

    outlen=0;
    if((r=MXQ_tls_records_initial(MXQ_OP_ENCRYPT,
        0xeeee, 
        IV,16, 
        NULL,0,
        NULL,&outlen,
        dummy,13,IS_FIRST_SEND))!=0) return r;

    outlen=cleartext_len;
    r=MXQ_tls_records_update(
        0xeeee,
        cleartext,cleartext_len,
        ciphertext,&outlen,IS_NOT_FIRST_SEND);

    return r;

}

mxq_err_t MXQ_client_key_exchange_psk(
        mxq_u1* remote_ecdhe_pubkey,
        const mxq_u1* remote_random, //in
         // out. format 0x04 X Y
        mxq_u1* client_ecdhe_key, // out. format 0x04 X Y
        mxq_u1 client_random[SIZE_RND_BYTES], // out
        mxq_keyparam_id_t curve_id,
        int PSK_ID, 
        mxq_u1 aeskey[32] //out
        ) 

{
    mxq_err_t r;

    unsigned char rnd[SIZE_RND_BYTES*2];

    if((r=MXQ_Get_Random(client_random,SIZE_RND_BYTES))!=MXQ_OK) return r;

    memcpy(rnd,client_random,SIZE_RND_BYTES);
    memcpy(rnd+SIZE_RND_BYTES,remote_random,SIZE_RND_BYTES);

    // Derives Key using this algorithm:
    // Let Z=shared ECDH secret
    // Let PMK=Lz|Z|Lpsk|PSK
    // Let MK=PRF(PMK, "master secret", client_random|remote_random)[0..16]
    // Shared AES key=PRF(MK, "key expansion", remote_random|client_random)[0..16]
    // PRF: see 
    if((r=MXQ_Ecdh_Compute_Shared(MXQ_KEYPARAM_EC_P256R1,remote_ecdhe_pubkey,client_ecdhe_key,1+2*MXQ_get_curve_bytelen(curve_id),0xc035))!=MXQ_OK) return r;

    mxq_secret_context_api_t derive_key_info;
    derive_key_info.pass                  = PSK_FIRST;
    derive_key_info.CSID                  = 0xc035;
    derive_key_info.Random                = rnd; // client_random || remote_random
    derive_key_info.Random_size           = SIZE_RND_BYTES*2;
    derive_key_info.PSK_info.psk_id       = PSK_ID;
    derive_key_info.is_session_key_secret = 0;

    if((r=MXQ_Perform_Key_Exchange(&derive_key_info))!=MXQ_OK) return r;

    unsigned char l[]={0};
    unsigned char ra[]={0};
    unsigned char d[]={0};

    if((r=MXQ_tls_prf_sha_256(0,l,sizeof(l),ra,sizeof(ra),d,sizeof(d)))!=MXQ_OK) return r;

    memcpy(aeskey, derive_key_info.PSK_info.psk_key_bloc, 32);

#if DEBUG_KEY
    MXQ_PRINTF("AES KEY (client): ");
    MXQ_PRINTBUFFER(aeskey,32);
    MXQ_PRINTF("\n");
#endif

    return MXQ_OK;
}


mxq_err_t __API__ MXQ_GCU_Enable(void)
{
	mxq_err_t ret;

	ret = HOST_activate_gcu();
	if(ret != MXQ_OK) return ret;

	g_selectedmode = MXQ_MODE_GCU;

	return MXQ_OK;
}

mxq_err_t __API__ MXQ_GCU_Direct(void)
{
	mxq_err_t ret;

	ret = HOST_follow_gcu();
	if(ret != MXQ_OK) return ret;

	return MXQ_OK;
}
#endif //MAXQ106x

mxq_err_t __API__ MXQ_TLS_Direct(void)
{
	mxq_err_t ret;

	ret = HOST_follow_tls();
	if(ret != MXQ_OK) return ret;

	return MXQ_OK;
}

mxq_err_t __API__ MXQ_TLS_Enable(void)
{
#if defined(MAXQ106x)
	mxq_err_t ret;

	ret = HOST_activate_tls();
	if(ret != MXQ_OK) return ret;
#endif
	g_selectedmode = MXQ_MODE_TLS;

	return MXQ_OK;
}

#if defined(MAXQ108x)
mxq_err_t __API__ MAXQ1080_FW_Update(mxq_pu1 fw_img, mxq_length fw_len, mxq_u2 key_ID, const mxq_pu1 signature, mxq_algo_id_t algo, mxq_keyparam_id_t keyparam)
{
	cmd_response_t response;
	mxq_status_raw_t rs;
	mxq_err_t ret;
	mxq_bool in_bl_mode = MXQ_TRUE;
	mxq_u4 fw_body_len;
	mxq_pu1 body_len_ptr, img_body_ptr;
	mxq_u4 i;
	mxq_u1 sign_buffer[FW_UPDATE_SIGNATURE_SIZE] = { 0 };

	DBG_PRINT("MXQ>> FW UPDATE\n");

	if (fw_img == NULL || signature == NULL) {
		return MXQ_RSP_WRONG_PARAM;
	}

	body_len_ptr = fw_img + FW_UPDATE_IV_LEN + FW_UPDATE_AAD_LEN + FW_UPDATE_TAG_LEN;
	get_u4(body_len_ptr, &fw_body_len);

	if (fw_len != fw_body_len + FW_UPDATE_IMG_HEADER_LEN) {
		return MXQ_ERR_BAD_LENGTH;
	}

	mxq_length len = sizeof(mxq_status_t);
	response.p_data = (mxq_u1*)&rs;
	response.pdata_length = &len;

	DBG_PRINT("MXQ>> Get Status\n");

	cmd_reset_offset();

	ret = CMD_SendCommand(CMDNUM_GETSTATUS, &response);
	if(ret != MXQ_OK) return ret;

	if (strncmp(MAXQ1080_BOOTLOADER_PRODUCT_NAME, (char*)rs.product_name, MXQ_MAXQ106x_CHIP_NAME_LENGTH)) {
		in_bl_mode = MXQ_FALSE;
	}

	memcpy(sign_buffer, signature, FW_UPDATE_SIGNATURE_SIZE);

	DBG_PRINT("Firmware Update Information\n");
	DBG_PRINT("IV: ");
	for (i = 0; i < FW_UPDATE_IV_LEN; i++) {
		DBG_PRINT("%2x ", fw_img[i]);
	}
	DBG_PRINT("\nAAD: %.*s\n", FW_UPDATE_AAD_LEN, fw_img + FW_UPDATE_IV_LEN);
	DBG_PRINT("Tag: ");
	for (i = 0; i < FW_UPDATE_TAG_LEN; i++) {
		DBG_PRINT("%2x ", (fw_img + FW_UPDATE_IV_LEN + FW_UPDATE_AAD_LEN)[i]);
	}
	DBG_PRINT("\nFirmware size: %lu Bytes\n", fw_body_len);
	DBG_PRINT("Public Key ID: %hu \n", key_ID);
	DBG_PRINT("Auth Code: ");
	for (i = 0; i < FW_UPDATE_SIGNATURE_SIZE; i++) {
		DBG_PRINT("%2x ", sign_buffer[i]);
	}
	DBG_PRINT("\nUpdate Requested...\n");

	cmd_reset_offset();
	cmd_put_buffer(fw_img, FW_UPDATE_IMG_HEADER_LEN);
	cmd_put_u2(key_ID);
	cmd_put_u1(algo);
	cmd_put_u1(keyparam);
	cmd_put_buffer(sign_buffer, FW_UPDATE_SIGNATURE_SIZE);

	ret = CMD_SendCommand(CMDNUM_START_UPDATE, &response);
	if(ret != MXQ_OK) return ret;

	if (response.status != MXQ_OK) return (mxq_err_t)response.status;

	DBG_PRINT("Update Authorized\n");

	if (!in_bl_mode) {
		DBG_PRINT("Waiting for MAXQ to reboot\n");
		HOST_wait_usec(FW_UPDATE_AWAIT_BOOTLOADER_USEC);
		DBG_PRINT("MXQ>> Get Status\n");

		len = sizeof(mxq_status_t);
		response.p_data = (mxq_u1*)&rs;
		response.pdata_length = &len;

		cmd_reset_offset();

		ret = CMD_SendCommand(CMDNUM_GETSTATUS, &response);
		if(ret != MXQ_OK) return ret;

		if (strncmp(MAXQ1080_BOOTLOADER_PRODUCT_NAME, (char*)rs.product_name, MXQ_MAXQ106x_CHIP_NAME_LENGTH)) {
			return MXQ_ERR_INTERNAL_ERROR;
		}
	}

	img_body_ptr = fw_img + FW_UPDATE_IMG_HEADER_LEN;

	for (i = 0; i + 1024 <= fw_body_len; i = i + 1024) {
		cmd_reset_offset();
		cmd_put_buffer(img_body_ptr + i, 1024);
		ret = CMD_SendCommand(CMDNUM_PROCESS_UPDATE, &response);
		if(ret != MXQ_OK) return ret;
		if (response.status != MXQ_OK) return (mxq_err_t)response.status;
	}

	if (i != fw_body_len) {
		cmd_reset_offset();
		cmd_put_buffer(img_body_ptr + i, fw_body_len - i);
		ret = CMD_SendCommand(CMDNUM_PROCESS_UPDATE, &response);
		if(ret != MXQ_OK) return ret;
		if (response.status != MXQ_OK) return (mxq_err_t)response.status;
	}

	cmd_reset_offset();
	ret = CMD_SendCommand(CMDNUM_FINISH_UPDATE, &response);
	if(ret != MXQ_OK) return ret;
	if (response.status != MXQ_OK) return (mxq_err_t)response.status;

	return MXQ_OK;
}
#endif

#if defined(MAXQ1065)
mxq_err_t __API__ MAXQ1065_FW_Update(mxq_pu1 fw_img, mxq_length fw_len)
{
	cmd_response_t response;
	mxq_status_raw_t rs;
	mxq_err_t ret;
	mxq_bool in_bl_mode = MXQ_TRUE;
	mxq_u4 fw_body_len;
	mxq_pu1 body_len_ptr, img_body_ptr;
#if (MXQ_DUMP_DEBUG!=(-1))
	mxq_pu1 info_ptr;
	info_ptr = fw_img;
#endif
	mxq_u4 i;


	DBG_PRINT("MXQ>> FW UPDATE2\n");

	if (fw_img == NULL) {
		DBG_PRINT("MXQ_RSP_WRONG_PARAM\n");
		return MXQ_RSP_WRONG_PARAM;
	}

	body_len_ptr = fw_img + FW_UPDATE_IV_LEN + FW_UPDATE_AAD_LEN + FW_UPDATE_TAG_LEN;
	get_u4(body_len_ptr, &fw_body_len);

	if (fw_len != fw_body_len + FW_UPDATE_IMG_HEADER_LEN) {
		DBG_PRINT("MXQ_ERR_BAD_LENGTH\n");
		return MXQ_ERR_BAD_LENGTH;
	}

	mxq_length len = sizeof(mxq_status_t);
	response.p_data = (mxq_u1*)&rs;
	response.pdata_length = &len;

	DBG_PRINT("MXQ>> Get Status\n");

	cmd_reset_offset();

	ret = CMD_SendCommand(CMDNUM_GETSTATUS, &response);
	if(ret != MXQ_OK) return ret;

	if (strncmp(MAXQ1065_BOOTLOADER_PRODUCT_NAME, (char*)rs.product_name, MXQ_MAXQ106x_CHIP_NAME_LENGTH)) {
		in_bl_mode = MXQ_FALSE;
	}

	DBG_PRINT("Firmware Update Information\n");
	DBG_PRINT("IV: ");
	for (i = 0; i < FW_UPDATE_IV_LEN; i++) {
		DBG_PRINT("%2x ", *info_ptr++);
	}
	DBG_PRINT("\nAAD: %.*s\n", FW_UPDATE_AAD_LEN, fw_img + FW_UPDATE_IV_LEN);
	DBG_PRINT("Tag: ");
	for (i = 0; i < FW_UPDATE_TAG_LEN; i++) {
		DBG_PRINT("%2x ", *info_ptr++);
	}
	DBG_PRINT("\nFirmware size: %lu Bytes\n", fw_body_len);
	DBG_PRINT("Auth Code: ");
	for (i = 0; i < FW_UPDATE_SIGNATURE_SIZE; i++) {
		DBG_PRINT("%2x ", *info_ptr++);
	}

	if (!in_bl_mode) {
		DBG_PRINT("Enter Bootloader Request\n");

		response.p_data = NULL;
		response.pdata_length = 0;
		DBG_PRINT("MXQ>> Enter Bootloader\n");
		cmd_reset_offset();
		ret = CMD_SendCommand(CMDNUM_ENTER_BOOTLOADER, &response);
		if(ret != MXQ_OK) return ret;
		if (response.status != MXQ_OK) return (mxq_err_t)response.status;

		DBG_PRINT("Waiting for MAXQ to reboot\n");
		HOST_wait_usec(FW_UPDATE_AWAIT_BOOTLOADER_USEC);
		len = sizeof(mxq_status_t);
		response.p_data = (mxq_u1*)&rs;
		response.pdata_length = &len;

		DBG_PRINT("MXQ>> Get Status\n");

		cmd_reset_offset();

		ret = CMD_SendCommand(CMDNUM_GETSTATUS, &response);
		if(ret != MXQ_OK) return ret;

		if (strncmp(MAXQ1065_BOOTLOADER_PRODUCT_NAME, (char*)rs.product_name, MXQ_MAXQ106x_CHIP_NAME_LENGTH)) {
			return MXQ_ERR_INTERNAL_ERROR;
		}
	}

	cmd_reset_offset();
	cmd_put_buffer(fw_img, FW_UPDATE_IMG_HEADER_LEN);

	DBG_PRINT("\nUpdate Requested...\n");

	ret = CMD_SendCommand(CMDNUM_START_UPDATE, &response);
	if(ret != MXQ_OK) return ret;

	if (response.status != MXQ_OK) return (mxq_err_t)response.status;

	DBG_PRINT("Update Authorized\n");

	img_body_ptr = fw_img + FW_UPDATE_IMG_HEADER_LEN;

	for (i = 0; i + 1024 <= fw_body_len; i = i + 1024) {
		DBG_PRINT("MXQ>> Packet %lu\n", i);
		cmd_reset_offset();
		cmd_put_buffer(img_body_ptr + i, 1024);
		ret = CMD_SendCommand(CMDNUM_PROCESS_UPDATE, &response);
		if(ret != MXQ_OK) return ret;
		if (response.status != MXQ_OK) return (mxq_err_t)response.status;
	}

	if (i != fw_body_len) {
		cmd_reset_offset();
		cmd_put_buffer(img_body_ptr + i, fw_body_len - i);
		ret = CMD_SendCommand(CMDNUM_PROCESS_UPDATE, &response);
		if(ret != MXQ_OK) return ret;
		if (response.status != MXQ_OK) return (mxq_err_t)response.status;
	}

	cmd_reset_offset();
	ret = CMD_SendCommand(CMDNUM_FINISH_UPDATE, &response);
	if(ret != MXQ_OK) return ret;
	if (response.status != MXQ_OK) return (mxq_err_t)response.status;

	return MXQ_OK;
}
#endif
/**
 * Definition of tamper command arguments.
 */
typedef enum _PACKED
{
    S_ERASE_SELECTED = 0x01,
    S_ERASE_ALL = 0x02
} mxq_tamper_cmd_t;

//#ifdef MXQ_FW_UPDATE_CAP
#if defined(MAXQ108x)
mxq_err_t __API__ MAXQ_FW_Update(mxq_pu1 fw_img, mxq_length fw_len, mxq_u2 key_ID, const mxq_pu1 signature, mxq_algo_id_t algo, mxq_keyparam_id_t keyparam)
{
	return MAXQ1080_FW_Update(fw_img, fw_len, key_ID, signature, algo, keyparam);
}
#elif defined(MAXQ1065)
mxq_err_t __API__ MAXQ_FW_Update(mxq_pu1 fw_img, mxq_length fw_len)
{
	return MAXQ1065_FW_Update(fw_img, fw_len);
}
#endif
//#endif

mxq_err_t __API__ MXQ_Tamper(mxq_tamper_cmd_t cmd)
{
#if defined(USS_ST) && defined(USE_PROTOCOL_MAILBOX)
    cmd_response_t response;
    mxq_err_t ret;
    mxq_length resp_len = 0;

    if (cmd > S_ERASE_ALL) return MXQ_ERR_WRONG_PARAMETER;

    cmd_reset_offset();
    cmd_put_u1(cmd);

    response.p_data = NULL;
    response.pdata_length = &resp_len;

    ret = CMD_SendCommand(CMDNUM_TAMPER, &response);
    if(ret != MXQ_OK) return ret;

    return (mxq_err_t)response.status;
#else
    (void) cmd;
    return MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;
#endif
}


#ifdef USS_ST
/* Re-defining mailbox here
 * Ensure definition matches Hal_Mailbox.h
 */
typedef struct {
	_Bool data_rdy;
	_Bool message_is_IO_buf;
    mxq_u2 position;
    mxq_u2 size;
    mxq_u1* message;
} fw_mailbox_t;

extern fw_mailbox_t gMailboxIn;
extern fw_mailbox_t gMailboxOut;

extern int Receive_Process_Cmd(_Bool);

mxq_err_t __API__ MXQ_Bypass(mxq_u1** buf_p, 
							 mxq_u2* buf_len)
{
	mxq_err_t err = MXQ_OK;

    if (((*buf_p)[0] == 0xAA) && ((*buf_p)[1] == 0x00) && ((*buf_p)[2] == CMDNUM_TAMPER))
    {
        err = MXQ_ERR_INTERNAL_ERROR;
    }
    else
    {
        err = CMD_SendBypassCMD(buf_p, buf_len);
    }

	return err;
}
#endif /* USS_ST */

char* __API__ MXQ_get_str_code(int error_code)
{
	MXQ_PRINTF("Error: %d(%08x)", error_code, (unsigned int)error_code);
    switch(error_code){

    /* Global error codes */
    	case 0:  return "No Error";
        case -10: return "Application error"; /* i.e. not from MAXQ or API */

    /*MAXQ errors*/
		case MXQ_RSP_BAD_OBJ_ACCESS                 : return "";
		case MXQ_RSP_BAD_PARAM                      : return "ID already used or incorrect parameter";
		case MXQ_RSP_FS_WRONG_PARAM_ACCESS_COND     : return "";
		case MXQ_RSP_FS_WRONG_OBJECT_TYPE		    : return "";
		case MXQ_RSP_NOT_FOUND						: return "Object not found";
		case MXQ_RSP_FS_OUT_OF_BOUNDS				: return "Wrong offset or length";
		case MXQ_RSP_ERROR_UNKNOWN					: return "Unknown Error, or wrong length in parameter";
		case MXQ_RSP_MEMORY_FULL					: return "Memory is Full";
		case MXQ_RSP_OTHER_UNKNOWN_ERROR         	: return "Security condition not satisfied, access denied";
		case MXQ_RSP_NO_HASH_AVAIL					: return "Access denied, or No previously calculated Hash Available";
		case MXQ_RSP_WRONG_MAC						: return "No previously generated random Available / Wrong MAC";
		case MXQ_RSP_SIGNATURE_IS_WRONG			    : return "Signature is wrong / memory error";
		case MXQ_RSP_SC_ERROR                       : return "Secure Channel error";
		case MXQ_RSP_ERR_NOT_IMPLEMENTED            : return "Function not implemented";
		case MXQ_RSP_NOT_ALLOWED                    : return "Command not allowed in this context";
		case MXQ_RSP_CIPHER_BAD_INPUT_DATA			: return "Encoding Error in Server Parameters";
		case MXQ_RSP_ERR_WRONG_MODE                 : return "Wrong mode of operation (AES-SPI vs Crypto toolbox mode)";
		case MXQ_RSP_CR_NOK                         : return "Crypto operation failed";
		case MXQ_RSP_BAD_KEY_TYPE					: return "Unsupported key Type";
		case MXQ_RSP_CRYPTO_FAILURE			        : return "Error during cryptographic operation (bad key length, bad sequence, or other error)";
		case MXQ_RSP_ALGO_NOT_SUPPORTED			    : return "Algorithm not supported / mem alignment error or memory alignment error or memory access erro";
		case MXQ_RSP_WRONG_PARAMS					: return "Invalid Object ID";
		case MXQ_RSP_CR_PADDING_ERROR               : return "Padding error";
		case MXQ_RSP_WRONG_SIZE         		    : return "Inconsistent message length (crypto)";
		case MXQ_RSP_BUFFER_TOO_SMALL				: return "Signature Length Invalid";
		case MXQ_RSP_HASH_OUTPUT_LENGTH_TOO_SHORT   : return "Invalid hash algo vs EC curve length";
		case MXQ_RSP_CURVE_NOT_SUPPORTED     		: return "ECC Curve not supported";
		case MXQ_RSP_FS_NAME_ALREADY_EXISTS         : return "Name already used";
		case MXQ_RSP_FS_INVALID_OBJECT              : return "";
		case MXQ_RSP_COUNTER_OVERFLOW 				: return "Counter reach its limit";
		case MXQ_RSP_CONSISTENCY_ERROR				: return "Bad format of certificate data";
		case MXQ_RSP_OBJECT_EMPTY					: return "The Object is empty";
		case MXQ_RSP_INTERNAL_BUFFER_OVERFLOW       : return "Memory overflow issue";
		case MXQ_RSP_BUFFER_OVERFLOW                : return "Memory overflow issue";
		case MXQ_RSP_INVALID_SIZE					: return "Invalid size ( too big ), wrong param";
		case MXQ_RSP_ACC_DENIED                     : return "";
		case MXQ_RSP_WRONG_PARAM 					: return "Wrong command parameter";
		case MXQ_ERR_INTERNAL_ERROR			        : return "API internal error";
		case MXQ_ERR_WRONG_PARAMETER				: return "Wrong parameter provided to the function (bad pointer...)";
		case MXQ_ERR_BAD_LENGTH				   	    : return "Bad Length";
		case MXQ_ERR_TRANS_ERROR					: return "Generic Transmission error, Unsupported Interface (SPI,I2C), Interface not implemented";
		case MXQ_ERR_TRANS_BUFFER_OVERFLOW			: return "Buffer Overflow during receptio";
		case MXQ_ERR_TRANS_NOT_ENOUGH_DATA			: return "Not enough data received";
		case MXQ_ERR_TRANS_BAD_CRC					: return "Bad CRC";
		case MXQ_ERR_TRANS_RX_BAD_FORMAT			: return "Bad format of the received frame";
		case MXQ_ERR_TRANS_TIMEOUT					: return "Transmission Timeout";
		case MXQ_ERR_SECURE_CHANNEL_INTEGRITY		: return "Error when verifying SECURE CHANNEL integrity";
		case MXQ_ERR_GCU_TIMEOUT					: return "Timeout during GCU communication";
		case MXQ_ERR_GCU_ERROR_STATUS				: return "Bad GCU status";
		case MXQ_ERR_GCU_UNKNOWN_STATUS			    : return "Unknown GCU status";
		case MXQ_ERR_MODE				         	: return "Wrong mode  (aes spi vs crypto toolbox";

		/* Other errors */
        default: return " Unknown FATAL Error ";
    }
}


#undef _MXQ_API_C
