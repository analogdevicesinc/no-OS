/**
* \file
* \brief Contains Tx features related function implementation defined in
* adi_adrv9001_tx.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* "adi_adrv9001_user.h" contains the #define that other header file use */
#include "adi_adrv9001_user.h"

/* Header file corresponding to the C file */
#include "adi_adrv9001_tx.h"

/* ADI specific header files */
#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_radio.h"

#include "adrv9001_validators.h"
#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adi_common_macros.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_bf.h"

/* System header files */
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/math64.h>
#else
#include <stdlib.h>
#include <math.h>
#endif

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

#define NEGATIVE(x)		-(x)

static const uint16_t NORMAL_MAX_ATTENUATION_MDB = 41950;
static const uint16_t NORMAL_ATTENUATION_RESOLUTION_MDB = 50;
static const uint16_t DM_MAX_ATTENUATION_MDB = 12000;
static const uint16_t DM_ATTENUATION_RESOLUTION_MDB = 500;

/* JS: To completely get rid of these LUTs, we need to read the attenuation table, iterate over it calculating the
 * analog attenuation given by the attenHp word and returning the index of the value closest to desired attenuation.
 * Reverse lookup can be accomplished more directly - read the current index, look up the attenHp word and calculate
 * the actual attenuation. This approach is only necessary if the user is allowed to change the attenuation table */
/* Tx Attenuation index for DM mode */
static const uint16_t txAttenIndex[] = {   0,  12, 21, 30, 40, 50,
                                          62,  70,  82,  91, 100, 110,
                                         121, 132, 144, 150, 164, 171,
                                         178, 194, 203, 211, 221, 231,
                                         241, 241
                                       };

/* Tx Attenuation index for DM mode */
static const uint16_t txAttenValue[] = {     0,   561,  1006,  1476,  1972,  2499,
                                          3059,  3454,  4082,  4528,  4998,  5494,
                                          6021,  6581,  7180,  7496,  8165,  8519,
                                          8889,  9679, 10103, 10549, 11018, 11515,
                                         12041, 12041
                                       };

static const uint32_t TX_ATTEN_TABLE_LENGTH = ADI_ARRAY_LEN(txAttenIndex);

/*********************************************************************************************************/

static adrv9001_BfNvsRegmapTx_e Tx_Addr_Get(adi_common_ChannelNumber_e channel)
{
    switch (channel)
    {
    case ADI_CHANNEL_1:
        return ADRV9001_BF_TX1_CORE;
    case ADI_CHANNEL_2:
        return ADRV9001_BF_TX2_CORE;
    default:
        return 0;
    }
}

static adrv9001_BfTxdacMemMap_e Tx_DacAddr_Get(adi_common_ChannelNumber_e channel)
{
    switch (channel)
    {
    case ADI_CHANNEL_1:
        return ADRV9001_BF_TX1_DAC;
    case ADI_CHANNEL_2:
        return ADRV9001_BF_TX2_DAC;
    default:
        return 0;
    }
}

static uint32_t Tx_AttenTableAddr_Get(adi_common_ChannelNumber_e channel)
{
    switch (channel)
    {
    case ADI_CHANNEL_1:
        return ADRV9001_ADDR_TX1_ATTEN_TABLE;
    case ADI_CHANNEL_2:
        return ADRV9001_ADDR_TX2_ATTEN_TABLE;
    default:
        return 0;
    }
}

/*********************************************************************************************************/

static int32_t __maybe_unused adi_adrv9001_Tx_AttenuationMode_Set_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           adi_adrv9001_TxAttenuationControlMode_e mode)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    switch (mode)
    {
    case ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_BYPASS:   /* Falls through */
    case ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI:      /* Falls through */
    case ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_PIN:
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         mode,
                         "Invalid parameter value. mode must be a valid adi_adrv9001_TxAttenuationControlMode_e");
        ADI_API_RETURN(device);
    }

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);
    switch (state)
    {
    case ADI_ADRV9001_CHANNEL_STANDBY:      /* Falls through */
    case ADI_ADRV9001_CHANNEL_CALIBRATED:
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[port_index][chan_index],
                         "Error while attempting to set attenuation mode. Channel must be in STANDBY or CALIBRATED.");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_AttenuationMode_Set(adi_adrv9001_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_TxAttenuationControlMode_e mode)
{
    adrv9001_BfNvsRegmapTx_e txChannelBaseAddr = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_AttenuationMode_Set_Validate, device, channel, mode);

    txChannelBaseAddr = Tx_Addr_Get(channel);

    ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenMode_Set, device, txChannelBaseAddr, (uint8_t)mode);

    ADI_API_RETURN(device)
}

static int32_t __maybe_unused adi_adrv9001_Tx_AttenuationMode_Get_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           adi_adrv9001_TxAttenuationControlMode_e *mode)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, mode);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_AttenuationMode_Get(adi_adrv9001_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_TxAttenuationControlMode_e *mode)
{
    uint8_t regData = 0;
    adrv9001_BfNvsRegmapTx_e txChannelBaseAddr = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_AttenuationMode_Get_Validate, device, channel, mode);

    txChannelBaseAddr = Tx_Addr_Get(channel);

    ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenMode_Get, device, txChannelBaseAddr, &regData);
    *mode = (adi_adrv9001_TxAttenuationControlMode_e)regData;

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_Attenuation_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                             adi_common_ChannelNumber_e channel,
                                                                             adi_adrv9001_TxAttenuationConfig_t *config)
{
    ADI_NULL_PTR_RETURN(&device->common, config);
    ADI_EXPECT(adi_adrv9001_Tx_AttenuationMode_Set_Validate, device, channel, config->attenMode);
    ADI_RANGE_CHECK(device, config->txAttenStepSize, ADI_ADRV9001_TXATTEN_0P05_DB, ADI_ADRV9001_TXATTEN_0P4_DB);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_Attenuation_Configure(adi_adrv9001_Device_t *device,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_TxAttenuationConfig_t *config)
{
    adrv9001_BfNvsRegmapTx_e txChannelBaseAddr = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_Attenuation_Configure_Validate, device, channel, config);

    txChannelBaseAddr = Tx_Addr_Get(channel);

    ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenConfig_Set, device, txChannelBaseAddr, config->txAttenStepSize);
    ADI_EXPECT(adi_adrv9001_Tx_AttenuationMode_Set, device, channel, config->attenMode);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_Attenuation_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           adi_adrv9001_TxAttenuationConfig_t *config)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, config);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_Attenuation_Inspect(adi_adrv9001_Device_t *device,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_TxAttenuationConfig_t *config)
{
    uint8_t disableTxOnPllUnlock = 0;
    uint8_t txAttenStepRead = ADI_ADRV9001_TXATTEN_0P05_DB;
    adi_adrv9001_TxAttenuationControlMode_e txModeRead = ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI;

    adrv9001_BfNvsRegmapTx_e txChannelBaseAddr = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_Attenuation_Inspect_Validate, device, channel, config);

    txChannelBaseAddr = Tx_Addr_Get(channel);

    /* RFPLL unlock setting for Tx attenuation ramp */
    ADI_EXPECT(adrv9001_NvsRegmapTx_TxDataRampDownOnPllUnlock_Get, device, txChannelBaseAddr, &disableTxOnPllUnlock);

    /* Attenuation step size */
    ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenConfig_Get, device, txChannelBaseAddr, &txAttenStepRead);

    /* Retrieve attenuation mode */
    ADI_EXPECT(adi_adrv9001_Tx_AttenuationMode_Get, device, channel, &txModeRead);

    config->disableTxOnPllUnlock = (bool)disableTxOnPllUnlock;
    config->attenMode = txModeRead;
    config->txAttenStepSize = (adi_adrv9001_TxAttenStepSize_e)txAttenStepRead;

    ADI_API_RETURN(device);
}

