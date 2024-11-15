/***************************************************************************//**
 *   @file   errno.h
 *   @brief  Error macro definition for ARM Compiler
********************************************************************************
 * Copyright (c) 2021-2022 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

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
