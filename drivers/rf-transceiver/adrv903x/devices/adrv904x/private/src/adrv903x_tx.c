/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adrv903x_tx.c
*
* ADRV903X API Version: 2.12.1.4
*/
#include "../../private/include/adrv903x_tx.h"
#include "../../private/bf/adrv903x_bf_pll_mem_map.h"
#include "../../private/include/adrv903x_cpu.h"

#include "adi_adrv903x_tx_types.h"
#include "adi_adrv903x_tx.h"

#define ADI_FILE    ADI_ADRV903X_FILE_PRIVATE_TX

ADI_API uint32_t adrv903x_txAttenAddrLookup(const adi_adrv903x_TxChannels_e txChannel)
{
    uint32_t retval = 0U;

    switch (txChannel)
    {
        case ADI_ADRV903X_TX0:
            retval = ADRV903X_ADDR_TX0_ATTEN_TABLE;
            break;

        case ADI_ADRV903X_TX1:
            retval = ADRV903X_ADDR_TX1_ATTEN_TABLE;
            break;

        case ADI_ADRV903X_TX2:
            retval = ADRV903X_ADDR_TX2_ATTEN_TABLE;
            break;

        case ADI_ADRV903X_TX3:
            retval = ADRV903X_ADDR_TX3_ATTEN_TABLE;
            break;

        case ADI_ADRV903X_TX4:
            retval = ADRV903X_ADDR_TX4_ATTEN_TABLE;
            break;

        case ADI_ADRV903X_TX5:
            retval = ADRV903X_ADDR_TX5_ATTEN_TABLE;
            break;

        case ADI_ADRV903X_TX6:
            retval = ADRV903X_ADDR_TX6_ATTEN_TABLE;
            break;

        case ADI_ADRV903X_TX7:
            retval = ADRV903X_ADDR_TX7_ATTEN_TABLE;
            break;

        default:
            retval = 0U;
            break;
    }

    return retval;
}