static int32_t adi_adrv9001_TxAttenDmGet(adi_adrv9001_Device_t *device,
                                         uint16_t txAttenuationIndex,
                                         uint16_t *txAttenuation_mdB)
{
    uint16_t i = 0;

    /* Get back the TX Atten value that is already set */
    if (txAttenuationIndex == txAttenIndex[0])
    {
        *txAttenuation_mdB = txAttenValue[0];
        ADI_API_RETURN(device);
    }
    if (txAttenuationIndex == txAttenIndex[TX_ATTEN_TABLE_LENGTH - 1])
    {
        *txAttenuation_mdB = txAttenValue[TX_ATTEN_TABLE_LENGTH - 1];
        ADI_API_RETURN(device);
    }

    for (i = 1; i < (TX_ATTEN_TABLE_LENGTH - 1); i++)
    {
        if (txAttenuationIndex == txAttenIndex[i])
        {
            *txAttenuation_mdB = txAttenValue[i];
            ADI_API_RETURN(device);
        }
    }

    ADI_ERROR_REPORT(&device->common,
        ADI_COMMON_ERRSRC_API,
        ADI_ADRV9001_ERR_INV_TX_ATTEN_MODE,
        ADI_COMMON_ACT_ERR_CHECK_PARAM,
        txAttenuationIndex,
        "Invalid Tx attenuation index readback from register");

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_Attenuation_Set_Validate(adi_adrv9001_Device_t* device,
                                                                       adi_common_ChannelNumber_e channel,
                                                                       uint16_t attenuation_mdB)
{
    uint8_t chan_index = 0;
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    if (device->devStateInfo.outputSignaling[chan_index] == ADI_ADRV9001_TX_DIRECT_FM_FSK)
    {
        /* Below 12dB, changing the analog attenuation word by 1 changes the attenuation by more than 0.5 dB */
        ADI_RANGE_CHECK(device, attenuation_mdB, 0, DM_MAX_ATTENUATION_MDB);

        if (attenuation_mdB % DM_ATTENUATION_RESOLUTION_MDB != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             attenuation_mdB,
                             "Invalid attenuation_mdB value. In FM_DM mode, the resolution of adi_adrv9001_Tx_Attenuation_Set() is only 0.5dB");
        }
    }
    else
    {
        ADI_RANGE_CHECK(device, attenuation_mdB, 0, NORMAL_MAX_ATTENUATION_MDB);

        if (attenuation_mdB % NORMAL_ATTENUATION_RESOLUTION_MDB != 0)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             attenuation_mdB,
                             "Invalid attenuation_mdB value. The resolution of adi_adrv9001_Tx_Attenuation_Set() is only 0.05dB");
        }
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_Attenuation_Set(adi_adrv9001_Device_t* device,
                                        adi_common_ChannelNumber_e channel,
                                        uint16_t attenuation_mdB)
{
    uint8_t chan_index = 0;
    uint16_t regData = 0;
    adrv9001_BfNvsRegmapTx_e txChannelBaseAddr = ADRV9001_BF_TX1_CORE;
    adi_adrv9001_TxAttenuationControlMode_e attenMode = ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_BYPASS;
    adi_adrv9001_TxAttenStepSize_e stepSize = ADI_ADRV9001_TXATTEN_0P05_DB;
    uint8_t bfVal = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_Attenuation_Set_Validate, device, channel, attenuation_mdB);

    adi_common_channel_to_index(channel, &chan_index);

    /* Save the current attenuation mode and set to the required mode */
    ADI_EXPECT(adi_adrv9001_Tx_AttenuationMode_Get, device, channel, &attenMode);
    if (attenMode != ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI)
    {
        ADI_EXPECT(adi_adrv9001_Tx_AttenuationMode_Set, device, channel, ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI);
    }

    if (device->devStateInfo.outputSignaling[chan_index] == ADI_ADRV9001_TX_DIRECT_FM_FSK)
    {
        /* Lookup the attenuation index with the analog attenuation word closest to the desired attenuation */
        regData = txAttenIndex[attenuation_mdB / DM_ATTENUATION_RESOLUTION_MDB];
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenConfig_Get, device, txChannelBaseAddr, &bfVal);
        stepSize = (adi_adrv9001_TxAttenStepSize_e)bfVal;
        /* Attenuation register value depends on txAttenStepSize */
        switch (stepSize)
        {
        case ADI_ADRV9001_TXATTEN_0P05_DB:
            regData = (attenuation_mdB / 50);
            break;
        case ADI_ADRV9001_TXATTEN_0P1_DB:
            regData = (attenuation_mdB / 100);
            break;
        case ADI_ADRV9001_TXATTEN_0P2_DB:
            regData = (attenuation_mdB / 200);
            break;
        case ADI_ADRV9001_TXATTEN_0P4_DB:
            regData = (attenuation_mdB / 400);
            break;
        default:
            {
                ADI_SHOULD_NOT_EXECUTE(device);
            }
        }
    }

    txChannelBaseAddr = Tx_Addr_Get(channel);
    ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenuation_Set, device, txChannelBaseAddr, regData);

    /* Restore the atten mode */
    if (attenMode != ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_SPI)
    {
        ADI_EXPECT(adi_adrv9001_Tx_AttenuationMode_Set, device, channel, attenMode);
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_Attenuation_Get_Validate(adi_adrv9001_Device_t* device,
                                                                       adi_common_ChannelNumber_e channel,
                                                                       uint16_t *attenuation_mdB)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, attenuation_mdB);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_Attenuation_Get(adi_adrv9001_Device_t* device,
                                        adi_common_ChannelNumber_e channel,
                                        uint16_t *attenuation_mdB)
{
    adrv9001_BfNvsRegmapTx_e txBfChannel = ADRV9001_BF_TX1_CORE;
    uint32_t waitInterval_us = 0;
    int32_t halError = 0;
    uint16_t txAttenReadBack = 0;
    uint16_t attenStepSizeDiv = 50;
    uint16_t regData = 0;
    uint8_t chan_index = 0;
    adi_adrv9001_TxAttenStepSize_e stepSize = ADI_ADRV9001_TXATTEN_0P05_DB;
    uint8_t bfVal = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_Attenuation_Get_Validate, device, channel, attenuation_mdB);

    adi_common_channel_to_index(channel, &chan_index);
    txBfChannel = Tx_Addr_Get(channel);

    /* Need to set this Read register to readback the value */
    ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenuationRead_Set, device, txBfChannel, regData);

    /* A delay is needed between set and get function */
    waitInterval_us = 100;

    halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
    ADI_ERROR_REPORT(&device->common,
                    ADI_COMMON_ERRSRC_ADI_HAL,
                    halError,
                    ADI_COMMON_ACT_ERR_CHECK_TIMER,
                    device,
                    "Timer not working");

    ADI_ERROR_RETURN(device->common.error.newAction);

    ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenuationRead_Get, device, txBfChannel, &txAttenReadBack);

    /* Check whether the mode is DM mode */
    if (device->devStateInfo.outputSignaling[chan_index] == ADI_ADRV9001_TX_DIRECT_FM_FSK)
    {
        ADI_EXPECT(adi_adrv9001_TxAttenDmGet, device, txAttenReadBack, attenuation_mdB);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapTx_TxAttenConfig_Get, device, txBfChannel, &bfVal);
        stepSize = (adi_adrv9001_TxAttenStepSize_e)bfVal;
        switch (stepSize)
        {
        case ADI_ADRV9001_TXATTEN_0P05_DB:
            attenStepSizeDiv = 50;
            break;
        case ADI_ADRV9001_TXATTEN_0P1_DB:
            attenStepSizeDiv = 100;
            break;
        case ADI_ADRV9001_TXATTEN_0P2_DB:
            attenStepSizeDiv = 200;
            break;
        case ADI_ADRV9001_TXATTEN_0P4_DB:
            attenStepSizeDiv = 400;
            break;
        default:
            {
                ADI_SHOULD_NOT_EXECUTE(device);
            }
        }

        /* Readback word always reads back with 0.05dB resolution */
        *attenuation_mdB = (txAttenReadBack * attenStepSizeDiv);
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_OutputPowerBoost_Set_Validate(adi_adrv9001_Device_t* device,
                                                                            adi_common_ChannelNumber_e channel)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_EXPECT(adi_adrv9001_Channel_Validate, device, channel);

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);
    if (state != ADI_ADRV9001_CHANNEL_STANDBY)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         state,
                         "Invalid channel state. Channel must be in STANDBY");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_OutputPowerBoost_Set(adi_adrv9001_Device_t* device,
                                             adi_common_ChannelNumber_e channel,
                                             bool boostEnable)
{
    adrv9001_BfTxdacMemMap_e txDacChannelBaseAddr = ADRV9001_BF_TX1_DAC;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_OutputPowerBoost_Set_Validate, device, channel);

    txDacChannelBaseAddr = Tx_DacAddr_Get(channel);

    /* DAC full scale setting */
    ADI_EXPECT(adrv9001_TxdacMemMap_TxdacFscBoostI_Set, device, txDacChannelBaseAddr, boostEnable);
    ADI_EXPECT(adrv9001_TxdacMemMap_TxdacFscBoostQ_Set, device, txDacChannelBaseAddr, boostEnable);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_OutputPowerBoost_Get(adi_adrv9001_Device_t* device,
                                             adi_common_ChannelNumber_e channel,
                                             bool *boostEnabled)
{
    adrv9001_BfTxdacMemMap_e txDacChannelBaseAddr = ADRV9001_BF_TX1_DAC;
    uint8_t bfValue = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Channel_Validate, device, channel);
    ADI_NULL_PTR_RETURN(&device->common, boostEnabled);

    txDacChannelBaseAddr = Tx_DacAddr_Get(channel);

    /* Get DAC full scale current boost */
    ADI_EXPECT(adrv9001_TxdacMemMap_TxdacFscBoostI_Get, device, txDacChannelBaseAddr, &bfValue);

    *boostEnabled = (bool)bfValue;

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_AttenuationTable_Write_Validate(adi_adrv9001_Device_t *device,
                                                                              uint32_t channelMask,
                                                                              uint32_t indexOffset,
                                                                              adi_adrv9001_TxAttenTableRow_t attenTableRows[],
                                                                              uint32_t arraySize)
{
    static const uint8_t ATTEN_HP_MIN = 0;
    static const uint8_t ATTEN_HP_MAX = 63;
    static const uint16_t ATTEN_MULT_MIN = 0;
    static const uint16_t ATTEN_MULT_MAX = 4095;
    uint16_t idx = 0;

    /* Check that channelMask is valid */
    ADI_RANGE_CHECK(device, channelMask, ADI_CHANNEL_1, (ADI_CHANNEL_1 | ADI_CHANNEL_2));

    /* Check that the index offset is within range */
    ADI_RANGE_CHECK(device, indexOffset, 0, ADRV9001_TX_ATTEN_TABLE_MAX);

    /* Check for a valid array */
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, attenTableRows, arraySize);

    /* Check that the entire array fits */
    if (arraySize > ((ADRV9001_TX_ATTEN_TABLE_MAX - indexOffset)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid parameter value. arraySize must be less than ADRV9001_TX_ATTEN_TABLE_MAX - indexOffset");
    }

    /* Check that Tx Profile is valid */
    if ((device->devStateInfo.profilesValid & ADI_ADRV9001_TX_PROFILE_VALID) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channelMask,
                         "Tx profile is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Range check individual atten table row entries */
    for (idx = 0; idx < arraySize; idx++)
    {
        ADI_RANGE_CHECK(device, attenTableRows[idx].txAttenHp, ATTEN_HP_MIN, ATTEN_HP_MAX);
        ADI_RANGE_CHECK(device, attenTableRows[idx].txAttenMult, ATTEN_MULT_MIN, ATTEN_MULT_MAX);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_AttenuationTable_Write(adi_adrv9001_Device_t *device,
                                               uint32_t channelMask,
                                               uint32_t indexOffset,
                                               adi_adrv9001_TxAttenTableRow_t attenTableRows[],
                                               uint32_t arraySize)
{
    static const uint32_t ATTEN_HP_MASK = 0x0003F000;
    static const uint32_t ATTEN_HP_SHIFT = 12;
    static const uint32_t ATTEN_MULT_MASK = 0x00000FFF;
    static const uint32_t ATTEN_MULT_SHIFT = 0;

    uint32_t txAttenEntry = 0;
    uint32_t idx = 0;
    uint32_t baseAddr = 0;
    uint32_t maskBit = 0;
    uint32_t start = 0;
    uint32_t stop = 0;
    uint32_t data = 0;
    uint32_t offset = 0;

    /* FIXME: Get rid of CACHE_MAX and +10 */
    uint8_t  cfgData[ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX + 10] = { 0 };
    static const uint8_t TX_ENTRY_SIZE = 4;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_AttenuationTable_Write_Validate, device, channelMask, indexOffset, attenTableRows, arraySize);

    /* Enable ARM clock to access attenuation table memory */
    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB1_CORE, true);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1GainTableClkEn_Set, device, true);
    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB2_CORE, true);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2GainTableClkEn_Set, device, true);

    for (idx = 0; idx < ADI_ADRV9001_MAX_TXCHANNELS; idx++)
    {
        maskBit = (1UL << idx);
        if (ADRV9001_BF_EQUAL(channelMask, maskBit))
        {
            baseAddr = Tx_AttenTableAddr_Get(maskBit);

            /* TODO: Refactor this bitwise logic to a Format function a la RxGainTableFormat */
            offset = (indexOffset * TX_ENTRY_SIZE) + baseAddr;
            for (txAttenEntry = 0; txAttenEntry < arraySize; txAttenEntry++)
            {
                data  = ADRV9001_BF_ENCODE((uint32_t)(attenTableRows[txAttenEntry].txAttenHp), ATTEN_HP_MASK, ATTEN_HP_SHIFT);
                data |= ADRV9001_BF_ENCODE((uint32_t)(attenTableRows[txAttenEntry].txAttenMult), ATTEN_MULT_MASK, ATTEN_MULT_SHIFT);

                cfgData[start + 0] = (uint8_t)(data & 0x000000FF);
                cfgData[start + 1] = (uint8_t)((data >> 8) & 0x000000FF);
                cfgData[start + 2] = (uint8_t)((data >> 16) & 0x000000FF);
                cfgData[start + 3] = (uint8_t)((data >> 24) & 0x000000FF);

                start += TX_ENTRY_SIZE;
                if (start >= ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX)
                {
                    ADI_EXPECT(adrv9001_DmaMemWrite, device, offset + stop, &cfgData[0], start, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

                    stop += start;
                    start = 0;
                }
            }

            if (start > 0)
            {
                ADI_EXPECT(adrv9001_DmaMemWrite, device, offset + stop, &cfgData[0], start, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
            }
        }
        start = 0;
        stop = 0;
    }

    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB1_CORE, false);
    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB2_CORE, false);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_AttenuationTable_Read_Validate(adi_adrv9001_Device_t *device,
                                                                             adi_common_ChannelNumber_e channel,
                                                                             uint32_t indexOffset,
                                                                             adi_adrv9001_TxAttenTableRow_t attenTableRows[],
                                                                             uint32_t arraySize,
                                                                             uint16_t *numAttenIndicesRead)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_RANGE_CHECK(device, indexOffset, 0, ADRV9001_TX_ATTEN_TABLE_MAX);

    ADI_NULL_PTR_RETURN(&device->common, attenTableRows);

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, attenTableRows, arraySize);
    if (arraySize > ((ADRV9001_TX_ATTEN_TABLE_MAX - indexOffset)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid parameter value. arraySize must be less than ADRV9001_TX_ATTEN_TABLE_MAX - indexOffset");
    }

    /* No need to null check numAttenIndicesRead as it is optional */

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_AttenuationTable_Read(adi_adrv9001_Device_t *device,
                                              adi_common_ChannelNumber_e channel,
                                              uint32_t indexOffset,
                                              adi_adrv9001_TxAttenTableRow_t attenTableRows[],
                                              uint32_t arraySize,
                                              uint16_t *numAttenIndicesRead)
{
    static const uint32_t ATTEN_HP_MASK = 0x0003F000;
    static const uint32_t ATTEN_HP_SHIFT = 12;
    static const uint32_t ATTEN_MULT_MASK = 0x00000FFF;
    static const uint32_t ATTEN_MULT_SHIFT = 0;

    uint32_t idx = 0;
    uint32_t baseAddr = 0;
    uint32_t start = 0;
    uint32_t stop = 0;
    uint32_t data = 0;
    uint32_t offset = 0;
    uint32_t numTxAttenEntriesRead = 0;

    /* FIXME: Get rid of CACHE_MAX and +10 */
    uint8_t cfgData[ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX + 10] = { 0 };
    static const uint8_t TX_ENTRY_SIZE = 4;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_AttenuationTable_Read_Validate,
                           device,
                           channel,
                           indexOffset,
                           attenTableRows,
                           arraySize,
                           numAttenIndicesRead);

    baseAddr = Tx_AttenTableAddr_Get(channel);

    offset = (indexOffset * TX_ENTRY_SIZE) + baseAddr;

    numTxAttenEntriesRead = arraySize * TX_ENTRY_SIZE;
    if (numTxAttenEntriesRead > ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX)
    {
        numTxAttenEntriesRead = ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX;
    }

    /* Enable ARM clock to access attenuation table memory */
    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB1_CORE, true);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1GainTableClkEn_Set, device, true);
    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB2_CORE, true);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2GainTableClkEn_Set, device, true);

    ADI_EXPECT(adrv9001_DmaMemRead, device, offset + stop, &cfgData[0], numTxAttenEntriesRead, ADRV9001_ARM_MEM_READ_AUTOINCR);

    /* Disable ARM clock to Tx1/2 atten table memory access */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1GainTableClkEn_Set, device, false);
    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB1_CORE, false);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2GainTableClkEn_Set, device, false);
    ADI_EXPECT(adrv9001_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, ADRV9001_BF_TXB2_CORE, false);

    for (idx = 0; idx < arraySize; idx++)
    {
        data = (((uint32_t)cfgData[start + 3] << 24) |
                ((uint32_t)cfgData[start + 2] << 16) |
                ((uint32_t)cfgData[start + 1] <<  8) |
                 (uint32_t)cfgData[start + 0]);

        attenTableRows[idx].txAttenHp   = ADRV9001_BF_DECODE(data, ATTEN_HP_MASK, ATTEN_HP_SHIFT);
        attenTableRows[idx].txAttenMult = ADRV9001_BF_DECODE(data, ATTEN_MULT_MASK, ATTEN_MULT_SHIFT);

        start += TX_ENTRY_SIZE;

        if ((start >= ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX) &&
            ((idx + 1) < arraySize))
        {
            numTxAttenEntriesRead = (arraySize - (idx + 1)) * TX_ENTRY_SIZE;

            if (numTxAttenEntriesRead > ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX)
            {
                numTxAttenEntriesRead = ADI_ADRV9001_TX_ATTEN_TABLE_CACHE_MAX;
            }

            stop += start;

            ADI_EXPECT(adrv9001_DmaMemRead, device, offset + stop, &cfgData[0], numTxAttenEntriesRead, 1);

            start = 0;
        }
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_PaProtection_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                              adi_common_ChannelNumber_e channel,
                                                                              adi_adrv9001_TxPaProtectCfg_t *config)
{
    /* Valid ranges for the pa protection configuration values */
    static const uint8_t AVG_DURATION_MAX = 0x0F;
    static const uint8_t PEAK_DURATION_MAX = 0x0F;
    static const uint16_t POWER_THRESH_MAX = 0x1FFF;
    static const uint16_t PEAK_THRESH_MAX = 0x1FFF;
    static const uint8_t PEAK_COUNT_MAX = 0x1F;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, config);
    ADI_RANGE_CHECK(device, config->avgDuration, 0, AVG_DURATION_MAX);
    ADI_RANGE_CHECK(device, config->peakDuration, 0, PEAK_DURATION_MAX);
    ADI_RANGE_CHECK(device, config->powerThreshold, 0, POWER_THRESH_MAX);
    ADI_RANGE_CHECK(device, config->peakThreshold, 0, PEAK_THRESH_MAX);
    ADI_RANGE_CHECK(device, config->peakCount, 0, PEAK_COUNT_MAX);
    ADI_RANGE_CHECK(device, config->inputSel, ADI_ADRV9001_COMPLEX_MULT_OUTPUT, ADI_ADRV9001_TXQEC_ACTUATOR_OUTPUT);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_PaProtection_Configure(adi_adrv9001_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_TxPaProtectCfg_t *config)
{
    adrv9001_BfNvsRegmapTx_e baseAddress = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_PaProtection_Configure_Validate, device, channel, config);

    /* FIXME: Vivek - For Day1, the following bits are used (disabled by setting '0' in adi_adrv9001_TxPaProtectCfg_t struct).
     * For actual PA protection config, many bits are enabled/disabled, but ignoring for now.
     * 1. Average Power Ratio Calculation Enable bit
     * 2. Average Power Enable bit
     * 3. Peak Power Enable bit */

    baseAddress = Tx_Addr_Get(channel);

    /* Set Average Power Ratio Calculation Enable bit */
    ADI_EXPECT(adrv9001_NvsRegmapTx_PaProtectionAprEn_Set, device, baseAddress, config->avgPeakRatioEnable);

    /* Set Average Power Enable bit */
    ADI_EXPECT(adrv9001_NvsRegmapTx_PaProtectionAvgpowerEn_Set, device, baseAddress, config->avgPowerEnable);

    /* Set Peak Power Enable bit */
    ADI_EXPECT(adrv9001_NvsRegmapTx_PaProtectionPeakpowerEn_Set, device, baseAddress, config->peakPowerEnable);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_PaProtection_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            adi_adrv9001_TxPaProtectCfg_t *config)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&device->common, config);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_PaProtection_Inspect(adi_adrv9001_Device_t *device,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_TxPaProtectCfg_t *config)
{
    adrv9001_BfNvsRegmapTx_e baseAddress = ADRV9001_BF_TX1_CORE;
    uint8_t avgPowerEnable = 0;     /*!< PA Protection Avg Power Enable */
    uint8_t peakPowerEnable = 0;    /*!< PA Protection Peak Power Enable */
    uint8_t avgPeakRatioEnable = 0; /*!< PA Protection Average to Peak Ratio Calculation Enable */

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_PaProtection_Inspect_Validate, device, channel, config);

    baseAddress = Tx_Addr_Get(channel);

    /* Get Average Power Enable bit */
    ADI_EXPECT(adrv9001_NvsRegmapTx_PaProtectionAvgpowerEn_Get, device, baseAddress, &avgPowerEnable);

    /* Get Peak Power Enable bit */
    ADI_EXPECT(adrv9001_NvsRegmapTx_PaProtectionPeakpowerEn_Get, device, baseAddress, &peakPowerEnable);

    /* Get Average Power Ratio Calculation Enable bit */
    ADI_EXPECT(adrv9001_NvsRegmapTx_PaProtectionAprEn_Get, device, baseAddress, &avgPeakRatioEnable);

    /* Assignment of all the readback values to the structure object*/
    config->avgPowerEnable = avgPowerEnable;
    config->peakPowerEnable = peakPowerEnable;
    config->avgPeakRatioEnable = avgPeakRatioEnable;

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_InternalToneGeneration_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                        adi_common_ChannelNumber_e channel,
                                                                                        adi_adrv9001_TxInternalToneGeneration_t *tone)
{
    int32_t txSampleRateDiv2_Hz = 0;
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    
    ADI_NULL_PTR_RETURN(&adrv9001->common, tone);
    if (tone->enable)
    {
        ADI_RANGE_CHECK(adrv9001, tone->amplitude, ADI_ADRV9001_TXINTERNALTONEAMPLITUDE_0_DB, ADI_ADRV9001_TXINTERNALTONEAMPLITUDE_12_DB);
        txSampleRateDiv2_Hz = KILO_TO_BASE_UNIT(adrv9001->devStateInfo.txInputRate_kHz[channel-1] >> 1);
        ADI_RANGE_CHECK(adrv9001, tone->frequency_Hz, NEGATIVE(txSampleRateDiv2_Hz), txSampleRateDiv2_Hz);

        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, ADI_TX, channel, &state);
        if (ADI_ADRV9001_CHANNEL_CALIBRATED != state)
        {
            ADI_ERROR_REPORT(&adrv9001->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_INV_PARAM,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             state,
                             "Channel state must be CALIBRATED");
        }
    }
    
    ADI_API_RETURN(adrv9001);
}

