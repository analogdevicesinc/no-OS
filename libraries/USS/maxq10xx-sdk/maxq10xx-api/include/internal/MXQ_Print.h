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
 * @file MXQ_Print.h
 * @date Feb 2018
 * @authors Benjamin Vinot, Maxim Integrated Products Inc.
 *
 * @brief Provide display utilities function.
 * @details Provide print function to dump commuynication, and function to display MAXQ106x status.
 * @ingroup APDU
 */

#ifndef _MXQ_PRINT_H_
#define _MXQ_PRINT_H_


#ifdef __MINGW32__
#    include <windows.h>
#endif


/**
 * Debug output for commands sent in crypto toolbox mode, in a secure channel
 * @param identifier
 * @param data
 * @param length
 */
void cmd_print_seccmd(const char * identifier, const unsigned char * data, mxq_length length);

/**
 * Debug output for commands sent in crypto toolbox mode
 * @param identifier
 * @param data
 * @param blocklen
 */
void cmd_print(const char * identifier, const unsigned char * data, mxq_length blocklen);



/**
 * Debug output for commands sent in AES-SPI mode.
 * @param identifier
 * @param pdest
 * @param blocklen
 */
void cmd_print_gcu(const char * identifier, const unsigned char * pdest, mxq_length blocklen);


/**
 * Debug output for commands sent in AES-SPI mode.
 * @param buffer1
 * @param buffer2
 * @param len
 */
void cmd_print_array(const unsigned char * buffer1, const unsigned char * buffer2, unsigned int len);


void start_time(void);
void stop_time(void);

void start_time_all(void);
void stop_time_all(void);
void start_time_proc(void);
void stop_time_proc(void);


#if (MXQ_DUMP_DEBUG!=(-1))
extern int g_debugoutputlevel;
#define DBG_PRINT(...) {if(g_debugoutputlevel) MXQ_PRINTF(__VA_ARGS__);}
#define DBG_PRINT_BUFFER(b,l) MXQ_PRINTBUFFER(b,l)
#else
#define DBG_PRINT(...)
#define DBG_PRINT_BUFFER(b,l)
#endif

#if (MXQ_DUMP_COMM!=(-1))
extern int g_dump_comm;
#define DBG_COMM_PRINT(...)        {if(g_dump_comm) MXQ_PRINTF(__VA_ARGS__);}
#define DBG_COMM_PRINT_BUF(...)    {if(g_dump_comm) cmd_print_array(__VA_ARGS__);}
#define DBG_COMM_PRINT_(...)       {if(g_dump_comm) cmd_print_gcu(__VA_ARGS__);}
#define DBG_COMM_PRINT_CMD(...)    {if(g_dump_comm) cmd_print(__VA_ARGS__);}
#define DBG_COMM_PRINT_CMDSEC(...) {if(g_dump_comm) cmd_print_seccmd(__VA_ARGS__);}
#else
#define DBG_COMM_PRINT(...)
#define DBG_COMM_PRINT_BUF(...)
#define DBG_COMM_PRINT_(...)
#define DBG_COMM_PRINT_CMD(...)
#define DBG_COMM_PRINT_CMDSEC(...)
#define cmd_print_seccmd(...)
#define cmd_print(...)
#define cmd_print_gcu(...)
#define cmd_print_array(...)


#endif


#endif /* _MXQ_PRINT_H_ */