ADI_API uint8_t adrv903x_TxChannelsToId(const adi_adrv903x_TxChannels_e txChannel)
{
    switch (txChannel)
    {
    case ADI_ADRV903X_TX0:
        return 0;
        break;

    case ADI_ADRV903X_TX1:
        return 1;
        break;

    case ADI_ADRV903X_TX2:
        return 2;
        break;

    case ADI_ADRV903X_TX3:
        return 3;
        break;

    case ADI_ADRV903X_TX4:
        return 4;
        break;

    case ADI_ADRV903X_TX5:
        return 5;
        break;

    case ADI_ADRV903X_TX6:
        return 6;
        break;

    case ADI_ADRV903X_TX7:
        return 7;
        break;

    default:
        return ADI_ADRV903X_TX_CHAN_ID_MAX + 1;
        break;
    }
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxBitfieldAddressGet(adi_adrv903x_Device_t* const    device,
                                                               const adi_adrv903x_TxChannels_e txChannel,
                                                               adrv903x_BfTxChanAddr_e* const  txChannelBitfieldAddr)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check tx bitfield channel address pointer is not null */
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txChannelBitfieldAddr);

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               device->devStateInfo.initializedChannels,
                               "Selected Tx channel is not initialized for this device.");
        return recoveryAction;
    }

    switch (txChannel)
    {
        case ADI_ADRV903X_TX0:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH0;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        case ADI_ADRV903X_TX1:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH1;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        case ADI_ADRV903X_TX2:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH2;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        case ADI_ADRV903X_TX3:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH3;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        case ADI_ADRV903X_TX4:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH4;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        case ADI_ADRV903X_TX5:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH5;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        case ADI_ADRV903X_TX6:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH6;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        case ADI_ADRV903X_TX7:
            *txChannelBitfieldAddr = ADRV903X_BF_TX_CH7;
            recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
            break;

        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    txChannel,
                                    "Invalid txChannel parameter for adrv903x_TxBitfieldAddressGet() function");
            return recoveryAction;
            break;

    }

    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxFuncsBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                    const adi_adrv903x_TxChannels_e txChannel,
                                                                    adrv903x_BfTxFuncsChanAddr_e* const txFuncsChannelBitfieldAddr)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txFuncsChannelBitfieldAddr);
    
    if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               device->devStateInfo.initializedChannels,
                               "Selected Tx channel is not initialized for this device.");
        return recoveryAction;
    }
    
    switch (txChannel)
    {
    case ADI_ADRV903X_TX0:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
        break;
            
    case ADI_ADRV903X_TX1:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_1__TX_FUNCS;
        break;
            
    case ADI_ADRV903X_TX2:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_2__TX_FUNCS;
        break;
    case ADI_ADRV903X_TX3:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_3__TX_FUNCS;
        break;
            
    case ADI_ADRV903X_TX4:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_4__TX_FUNCS;
        break;
            
    case ADI_ADRV903X_TX5:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_5__TX_FUNCS;
        break;
            
    case ADI_ADRV903X_TX6:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_6__TX_FUNCS;
        break;
            
    case ADI_ADRV903X_TX7:
        *txFuncsChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_7__TX_FUNCS;
        break;
            
    default:
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                txChannel,
                "Invalid txChannel parameter ");
            return recoveryAction;
        }
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;    
    
    return recoveryAction;
}


ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDatapathBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                       const adi_adrv903x_TxChannels_e txChannel,
                                                                       adrv903x_BfTxDatapathChanAddr_e* const txDatapathChannelBitfieldAddr)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txDatapathChannelBitfieldAddr);
    
    if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               device->devStateInfo.initializedChannels,
                               "Selected Tx channel is not initialized for this device.");
        return recoveryAction;
    }
    
    switch (txChannel)
    {
        case ADI_ADRV903X_TX0:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_0__TX_DATAPATH;
            break;
            
        case ADI_ADRV903X_TX1:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_1__TX_DATAPATH;
            break;
            
        case ADI_ADRV903X_TX2:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_2__TX_DATAPATH;
            break;
        case ADI_ADRV903X_TX3:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_3__TX_DATAPATH;
            break;
            
        case ADI_ADRV903X_TX4:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_4__TX_DATAPATH;
            break;
            
        case ADI_ADRV903X_TX5:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_5__TX_DATAPATH;
            break;
            
        case ADI_ADRV903X_TX6:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_6__TX_DATAPATH;
            break;
            
        case ADI_ADRV903X_TX7:
            *txDatapathChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_7__TX_DATAPATH;
            break;
            
        default:
            {
                recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       txChannel,
                                       "Invalid txChannel parameter ");
                return recoveryAction;
            }
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;    
    
    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDigBitfieldAddressGet(adi_adrv903x_Device_t* const device,
                                                                  const adi_adrv903x_TxChannels_e txChannel,
                                                                  adrv903x_BfTxDigChanAddr_e* const txDigChannelBitfieldAddr)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check rx bitfield channel address pointer is not null */
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txDigChannelBitfieldAddr);

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               device->devStateInfo.initializedChannels,
                               "Selected Tx channel is not initialized for this device.");
        return recoveryAction;
    }
    
    switch (txChannel)
    {
    case ADI_ADRV903X_TX0:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_0__TX_DIG;
        break;
            
    case ADI_ADRV903X_TX1:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_1__TX_DIG;
        break;
            
    case ADI_ADRV903X_TX2:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_2__TX_DIG;
        break;
    case ADI_ADRV903X_TX3:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_3__TX_DIG;
        break;
            
    case ADI_ADRV903X_TX4:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_4__TX_DIG;
        break;
            
    case ADI_ADRV903X_TX5:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_5__TX_DIG;
        break;
            
    case ADI_ADRV903X_TX6:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_6__TX_DIG;
        break;
            
    case ADI_ADRV903X_TX7:
        *txDigChannelBitfieldAddr = ADRV903X_BF_SLICE_TX_7__TX_DIG;
        break;
            
    default:
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                txChannel,
                "Invalid txChannel parameter ");
            return recoveryAction;
        }
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;    
    
    return recoveryAction;
}

