 /**
 * \file adrv904x_platform_pack.h
 * 
 * \brief   Contains platform pack define
 *
 * \details Contains platform pack define
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2019 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_PLATFORM_PACK_H__
#define __ADRV904X_PLATFORM_PACK_H__

#ifndef ADI_ADRV904X_FW
#include "adi_adrv904x_user.h"
#ifdef ADI_ADRV904X_PACKED
#define ADRV904X_PACKED ADI_ADRV904X_PACKED
#endif
#endif

#ifndef ADRV904X_PACKED
#ifdef __GNUC__
#define ADRV904X_PACKED(d) _Pragma("pack(1)") d _Pragma("pack()")
#elif defined  __ICCARM__
/*
 * Error[Pm154]: in the definition of a function-like macro, each instance of a
 * parameter shall be enclosed in parenthesis (MISRA C 2004 rule 19.10)
 *
 * ADRV904X_PACKED() is a macro used for structure packing. The parameter
 * for this macro must be a structure definition, which cannot be enclosed in
 * parenthesis (syntatically invalid).
 */
#pragma diag_suppress=Pm154
#define ADRV904X_PACKED(d) _Pragma("pack(1)") d _Pragma("pack()")
#pragma diag_default=Pm154
#elif defined _MSC_VER
#define ADRV904X_PACKED(d) __pragma(pack(1)) d __pragma(pack())
#else
#error( "Define the ADRV904X_PACKED() macro for your compiler.")
#endif
#endif

#endif /* __ADRV904X_PLATFORM_PACK_H__ */
