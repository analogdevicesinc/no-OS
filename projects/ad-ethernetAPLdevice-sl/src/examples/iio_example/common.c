/***************************************************************************//**
 *   @file   common.c
 *   @brief  Common utility file for Hockey Puck application.
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_error.h"

/* Debug string buffer */
char aDebugString[150u];

/* Common error string constants */
const char * err_strings[] = {
    "NOERR",
    "ENOENT",
    "EIO",
    "EAGAIN",
    "ENOMEM",
    "EBUSY",
    "ENODEV",
    "EINVAL",
    "ENOSYS",
    "EBADMSG",
    "ENOTCONN",
    "EPROTONOSUPPORT",
    "ETIMEDOUT",
    "ECONNREFUSED",
    "UNKNOWN ERROR",
};

/**
 * @brief   Return error strings for error code.
 */
const char * error_to_str(int32_t err_code)
{
    const char * err_str = err_strings[0];

    switch (-err_code)
    {
        case 0:
        err_str = err_strings[0];
        break;

        case ENOENT:
        err_str = err_strings[1];
        break;

        case EIO:
        err_str = err_strings[2];
        break;

        case EAGAIN:
        err_str = err_strings[3];
        break;

        case ENOMEM:
        err_str = err_strings[4];
        break;

        case EBUSY:
        err_str = err_strings[5];
        break;

        case ENODEV:
        err_str = err_strings[6];
        break;

        case EINVAL:
        err_str = err_strings[7];
        break;

        case ENOSYS:
        err_str = err_strings[8];
        break;

        case EBADMSG:
        err_str = err_strings[9];
        break;

        case ENOTCONN:
        err_str = err_strings[10];
        break;

        case EPROTONOSUPPORT:
        err_str = err_strings[11];
        break;

        case ETIMEDOUT:
        err_str = err_strings[12];
        break;

        case ECONNREFUSED:
        err_str = err_strings[13];
        break;

        default:
        err_str = err_strings[(sizeof(err_strings) / sizeof(err_strings[0])) - 1];
        break;
    }

    return err_str;
}