ADI_API adi_adrv903x_ErrAction_e adrv903x_TxPowerMonitorCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                      const adi_adrv903x_PowerMonitorCfg_t * const txPowerMonitorCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check txPowerMonitorCfg address pointer is not null */
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txPowerMonitorCfg);
    
    
    /* Valid ranges for the pa protection configuration values */
    static const uint8_t  MEAS_DURATION_MAX  = 0x0FU;
    static const uint8_t  PEAK_COUNT_MAX    = 0x3FU;
    static const uint8_t  SINGLE_BIT_MAX    = 0x01U;

    if (txPowerMonitorCfg->measDuration > MEAS_DURATION_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->measDuration,
            "Average/Peak duration exceeds maximum limit. Valid range 0-15");
        return recoveryAction;
    }

    if (txPowerMonitorCfg->peakCount > PEAK_COUNT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->peakCount,
            "Peak count exceeds maximum limit. Valid range 0-63");
        return recoveryAction;
    }
    

    if (txPowerMonitorCfg->avgPowerEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->avgPowerEnable,
            "Avg power enable value is invalid. Valid values 0-1");
        return recoveryAction;
    }

    if (txPowerMonitorCfg->peakPowerEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->peakPowerEnable,
            "Peak power enable value is invalid. Valid values 0-1");
        return recoveryAction;
    }

    if (txPowerMonitorCfg->avgPeakRatioEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->avgPeakRatioEnable,
            "Avg Peak power ratio enable value is invalid. Valid values 0-1");
        return recoveryAction;
    }
    
    if (txPowerMonitorCfg->peakErrorClearRequired > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->peakErrorClearRequired,
            "Peak error cleared required selection is invalid. Valid values 0-1");
        return recoveryAction;
    }

    if (txPowerMonitorCfg->peakPowerIrqEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->peakPowerIrqEnable,
            "Peak power irq enable value is invalid. Valid values 0-1");
        return recoveryAction;
    }
    
    if (txPowerMonitorCfg->avgErrorClearRequired > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->avgErrorClearRequired,
            "Avg error cleared required selection is invalid. Valid values 0-1");
        return recoveryAction;
    }

    if (txPowerMonitorCfg->avgPowerIrqEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->avgPowerIrqEnable,
            "Avg power irq enabled value is invalid. Valid values 0-1");
        return recoveryAction;
    }

    if (txPowerMonitorCfg->inputSel > ADI_ADRV903X_TXQEC_ACTUATOR_OUTPUT)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            txPowerMonitorCfg->inputSel,
            "Power monitor input selection is invalid. Valid selections are COMPLEX_MULT_OUTPUT and TXQEC_ACTUATOR_OUTPUT");
        return recoveryAction;
    }
    
    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    
    return recoveryAction;
}


ADI_API adi_adrv903x_ErrAction_e adrv903x_TxSlewRateDetectorCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                          const adi_adrv903x_SlewRateDetectorCfg_t * const txSlewRateDetectorCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    /* Check txSlewRateDetectorCfg address pointer is not null */
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txSlewRateDetectorCfg);
    
    /* Valid ranges for the srd configuration values */
    static const uint8_t SINGLE_BIT_MAX    = 0x01U;
    static const uint8_t ARV_MAX_WAIT_TIME = 15U;

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_HAL_LOG_API_PRIV);

    if (txSlewRateDetectorCfg->inputSel > ADI_ADRV903X_TXQEC_ACTUATOR_OUTPUT)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->inputSel,
                               "Input selection should be either HB1 Output or TXQEC Actuator Output");
        return recoveryAction;
    }

    if (txSlewRateDetectorCfg->srdEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->srdEnable,
                               "srdEnable value is invalid. Valid values 0-1");
        return recoveryAction;
    }
    
    if (txSlewRateDetectorCfg->srdIrqEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->srdIrqEnable,
                               "srdIrqEnable value is invalid. Valid values 0-1");
        return recoveryAction;
    }
    
    if (txSlewRateDetectorCfg->autoRecoveryWaitTime > ARV_MAX_WAIT_TIME)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->autoRecoveryWaitTime,
                               "autoRecoveryWaitTime value is invalid. Valid range [0-15]");
        return recoveryAction;
    }
    
    if (txSlewRateDetectorCfg->autoRecoveryEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->autoRecoveryEnable,
                               "autoRecoveryEnable value is invalid. Valid values 0-1");
        return recoveryAction;
    }
    
    if (txSlewRateDetectorCfg->autoRecoveryDisableTimerWhenTxOff > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->autoRecoveryDisableTimerWhenTxOff,
                               "autoRecoveryDisableTimerWhenTxOff value is invalid. Valid values 0-1");
        return recoveryAction;
    }

    if (txSlewRateDetectorCfg->srdStatisticsEnable > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->srdStatisticsEnable,
                               "srdStatisticsEnable value is invalid. Valid values 0-1");
        return recoveryAction;
    }

    if (txSlewRateDetectorCfg->srdStatisticsMode > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSlewRateDetectorCfg->srdStatisticsMode,
                               "srdStatisticsMode value is invalid. Valid values 0-1");
        return recoveryAction;
    }

    /* No error, set recovery outside switch statement to save code */
    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;

    return recoveryAction;
}


