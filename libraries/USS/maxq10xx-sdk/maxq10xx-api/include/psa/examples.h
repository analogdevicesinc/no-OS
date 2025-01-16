/*******************************************************************************
* Copyright (C) 2018 Maxim Integrated Products, Inc. (now owned by Analog Devices, Inc.),
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
*******************************************************************************
*
*	@file		examples.h
*	@author		Benjamin VINOT - <benjamin.vinot@maximintegrated.com>
*   @date		Apr 9, 2018
*
*/


#ifndef __EXAMPLES_H_
#define __EXAMPLES_H_

#include "MXQ_API.h"

#define TEST_FAILED -1
#define ASSERTSUCCESS(fn) { int err; if((err=fn)!=0) {printf ("Error %04x @line %d in %s\n",err, __LINE__ , __FILE__ ); return TEST_FAILED;}}
#define ASSERTERROR(fn,status) { mxq_u2 err; if((err=fn)!=status) {printf ("Error condition %04x not verified (got %04x) @line %d in %s\n", status,err, __LINE__ , __FILE__);return TEST_FAILED;}}
#define ASSERTTRUE(x ) { int a=(x); if((a==0)) {printf ("Error condition not verified %d @line %d in %s\n",a, __LINE__ , __FILE__); return TEST_FAILED;}}

#define EXITPROG(err) { if(err) mxq_perror(err, __FILE__, __LINE__); exit(err);}


// The below keys are pre-loaded (public part only) in the MAXQ1061
#define KEYCOMPLEN 32  // ECC=secp256r1 key size in bytes

#define PUBKEYADMINID 0x1001
#define PUBKEYIMPORTOBJID 0x1000
#define KEYCOMPLEN 32  // ECC=secp256r1 key size in bytes

#define PUBKEYADMINID     0x1001

#define PUBKEYIMPORTOBJID 0x1000

extern mxq_u1 KeyPairAdmin[96];
extern mxq_u1 KeyPairImport[96];
extern mxq_u1 KeySecureChannel[32];
extern mxq_u1 KeyPairSecureBoot[96];

int Administrator_Authentication(void);
int ECDSA_sign_secp256r1_sha256(mxq_u1* dest, mxq_u1* key, mxq_u1* data, mxq_length data_length);

void print_buffer(const unsigned char * buffer,unsigned int len);
void mxq_perror(mxq_u2 error_code, char* file, int line);
#endif /* __EXAMPLES_H_ */
