/**
* Copyright 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv904x_dfe_vswr.c
* \brief Contains VSWR feature related function implementations
*
* ADRV904X API Version: 2.10.0.4
*/

#include "adi_adrv904x_hal.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"


#include "../../private/bf/adrv904x_bf_tx_dfe_dig_regs.h"


#define ADI_FILE    ADI_ADRV904X_FILE_PRIVATE_DFE_VSWR


ADI_API adi_adrv904x_ErrAction_e adrv904x_VswrPlaybackDatNumOfSamplesInit(adi_adrv904x_Device_t* const device,
                                                                          const adi_adrv904x_TxChannels_e chanSel)
{

        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    adrv904x_BfTxDfeDigRegsChanAddr_e txDfeDigBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    
    recoveryAction = adrv904x_TxDfeDigBitfieldAddressGet(device, chanSel, &txDfeDigBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading dfe dig base address");
        return recoveryAction;
    }
    
    /* Program the number of samples to 0 */
    recoveryAction =  adrv904x_TxDfeDigRegs_TxVswrDataGenMaxSamples_BfSet(device,
                                                                          NULL,
                                                                          txDfeDigBaseAddr,
                                                                          0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing number of samples");
        return recoveryAction;
    }

    return recoveryAction;
}