ADI_API adi_adrv903x_ErrAction_e adrv903x_TxLoSourceGet(adi_adrv903x_Device_t* const device,
                                                        const adi_adrv903x_TxChannels_e txChannel,
                                                        adi_adrv903x_LoSel_e* const txLoSource)
{
    static const uint8_t TX_LO_CURRENT_MASK = 0x01U;
    uint8_t rxtxLoMuxReg   = 0U;
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    /* Check txPowerMonitorCfg address pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, txLoSource);

    if ((txChannel == ADI_ADRV903X_TX0) ||
        (txChannel == ADI_ADRV903X_TX1) ||
        (txChannel == ADI_ADRV903X_TX2) ||
        (txChannel == ADI_ADRV903X_TX3))
    {
        /*Read the Tx Lo Mux reg from the east side of the chip */
        recoveryAction = adrv903x_PllMemMap_SelTxLo_BfGet(device,
                                                          NULL,
                                                          ADRV903X_BF_DIGITAL_CORE_EAST_RFPLL,
                                                          &rxtxLoMuxReg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE   )
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to retrieve Rx Lo Selection.");
            return recoveryAction;
        }
        
        /*For East Side, 1 = LO0, 0 = LO1*/
        if ((rxtxLoMuxReg & TX_LO_CURRENT_MASK) == TX_LO_CURRENT_MASK)
        {
            *txLoSource = ADI_ADRV903X_LOSEL_LO0;
        }
        else
        {
            *txLoSource = ADI_ADRV903X_LOSEL_LO1;
        }
    }
    else if ((txChannel == ADI_ADRV903X_TX4) ||
        (txChannel == ADI_ADRV903X_TX5) ||
        (txChannel == ADI_ADRV903X_TX6) ||
        (txChannel == ADI_ADRV903X_TX7))
    {
        /*Read the Tx Lo Mux reg from the west side of the chip */
        recoveryAction = adrv903x_PllMemMap_SelTxLo_BfGet(device,
                                                          NULL,
                                                          ADRV903X_BF_DIGITAL_CORE_WEST_RFPLL,
                                                          &rxtxLoMuxReg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "PllMemMap_SelTxLo_Bfget issue");
            return recoveryAction;
        }
        
        /*For West Side, 1 = LO1, 0 = LO0*/
        if ((rxtxLoMuxReg & TX_LO_CURRENT_MASK) == TX_LO_CURRENT_MASK)
        {
            *txLoSource = ADI_ADRV903X_LOSEL_LO1;
        }
        else
        {
            *txLoSource = ADI_ADRV903X_LOSEL_LO0;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel parameter. Valid Tx channel must be Tx0-Tx7");
        return recoveryAction;
    }

    return recoveryAction;
}


ADI_API adi_adrv903x_ErrAction_e adrv903x_TxDecPowerCfgRangeCheck(adi_adrv903x_Device_t* const device,
                                                                  const adi_adrv903x_TxDecimatedPowerCfg_t* const decPowerCfg)
{
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Null pointer checks */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);
    ADI_ADRV903X_NULL_PTR_REPORT_RETURN(&device->common, decPowerCfg);

    static const uint8_t ENABLE_MAX = 1U;
    static const uint8_t MAX_POWER_INPUT_SELECT = 3U;
    static const uint8_t MAX_MEASUREMENT_DURATION_AX = 24U;
    static const uint8_t MAX_MEASUREMENT_DURATION_B0 = 25U;
    
    uint8_t deviceRev = 0U;

    if (((decPowerCfg->txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (decPowerCfg->txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        return recoveryAction;
    }
    
    if (decPowerCfg->measurementEnable > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->measurementEnable,
                               "Invalid measurementEnable selection.");
        return recoveryAction;
    }
    
    if (decPowerCfg->powerInputSelect > MAX_POWER_INPUT_SELECT)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->powerInputSelect,
                               "Invalid powerInputSelect selection.");
        return recoveryAction;
    }
    
    recoveryAction = adi_adrv903x_DeviceRevGet(device, &deviceRev);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Unable to extract device Revision");
        return recoveryAction;
    }
    if (deviceRev >= 0xB0)
    {
        if (decPowerCfg->powerMeasurementDuration > MAX_MEASUREMENT_DURATION_B0)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                    recoveryAction,
                                    decPowerCfg->powerMeasurementDuration,
                                    "Invalid powerMeasurementDuration selection.");
            return recoveryAction;
        }
    }
    else if ((deviceRev & 0xA0) == 0xA0)
    {
        if (decPowerCfg->powerMeasurementDuration > MAX_MEASUREMENT_DURATION_AX)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                    recoveryAction,
                                    decPowerCfg->powerMeasurementDuration,
                                    "Invalid powerMeasurementDuration selection.");
            return recoveryAction;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                                recoveryAction,
                                deviceRev,
                                "Invalid device revision.");
        return recoveryAction;
    }


    if (decPowerCfg->peakToPowerMode > ENABLE_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               decPowerCfg->peakToPowerMode,
                               "Invalid peakToPowerMode selection.");
        return recoveryAction;
    }

    recoveryAction = ADI_ADRV903X_ERR_ACT_NONE;
    return recoveryAction;
}

