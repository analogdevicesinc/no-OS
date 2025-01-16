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

*******************************************************************************
*/

#include "common.h"
#include "mbedtls/entropy.h"
#ifdef __MAXQ_USS__
#include "maxq10xx-api/include/MXQ_API.h"
#else
#include "maxq10xx_api/MXQ_API.h"
#endif


int is_module_init = 0; /* extern int is_module_init; */

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);

static int maxq_get_random(unsigned char *output, size_t output_len)
{
    mxq_err_t ret = 0;
    mxq_pu1 tmp[MAX_RANDOM_NUMBER_SIZE];

    if (!is_module_init) {
        MXQ_Module_Init();
        is_module_init = 1;
    }

    ret = MXQ_Get_Random((mxq_pu1)tmp, (mxq_length)output_len);
    if (ret != 0) {
        return ret;
    }

    /* random are copied in right buffer */
    memcpy(output, tmp, output_len);
    return ret;
}

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    mxq_err_t ret = 0;
    (void)data;

    ret = maxq_get_random(output, len);
    if(ret != 0) {
#if defined(MBEDTLS_ENTROPY_C)
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
#else
        return ret;
#endif
    }

    *olen = len;
    return ret;
}
