/** 
 * \file adrv904x_bf_cduc_funcs.c Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#include "../../private/bf/adrv904x_bf_cduc_funcs.h"
#include "adi_common_error.h"
#include "adi_adrv904x_error.h"

#include "adrv904x_bf_error_types.h"

#define ADI_FILE ADI_ADRV904X_FILE_PRIVATE_BF_CDUC_FUNCS

ADI_API adi_adrv904x_ErrAction_e adrv904x_CducFuncs_CducJesdClkDiv_BfSet(adi_adrv904x_Device_t* const device,
                                                                         adi_adrv904x_SpiCache_t* const spiCache,
                                                                         const adrv904x_BfCducFuncsChanAddr_e baseAddr,
                                                                         const adrv904x_Bf_CducFuncs_CducJesdClkDiv_e bfValue)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_BF);

#if ADRV904X_BITFIELD_VALUE_CHECK > 0
    if (
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_1                       ) &&
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_2                       ) &&
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_4                       ) &&
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_8                       ) &&
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_16                      ) &&
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_32                      ) &&
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_64                      ) &&
            (bfValue != ADRV904X_BF_CDUC_FUNCS_CDUC_JESD_CLK_DIV_DIV_BY_128                     )  
           )
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM, 
                         bfValue,
                         "Invalid BitField Value in adrv904x_CducFuncs_CducJesdClkDiv_BfSet");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_VALUE_CHECK */

#if ADRV904X_BITFIELD_ADDR_CHECK > 0
    if ((baseAddr != ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_1__TX_CDUC_TX_CDUC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_2__TX_CDUC_TX_CDUC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_3__TX_CDUC_TX_CDUC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_4__TX_CDUC_TX_CDUC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_5__TX_CDUC_TX_CDUC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_6__TX_CDUC_TX_CDUC_FUNCS) &&
            (baseAddr != ADRV904X_BF_SLICE_TX_7__TX_CDUC_TX_CDUC_FUNCS))
    {
        ADI_ERROR_REPORT(&device->common, 
                         ADI_ADRV904X_ERRSRC_DEVICEBF, 
                         ADI_COMMON_ERRCODE_INVALID_PARAM, 
                         ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                         baseAddr, 
                         "Invalid adrv904x_CducFuncs_CducJesdClkDiv_BfSet Address");
        return recoveryAction;
    }
#endif /* ADRV904X_BITFIELD_ADDR_CHECK */ 

    recoveryAction = adi_adrv904x_Register32Write(device,
                                                  spiCache,
                                                  ((uint32_t) baseAddr + 0x1B0U),
                                                  ((uint32_t) bfValue << 4),
                                                  0x70U);
    if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register32Write Issue");
        return recoveryAction;
    }

    return recoveryAction;
}

