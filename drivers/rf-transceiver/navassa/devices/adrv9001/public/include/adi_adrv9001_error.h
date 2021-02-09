/**
* \file
* \brief ADRV9001 ERROR header file
*
* Extension of adi_common_error
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_ERROR_H_
#define _ADI_ADRV9001_ERROR_H_

#include "adi_common_error.h"

/* Extending Common Errors for ADRV9001 */
#define ADI_ADRV9001_ERR_INV_TX_ATTEN_MODE          (ADI_COMMON_ERR_EXTENSION_1 + 0x01)     /*!< Invalid Tx attenuation control mode found */
#define ADI_ADRV9001_ERR_GP_INTERRUPT               (ADI_COMMON_ERR_EXTENSION_1 + 0x02)     /*!< GP interrupt error has been detected, more info can be found in adrv9001_GpIntHandler */
#define ADI_ADRV9001_ERR_SHARED_RESOURCE_RELEASE    (ADI_COMMON_ERR_EXTENSION_1 + 0x03)     /*!< Error while releasing shared source */
#define ADI_ADRV9001_ERR_SHARED_RESOURCE_ACQUIRE    (ADI_COMMON_ERR_EXTENSION_1 + 0x04)     /*!< Error while acquiring shared source */
#define ADI_ADRV9001_ERR_NUMBER_OF_ERRORS           (ADI_COMMON_ERR_EXTENSION_1 + 0x05)     /*!< End of Errors for ADRV9001 */

/* Extending Common Actions for ADRV9001 */
#define ADI_ADRV9001_ACT_WARN_RERUN_TRCK_CAL         (ADI_COMMON_ACT_WARN_EXTENSION_1 + 3)  /*!< API OK - RESET  SPEC TRACK CALS */
#define ADI_ADRV9001_ACT_WARN_RESET_GPIO             (ADI_COMMON_ACT_WARN_EXTENSION_1 + 2)  /*!< API OK - GPIO Not working */
#define ADI_ADRV9001_ACT_WARN_BBIC_LOGERROR          (ADI_COMMON_ACT_WARN_EXTENSION_1 + 1)  /*!< API OK - Recommend for BBIC to log and track error frequency */
#define ADI_ADRV9001_ACT_ERR_RESET_ARM               (ADI_COMMON_ACT_ERR_EXTENSION_1 - 1)   /*!< API NG - RESET ARM ONLY */
#define ADI_ADRV9001_ACT_ERR_RERUN_INIT_CALS         (ADI_COMMON_ACT_ERR_EXTENSION_1 - 2)   /*!< API NG - RESET INIT CAL SEQ */
#define ADI_ADRV9001_ACT_ERR_RESET_GPIO              (ADI_COMMON_ACT_ERR_EXTENSION_1 - 3)   /*!< API NG - GPIO Not working */
#define ADI_ADRV9001_ACT_ERR_BBIC_LOG_ERROR          (ADI_COMMON_ACT_ERR_EXTENSION_1 - 4)   /*!< API NG - USER Should log this error and decide recovery action */
#define ADI_ADRV9001_ACT_ERR_RESET_CACHE             (ADI_COMMON_ACT_ERR_EXTENSION_1 - 5)   /*!< API NG - SPI CACHE Requires reset */
#define ADI_ADRV9001_ACT_ERR_FLUSH_CACHE             (ADI_COMMON_ACT_ERR_EXTENSION_1 - 6)   /*!< API NG - Bitfield CACHE requires flushing */

/* Extending common error sources */
#define ADI_ADRV9001_SRC_AGC                         (ADI_COMMON_SRC_EXTENSION_1 + 0x01)    /*!< Error detected in adi_adrv9001_agc.c */
#define ADI_ADRV9001_SRC_ARM                         (ADI_COMMON_SRC_EXTENSION_1 + 0x02)    /*!< Error detected in adi_adrv9001_arm.c */
#define ADI_ADRV9001_SRC_CALS                        (ADI_COMMON_SRC_EXTENSION_1 + 0x03)    /*!< Error detected in adi_adrv9001_cals.c */
#define ADI_ADRV9001_SRC_DATAINTERFACE               (ADI_COMMON_SRC_EXTENSION_1 + 0x04)    /*!< Error detected in adi_adrv9001_data_interface.c */
#define ADI_ADRV9001_SRC_DPD                         (ADI_COMMON_SRC_EXTENSION_1 + 0x05)    /*!< Error detected in adi_adrv9001_dpd.c */
#define ADI_ADRV9001_SRC_GPIO                        (ADI_COMMON_SRC_EXTENSION_1 + 0x06)    /*!< Error detected in adi_adrv9001_gpio.c */
#define ADI_ADRV9001_SRC_HAL                         (ADI_COMMON_SRC_EXTENSION_1 + 0x07)    /*!< Error detected in adi_adrv9001_hal.c */
#define ADI_ADRV9001_SRC_PCA                         (ADI_COMMON_SRC_EXTENSION_1 + 0x08)    /*!< Error detected in adi_adrv9001_pca.c */
#define ADI_ADRV9001_SRC_PHM                         (ADI_COMMON_SRC_EXTENSION_1 + 0x09)    /*!< Error detected in adi_adrv9001_phm.c */
#define ADI_ADRV9001_SRC_RADIOCTRL                   (ADI_COMMON_SRC_EXTENSION_1 + 0x0a)    /*!< Error detected in adi_adrv9001_radioctrl.c */
#define ADI_ADRV9001_SRC_RX                          (ADI_COMMON_SRC_EXTENSION_1 + 0x0b)    /*!< Error detected in adi_adrv9001_rx.c */
#define ADI_ADRV9001_SRC_TX                          (ADI_COMMON_SRC_EXTENSION_1 + 0x0c)    /*!< Error detected in adi_adrv9001_tx.c */
#define ADI_ADRV9001_SRC_UTILITIES                   (ADI_COMMON_SRC_EXTENSION_1 + 0x0d)    /*!< Error detected in adi_adrv9001_utilities.c */
#define ADI_ADRV9001_SRC_INIT                        (ADI_COMMON_SRC_EXTENSION_1 + 0x0e)    /*!< Error detected in adi_adrv9001.c adrv9001_init.c */


/* Extending feature errors*/
#define ADI_ADRV9001_SRC_ARMCMD                      (ADI_COMMON_SRC_EXTENSION_1 + 0x11)    /*!< Error detected in adi_adrv9001_arm.c  arm cmd */
#define ADI_ADRV9001_SRC_ARMFWSTATUS                 (ADI_COMMON_SRC_EXTENSION_1 + 0x12)    /*!< Error detected in adi_adrv9001_arm.c  arm start status check */
#define ADI_ADRV9001_SRC_ARMINITCALS                 (ADI_COMMON_SRC_EXTENSION_1 + 0x13)    /*!< Error detected in adi_adrv9001_cals.c  arm cmd init cals */

#define ADI_ADRV9001_SRC_STREAMCMD                   (ADI_COMMON_SRC_EXTENSION_1 + 0x20)    /*!< Error detected in adi_adrv9001_stream.c */

#endif /* _ADI_ADRV9001_ERROR_H_ */
