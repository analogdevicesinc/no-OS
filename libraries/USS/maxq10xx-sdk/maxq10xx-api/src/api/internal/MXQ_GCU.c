/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc., All rights Reserved.
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

/**
 * @internal
 * @file MXQ_GCU.c
 * @date June 2016
 * @authors Benjamin VINOT, Maxim Integrated Products Inc.
 *
 * @brief Handles GCU communications with the MAXQ106x.
 * @details Refer to the architecture diagram for more information.
 * @ingroup GCU
 */

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


mxq_err_t GCU_SendCommand(gcu_cmd_num_t command)
{
	mxq_err_t result = MXQ_OK;
	int i = 0;

	unsigned char data_in[1] = {0};
	unsigned char data_out[1] = {0};

	data_out[0] = command;
	data_in[0] = 0;

    DBG_COMM_PRINT("HOST->MAXQ(GCU command): ");

	while(data_in[0] == 0 && i < CONFIG_MAX_GCU_SEND_CMD_TRY)
	{
		exchangebytes(data_out, 1, data_in);
		i++;
	}

	DBG_COMM_PRINT("\n");
	
	if(i == CONFIG_MAX_GCU_SEND_CMD_TRY)
	{
		result = MXQ_ERR_GCU_TIMEOUT;
	}
	else if(data_in[0] == 0xFF)
	{
		result = MXQ_ERR_GCU_ERROR_STATUS;
	}
	else if(data_in[0] != 0xAA)
	{
		result = MXQ_ERR_GCU_UNKNOWN_STATUS;
	}

	return result;
}


mxq_err_t GCU_LoadKey(mxq_u1 *key)
{
    DBG_PRINT("\nGCU_LoadKey\n"); 
	mxq_err_t result;
	
	result = GCU_SendCommand(MXQ_GCU_LOAD_KEY_OPCODE);

	if(!result)
	{
		cmd_print_gcu("HOST->MAXQ (GCU data): ", key, MXQ_GCU_BLOCK_SIZE);

		result = sendbytes(key, MXQ_GCU_BLOCK_SIZE);
		if(result == MXQ_GCU_BLOCK_SIZE) result = MXQ_OK;
	}

	return result;
}


mxq_err_t GCU_LoadIV(mxq_u1 *iv)
{
    DBG_PRINT("\nGCU_LoadIV\n"); 
	mxq_err_t result;
	
	if(iv==NULL) return MXQ_ERR_WRONG_PARAMETER;
	
	result = GCU_SendCommand(MXQ_GCU_LOAD_IV_OPCODE);

	if(!result){
	
		cmd_print_gcu("HOST->MAXQ (GCU data): ", iv, MXQ_GCU_IV_LENGTH);

		result = sendbytes(iv, MXQ_GCU_IV_LENGTH);
		if(result == (int)MXQ_GCU_IV_LENGTH) result = MXQ_OK;
	}

	return result;
}


mxq_err_t GCU_Reset(void)
{
    DBG_PRINT("\nGCU_Reset\n"); 
	mxq_err_t result;

	unsigned char data_in[1]    = {0};
	unsigned char data_out[1] = {0};

	data_out[0] = MXQ_GCU_RESET_OPCODE;
	data_in[0]    = 0;

	cmd_print_gcu("HOST->MAXQ (GCU data): ", data_out, 1);
	
	result = exchangebytes(data_out, 1, data_in);

	cmd_print_gcu("MAXQ->HOST (GCU data): ", data_in, 1);

	return result;
}


mxq_err_t GCU_Shutdown()
{
    DBG_PRINT("\nGCU_Shutdown\n"); 
	mxq_err_t result;

	unsigned char data_in[1] = {0};
	unsigned char data_out[1] = {0};

	data_out[0] = MXQ_GCU_SHUTDOWN_OPCODE;
	data_in[0] = 0;
	
	cmd_print_gcu("HOST->MAXQ (GCU data): ", data_out, 1);
	
	result = exchangebytes(data_out, 1, data_in);

	cmd_print_gcu("MAXQ->HOST (GCU data): ", data_in, 1);

	return result;
}


mxq_err_t GCU_Abort(void)
{
    DBG_PRINT("\nGCU_Abort\n"); 
	return GCU_SendCommand(MXQ_GCU_ABORT_OPCODE);
}


mxq_err_t GCU_LinkTest(void)
{
    DBG_PRINT("\nGCU_LinkTest\n"); 
	return GCU_SendCommand(MXQ_GCU_LINK_OPCODE);
}


mxq_err_t GCU_SendBlock(const mxq_u1* block)
{
    DBG_PRINT("\nGCU_SendBlock\n"); 
	mxq_err_t result;

	result = GCU_SendCommand(MXQ_GCU_SEND_BLOCK_OPCODE);
	if(result != MXQ_OK) return result;
	
	cmd_print_gcu("HOST->MAXQ (GCU data): ", block, MXQ_GCU_BLOCK_SIZE);
	
	result = sendbytes(block, MXQ_GCU_BLOCK_SIZE);
	if(result == MXQ_GCU_BLOCK_SIZE) result = MXQ_OK;

	return result;
}


mxq_err_t GCU_RequestBlock(mxq_u1 *block)
{
    DBG_PRINT("\nGCU_RequestBlock\n"); 

	mxq_err_t result;

	result = GCU_SendCommand(MXQ_GCU_REQUEST_BLOCK_OPCODE);
	if(result != MXQ_OK) return result;

	result = receivebytes(block, MXQ_GCU_BLOCK_SIZE);

	cmd_print_gcu("\nMAXQ->HOST (GCU data): ", block, MXQ_GCU_BLOCK_SIZE);

	return result;
}


