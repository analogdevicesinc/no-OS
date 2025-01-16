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

#define _DRIVER_CRC_C

#include "MXQ_Internal.h"

/**
 *
 * @internal
 * @file
 * @date Apr 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 *
 * @brief CRC-16 using polynom 8005
 * @details
 * @ingroup drivers
 * @internal
 */

#define CRC16 0x8005

PRIVATE mxq_u2 outputval;

PRIVATE mxq_u2 get_crc( mxq_pu1 data, mxq_length size)
{
	int bits_read = 0, bit_flag;
    int i, j;
    mxq_u2 output_bak;
    mxq_u2 crc = 0;

    while(size > 0)
    {
        bit_flag = outputval >> 15;
        outputval <<= 1;
        outputval |= (*data >> bits_read) & 1;
        bits_read++;

        if(bits_read > 7)
        {
            bits_read = 0;
            data++;
            size--;
        }

        if(bit_flag)
            outputval ^= CRC16;
    }


    output_bak = outputval;

    for (i = 0; i < 16; ++i)
    {
        bit_flag = output_bak >> 15;
        output_bak <<= 1;

        if(bit_flag)
            output_bak ^= CRC16;
    }

    i = 0x8000;

    j = 0x0001;

    for (; i != 0; i >>=1, j <<= 1)
    {
        if (i & output_bak) crc |= j;
    }

    return crc;
}

mxq_u2 HOST_CRCGet(mxq_pu1 data, mxq_length length)
{
	outputval=0x0000;

	return get_crc(data,length);
}


#undef DRIVER_CRC_C
