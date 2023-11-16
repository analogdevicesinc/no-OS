/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_eeprom_core.c
 * \brief Contains source code for configuring and initializing EEPROM clock device
 */
#include "adi_eeprom_core.h"
#include "adi_eeprom_error_types.h"
#include "adi_platform.h"
#include "../../../../platforms/platform_utils/adi_fmc_eeprom.h"
#include "adi_library.h"

#define ADI_EEPROM_LOGGING (uint32_t) (ADI_HAL_LOG_ALL) /* LogLevel Set to All */

#define ADI_FILE    ADI_EEPROM_FILE_PUBLIC_CORE

ADI_API adi_common_ErrAction_e adi_eeprom_HwOpen(adi_eeprom_Device_t* const device)
{
    adi_common_ErrAction_e recoveryAction       = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_common_ErrAction_e exitRecoveryAction   = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* ApiEnter ADI_FALSE => Do all the normal API enter jobs *except* take the mutex
     * This is because we don't have a mutex initialized until commonOpen completes. */
	recoveryAction = adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (ADI_COMMON_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }

    if (!ADI_COMMON_DEVICE_STATE_IS_OPEN(device->common))
    {
        recoveryAction = adi_common_hal_HwOpen(&device->common);
        if (recoveryAction != ADI_COMMON_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Common HAL HwOpen Issue");
            goto cleanup;
        }
        /* Indicate device is now open or else BoardInfoRead will fail */
        ADI_COMMON_DEVICE_STATE_OPEN_SET(device->common);

        /* Earliest Point for Logging */
        adi_common_LogLevelSet(&device->common, ADI_EEPROM_LOGGING);

        ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API);
    }

cleanup:
    /* ADI_FALSE to not release mutex as we did not take it at the top of the fn */
	exitRecoveryAction = adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);
    if (ADI_COMMON_ERR_ACT_NONE != exitRecoveryAction)
    {
        recoveryAction = exitRecoveryAction;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_eeprom_HwClose(adi_eeprom_Device_t* const device)
{
    adi_common_ErrAction_e recoveryAction       = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_common_ErrAction_e exitRecoveryAction   = ADI_COMMON_ERR_ACT_CHECK_PARAM;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    /* ApiEnter ADI_FALSE => Do all the normal API enter jobs *except* take the mutex
     * This is because we don't commonHwClose will be destroying the mutex. */
	recoveryAction = adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (ADI_COMMON_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }

    if (ADI_COMMON_DEVICE_STATE_IS_OPEN(device->common))
    {
        recoveryAction = adi_common_hal_HwClose(&device->common);
        if (ADI_COMMON_ERR_ACT_NONE != recoveryAction)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Common HAL HwClose Issue");
            goto cleanup;
        }
        /* Indicate the Device is now closed */
        ADI_COMMON_DEVICE_STATE_OPEN_CLR(device->common);
    }

cleanup:
    /* ADI_FALSE to not release mutex as we did not take it at the top of the fn */
	exitRecoveryAction = adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);
    if (ADI_COMMON_ERR_ACT_NONE != exitRecoveryAction)
    {
        recoveryAction = exitRecoveryAction;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
    }

    return recoveryAction;
}

ADI_API adi_common_ErrAction_e adi_eeprom_BoardInfoRead(adi_eeprom_Device_t* const device, adi_eeprom_BoardInfo_t* const boardInfo)
{
    adi_common_ErrAction_e recoveryAction = ADI_COMMON_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e halError = ADI_HAL_ERR_NULL_PTR;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_API_ENTER_RTN(&device->common);

    ADI_NULL_PTR_REPORT_GOTO(&device->common, boardInfo, cleanup);

    ADI_LIBRARY_MEMSET(boardInfo, 0, sizeof(adi_eeprom_BoardInfo_t));

    /* NOTE: EPRC Limitation; Cant typedef a platform structure; Can Cast once eeprom & hal structures remain 1:1 */
    halError = adi_platform_FmcEepromRead(device->common.devHalInfo, (adi_hal_BoardInfo_t*) boardInfo);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction  = ADI_COMMON_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   &device->common,
                            ADI_COMMON_ERRSRC_DEVICEHAL,
                            halError,
                            recoveryAction,
                            halError,
                            "EEPROM Read Issue");
        goto cleanup;
    }

    /* Store Latest Board Information in EEPROM Device */
    ADI_LIBRARY_MEMCPY(&device->devStateInfo.boardInfo, boardInfo, sizeof(adi_eeprom_BoardInfo_t));

cleanup:
    ADI_API_EXIT(&device->common, recoveryAction);
}