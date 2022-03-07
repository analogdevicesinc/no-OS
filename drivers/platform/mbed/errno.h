/***************************************************************************//**
 *   @file   errno.h
 *   @brief  Error macro definition for ARM Compiler
********************************************************************************
 * Copyright (c) 2021-2022 Analog Devices, Inc.
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

#ifndef MBED_ERRNO_H_
#define MBED_ERRNO_H_

// Platform drivers needs to be C-compatible to work with other drivers
#ifdef __cplusplus
extern "C"
{
#endif //  _cplusplus

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#if defined(__ARMCC_VERSION)
#if !defined(__ELASTERROR)
#define __ELASTERROR 2000
#endif
#if !defined(ENOTSUP)
#define ENOTSUP 9926
#endif
#if !defined(EBADMSG)
#define EBADMSG 9905
#endif
#if !defined(ETIME)
#define ETIME 9935
#endif
#if !defined(EACCES)
#define EACCES 9973
#endif
#if !defined(ETIMEDOUT)
#define ETIMEDOUT 9938
#endif
#if !defined(ENODEV)
#define ENODEV 9967
#endif
#if !defined(EFAULT)
#define EFAULT 9948
#endif
#if !defined(EIO)
#define EIO 9961
#endif
#if !defined(ENOENT)
#define ENOENT 9968
#endif
#if !defined(EBUSY)
#define EBUSY 9952
#endif
#if !defined(EAGAIN)
#define EAGAIN 9976
#endif
#if !defined(EINVAL)
#define EINVAL 9943
#endif
#if !defined(ENOMEM)
#define ENOMEM 9971
#endif
#if !defined(ENOSYS)
#define ENOSYS 88
#endif
#if !defined(ENOTCONN)
#define ENOTCONN 128
#endif

// End of defined(__ARMCC_VERSION)
#endif

#include_next <errno.h>

#ifdef __cplusplus
}
#endif //  _cplusplus

#endif // MBED_ERRNO_H_
