/*******************************************************************************
* Copyright (C) 2018-2019 Maxim Integrated Products, Inc. (now owned by Analog Devices, Inc.),
* 2024 Analog Devices, Inc. This software is proprietary and
* confidential to Analog Devices, Inc. and its licensors. , All rights Reserved.
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

 * @file MXQ_Transport.c
 * @date Apr 2014
 * @authors Stéphane Di Vito, Maxim Integrated Products Inc.
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @brief Transport layer of the MAXQ106x's Host library.
 * @details Refer to the architecture diagram for more information.
 * @ingroup Transport
 * @internal
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

mxq_err_t init_interface(void)
{
	mxq_err_t err;

#ifdef USE_PROTOCOL_SPI
    err = HOST_init_spi();
#endif

#ifdef USE_PROTOCOL_I2C
    err = HOST_init_i2c();
#endif

#ifdef USE_PROTOCOL_PIPE
    err = HOST_init_pipe();
#endif

#ifdef USE_PROTOCOL_MAILBOX
    err = HOST_init_mailbox();
#endif

    return err;
}

mxq_err_t deinit_interface(void)
{
	mxq_err_t err;
	err = HOST_deinit_interface();
	return err;
}

mxq_err_t sendbytes(const mxq_u1* buf, mxq_length len)
{
	mxq_err_t err;

#ifdef USE_PROTOCOL_SPI
	err = HOST_send_bytes_spi(buf,len);
#endif

#ifdef USE_PROTOCOL_I2C
	err = HOST_send_bytes_i2c(buf,len);
#endif

#ifdef USE_PROTOCOL_PIPE
	err = HOST_send_bytes_pipe(buf,len);
#endif

#ifdef USE_PROTOCOL_MAILBOX
	err = HOST_send_bytes_mailbox(buf,len);
#endif

    return err;
}


mxq_err_t receivebytes_first(mxq_u1* buf, mxq_length len)
{
	mxq_err_t err;

#ifdef USE_PROTOCOL_SPI
    	err = HOST_receive_bytes_spi_first(buf,len);
#endif

#ifdef USE_PROTOCOL_I2C
    	err = HOST_receive_bytes_i2c_first(buf,len);
#endif

#ifdef USE_PROTOCOL_PIPE
    	err = HOST_receive_bytes_pipe_first(buf,len);
#endif

#ifdef USE_PROTOCOL_MAILBOX
    	err = HOST_receive_bytes_mailbox_first(buf,len);
#endif

    return err;
}


mxq_err_t exchangebytes(mxq_u1* bufsrc, mxq_length len, mxq_u1* bufdst)
{
	mxq_err_t err;

#ifdef USE_PROTOCOL_SPI
    	err = HOST_exchange_bytes_spi(bufsrc,len,bufdst);
#endif

#if defined (USE_PROTOCOL_I2C) || defined (USE_PROTOCOL_PIPE) || defined (USE_PROTOCOL_MAILBOX)
    	(void) bufsrc;
    	(void) len;
    	(void) bufdst;
    	err = MXQ_ERR_TRANS_UNSUPPORTED_INTERFACE;
#endif

    return err;
}


mxq_err_t receivebytes(mxq_u1* buf, mxq_length len)
{
	mxq_err_t err;

#ifdef USE_PROTOCOL_SPI
	err = HOST_receive_bytes_spi(buf,len);
#endif

#ifdef USE_PROTOCOL_I2C
	err = HOST_receive_bytes_i2c(buf,len);
#endif

#ifdef USE_PROTOCOL_PIPE
	err = HOST_receive_bytes_pipe(buf,len);
#endif

#ifdef USE_PROTOCOL_MAILBOX
	err = HOST_receive_bytes_mailbox(buf,len);
#endif


    return err;
}