static const uint8_t OBJID_TX_INTERNAL_TONE_GENERATION = 0xB0;
int32_t adi_adrv9001_Tx_InternalToneGeneration_Configure(adi_adrv9001_Device_t *adrv9001,
                                                         adi_common_ChannelNumber_e channel,
                                                         adi_adrv9001_TxInternalToneGeneration_t *tone)
{
    uint8_t armData[12] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_InternalToneGeneration_Configure_Validate, adrv9001, channel, tone);

    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = tone->enable;
    armData[offset++] = tone->amplitude;
    offset += 2;
    adrv9001_LoadFourBytes(&offset, armData, tone->frequency_Hz);
    
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_CONFIG;
    extData[2] = OBJID_TX_INTERNAL_TONE_GENERATION;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001)
}

static int32_t __maybe_unused adi_adrv9001_Tx_InternalToneGeneration_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                      adi_common_ChannelNumber_e channel,
                                                                                      adi_adrv9001_TxInternalToneGeneration_t *tone)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, tone);
    
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, ADI_TX, channel, &state);
    if (ADI_ADRV9001_CHANNEL_STANDBY == state)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         state,
                         "Channel state must be any of CALIBRATED, PRIMED, RF_ENABLED");
    }
    
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Tx_InternalToneGeneration_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                       adi_common_ChannelNumber_e channel,
                                                       adi_adrv9001_TxInternalToneGeneration_t *tone)
{
    uint8_t armReadBack[8] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;
        
    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_InternalToneGeneration_Inspect_Validate, adrv9001, channel, tone);

    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_TX_INTERNAL_TONE_GENERATION, channelMask, offset, armReadBack, sizeof(armReadBack));
    
    tone->enable = (bool)armReadBack[offset++];
    tone->amplitude = (adi_adrv9001_TxInternalToneAmplitude_e)armReadBack[offset++];
    offset += 2;
    adrv9001_ParseFourBytes(&offset, armReadBack, (uint32_t *)&tone->frequency_Hz);

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_SlewRateLimiter_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                              adi_common_ChannelNumber_e channel,
                                                                              adi_adrv9001_SlewRateLimiterCfg_t *config)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, config);
    ADI_RANGE_CHECK(device,
                        config->srlStatisticsMode,
                        ADI_ADRV9001_SRL_STATISTICS_MIN_SLEW_FACTOR_OBSERVED,
                        ADI_ADRV9001_SRL_STATISTICS_CLEAR_TO_ZERO_11);
    ADI_RANGE_CHECK(device,
                        config->srlTableSelect,
                        ADI_ADRV9001_SRL_TABLE0,
                        ADI_ADRV9001_SRL_TABLE3);
    ADI_RANGE_CHECK(device, config->srlSlewOffset, 0, 15);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_SlewRateLimiter_Configure(adi_adrv9001_Device_t *device,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_SlewRateLimiterCfg_t *config)
{
    uint8_t armData[10] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_SlewRateLimiter_Configure_Validate, device, channel, config);

    /* Loading byte array with parsed bytes from pllLoFrequency_Hz word */
    armData[0] = (uint8_t)(config->srlEnable & 0xFF);
    armData[1] = (uint8_t)(config->srlTableSelect & 0xFF);
    armData[2] = (uint8_t)(config->srlSlewOffset & 0xFF);
    armData[3] = (uint8_t)(config->srlStatisticsEnable & 0xFF);
    armData[4] = (uint8_t)(config->srlStatisticsMode & 0xFF);
    armData[5] = (uint8_t)(config->srlEnable & 0xFF);
    armData[6] = (uint8_t)(config->srlTableSelect & 0xFF);
    armData[7] = (uint8_t)(config->srlSlewOffset & 0xFF);
    armData[8] = (uint8_t)(config->srlStatisticsEnable & 0xFF);
    armData[9] = (uint8_t)(config->srlStatisticsMode & 0xFF);

    /* Write SRL config to ARM mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write,
                   device,
                   (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_SET,
                   &armData[0],
                   sizeof(armData),
                   ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    /* Command ARM to set rest of the configuration for slew rate limiter of selected Tx channel */
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_SRL_CONTROL;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV9001_ARM_SET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV9001_SETSRLCONFIG_TIMEOUT_US,
        (uint32_t)ADI_ADRV9001_SETSRLCONFIG_INTERVAL_US);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_SlewRateLimiter_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            adi_adrv9001_SlewRateLimiterCfg_t *config)
{
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&device->common, config);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_SlewRateLimiter_Inspect(adi_adrv9001_Device_t *device,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_SlewRateLimiterCfg_t *config)
{
    uint8_t extData[2] = { 0 };
    uint8_t armData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_SlewRateLimiter_Inspect_Validate, device, channel, config);

    /* Command ARM to get the configuration for slew rate limiter of selected Tx channel */
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_SRL_CONTROL;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV9001_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV9001_GETSRLCONFIG_TIMEOUT_US,
        (uint32_t)ADI_ADRV9001_GETSRLCONFIG_INTERVAL_US);

    /* Read SRL configuration from mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
                   device,
                   (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_GET,
                   &armData[0],
                   sizeof(armData),
                   false);

    config->srlEnable = armData[0];
    config->srlTableSelect = (adi_adrv9001_SrlTableSel_e)armData[1];
    config->srlSlewOffset = armData[2];
    config->srlStatisticsEnable = armData[3];
    config->srlStatisticsMode = (adi_adrv9001_SrlStatisticsMode_e)armData[4];

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_PaRamp_Configure_Validate(adi_adrv9001_Device_t *device,
                                    adi_common_ChannelNumber_e channel,
                                    adi_adrv9001_PaRampCfg_t *paRampCfg)
{
    /* Check device pointer and gain pointer are not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, paRampCfg);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /* FIXME: Validate paRampCfg? */

    ADI_API_RETURN(device);
}

