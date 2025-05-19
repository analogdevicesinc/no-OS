/**
 * \file adi_apollo_platform_pack.h
 *
 * \brief   Contains platform pack define
 *
 * \details Contains platform pack define
 *
 * APOLLO API Version: $ADI_APOLLO_API_VERSION$
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2019 Analog Devices Inc.
 * Released under the APOLLO API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADI_APOLLO_PLATFORM_PACK_H__
#define __ADI_APOLLO_PLATFORM_PACK_H__

#ifndef ADI_APOLLO_FW
#include "adi_apollo_user.h"
#endif


#if (!defined(ADI_APOLLO_PACK_START) || !defined(ADI_APOLLO_PACK_FINISH))
#ifdef __GNUC__
#define ADI_APOLLO_PACK_START     _Pragma("pack(1)")
#define ADI_APOLLO_PACK_FINISH    _Pragma("pack()")
#elif defined  __ICCARM__
#define ADI_APOLLO_PACK_START     _Pragma("pack(1)")
#define ADI_APOLLO_PACK_FINISH    _Pragma("pack()")
#elif defined _MSC_VER
#define ADI_APOLLO_PACK_START     __pragma(pack(1))
#define ADI_APOLLO_PACK_FINISH    __pragma(pack())
#else
#error ( "Define the ADI_APOLLO_PACK_START and ADI_APOLLO_PACK_FINISH macros for your compiler." )
#endif
#endif

#ifndef ADI_APOLLO_PACKED
#if defined  __ICCARM__
/*
 * Error[Pm154]: in the definition of a function-like macro, each instance of a
 * parameter shall be enclosed in parenthesis (MISRA C 2004 rule 19.10)
 *
 * ADI_APOLLO_PACKED() is a macro used for structure packing. The parameter
 * for this macro must be a structure definition, which cannot be enclosed in
 * parenthesis (syntatically invalid).
 */
#pragma diag_suppress=Pm154
#define ADI_APOLLO_PACKED(d)    _Pragma("pack(1)") d _Pragma("pack()")
#pragma diag_default=Pm154
#else
#define ADI_APOLLO_PACKED(d)    ADI_APOLLO_PACK_START d ADI_APOLLO_PACK_FINISH
#endif
#endif

#endif /* __ADI_APOLLO_PLATFORM_PACK_H__ */