mxq_err_t GCU_RequestTag(mxq_u1 *tag)
{
    DBG_PRINT("\nGCU_RequestTag\n"); 

	mxq_err_t result;
	
	result = GCU_SendCommand(MXQ_GCU_REQUEST_TAG_OPCODE);
	if(result != MXQ_OK) return result;

	result = receivebytes(tag, MXQ_GCU_BLOCK_SIZE);
	
	cmd_print_gcu("MAXQ->HOST (GCU data): ", tag, MXQ_GCU_BLOCK_SIZE);

	return result;
}


mxq_err_t GCU_ECBEncDec(mxq_u1 mode, const mxq_u1* block)
{
    DBG_PRINT("\nGCU_ECBEncDec\n"); 

	mxq_err_t result;
	
	if(mode){
		result = GCU_SendCommand(MXQ_GCU_ECB_ENCR_OP_OPCODE);
	}else{
		result = GCU_SendCommand(MXQ_GCU_ECB_DECR_OP_OPCODE);
	}
	if(result != MXQ_OK) return result;

	cmd_print_gcu("HOST->MAXQ (GCU data): ", block, MXQ_GCU_BLOCK_SIZE);

	result = sendbytes(block, MXQ_GCU_BLOCK_SIZE);
	if(result == (int)MXQ_GCU_BLOCK_SIZE) result = MXQ_OK;

	return result;
}


mxq_err_t GCU_GCMEncDec(mxq_length data_length, const mxq_u1* block)
{
    DBG_PRINT("\nGCU_GCMEncDec\n"); 
	mxq_err_t result;
	mxq_u1 data[MXQ_GCU_BLOCK_SIZE + MXQ_GCU_LENGTH_FIELD_SIZE];
	const mxq_length DATA_LENGTH = MXQ_GCU_BLOCK_SIZE + MXQ_GCU_LENGTH_FIELD_SIZE;

	data[0] = (data_length >> 24) & 0xFF;
	data[1] = (data_length >> 16) & 0xFF;
	data[2] = (data_length >> 8) & 0xFF;
	data[3] = data_length & 0xFF;	

	MXQ_MEMCPY(&(data[4]), block, MXQ_GCU_BLOCK_SIZE);

	result = GCU_SendCommand(MXQ_GCU_ENCR_OP_OPCODE);
	if(result != MXQ_OK) return result;

	cmd_print_gcu("HOST->MAXQ (GCU data): ", data, DATA_LENGTH);
	
	result = sendbytes(data, DATA_LENGTH);
	if(result == (int)DATA_LENGTH) result = MXQ_OK;

	return result;

}


mxq_err_t GCU_GMAC(mxq_length aad_length, mxq_u1* aad_block)
{
    DBG_PRINT("\nGCU_GMAC\n"); 
	mxq_err_t result;
	mxq_u1 data[MXQ_GCU_BLOCK_SIZE + MXQ_GCU_LENGTH_FIELD_SIZE];
	const mxq_length DATA_LENGTH = MXQ_GCU_BLOCK_SIZE + MXQ_GCU_LENGTH_FIELD_SIZE;

	data[0] = (aad_length >> 24) & 0xFF;
	data[1] = (aad_length >> 16) & 0xFF;
	data[2] = (aad_length >> 8) & 0xFF;
	data[3] = aad_length & 0xFF;

	MXQ_MEMCPY(&(data[4]), aad_block, MXQ_GCU_BLOCK_SIZE);

	result = GCU_SendCommand(MXQ_GCU_AUTH_OP_OPCODE);
	if(result != MXQ_OK) return result;
	
	cmd_print_gcu("HOST->MAXQ (GCU data): ", data, DATA_LENGTH);

	result = sendbytes(data, DATA_LENGTH);
	if(result == (int)DATA_LENGTH) result = MXQ_OK;

	return result;

}


mxq_err_t GCU_GCMAuthEncDec(mxq_u1 mode, mxq_length aad_length, mxq_length data_length, const mxq_u1* aad_block)
{

   DBG_PRINT("\nGCU_GCMAuthEncDec\n"); 
	mxq_err_t result;
	mxq_u1 data[MXQ_GCU_BLOCK_SIZE + 2 * MXQ_GCU_LENGTH_FIELD_SIZE];
	const mxq_length DATA_LENGTH = MXQ_GCU_BLOCK_SIZE + 2 * MXQ_GCU_LENGTH_FIELD_SIZE;

	data[0] = (aad_length >> 24) & 0xFF;
	data[1] = (aad_length >> 16) & 0xFF;
	data[2] = (aad_length >>  8) & 0xFF;
	data[3] = aad_length & 0xFF;	
	data[4] = (data_length >> 24) & 0xFF;
	data[5] = (data_length >> 16) & 0xFF;
	data[6] = (data_length >>  8) & 0xFF;
	data[7] = data_length & 0xFF;

	MXQ_MEMCPY(&(data[8]), aad_block, MXQ_GCU_BLOCK_SIZE);

	if(mode){
		result = GCU_SendCommand(MXQ_GCU_AUTH_ENCR_OP_OPCODE);
	}else{
		result = GCU_SendCommand(MXQ_GCU_AUTH_DECR_OP_OPCODE);
	}
	if(result != MXQ_OK) return result;

	cmd_print_gcu("HOST->MAXQ (GCU data): ", data, DATA_LENGTH);

	result = sendbytes(data, DATA_LENGTH);
	if(result == (int)DATA_LENGTH) result = MXQ_OK;

	return result;
}

