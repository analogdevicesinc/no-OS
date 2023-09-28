/* SPDX-License-Identifier: GPL-2.0 */
/**
* \file adi_adrv9025_error.h
* \brief ADRV9025 ERROR header file
*
* Extension of adi_common_error
*
* ADRV9025 API Version: 6.4.0.14
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_ERROR_H_
#define _ADI_ADRV9025_ERROR_H_

#include "adi_common_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Extending Common Errors for ADRV9025 */
#define ADI_ADRV9025_ERR_INV_TX_ATTEN_MODE         0x1001       /*!< Invalid Tx attenuation control mode found */
#define ADI_ADRV9025_ERR_GP_INTERRUPT              0x1002       /*!< GP interrupt error has been detected, more info can be found in adrv9025_GpIntHandler */
#define ADI_ADRV9025_ERR_SHARED_RESOURCE_RELEASE   0x1003       /*!< Error while releasing shared source */
#define ADI_ADRV9025_ERR_SHARED_RESOURCE_ACQUIRE   0x1004       /*!< Error while acquiring shared source */
#define ADI_ADRV9025_ERR_NUMBER_OF_ERRORS          0x1005       /*!< End of Errors for ADRV9025 */

/* Extending Common Actions for ADRV9025
 * TODO remove once common actions are associated */
#define ADI_ADRV9025_ACT_WARN_RERUN_TRCK_CAL         (2)         /*!< API NG - RESET  SPEC TRACK CALS */
#define ADI_ADRV9025_ACT_WARN_RESET_GPIO             (3)         /*!< API OK - GPIO Not working */
#define ADI_ADRV9025_ACT_WARN_BBIC_LOGERROR          (5)         /*!< API OK - Recommend for BBIC to log and track error frequency */
#define ADI_ADRV9025_ACT_WARN_CHECK_TX_DATA          (6)         /*!< API OK - Recommend for BBIC to check Tx data for SRL violations */
#define ADI_ADRV9025_ACT_ERR_RESET_CPU               (-2)        /*!< API NG - RESET CPU ONLY */
#define ADI_ADRV9025_ACT_ERR_RERUN_INIT_CALS         (-3)        /*!< API NG - RESET INIT CAL SEQ */
#define ADI_ADRV9025_ACT_ERR_RESET_SPI               (-4)        /*!< API NG - SPI Not Working */
#define ADI_ADRV9025_ACT_ERR_RESET_GPIO              (-5)        /*!< API NG - GPIO Not working */
#define ADI_ADRV9025_ACT_ERR_BBIC_LOG_ERROR          (-12)       /*!< API NG - USER Should log this error and decide recovery action */
#define ADI_ADRV9025_ACT_ERR_RESET_CACHE             (-14)       /*!< API NG - SPI CACHE Requires reset */
#define ADI_ADRV9025_ACT_ERR_FLUSH_CACHE             (-15)       /*!< API NG - Bitfield CACHE requires flushing */

/* Extending common error sources */
#define ADI_ADRV9025_SRC_AGC                         0x1001     /*!< Error detected in adi_adrv9025_agc.c */
#define ADI_ADRV9025_SRC_CPU                         0x1002     /*!< Error detected in adi_adrv9025_cpu.c */
#define ADI_ADRV9025_SRC_CALS                        0x1003     /*!< Error detected in adi_adrv9025_cals.c */
#define ADI_ADRV9025_SRC_DATAINTERFACE               0x1004     /*!< Error detected in adi_adrv9025_data_interface.c */
#define ADI_ADRV9025_SRC_DPD                         0x1005     /*!< Error detected in adi_adrv9025_dpd.c */
#define ADI_ADRV9025_SRC_GPIO                        0x1006     /*!< Error detected in adi_adrv9025_gpio.c */
#define ADI_ADRV9025_SRC_HAL                         0x1007     /*!< Error detected in adi_adrv9025_hal.c */
#define ADI_ADRV9025_SRC_PCA                         0x1008     /*!< Error detected in adi_adrv9025_pca.c */
#define ADI_ADRV9025_SRC_PHM                         0x1009     /*!< Error detected in adi_adrv9025_phm.c */
#define ADI_ADRV9025_SRC_RADIOCTRL                   0x100a     /*!< Error detected in adi_adrv9025_radioctrl.c */
#define ADI_ADRV9025_SRC_RX                          0x100b     /*!< Error detected in adi_adrv9025_rx.c */
#define ADI_ADRV9025_SRC_TX                          0x100c     /*!< Error detected in adi_adrv9025_tx.c */
#define ADI_ADRV9025_SRC_UTILITIES                   0x100d     /*!< Error detected in adi_adrv9025_utilities.c */

/* Extending feature errors*/
#define ADI_ADRV9025_SRC_CPUCMD                      0x1102     /*!< Error detected in adi_adrv9025_cpu.c  cpu cmd */
#define ADI_ADRV9025_SRC_CPUFWSTATUS                 0x1202     /*!< Error detected in adi_adrv9025_cpu.c  cpu start status check */
#define ADI_ADRV9025_SRC_CPUINITCALS                 0x1103     /*!< Error detected in adi_adrv9025_cals.c  cpu cmd init cals */

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_ERROR_H_ */
