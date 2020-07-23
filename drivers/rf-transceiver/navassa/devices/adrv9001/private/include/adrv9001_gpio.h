/**
* \file
* \brief Contains ADRV9001 transmit related function prototypes for
*        adrv9001_gpio.c
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*
* Copyright 2015-2018 Analog Devices Inc.
* Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file.
*/

#ifndef _ADRV9001_GPIO_H_
#define _ADRV9001_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#include "adi_common_error.h"
#include "adi_adrv9001_gpio_types.h"
#include "adi_adrv9001_types.h"

/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

/**
* \brief Private function called by API's adi_adrv9001_gpio_GpIntHandler.  This
* function performs all possible recover actions for any GP_INT(General Purpose Interrupt) source as well
* as returning the required BBIC(Baseband IC) recovery action.
*
* \pre This function can only be called by the adi_adrv9001_gpio_GpIntHandler.
*
* \param device Pointer to the ADRV9001 data structure
* \param gpIntStatus Pointer containing all information for the GP interrupt.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for ARM/WATCHDOG errors
* \retval ADI_COMMON_ACT_ERR_RERUN_INIT_CALS Recovery actions for INIT CALIBRATION errors
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_GpIntHandler(adi_adrv9001_Device_t *device, adi_adrv9001_gpIntStatus_t *gpIntStatus);

/**
 * \brief general purpose interrupt status word readback
 *
 * \param device   Pointer to the ADRV9001 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *                 The parameter is of type 'adrv9001_BfNvsRegmapCore1ChanAddr_e'
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset.
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check.
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
 * \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required.
 *
 */
int32_t adrv9001_GpInterruptsMaskPinBfGet(adi_adrv9001_Device_t *device, uint32_t *bfValue);

/**
* \brief Logic 1 masks the specific interrupt, per bit position, for output on gp_interrupt[0]
*
* \param device   Pointer to the ADRV9001 device data structure.
* \param baseAddr Base Address of instance to be configured.
*                 The parameter is of type 'adrv9001_BfNvsRegmapCore1ChanAddr_e'
* \param bfValue  Data to be configured. Parameter is of type uint32_t.
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset.
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check.
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
* \retval ADI_COMMON_ACT_NO_ACTION Function  Completed successfully, no action required.
*
*/
int32_t adrv9001_GpInterruptsMaskPinBfSet(adi_adrv9001_Device_t *device, uint32_t bfValue);


/**
 * \brief general purpose interrupt status word readback
 *
 * \param device   Pointer to the ADRV9001 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *                 The parameter is of type 'adrv9001_BfNvsRegmapCore1ChanAddr_e'
 * \param bfValue  Data to be configured. Parameter is of type uint64_t.
 *
 * \retval ADI_COMMON_ACT_WARN_RESET_LOG      Recovery action for log reset.
 * \retval ADI_COMMON_ACT_ERR_CHECK_PARAM     Recovery action for bad parameter check.
 * \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required.
 * \retval ADI_COMMON_ACT_NO_ACTION           Function completed successfully, no action required.
 *
 */
int32_t adrv9001_GpInterruptsStatusWordBfGet(adi_adrv9001_Device_t *device, uint32_t *bfValue);

#ifdef __cplusplus
}
#endif

#endif /* _ADRV9001_GPIO_H_ */
