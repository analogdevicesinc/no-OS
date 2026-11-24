/**
 * \file
 * \brief Extending Common Actions for ADRV910X
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_ADRV910X_ERROR_H_
#define _ADI_ADRV910X_ERROR_H_

#include "adi_common_error.h"

/** \brief API OK - RESET SPEC TRACK CALS */
#define ADI_ADRV910X_ACT_WARN_RERUN_TRCK_CAL (ADI_COMMON_ACT_WARN_EXTENSION_1 + 3)
/** \brief API OK - GPIO Not working */
#define ADI_ADRV910X_ACT_WARN_RESET_GPIO (ADI_COMMON_ACT_WARN_EXTENSION_1 + 2)
/** \brief API OK - Recommend for BBIC to log and track error frequency */
#define ADI_ADRV910X_ACT_WARN_BBIC_LOGERROR (ADI_COMMON_ACT_WARN_EXTENSION_1 + 1)
/** \brief API NG - RESET ARM ONLY */
#define ADI_ADRV910X_ACT_ERR_RESET_ARM (ADI_COMMON_ACT_ERR_EXTENSION_1 - 1)
/** \brief API NG - RESET INIT CAL SEQ */
#define ADI_ADRV910X_ACT_ERR_RERUN_INIT_CALS (ADI_COMMON_ACT_ERR_EXTENSION_1 - 2)
/** \brief API NG - GPIO Not working */
#define ADI_ADRV910X_ACT_ERR_RESET_GPIO (ADI_COMMON_ACT_ERR_EXTENSION_1 - 3)
/** \brief API NG - USER Should log this error and decide recovery action */
#define ADI_ADRV910X_ACT_ERR_BBIC_LOG_ERROR (ADI_COMMON_ACT_ERR_EXTENSION_1 - 4)
/** \brief API NG - SPI CACHE Requires reset */
#define ADI_ADRV910X_ACT_ERR_RESET_CACHE (ADI_COMMON_ACT_ERR_EXTENSION_1 - 5)
/** \brief API NG - Bitfield CACHE requires flushing */
#define ADI_ADRV910X_ACT_ERR_FLUSH_CACHE (ADI_COMMON_ACT_ERR_EXTENSION_1 - 6)
/** \brief Error detected in adi_adrv910x_arm.c arm cmd */
#define ADI_ADRV910X_SRC_ARMCMD (ADI_COMMON_SRC_EXTENSION_1 + 0x11)
/** \brief Error detected in adi_adrv910x_arm.c arm start status check */
#define ADI_ADRV910X_SRC_ARMFWSTATUS (ADI_COMMON_SRC_EXTENSION_1 + 0x12)
/** \brief Error detected in adi_adrv910x_stream.c */
#define ADI_ADRV910X_SRC_STREAMCMD (ADI_COMMON_SRC_EXTENSION_1 + 0x20)

#endif /* _ADI_ADRV910X_ERROR_H_ */