static int32_t adrv9001_LutDataWrite(adi_adrv9001_Device_t *device, uint16_t lutAddr, uint8_t lutData)
{
    uint8_t lutWrEnableBit = 0;
    uint32_t eventCount = 0;
    int32_t halError = 0;
    uint32_t numEventChecks = 0;

    static const uint32_t ADI_ADRV9001_PA_RAMP_WRITE_ENABLE_TIMEOUT_US = 10000;
    static const uint32_t ADI_ADRV9001_PA_RAMP_WRITE_ENABLE_INTERVAL_US = 1000;

    numEventChecks = ADI_ADRV9001_PA_RAMP_WRITE_ENABLE_TIMEOUT_US / ADI_ADRV9001_PA_RAMP_WRITE_ENABLE_INTERVAL_US;

    /* Write PA LUT data address */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_LutAddr_Set, device, lutAddr);
    /* Write PA LUT data */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_LutWrData_Set, device, lutData);
    /* Set 'lut_wr_enable' to '1' */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_LutWrEnable_Set, device, 0x1);

    /* timeout event check loop */
    for (eventCount = 0; eventCount <= numEventChecks; eventCount++)
    {
        /* read status of 'lut_wr_enable' bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_LutWrEnable_Get, device, &lutWrEnableBit);

        /* check whether 'lut_wr_enable' bit is reset to '0' to continue with next write */
        if (((lutWrEnableBit * 0x1) > 0) &&
             (eventCount < numEventChecks))
        {
            halError = adi_common_hal_Wait_us(&device->common, ADI_ADRV9001_PA_RAMP_WRITE_ENABLE_INTERVAL_US);
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV9001_SRC_INIT,
                halError,
                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                device,
                "Timer not working in adi_adrv9001_Tx_PaRamp_Configure()");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        else
        {
            break;
        }
    }

    /* check whether 'lut_wr_enable' bit is reset to '0' after timeout */
    if ((lutWrEnableBit * 0x1) > 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV9001_SRC_ARMCMD,
            ADI_COMMON_ERR_API_FAIL,
            ADI_ADRV9001_ACT_ERR_RESET_ARM,
            device,
            "Time out!!! adi_adrv9001_Tx_PaRamp_Configure() failed due to 'lut_wr_enable' bit not getting reset to perform write operation.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_API_RETURN(device);
}
int32_t adi_adrv9001_Tx_PaRamp_Configure(adi_adrv9001_Device_t *device,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv9001_PaRampCfg_t *paRampCfg)
{
    uint8_t clkDivRatio = 0;
    uint8_t crossbarConfig = 0;
    uint8_t chNumber = 0;
    uint8_t muxSel = 0;
    uint8_t auxDacChannelBitPosition = 0;
    uint8_t bfValue = 0;
    uint8_t i = 0;
    uint8_t lutData = 0;
    uint16_t idx = 0;
    uint16_t paRampDpClkDiv = 0;
    uint16_t lutAddr = 0;
    uint32_t refClk_Hz = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_PaRamp_Configure_Validate, device, channel, paRampCfg);

    clkDivRatio = device->devStateInfo.clkDivideRatios.anaRefClockRatio;
    refClk_Hz = KILO_TO_BASE_UNIT(device->devStateInfo.deviceClock_kHz) >> clkDivRatio;

    paRampDpClkDiv = DIV_ROUND_CLOSEST(refClk_Hz, KILO_TO_BASE_UNIT(paRampCfg->rampClock_kHz));

    if (channel == ADI_CHANNEL_1)
    {
        /* Select pa_ramp_tx1_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1LutSel_Set, device, 0x1);
    }
    else
    {
        /* Select pa_ramp_tx2_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2LutSel_Set, device, 0x1);
    }

    for (idx = 0; idx < ADRV9001_TX_PA_RAMP_LUT_SIZE; idx++)
    {
        /* The LUT size is 256x16 bits, which needs 512 SPI writes. SPI writes address start from 0 to 511 */
        lutAddr = idx*2;
        lutData = ((paRampCfg->paRampLUT[idx] << 4) & 0xFF);
        ADI_EXPECT(adrv9001_LutDataWrite, device, lutAddr, lutData);

        lutAddr++;
        lutData = ((paRampCfg->paRampLUT[idx] >> 4) & 0xFF);
        ADI_EXPECT(adrv9001_LutDataWrite, device, lutAddr, lutData);
    }

    if (channel == ADI_CHANNEL_1)
    {
        /* De-select pa_ramp_tx1_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1LutSel_Set, device, 0x0);

        /* Set PA Ramp up/down threshold for Tx1 */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1UpThreshold_Set, device, paRampCfg->upEndIndex);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1DownThreshold_Set, device, paRampCfg->downEndIndex);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1Asymmetric_Set, device, (uint8_t)paRampCfg->asymmetricRamp);
    }
    else
    {
        /* De-select pa_ramp_tx2_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2LutSel_Set, device, 0x0);

        /* Set PA Ramp up/down threshold for Tx2 */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2UpThreshold_Set, device, paRampCfg->upEndIndex);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2DownThreshold_Set, device, paRampCfg->downEndIndex);
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2Asymmetric_Set, device, (uint8_t)paRampCfg->asymmetricRamp);
    }

    /* crossbar config for AUX DAC 0/1/2/3.
       Bit[0]: xbar 0 select: 1: TX2, 0: TX1
       Bit[1]: xbar 1 select: 1: TX2, 0: TX1
       Bit[2]: xbar 2 select: 1: TX2, 0: TX1
       Bit[3]: xbar 3 select: 1: TX2, 0: TX1. */
    chNumber = (uint8_t)channel - 1;

    crossbarConfig = (chNumber << 3) | (chNumber << 2) | (chNumber << 1) | (chNumber << 0);
    ADI_EXPECT(adrv9001_NvsRegmapCore2_AuxdacXconfig_Set, device, crossbarConfig);

    /* 4bits AUXDAC mux selection.
       Bit[0] : AUXDAC 0 select :  1 : xbar_dac0, 0 : spi_auxdac0
       Bit[1] : AUXDAC 1 select :  1 : xbar_dac1, 0 : spi_auxdac1
       Bit[2] : AUXDAC 2 select :  1 : xbar_dac2, 0 : spi_auxdac2
       Bit[3] : AUXDAC 3 select :  1 : xbar_dac3, 0 : spi_auxdac3 */

    for (i = 0; i < ADI_ADRV9001_MAX_AUXDACS; i++)
    {
        if (ADRV9001_BF_EQUAL(paRampCfg->auxDacChannelSelect, ((uint32_t)1 << (uint32_t)i)))
        {
            /* Get the bit position for the auxDAC selected */
            auxDacChannelBitPosition = i;
            break;
        }
    }

    muxSel = (uint8_t)paRampCfg->enable << auxDacChannelBitPosition;
    ADI_EXPECT(adrv9001_NvsRegmapCore2_AuxdacMuxsel_Set, device, muxSel);

    bfValue =(uint8_t) paRampCfg->gpioTriggerSelect;

    if (channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampClkDivValue_Set, device, paRampDpClkDiv);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampDelayedEnableStartSel_Set, device, bfValue);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampDelayedEnableStopSel_Set, device, bfValue);

        if (paRampCfg->gpioTriggerSelect == true)
        {
            ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampEnableGpioSel_Set, device, (uint8_t)paRampCfg->gpioSource);
        }

        /* Enable the clock only after all registers are configured */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampClkEn_Set, device, 0x1);

        /* enable PA ramp module */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampEnable_Set, device, (uint8_t)paRampCfg->enable);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampPedTrigStartDelay_Set, device, paRampCfg->triggerDelayRise);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampPedTrigEndDelay_Set, device, paRampCfg->triggerDelayFall);

        /* enable PA ramp delayed enable unit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampPedEn_Set, device, 0x1);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampClkDivValue_Set, device, paRampDpClkDiv);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampDelayedEnableStartSel_Set, device, bfValue);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampDelayedEnableStopSel_Set, device, bfValue);

        if (paRampCfg->gpioTriggerSelect == true)
        {
            ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampEnableGpioSel_Set, device, (uint8_t)paRampCfg->gpioSource);
        }

        /* Enable the clock only after all registers are configured */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampClkEn_Set, device, 0x1);

        /* enable PA ramp module */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampEnable_Set, device, (uint8_t)paRampCfg->enable);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampPedTrigStartDelay_Set, device, paRampCfg->triggerDelayRise);

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampPedTrigEndDelay_Set, device, paRampCfg->triggerDelayFall);

        /* enable PA ramp delayed enable unit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampPedEn_Set, device, 0x1);
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_PaRamp_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                      adi_common_ChannelNumber_e channel,
                                                                      adi_adrv9001_PaRampCfg_t *paRampCfg)
{
    /* Check device pointer and gain pointer are not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, paRampCfg);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_API_RETURN(device);
}

static int32_t adrv9001_LutDataRead(adi_adrv9001_Device_t *device,
                                    uint16_t lutAddr,
                                    uint8_t *lutData)
{
    uint8_t bfValue = 0;

    /* Write PA LUT data address */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_LutAddr_Set, device, lutAddr);
    /* Set 'lut_wr_enable' to '0' to enable read mode */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_LutWrEnable_Set, device, 0x0);
    /* Read PA LUT data */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampLutRdData_Get, device, &bfValue);
    *lutData = bfValue;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_PaRamp_Inspect(adi_adrv9001_Device_t *device,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv9001_PaRampCfg_t *paRampCfg)
{
    uint8_t clkDivRatio = 0;
    uint8_t muxSel = 0;
    uint8_t bfValue = 0;
    uint8_t auxDacChannelShift = 0;
    uint8_t i = 0;
    uint8_t lutData = 0;
    uint16_t idx = 0;
    uint16_t paRampDpClkDiv = 0;
    uint16_t lutAddr = 0;
    uint32_t refClk_Hz = 0;
    uint32_t rampClock_kHz = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_PaRamp_Inspect_Validate, device, channel, paRampCfg);

    clkDivRatio = device->devStateInfo.clkDivideRatios.anaRefClockRatio;
    refClk_Hz = KILO_TO_BASE_UNIT(device->devStateInfo.deviceClock_kHz) >> clkDivRatio;

    if (channel == ADI_CHANNEL_1)
    {
        /* Select pa_ramp_tx1_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1LutSel_Set, device, 0x1);
    }
    else
    {
        /* Select pa_ramp_tx2_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2LutSel_Set, device, 0x1);
    }

    for (idx = 0; idx < ADRV9001_TX_PA_RAMP_LUT_SIZE; idx++)
    {
        lutAddr = idx * 2;
        ADI_EXPECT(adrv9001_LutDataRead, device, lutAddr, &lutData);
        paRampCfg->paRampLUT[idx] = (lutData >> 4) & 0xF;

        lutAddr++;
        ADI_EXPECT(adrv9001_LutDataRead, device, lutAddr, &lutData);
        paRampCfg->paRampLUT[idx] |= ((uint16_t)(lutData & 0xFF)) << 4;
    }

    if (channel == ADI_CHANNEL_1)
    {
        /* De-select pa_ramp_tx1_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1LutSel_Set, device, 0x0);

        /* Set PA Ramp up/down threshold for Tx1 */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1UpThreshold_Get, device, &(paRampCfg->upEndIndex));
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1DownThreshold_Get, device, &(paRampCfg->downEndIndex));
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx1Asymmetric_Get, device, &bfValue);
        paRampCfg->asymmetricRamp = (bool)bfValue;
    }
    else
    {
        /* De-select pa_ramp_tx2_lut_sel bit */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2LutSel_Set, device, 0x0);

        /* Set PA Ramp up/down threshold for Tx2 */
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2UpThreshold_Get, device, &(paRampCfg->upEndIndex));
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2DownThreshold_Get, device, &( paRampCfg->downEndIndex));
        ADI_EXPECT(adrv9001_NvsRegmapCore2_PaRampTx2Asymmetric_Get, device, &bfValue);
        paRampCfg->asymmetricRamp = (bool)bfValue;
    }

    /* 4bits AUXDAC mux selection.
       Bit[0] : AUXDAC 0 select :  1 : xbar_dac0, 0 : spi_auxdac0
       Bit[1] : AUXDAC 1 select :  1 : xbar_dac1, 0 : spi_auxdac1
       Bit[2] : AUXDAC 2 select :  1 : xbar_dac2, 0 : spi_auxdac2
       Bit[3] : AUXDAC 3 select :  1 : xbar_dac3, 0 : spi_auxdac3 */
    ADI_EXPECT(adrv9001_NvsRegmapCore2_AuxdacMuxsel_Get, device, &muxSel);

    for (i = 0; i < ADI_ADRV9001_MAX_AUXDACS; i++)
    {
        if (ADRV9001_BF_EQUAL(muxSel, ((uint32_t)1 << (uint32_t)i)))
        {
            /* Get the bit position for the auxDAC selected */
            auxDacChannelShift = i;
            break;
        }
    }

    paRampCfg->auxDacChannelSelect = (adi_adrv9001_AuxDac_e)((uint32_t)1 << auxDacChannelShift);

    if (channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampClkDivValue_Get, device, &paRampDpClkDiv);
        if (0 == paRampDpClkDiv)
        {
            paRampDpClkDiv = 1;
        }
        rampClock_kHz = refClk_Hz / paRampDpClkDiv;
        /* Convert to kHz */
        paRampCfg->rampClock_kHz = rampClock_kHz / 1000;

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampDelayedEnableStartSel_Get, device, &bfValue);

        paRampCfg->gpioTriggerSelect = (bool)bfValue;

        bfValue = 0;
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampEnableGpioSel_Get, device, &bfValue);

        paRampCfg->gpioSource = (adi_adrv9001_GpioPin_e)bfValue;    /* FIXME: JS: Not sure if this is correct */

        bfValue = 0;
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampEnable_Get, device, &bfValue);
        paRampCfg->enable = (bool)bfValue;

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampPedTrigStartDelay_Get, device, &(paRampCfg->triggerDelayRise));

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx1PaRampPedTrigEndDelay_Get, device, &(paRampCfg->triggerDelayFall));
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampClkDivValue_Get, device, &paRampDpClkDiv);
        if (0 == paRampDpClkDiv)
        {
            paRampDpClkDiv = 1;
        }
        rampClock_kHz = refClk_Hz / paRampDpClkDiv;
        /* Convert to kHz */
        paRampCfg->rampClock_kHz = rampClock_kHz / 1000;

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampDelayedEnableStartSel_Get, device, &bfValue);

        paRampCfg->gpioTriggerSelect = (bool)bfValue;

        bfValue = 0;
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampEnableGpioSel_Get, device, &bfValue);

        paRampCfg->gpioSource = (adi_adrv9001_GpioPin_e)bfValue;    /* FIXME: JS: Not sure if this is correct */

        bfValue = 0;
        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampEnable_Get, device, &bfValue);
        paRampCfg->enable = (bool)bfValue;

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampPedTrigStartDelay_Get, device, &(paRampCfg->triggerDelayRise));

        ADI_EXPECT(adrv9001_NvsRegmapCore2_Tx2PaRampPedTrigEndDelay_Get, device, &(paRampCfg->triggerDelayFall));
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_Attenuation_PinControl_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                                        adi_common_ChannelNumber_e channel,
                                                                                        adi_adrv9001_TxAttenuationPinControlCfg_t *config)
{
    uint8_t chan_index = 0;

    static const uint16_t TX_ATTEN_STEP_SIZE_MIN = 0;
    static const uint16_t TX_ATTEN_STEP_SIZE_MAX = 1550;

    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    /* Check device pointer and gain pointer are not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, config);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, config->stepSize_mdB, TX_ATTEN_STEP_SIZE_MIN, TX_ATTEN_STEP_SIZE_MAX);
    ADI_RANGE_CHECK(device, config->incrementPin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
    ADI_RANGE_CHECK(device, config->decrementPin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);
    if (config->incrementPin == config->decrementPin)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            config->decrementPin,
            "Invalid pin assignment. config->incrementPin cannot be the same as config->decrementPin");

        ADI_API_RETURN(device);
    }

    adi_common_channel_to_index(channel, &chan_index);

    if (device->devStateInfo.outputSignaling[chan_index] == ADI_ADRV9001_TX_DIRECT_FM_FSK)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_ADRV9001_ERR_INV_TX_ATTEN_MODE,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txAttenuationIndex,
            "Tx attenuation pin control configuration is not supported in TX_DIRECT_FM_FSK mode");
    }

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);
    if (state != ADI_ADRV9001_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            state,
            "Invalid channel state. Channel must be in CALIBRATED state");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_Attenuation_PinControl_Configure(adi_adrv9001_Device_t *device,
                                                         adi_common_ChannelNumber_e channel,
                                                         adi_adrv9001_TxAttenuationPinControlCfg_t *config)
{
    int8_t bfValue = 0;
    adrv9001_BfNvsRegmapTx_e txChannelBaseAddr = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_Attenuation_PinControl_Configure_Validate, device, channel, config);

    txChannelBaseAddr = Tx_Addr_Get(channel);

    bfValue = config->stepSize_mdB / 50;
    ADI_EXPECT(adrv9001_NvsRegmapTx_TxIncrDecrWord_Set, device, txChannelBaseAddr, bfValue);

    /* Set up GPIO pins */
    ADI_EXPECT(adi_adrv9001_gpio_ManualInput_Configure, device, config->incrementPin);
    ADI_EXPECT(adi_adrv9001_gpio_ManualInput_Configure, device, config->decrementPin);

    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx1TpcIncrAttenGpioSelect_Set, device, (config->incrementPin - 1));
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx1TpcIncrAttenGpioMask_Set, device, 0x0);

        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx1TpcDecrAttenGpioSelect_Set, device, (config->decrementPin - 1));
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx1TpcDecrAttenGpioMask_Set, device, 0x0);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx2TpcIncrAttenGpioSelect_Set, device, (config->incrementPin - 1));
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx2TpcIncrAttenGpioMask_Set, device, 0x0);

        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx2TpcDecrAttenGpioSelect_Set, device, (config->decrementPin - 1));
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx2TpcDecrAttenGpioMask_Set, device, 0x0);
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_Tx_Attenuation_PinControl_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                                      adi_common_ChannelNumber_e channel,
                                                                                      adi_adrv9001_TxAttenuationPinControlCfg_t *config)
{
    uint8_t chan_index = 0;

    /* Check device pointer and gain pointer are not null */
    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, config);
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    if (device->devStateInfo.outputSignaling[chan_index] == ADI_ADRV9001_TX_DIRECT_FM_FSK)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_ADRV9001_ERR_INV_TX_ATTEN_MODE,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            txAttenuationIndex,
            "Tx attenuation pin control Inspect is not supported in TX_DIRECT_FM_FSK mode");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_Attenuation_PinControl_Inspect(adi_adrv9001_Device_t *device,
                                                       adi_common_ChannelNumber_e channel,
                                                       adi_adrv9001_TxAttenuationPinControlCfg_t *config)
{
    uint8_t bfValue = 0;
    adrv9001_BfNvsRegmapTx_e txChannelBaseAddr = ADRV9001_BF_TX1_CORE;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Tx_Attenuation_PinControl_Inspect_Validate, device, channel, config);

    txChannelBaseAddr = Tx_Addr_Get(channel);

    ADI_EXPECT(adrv9001_NvsRegmapTx_TxIncrDecrWord_Get, device, txChannelBaseAddr, &bfValue);
    config->stepSize_mdB = (uint16_t)bfValue * 50;
    bfValue = 0;

    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx1TpcIncrAttenGpioSelect_Get, device, &bfValue);
        config->incrementPin = (adi_adrv9001_GpioPin_e)(bfValue + 1);
        bfValue = 0;

        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx1TpcDecrAttenGpioSelect_Get, device, &bfValue);
        config->decrementPin = (adi_adrv9001_GpioPin_e)(bfValue + 1);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx2TpcIncrAttenGpioSelect_Get, device, &bfValue);
        config->incrementPin = (adi_adrv9001_GpioPin_e)(bfValue + 1);
        bfValue = 0;

        ADI_EXPECT(adrv9001_NvsRegmapCore1_Tx2TpcDecrAttenGpioSelect_Get, device, &bfValue);
        config->decrementPin = (adi_adrv9001_GpioPin_e)(bfValue + 1);
    }

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adrv9001_Tx_FrequencyCorrection_Set_Validate(adi_adrv9001_Device_t *device,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           int32_t frequencyOffset_Hz)
{
    uint8_t chan_index = 0;
    int32_t maxFrequency_Hz = 0;
    static const int32_t DIVISOR = 5;
    static const int32_t ABSOLUTE_MAX_FREQUENCY_HZ = 20000;
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    maxFrequency_Hz  = KILO_TO_BASE_UNIT(device->devStateInfo.txInputRate_kHz[chan_index]) / DIVISOR;
    maxFrequency_Hz = (maxFrequency_Hz < ABSOLUTE_MAX_FREQUENCY_HZ) ? maxFrequency_Hz : ABSOLUTE_MAX_FREQUENCY_HZ;

    ADI_RANGE_CHECK(device, frequencyOffset_Hz, -maxFrequency_Hz, maxFrequency_Hz);

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);
    if (ADI_ADRV9001_CHANNEL_STANDBY == state)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            State.channelStates[ADI_TX][chan_index],
            "Error while attempting to set Tx frequency correction. Channel must be in CALIBRATED/PRIMED/RF_ENABLED.");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Tx_FrequencyCorrection_Set(adi_adrv9001_Device_t *device,
                                                adi_common_ChannelNumber_e channel,
                                                int32_t frequencyOffset_Hz,
                                                bool immediate)
{
    uint8_t extData[2] = { 0 };
    uint8_t armData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adrv9001_Tx_FrequencyCorrection_Set_Validate, device, channel, frequencyOffset_Hz);

    adrv9001_LoadFourBytes(&offset, armData, frequencyOffset_Hz);
    armData[offset] = (uint8_t)immediate;

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, (uint32_t)ADRV9001_ADDR_ARM_HIGHPRIORITY_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4)

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = ADRV9001_ARM_HIGHPRIORITY_SET_TX_FREQCORRECTION;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_HIGHPRIORITY_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
                                        ADRV9001_ARM_HIGHPRIORITY_SET_TX_FREQCORRECTION,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}
