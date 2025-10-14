/**
* \file
* \brief ADRV910X ERROR header file
*
* Extension of adi_common_error
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV910X_ERROR_H_
#define _ADI_ADRV910X_ERROR_H_

#include "adi_common_error.h"

/* Extending Common Actions for ADRV910X */
#define ADI_ADRV910X_ACT_WARN_RERUN_TRCK_CAL         (ADI_COMMON_ACT_WARN_EXTENSION_1 + 3)  /*!< API OK - RESET  SPEC TRACK CALS */
#define ADI_ADRV910X_ACT_WARN_RESET_GPIO             (ADI_COMMON_ACT_WARN_EXTENSION_1 + 2)  /*!< API OK - GPIO Not working */
#define ADI_ADRV910X_ACT_WARN_BBIC_LOGERROR          (ADI_COMMON_ACT_WARN_EXTENSION_1 + 1)  /*!< API OK - Recommend for BBIC to log and track error frequency */
#define ADI_ADRV910X_ACT_ERR_RESET_ARM               (ADI_COMMON_ACT_ERR_EXTENSION_1 - 1)   /*!< API NG - RESET ARM ONLY */
#define ADI_ADRV910X_ACT_ERR_RERUN_INIT_CALS         (ADI_COMMON_ACT_ERR_EXTENSION_1 - 2)   /*!< API NG - RESET INIT CAL SEQ */
#define ADI_ADRV910X_ACT_ERR_RESET_GPIO              (ADI_COMMON_ACT_ERR_EXTENSION_1 - 3)   /*!< API NG - GPIO Not working */
#define ADI_ADRV910X_ACT_ERR_BBIC_LOG_ERROR          (ADI_COMMON_ACT_ERR_EXTENSION_1 - 4)   /*!< API NG - USER Should log this error and decide recovery action */
#define ADI_ADRV910X_ACT_ERR_RESET_CACHE             (ADI_COMMON_ACT_ERR_EXTENSION_1 - 5)   /*!< API NG - SPI CACHE Requires reset */
#define ADI_ADRV910X_ACT_ERR_FLUSH_CACHE             (ADI_COMMON_ACT_ERR_EXTENSION_1 - 6)   /*!< API NG - Bitfield CACHE requires flushing */

/* Extending feature errors*/
#define ADI_ADRV910X_SRC_ARMCMD                      (ADI_COMMON_SRC_EXTENSION_1 + 0x11)    /*!< Error detected in adi_adrv910x_arm.c  arm cmd */
#define ADI_ADRV910X_SRC_ARMFWSTATUS                 (ADI_COMMON_SRC_EXTENSION_1 + 0x12)    /*!< Error detected in adi_adrv910x_arm.c  arm start status check */

#endif /* _ADI_ADRV910X_ERROR_H_ */
