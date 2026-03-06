/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_tx.c
* \brief Contains ADRV903X features related function implementation defined in
* adi_adrv903x_tx.h
*
* ADRV903X API Version: 2.12.1.4
*/

#include "adi_adrv903x_tx.h"

#include "../../private/include/adrv903x_tx.h"
#include "../../private/include/adrv903x_cpu.h"
#include "../../private/include/adrv903x_reg_addr_macros.h"
#include "../../private/include/adrv903x_gpio.h"
#include "../../private/include/adrv903x_radioctrl.h"
#include "../../private/include/adrv903x_stream_proc_types.h"
#include "../../private/bf/adrv903x_bf_tx_funcs.h"
#include "../../private/bf/adrv903x_bf_tx_dig.h"


#define ADI_FILE ADI_ADRV903X_FILE_PUBLIC_TX

#define TX_ATTEN_EDGE_TRIGGER 0U
#define TX_ATTEN_LEVEL_TRIGGER 1U
#define TX_ATTEN_TABLE_INDEX_RANGE_CHECK 33U

/* Some tyepdefs and static fn ptrs to make the S0S1 Bf access manageable */
typedef adi_adrv903x_ErrAction_e(*txAttenSetFnPtr_t)(adi_adrv903x_Device_t *device,
    adi_adrv903x_SpiCache_t *spiCache,
    adrv903x_BfCoreChanAddr_e baseAddr,
    uint16_t bfValue);

typedef adi_adrv903x_ErrAction_e(*txAttenGetFnPtr_t)(adi_adrv903x_Device_t *device,
    adi_adrv903x_SpiCache_t *spiCache,
    adrv903x_BfCoreChanAddr_e baseAddr,
    uint16_t *bfValue);

typedef adi_adrv903x_ErrAction_e(*txAttenUpdateFnPtr_t)(adi_adrv903x_Device_t* const device,
    adi_adrv903x_SpiCache_t* const spiCache,
    const adrv903x_BfCoreChanAddr_e baseAddr,
    const uint8_t bfValue);

static const txAttenSetFnPtr_t txAttenChanS0S1SetFnPtrs[ADI_ADRV903X_MAX_TXCHANNELS][2] = 
{
    { adrv903x_Core_Tx0AttenS0_BfSet, adrv903x_Core_Tx0AttenS1_BfSet },
    { adrv903x_Core_Tx1AttenS0_BfSet, adrv903x_Core_Tx1AttenS1_BfSet },
    { adrv903x_Core_Tx2AttenS0_BfSet, adrv903x_Core_Tx2AttenS1_BfSet },
    { adrv903x_Core_Tx3AttenS0_BfSet, adrv903x_Core_Tx3AttenS1_BfSet },
    { adrv903x_Core_Tx4AttenS0_BfSet, adrv903x_Core_Tx4AttenS1_BfSet },
    { adrv903x_Core_Tx5AttenS0_BfSet, adrv903x_Core_Tx5AttenS1_BfSet },
    { adrv903x_Core_Tx6AttenS0_BfSet, adrv903x_Core_Tx6AttenS1_BfSet },        
    { adrv903x_Core_Tx7AttenS0_BfSet, adrv903x_Core_Tx7AttenS1_BfSet },        
};

static const txAttenGetFnPtr_t txAttenChanS0S1GetFnPtrs[ADI_ADRV903X_MAX_TXCHANNELS][2] = 
{
    { adrv903x_Core_Tx0AttenS0_BfGet, adrv903x_Core_Tx0AttenS1_BfGet },
    { adrv903x_Core_Tx1AttenS0_BfGet, adrv903x_Core_Tx1AttenS1_BfGet },
    { adrv903x_Core_Tx2AttenS0_BfGet, adrv903x_Core_Tx2AttenS1_BfGet },
    { adrv903x_Core_Tx3AttenS0_BfGet, adrv903x_Core_Tx3AttenS1_BfGet },
    { adrv903x_Core_Tx4AttenS0_BfGet, adrv903x_Core_Tx4AttenS1_BfGet },
    { adrv903x_Core_Tx5AttenS0_BfGet, adrv903x_Core_Tx5AttenS1_BfGet },
    { adrv903x_Core_Tx6AttenS0_BfGet, adrv903x_Core_Tx6AttenS1_BfGet },        
    { adrv903x_Core_Tx7AttenS0_BfGet, adrv903x_Core_Tx7AttenS1_BfGet },        
};

static const txAttenUpdateFnPtr_t txAttenChanS0S1UpdateFnPtrs[ADI_ADRV903X_MAX_TXCHANNELS][2] = 
{
    { adrv903x_Core_Tx0AttenUpdateS0_BfSet, adrv903x_Core_Tx0AttenUpdateS1_BfSet },
    { adrv903x_Core_Tx1AttenUpdateS0_BfSet, adrv903x_Core_Tx1AttenUpdateS1_BfSet },
    { adrv903x_Core_Tx2AttenUpdateS0_BfSet, adrv903x_Core_Tx2AttenUpdateS1_BfSet },
    { adrv903x_Core_Tx3AttenUpdateS0_BfSet, adrv903x_Core_Tx3AttenUpdateS1_BfSet },
    { adrv903x_Core_Tx4AttenUpdateS0_BfSet, adrv903x_Core_Tx4AttenUpdateS1_BfSet },
    { adrv903x_Core_Tx5AttenUpdateS0_BfSet, adrv903x_Core_Tx5AttenUpdateS1_BfSet },
    { adrv903x_Core_Tx6AttenUpdateS0_BfSet, adrv903x_Core_Tx6AttenUpdateS1_BfSet },
    { adrv903x_Core_Tx7AttenUpdateS0_BfSet, adrv903x_Core_Tx7AttenUpdateS1_BfSet },
};
ADI_API adi_adrv903x_ErrAction_e  adi_adrv903x_TxAttenTableRead(adi_adrv903x_Device_t* const device,
                                                                const adi_adrv903x_TxChannels_e txChannel,
                                                                const uint32_t txAttenIndexOffset,
                                                                adi_adrv903x_TxAttenTableRow_t txAttenTableRows[],
                                                                const uint32_t numTxAttenEntries)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t i = 0U;
    uint32_t tableAddr = 0U;
    /* Ensure alignment of cfgData */
    uint32_t cfgData32[ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX / 4U] = { 0U };
    uint8_t* cfgData = (uint8_t*) cfgData32;
    uint32_t start = 0U;
    uint32_t stop = 0U;
    uint32_t data = 0U;
    uint32_t offset;
    uint32_t numBytesToRead = 0U;
    static const uint8_t TX_ENTRY_SIZE = 4U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    tableAddr = adrv903x_txAttenAddrLookup(txChannel);
    if (tableAddr == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid TxChannel");
        goto cleanup;
    }

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & txChannel) != txChannel)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "TxChannel not initialized");
        goto cleanup;
    }

    if (numTxAttenEntries == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numTxAttenEntries, "Number of TxAttenuation Entries cannot be 0");
        goto cleanup;
    }

    if ((txAttenIndexOffset + numTxAttenEntries - 1U) > ADRV903X_TX_ATTEN_TABLE_MAX_IDX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numTxAttenEntries, "Request exceeds Attenuation Table Bounds");
        goto cleanup;
    }

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txAttenTableRows, cleanup);

    start = 0U;
    stop = 0U;
    offset = (txAttenIndexOffset * TX_ENTRY_SIZE) + tableAddr;

    numBytesToRead = numTxAttenEntries * TX_ENTRY_SIZE;
    if (numBytesToRead > ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX)
    {
        numBytesToRead = ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX;
    }

    recoveryAction = adi_adrv903x_Registers32bOnlyRead(device, NULL, (offset + stop), cfgData, numBytesToRead);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read Issue");
        goto cleanup;
    }

    for (i = 0; i < numTxAttenEntries; i++)
    {
        data = *((uint32_t*)(cfgData + start));
        txAttenTableRows[i].txAttenHp = ADRV903X_BF_DECODE(data, ADRV903X_TX_ATTEN_TABLE_HP_MASK, ADRV903X_TX_ATTEN_TABLE_HP_SHIFT);
        txAttenTableRows[i].txAttenMult = ADRV903X_BF_DECODE(data, ADRV903X_TX_ATTEN_TABLE_MULT_MASK, ADRV903X_TX_ATTEN_TABLE_MULT_SHIFT);

        start += TX_ENTRY_SIZE;

        if ((start >= ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX) &&
            ((i + 1U) < numTxAttenEntries))
        {
            numBytesToRead = (numTxAttenEntries - (i + 1U)) * TX_ENTRY_SIZE;
            if (numBytesToRead > ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX)
            {
                numBytesToRead = ADI_ADRV903X_TX_ATTEN_TABLE_CACHE_MAX;
            }

            stop += start;
            recoveryAction = adi_adrv903x_Registers32bOnlyRead(device, NULL, (offset + stop), cfgData, numBytesToRead);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read Issue");
                goto cleanup;
            }
            start = 0U;
        }
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenSet(adi_adrv903x_Device_t* const device,
                                                         const adi_adrv903x_TxAtten_t txAttenuation[],
                                                         const uint32_t               numTxAttenConfigs)
{
        static const uint8_t DATA_TRANSACTION_1_SIZE   = 9u;
    static const uint8_t DATA_TRANSACTION_2_SIZE   = 6U;
    static const uint8_t DATA_TRANSATCTION_FF_SIZE = 3U;
    static const uint8_t CH0_START_ADDRESS         = 0xB3U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e            halError       = ADI_HAL_ERR_PARAM;

    uint32_t attenRegVal = 0U;
    uint32_t i           = 0U;

    /* Variables for minimum Tx attenuation range check - only for A0 and A1 silicon  */
    uint16_t minAttenIndexSet = UINT16_MAX;
    uint16_t attenIndex = 0U;

    uint8_t chanId          = 0U;
    uint8_t addrLsb         = 0U;
    uint8_t latchMask       = 0U;
    uint8_t attenLsb        = 0U;
    /* Hardcoded 9 byte data array for first spi transaction with placeholders for addresses and data
       data1[0] : 0x01U msb of masking register address
       data1[1] : 0x32U lsb of masking register address
       data1[2] : 0x03U mask indicating which bits to set in target register
       data1[3] : 0x01U msb of address to write
       data1[4] : 0x00U PLACEHOLDER - will be replaced with msb (i.e. lsb +1 address ) corresponding to address of relevant tx channel
       data1[5] : 0x00U PLACEHOLDER - will be replaced with the lsb for given tx attenuation value
       data1[6] : 0x01U msb of address to write
       data1[7] : 0x00U PLACEHOLDER - will be replaced with lsb corresponding to address of relevant tx channel
       data1[8] : 0x00U PLACEHOLDER - will be replaced with the msb for given tx attenuation value
    */
    uint8_t data1[] = { 0x01U, 0x32U, 0x03U, 0x01U, 0x00U, 0x00U, 0x01U, 0x00U, 0x00U };

    /* Spi Transaction array data for general case of latching mask != 0xFF which does require masking register usage
       data1[0] : 0x01U msb of masking register address
       data1[1] : 0x32U lsb of masking register address
       data1[2] : 0x00U Placeholder - to be replaced by latching mask calculated during function
       data1[3] : 0x01U msb of latch register address
       data1[4] : 0xD3U lsb of latch register address
       data1[5] : 0x00U Placeholder - will be replaced with latch mask value calculated in function
    */
    uint8_t data2[] = { 0x01U, 0x32U, 0x00U, 0x01U, 0xD3U, 0x00U };
    /*  Spi Transaction array data for special case of latching mask == 0xFF (all channels) which does not require masking register 
        dataFF[0] : 0x01U msb of latch register address
        dataFF[1] : 0xD3U lsb of latch register address
        dataFF[2] : 0x00U Placeholder - will be replaced with latch mask value calculated in function
    */
    uint8_t dataFF[] = { 0x01U, 0xD3U, 0x00U };
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txAttenuation, cleanup);

    if (numTxAttenConfigs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numTxAttenConfigs, "Invalid Number of TxAttentuation Configurations");
        goto cleanup;
    }
    
    /* Loop through the number of configurations and check attenuation mode */
    for (i = 0U; i < numTxAttenConfigs; ++i)
    {
        if (txAttenuation[i].txAttenuation_mdB > ADRV903X_TX_ATTEN_VALUE_MILLI_DB_MAX)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txAttenuation[i].txAttenuation_mdB,
                                   "Invalid txAttenuation_mdB value");
            goto cleanup;
        }
    
        /*Check that if requested Tx Channel valid*/
        if (((txAttenuation[i].txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) ||
            (txAttenuation[i].txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txAttenuation[i].txChannelMask,
                                   "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            goto cleanup;
        }
    }

    /* Loop through the number of configurations passed */
    for (i = 0U; i < numTxAttenConfigs; ++i)
    {
        /* for each Tx channel in chanMask */
        for (chanId = 0U; chanId <= ADI_ADRV903X_TX_CHAN_ID_MAX; ++chanId)
        {
            if ((txAttenuation[i].txChannelMask & (1U << chanId)) == 0U)
            {
                /* Skip channel. It's not in chanMask. */
                continue;
            }

            /* Conversion from the requested atten level (milli-dB) to equivalent 
             * TxAttenTable index is always done based on a step size of 0.05.
             * This also means the register value to be programmed equals the
             * atten table index. Other step sizes are not supported. */
            attenIndex = (txAttenuation[i].txAttenuation_mdB / ADRV903X_TX_ATTEN_STEP_SIZE_DIV_0P05);
            attenRegVal = attenIndex;
            if (attenIndex  < minAttenIndexSet)
            {
                minAttenIndexSet = attenIndex;
            }

            /* Calculate address based on channel id */
            addrLsb = CH0_START_ADDRESS + (chanId << 2U);
            /* Isolate Lsb for data to write */
            attenLsb = (uint8_t)(attenRegVal - ((attenRegVal >> 8U) << 8U));
            /* pack placeholders in spi data1 array */
            data1[4U] = (uint8_t)(addrLsb + 1U);
            data1[5U] = (uint8_t)(attenRegVal >> 8U);
            data1[7U] = addrLsb;
            data1[8U] = attenLsb;
            halError = adi_hal_SpiWrite(device->common.devHalInfo, data1, DATA_TRANSACTION_1_SIZE);
            recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       data1,
                                       "Error writing tx atten values");
                goto cleanup;
            }

            /* build mask for latching only selected channel latches */
            latchMask |= 1U << chanId;
        } 
    }
    /* Special Case where mask of 0xFF does not require a masking register to be wrote first */
    if (latchMask == 0xFFU)
    {
        dataFF[2U] = latchMask;
        halError = adi_hal_SpiWrite(device->common.devHalInfo, dataFF, DATA_TRANSATCTION_FF_SIZE);
        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dataFF,
                                   "Error writing Attenuation Latch Value");
            goto cleanup;
        }
    }
    else
    {
        data2[2U] = latchMask;
        data2[5U] = latchMask;
        halError = adi_hal_SpiWrite(device->common.devHalInfo, data2, DATA_TRANSACTION_2_SIZE);
        recoveryAction = (adi_adrv903x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   data2,
                                   "Error writing Attenuation Latch Value");
            goto cleanup;
        }
    }

    cleanup:
    /* Minimum atten range check is only applied for A0 and A1 silicon */
    if ((device->devStateInfo.deviceSiRev == 0xA0U) || (device->devStateInfo.deviceSiRev == 0xA1))
    {
        if (minAttenIndexSet <= TX_ATTEN_TABLE_INDEX_RANGE_CHECK)
        {
            /* The attenuation was set successfully but we need to log a warning due to one or more of the table indices used. */
            ADI_VARIABLE_LOG(&device->common, ADI_HAL_LOG_WARN, "WARNING: Tx Attenuation Table Index 0-33 are under implementation.", NULL, NULL);
        }
    }
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction); 

}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenGet(adi_adrv903x_Device_t * const    device,
                                                         const adi_adrv903x_TxChannels_e  txChannel,
                                                         adi_adrv903x_TxAtten_t * const   txAttenuation)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint16_t txAttenReadBack = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txAttenuation, cleanup);

    switch (txChannel)
    {
        case ADI_ADRV903X_TX0:
            recoveryAction = adrv903x_Core_Tx0AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TX1:
            recoveryAction = adrv903x_Core_Tx1AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TX2:
            recoveryAction = adrv903x_Core_Tx2AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TX3:
            recoveryAction = adrv903x_Core_Tx3AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TX4:
            recoveryAction = adrv903x_Core_Tx4AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TX5:
            recoveryAction = adrv903x_Core_Tx5AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TX6:
            recoveryAction = adrv903x_Core_Tx6AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TX7:
            recoveryAction = adrv903x_Core_Tx7AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV903X_TXOFF:    /* Fall Through */
        case ADI_ADRV903X_TXALL:
        
        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    txChannel,
                                    "Invalid TxChannel");
            goto cleanup;
            break;

    }

    /* API Call Check for all Switch Statement */
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV903X_ERRSRC_API,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            txChannel,
                            "Failure to Read Tx Attenuation for Given Channel");
        goto cleanup;
    }


    /* Convert from atten table index to milli dB */
    txAttenuation->txAttenuation_mdB = (txAttenReadBack * ADRV903X_TX_ATTEN_STEP_SIZE_DIV_0P05);
    txAttenuation->txChannelMask = txChannel;
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction); 
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxNcoShifterSet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_TxNcoMixConfig_t * const txNcoConfig)
{
        adi_adrv903x_ErrAction_e        recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_TxNcoMixConfig_t   txNcoInfo;
    adrv903x_TxNcoMixConfigResp_t   cmdRsp;
    adrv903x_CpuCmdStatus_e         cmdStatus       = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e         cpuErrorCode    = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                        cpuTypeIdx      = 0U;
    uint8_t                         channelMaskRet  = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txNcoConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoInfo, 0, sizeof(txNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (txNcoConfig->chanSelect == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    if (txNcoConfig->enable > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->enable, "Invalid enable provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoInfo.chanSelect   = (uint8_t) txNcoConfig->chanSelect;
    txNcoInfo.enable       = (uint8_t) txNcoConfig->enable;
    txNcoInfo.phase        = (uint32_t) ADRV903X_HTOCL(txNcoConfig->phase);
    txNcoInfo.frequencyKhz = (int32_t) ADRV903X_HTOCL(txNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(device,
                                             (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_SET_TX_MIX_NCO,
                                             (void*)&txNcoInfo,
                                             sizeof(txNcoInfo),
                                             (void*)&cmdRsp,
                                             sizeof(cmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == txNcoInfo.chanSelect)
        {
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV903X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV903X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxNcoShifterGet(adi_adrv903x_Device_t* const device,
                                                              adi_adrv903x_TxNcoMixConfigReadbackResp_t* const txRbConfig)
{
        adi_adrv903x_ErrAction_e                    recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_TxNcoMixConfigReadback_t           txNcoCfgRbCmd;
    adi_adrv903x_TxNcoMixConfigReadbackResp_t   txNcoCfgGetCmdRsp;
    adrv903x_CpuCmdStatus_e                     cmdStatus           = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e                     cpuErrorCode        = ADRV903X_CPU_NO_ERROR;
    uint32_t                                    cpuType             = 0U;
    uint32_t                                    chIdx               = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoCfgRbCmd, 0, sizeof(txNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&txNcoCfgGetCmdRsp, 0, sizeof(txNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated*/
    if (txRbConfig->chanSelect == (uint8_t) ADI_ADRV903X_TXOFF ||
        txRbConfig->chanSelect > (uint8_t) ADI_ADRV903X_TX7    ||
        (((txRbConfig->chanSelect - 1) & txRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Get CPU assigned to this channel */
    for (chIdx = 0U; chIdx < ADI_ADRV903X_MAX_CHANNELS; chIdx++)
    {
        if (txRbConfig->chanSelect == ((uint8_t)1U << chIdx))
        {
            cpuType = device->initExtract.rxTxCpuConfig[chIdx];
            break;
        }
    }

    /* Prepare the command payload */
    txNcoCfgRbCmd.chanSelect = (uint8_t) txRbConfig->chanSelect;

    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(device,
                                         (adi_adrv903x_CpuType_e)cpuType,
                                         ADRV903X_LINK_ID_0,
                                         ADRV903X_CPU_CMD_ID_GET_TX_MIX_NCO,
                                         (void*)&txNcoCfgRbCmd,
                                         sizeof(txNcoCfgRbCmd),
                                         (void*)&txNcoCfgGetCmdRsp,
                                         sizeof(txNcoCfgGetCmdRsp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO((adrv903x_CpuErrorCode_e)txNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
    }

    if (txNcoCfgGetCmdRsp.chanSelect == txNcoCfgRbCmd.chanSelect)
    {
        /* Extract the command-specific response from the response payload */
        txRbConfig->enable       = (uint8_t) txNcoCfgGetCmdRsp.enable;
        txRbConfig->phase        = (uint32_t) ADRV903X_CTOHL(txNcoCfgGetCmdRsp.phase);
        txRbConfig->frequencyKhz = (int32_t) ADRV903X_CTOHL(txNcoCfgGetCmdRsp.frequencyKhz);
    }
    else
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Channel is not handled by the CPU");
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxTestToneSet(adi_adrv903x_Device_t* const device,
                                                            const adi_adrv903x_TxTestNcoConfig_t * const txNcoConfig)
{
        adi_adrv903x_ErrAction_e        recoveryAction  = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adi_adrv903x_TxTestNcoConfig_t  txNcoInfo;
    adrv903x_TxTestNcoConfigResp_t  cmdRsp;
    adrv903x_CpuCmdStatus_e         cmdStatus       = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e         cpuErrorCode    = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                        cpuTypeIdx      = 0U;
    uint8_t                         channelMaskRet  = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txNcoConfig, cleanup);
    ADI_LIBRARY_MEMSET(&txNcoInfo, 0, sizeof(txNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (txNcoConfig->chanSelect == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    if (txNcoConfig->ncoSelect >= ADI_ADRV903X_TX_TEST_NCO_MAX_NUM)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->ncoSelect, "Invalid ncoSelect provided.");
        goto cleanup;
    }

    if (txNcoConfig->enable > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->enable, "Invalid enable provided.");
        goto cleanup;
    }

    if (txNcoConfig->attenCtrl >= ADI_ADRV903X_TX_TEST_NCO_ATTEN_MAX_NUM)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->attenCtrl, "Invalid attenCtrl provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoInfo.chanSelect   = (uint8_t) txNcoConfig->chanSelect;
    txNcoInfo.ncoSelect    = (adi_adrv903x_TxTestNcoSelect_t) txNcoConfig->ncoSelect;
    txNcoInfo.enable       = (uint8_t) txNcoConfig->enable;
    txNcoInfo.phase        = (uint32_t) ADRV903X_HTOCL(txNcoConfig->phase);
    txNcoInfo.frequencyKhz = (int32_t) ADRV903X_HTOCL(txNcoConfig->frequencyKhz);
    txNcoInfo.attenCtrl    = (adi_adrv903x_TxTestNcoAtten_t) txNcoConfig->attenCtrl;

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(device,
                                             (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_SET_TX_TEST_NCO,
                                             (void*)&txNcoInfo,
                                             sizeof(txNcoInfo),
                                             (void*)&cmdRsp,
                                             sizeof(cmdRsp),
                                             &cmdStatus);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == txNcoInfo.chanSelect)
        {
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV903X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV903X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxTestToneGet(adi_adrv903x_Device_t* const device,
                                                            adi_adrv903x_TxTestNcoConfigReadbackResp_t* const txRbConfig)
{
        adi_adrv903x_ErrAction_e                    recoveryAction      = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_TxTestNcoConfigReadback_t          txNcoCfgRbCmd;
    adi_adrv903x_TxTestNcoConfigReadbackResp_t  txNcoCfgGetCmdRsp;
    adrv903x_CpuCmdStatus_e                     cmdStatus           = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e                     cpuErrorCode        = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                    cpuTypeIdx          = 0U;
    uint8_t                                     channelMaskRet      = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoCfgRbCmd, 0, sizeof(txNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&txNcoCfgGetCmdRsp, 0, sizeof(txNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated*/
    if (txRbConfig->chanSelect == (uint8_t) ADI_ADRV903X_TXOFF || 
        txRbConfig->chanSelect > (uint8_t) ADI_ADRV903X_TX7    || 
        (((txRbConfig->chanSelect - 1) & txRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }


    if (txRbConfig->ncoSelect >= ADI_ADRV903X_TX_TEST_NCO_MAX_NUM)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->ncoSelect, "Invalid ncoSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoCfgRbCmd.chanSelect = (uint8_t) txRbConfig->chanSelect;
    txNcoCfgRbCmd.ncoSelect  = (adi_adrv903x_TxTestNcoSelect_t) txRbConfig->ncoSelect;

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV903X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv903x_CpuCmdSend(device,
                                             (adi_adrv903x_CpuType_e)cpuTypeIdx,
                                             ADRV903X_LINK_ID_0,
                                             ADRV903X_CPU_CMD_ID_GET_TX_TEST_NCO,
                                             (void*)&txNcoCfgRbCmd,
                                             sizeof(txNcoCfgRbCmd),
                                             (void*)&txNcoCfgGetCmdRsp,
                                             sizeof(txNcoCfgGetCmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO((adrv903x_CpuErrorCode_e)txNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet = txNcoCfgGetCmdRsp.chanSelect;
        if (channelMaskRet == txNcoCfgRbCmd.chanSelect)
        {
            /* Extract the command-specific response from the response payload */
            txRbConfig->chanSelect   = (uint8_t)txNcoCfgGetCmdRsp.chanSelect;
            txRbConfig->ncoSelect    = (uint8_t) txNcoCfgGetCmdRsp.ncoSelect;
            txRbConfig->enabled      = (uint8_t) txNcoCfgGetCmdRsp.enabled;
            txRbConfig->attenCtrl    = txNcoCfgGetCmdRsp.attenCtrl;
            txRbConfig->phase        = (uint32_t) ADRV903X_CTOHL(txNcoCfgGetCmdRsp.phase);
            txRbConfig->frequencyKhz = (int32_t) ADRV903X_CTOHL(txNcoCfgGetCmdRsp.frequencyKhz);
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV903X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV903X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenS0S1Set(adi_adrv903x_Device_t* const   device,
                                                             const uint32_t                 chanMask,
                                                             const uint32_t                 levelMilliDB,
                                                             const uint8_t                  isS0)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t chanId = 0U;
    uint8_t isS1 = !isS0;
    uint32_t attenRegVal = 0U;

    /* Variables for minimum Tx attenuation range check - only for A0 and A1 silicon  */
    uint16_t minAttenIndexSet = UINT16_MAX;
    uint16_t attenIndex = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    if ((chanMask > ADI_ADRV903X_TX_CHAN_MASK_MAX) ||
        (chanMask == 0U))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                chanMask,
                                "Invalid channel mask");
        goto cleanup;
    }

    if (levelMilliDB > ADRV903X_TX_ATTEN_VALUE_MILLI_DB_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                levelMilliDB,
                                "Value exceeds max attenuation value");
        goto cleanup;
    }

    /* for each Tx channel in chanMask */
    for (chanId = 0U; chanId <= ADI_ADRV903X_TX_CHAN_ID_MAX; ++chanId)
    {
        if ((chanMask & (1U << chanId)) == 0U)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        /* ChanId is selected in chanMask */

        /* Conversion from the requested atten level (milli-dB) to equivalent 
         * TxAttenTable index is always done based on a step size of 0.05.
         * This also means the register value to be programmed equals the
         * atten table index. Other step sizes are not supported. */
        attenIndex = (levelMilliDB / ADRV903X_TX_ATTEN_STEP_SIZE_DIV_0P05);
        attenRegVal = attenIndex;

        if (attenIndex  < minAttenIndexSet)
        {
            minAttenIndexSet = attenIndex;
        }

        /* Write to the S0/S1 register */
        recoveryAction = txAttenChanS0S1SetFnPtrs[chanId][isS1](device,
                                                                NULL,
                                                                ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                attenRegVal);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_ADRV903X_ERRSRC_API,
                                ADI_COMMON_ERRCODE_API,
                                recoveryAction,
                                chanId,
                                "Failure to write S0/S1 TxAttenuation");
            goto cleanup;
        }

        /* Latch in the S0/S1 register */
        recoveryAction = txAttenChanS0S1UpdateFnPtrs[chanId][isS1]( device,
                                                                    NULL,
                                                                    ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                    1U);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_ADRV903X_ERRSRC_API,
                                ADI_COMMON_ERRCODE_API,
                                recoveryAction,
                                chanId,
                                "Failure to update S0/S1 TxAttenuation");
            goto cleanup;
        }
    }   /* for each Tx channel */
cleanup:
    /* Minimum atten range check is only applied for A0 and A1 silicon */
    if ((device->devStateInfo.deviceSiRev == 0xA0) || (device->devStateInfo.deviceSiRev == 0xA1))
    {
        if ((recoveryAction == ADI_ADRV903X_ERR_ACT_NONE) &&
            (minAttenIndexSet <= TX_ATTEN_TABLE_INDEX_RANGE_CHECK))
        {
            /* The attenuation was set successfully but we need to give an error due to one or more of the table indices used. */
            recoveryAction = ADI_ADRV903X_ERR_ACT_RESET_FEATURE;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   minAttenIndexSet,
                                   "WARNING: Tx Attenuation Table Index 0-33 are under implementation.");
        }
    }
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);   
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenS0S1Get(adi_adrv903x_Device_t* const device,
                                                             const adi_adrv903x_TxChannels_e txChannel,
                                                             uint32_t* const levelMilliDB,
                                                             const uint8_t isS0)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t isS1 = !isS0;
    uint16_t attenRegVal = 0U;
    uint8_t chanId = 0U;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, levelMilliDB, cleanup);

    ADI_ADRV903X_API_ENTRY(&device->common);

    chanId = adrv903x_TxChannelsToId(txChannel);
    if (chanId > ADI_ADRV903X_TX_CHAN_ID_MAX)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
            chanId,                        
            "Invalid channel id");
        goto cleanup;
    }
        
    /* Read back the S0/S1 register */
    recoveryAction = (adi_adrv903x_ErrAction_e)txAttenChanS0S1GetFnPtrs[chanId][isS1](device,
        NULL,
        ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
        &attenRegVal);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV903X_ERRSRC_API,
            ADI_COMMON_ERRCODE_API,
            recoveryAction,
            chanId,
            "Failure to write S0/S1 TxAttenuation");
        goto cleanup;
    }
            
    /* Conversion from the attenTableIndex read back to the equivalent 
     * atten level (milli-Db) is always done based on a step size of
     * 0.05. Other step sizes are not supported.
     */
    *levelMilliDB = attenRegVal * ADRV903X_TX_ATTEN_STEP_SIZE_DIV_0P05;
                
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);   
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenUpdateCfgSet(adi_adrv903x_Device_t* const device,
                                                                  const uint32_t chanMask,
                                                                  const adi_adrv903x_TxAttenUpdateCfg_t* const cfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint8_t chanId = 0;
    uint32_t chanSel = 0U;
    adrv903x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv903x_BfTxDigChanAddr_e)0U;
    adrv903x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = (adrv903x_BfTxFuncsChanAddr_e)0U;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cfg, cleanup);

    if ((chanMask > ADI_ADRV903X_TX_CHAN_MASK_MAX) || 
        (chanMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
            chanMask,                        
            "Invalid channel mask");
        goto cleanup;
    }
    
    if (cfg->trgCfg.updateTrg != ADI_ADRV903X_TXATTEN_UPD_TRG_NONE &&
        cfg->trgCfg.updateTrg != ADI_ADRV903X_TXATTEN_UPD_TRG_SPI &&
        cfg->trgCfg.updateTrg != ADI_ADRV903X_TXATTEN_UPD_TRG_GPIO)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            cfg->trgCfg.updateTrg,
            "Invalid ADI_ADRV903X_TXATTEN_UPD_TRG value");
        goto cleanup;
    }
    
    if (cfg->srcCfg.updateSrc != ADI_ADRV903X_TXATTEN_UPD_SRC_S0 &&
        cfg->srcCfg.updateSrc != ADI_ADRV903X_TXATTEN_UPD_SRC_S0_OR_S1)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            cfg->srcCfg.updateSrc,
            "Invalid ADI_ADRV903X_TXATTEN_UPD_SRC value");
        goto cleanup;        
    }
        
    /* If update is via gpio verify the trigger gpio is valid.
     * Note: Due to h/w pinmux limitations pin0 cannot be routed for use as the trigger. */    
    if (cfg->trgCfg.updateTrg == ADI_ADRV903X_TXATTEN_UPD_TRG_GPIO && 
        (cfg->trgCfg.triggerGpio >= ADI_ADRV903X_GPIO_INVALID ||
         cfg->trgCfg.triggerGpio == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
            cfg->trgCfg.triggerGpio,                        
            "Invalid triggerGpio");
        goto cleanup;
    }
    
    /* If use of S0/S1 is determined via gpio ensure gpio is valid.
     * Note: Due to h/w pinmux limitations pin0 cannot be routed for use as the S0_S1 src */
    if (cfg->srcCfg.updateSrc == ADI_ADRV903X_TXATTEN_UPD_SRC_S0_OR_S1 &&
        (cfg->srcCfg.s0OrS1Gpio >= ADI_ADRV903X_GPIO_INVALID ||
         cfg->srcCfg.s0OrS1Gpio == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
            cfg->srcCfg.s0OrS1Gpio,                        
            "Invalid s0OrS1Gpio");
        goto cleanup;
    }
    
    /* If update is via gpio and S1/S0 is determined by gpio verify
     * the gpios are different */ 
    if (cfg->trgCfg.updateTrg == ADI_ADRV903X_TXATTEN_UPD_TRG_GPIO && 
        cfg->srcCfg.updateSrc == ADI_ADRV903X_TXATTEN_UPD_SRC_S0_OR_S1 &&        
        cfg->trgCfg.triggerGpio == cfg->srcCfg .s0OrS1Gpio )
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
            ADI_NO_VARIABLE,                        
            "Cannot use same GPIO for trigger gpio and S0/S1 selection");
        goto cleanup;
    }    
            
    /* The global CoreSpiEnable bit is set for SPI trigger; Unset for any other trigger. */
    recoveryAction =  adrv903x_Core_TxAttenUpdCoreSpiEn_BfSet(device,
                                                              NULL,
                                                              ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                              (cfg->trgCfg.updateTrg == ADI_ADRV903X_TXATTEN_UPD_TRG_SPI));
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv903x_Core_TxAttenUpdCoreSpiEn_BfSet failed.");
        goto cleanup;
    } 
    
    /* Routing of GPIO signals to TxChans for update triggering 
     * must be done for all channels in a single call not channel by channel */    
    if (cfg->trgCfg.updateTrg == ADI_ADRV903X_TXATTEN_UPD_TRG_GPIO)
    {        
        /* Route selected trigger GPIO to TxChans*/
        recoveryAction = adrv903x_GpioSignalSet(device,
            cfg->trgCfg.triggerGpio, 
            ADI_ADRV903X_GPIO_SIGNAL_TX_ATTEN_UPD_GPIO,
            chanMask);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                recoveryAction,
                "Call to adrv903x_GpioSignalSet failed for trigger GPIO");
            goto cleanup;
        }
    }
    
    /* Routing of GPIO signals to TxChans for S0/S1 selection  
     * must be done for all channels in a single call not channel by channel */
    if (cfg->srcCfg.updateSrc == ADI_ADRV903X_TXATTEN_UPD_SRC_S0_OR_S1)
    {
        /* Configuring which GPIO selects S0/S1. */
        recoveryAction = adrv903x_GpioSignalSet(device,
            cfg->srcCfg.s0OrS1Gpio,
            ADI_ADRV903X_GPIO_SIGNAL_SELECT_S1,
            chanMask);
            
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                recoveryAction,
                "Call to adrv903x_GpioSignalSet failed for S0/S1 gpio.");
            goto cleanup;
        }
    }

    /* for each Tx channel in chanMask */
    for (chanId = 0U; chanId <= ADI_ADRV903X_TX_CHAN_ID_MAX; chanId++)
    {
        chanSel = 1U << chanId;

        if ((chanMask & (1 << chanId)) == 0)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        /* Convert the chanId to the base address value required by the bitfield functions */
        recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txFuncsChanBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        recoveryAction = adrv903x_TxDigBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txDigChanBaseAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }
                
        switch (cfg->trgCfg.updateTrg)
        {
        case ADI_ADRV903X_TXATTEN_UPD_TRG_NONE:
            /* Do not trigger update from GPIO */
            recoveryAction = adrv903x_TxFuncs_TxAttenUpdGpioEn_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv903x_TxFuncs_TxAttenUpdGpioEn_BfSet failed.");
                goto cleanup;
            }
            break;
        case ADI_ADRV903X_TXATTEN_UPD_TRG_SPI:
            /* Hardware supports level trigger for SPI but we don't expose that. Is always edge triggered. */
            recoveryAction = adrv903x_TxFuncs_TxAttenUpdTrgSel_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     TX_ATTEN_EDGE_TRIGGER);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv903x_TxFuncs_TxAttenUpdTrgSel_BfSet failed.");
                goto cleanup;
            }
            break;
        case ADI_ADRV903X_TXATTEN_UPD_TRG_GPIO:
            /* Do trigger update from GPIO */
            recoveryAction = adrv903x_TxFuncs_TxAttenUpdGpioEn_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv903x_TxFuncs_TxAttenUpdGpioEn_BfSet failed.");
                goto cleanup;
            }
            
            /*  Always set edge triggered. Hardware supports level trigger for GPIO but we don't expose that. */
            recoveryAction = adrv903x_TxFuncs_TxAttenUpdTrgSel_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     TX_ATTEN_EDGE_TRIGGER);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv903x_TxFuncs_TxAttenUpdTrgSel_BfSet failed.");
                goto cleanup;
            }
            
            /* Setting the GPIO that triggers the update is done by adrv903x_GpioSignalSet above. */
            
            break;
        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                cfg->trgCfg.updateTrg,
                "Invalid ADI_ADRV903X_TXATTEN_UPD_TRG value");
            goto cleanup;
            break;
        }
        
        switch (cfg->srcCfg.updateSrc)
        {
        case ADI_ADRV903X_TXATTEN_UPD_SRC_S0:
            /* Turn off use of per-slice tx_attenuation register as that would override S0 */
            recoveryAction = adrv903x_TxFuncs_UseSliceAttenValue_BfSet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv903x_TxFuncs_UseSliceAttenValue_BfSet failed.");
                goto cleanup;
            }
            
            /* Turn off use of GPIO to select between S0/S1 */
            recoveryAction = adrv903x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet(device,
                                                                             NULL,
                                                                             txFuncsChanBaseAddr,
                                                                             0U);
            break;            
        case ADI_ADRV903X_TXATTEN_UPD_SRC_S0_OR_S1:
            /* Turn off use of per-slice tx_attenuation register that would override S0 */            
            recoveryAction = adrv903x_TxFuncs_UseSliceAttenValue_BfSet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       0U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv903x_TxFuncs_UseSliceAttenValue_BfSet failed.");
                goto cleanup;
            }
            
            /* Turn on use of GPIO to select between S0/S1 */
            recoveryAction = adrv903x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet(device,
                                                                             NULL,
                                                                             txFuncsChanBaseAddr,
                                                                             1U);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv903x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet failed.");
                goto cleanup;
            }
            
            /* Configuring which GPIO selects S0/S1 is done by adrv903x_GpioSignalSet above. */            
            break;            
        default:
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                cfg->srcCfg.updateSrc,
                "Invalid ADI_ADRV903X_TXATTEN_UPD_SRC value");
            goto cleanup;
            break;
        }
    }
    
cleanup:
        ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenUpdateCfgGet(adi_adrv903x_Device_t* const device,
                                                                  const adi_adrv903x_TxChannels_e txChannel,
                                                                  adi_adrv903x_TxAttenUpdateCfg_t* const cfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv903x_BfTxDigChanAddr_e)0U;
    adrv903x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = (adrv903x_BfTxFuncsChanAddr_e)0U;    
    uint8_t chanId = 0U;

    uint8_t updCoreSpiEn = 0U;       /* global config that overrides the per-channel updGpioEn */
    uint8_t updGpioEn = 0U;          /* enables gpio update trigger */
    uint8_t trgCfgTriggerGpio = 0U;  /* identifies gpio update trigger */
    uint8_t useS1ViaGpioPin = 0U;    /* enables S0/S1 selection via GPIO */
    uint8_t srcCfgS0OrS1Gpio = 0U;   /* identifies S0/S1 selection GPIO */
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);    

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, cfg, cleanup);
        
    chanId = adrv903x_TxChannelsToId(txChannel);
    if (chanId > ADI_ADRV903X_TX_CHAN_ID_MAX)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
            chanId,                        
            "Invalid channel id");
        goto cleanup;
    }
    
    /* Convert the chanId to the base address value required by the bitfield functions */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxDigBitfieldAddressGet(device, txChannel, &txDigChanBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }    
    
    recoveryAction =  adrv903x_Core_TxAttenUpdCoreSpiEn_BfGet(device,
                                                              NULL,
                                                              ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                              &updCoreSpiEn);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv903x_Core_TxAttenUpdCoreSpiEn_BfGet failed.");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_TxFuncs_TxAttenUpdGpioEn_BfGet(device,
                                                             NULL,
                                                             txFuncsChanBaseAddr,
                                                             &updGpioEn);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv903x_TxFuncs_TxAttenUpdGpioEn_BfGet failed.");
        goto cleanup;
    }
            
    recoveryAction = adrv903x_TxDig_TxAttenUpdGpioSelect_BfGet(device,
                                                               NULL,
                                                               txDigChanBaseAddr,
                                                               &trgCfgTriggerGpio);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv903x_TxDig_TxAttenUpdGpioSelect_BfGet failed.");
        goto cleanup;
    }
        
    /* Turn off use of GPIO to select between S0/S1 */
    recoveryAction = adrv903x_TxFuncs_UseAttenWordS1ViaGpioPin_BfGet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     &useS1ViaGpioPin);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv903x_TxFuncs_UseAttenWordS1ViaGpioPin_BfGet failed.");
        goto cleanup;
    }
        
    recoveryAction = adrv903x_TxDig_Spi2TxAttenGpioSelect_BfGet(device,
                                                                NULL,
                                                                txDigChanBaseAddr,
                                                                &srcCfgS0OrS1Gpio);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv903x_TxDig_Spi2TxAttenGpioSelect_BfGet failed.");
        goto cleanup;
    }
    
    /* Analyze the device settings and populate the cfg structure accordingly */    
    if (useS1ViaGpioPin == 0)
    {
        cfg->srcCfg.updateSrc = ADI_ADRV903X_TXATTEN_UPD_SRC_S0;            
    }
    else
    {
        cfg->srcCfg.updateSrc = ADI_ADRV903X_TXATTEN_UPD_SRC_S0_OR_S1;
        cfg->srcCfg.s0OrS1Gpio = (adi_adrv903x_GpioPinSel_e)srcCfgS0OrS1Gpio;
    }
    
    if (updCoreSpiEn > 0)
    {
        cfg->trgCfg.updateTrg = ADI_ADRV903X_TXATTEN_UPD_TRG_SPI;
    }
    else if (updGpioEn > 0)
    {
        cfg->trgCfg.updateTrg = ADI_ADRV903X_TXATTEN_UPD_TRG_GPIO;
        cfg->trgCfg.triggerGpio = (adi_adrv903x_GpioPinSel_e)trgCfgTriggerGpio ;
    }
    else
    {
        cfg->trgCfg.updateTrg = ADI_ADRV903X_TXATTEN_UPD_TRG_NONE;
    }                
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenUpdate(adi_adrv903x_Device_t *const device,
                                                            const uint32_t chanMask)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
        
    if ((chanMask > ADI_ADRV903X_TX_CHAN_MASK_MAX) ||
        (chanMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
                               ADI_ADRV903X_ERR_ACT_CHECK_PARAM,
                               chanMask,                        
                               "Invalid channel mask");
        goto cleanup;
    }

    recoveryAction = adrv903x_Core_TxAttenUpdCoreSpi_BfSet(device,
                                                           NULL,
                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                           chanMask);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to update TxAtten");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_Core_TxAttenUpdCoreSpi_BfSet(device,
                                                           NULL,
                                                           ADRV903X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                           0U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to reset update TxAtten bits");
        goto cleanup;
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxLoSourceGet(adi_adrv903x_Device_t * const   device,
                                                            const adi_adrv903x_TxChannels_e txChannel,
                                                            adi_adrv903x_LoSel_e * const    txLoSource)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    /* Check Tx data format config pointer is not NULL*/
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txLoSource, cleanup);
    
    /* Check that the requested txChannel is valid */
    if ((txChannel != ADI_ADRV903X_TX0) &&
        (txChannel != ADI_ADRV903X_TX1) &&
        (txChannel != ADI_ADRV903X_TX2) &&
        (txChannel != ADI_ADRV903X_TX3) &&
        (txChannel != ADI_ADRV903X_TX4) &&
        (txChannel != ADI_ADRV903X_TX5) &&
        (txChannel != ADI_ADRV903X_TX6) &&
        (txChannel != ADI_ADRV903X_TX7))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, 
                                 recoveryAction, 
                                 txChannel, 
                                 "Invalid Tx channel selected for LO source mapping read back. Valid Tx channels are Tx0-Tx7.");
        goto cleanup;
    }
    
    /* Check that Tx profile is valid in current config */
    if (((device->devStateInfo.profilesValid & ADI_ADRV903X_TX_PROFILE_VALID) != ADI_ADRV903X_TX_PROFILE_VALID)
        || ((device->devStateInfo.initializedChannels >> ADI_ADRV903X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 txChannel,
                                 "LO source read back requested for an Tx channel but Tx profile is invalid or channel not initialized in the device structure");
        goto cleanup;
    }
    
    /* Readback Tx LO source selection */
    recoveryAction = adrv903x_TxLoSourceGet(device, txChannel, txLoSource);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving Lo Source for Tx channel.");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxPowerMonitorCfgSet(adi_adrv903x_Device_t* const                    device,
                                                                   const adi_adrv903x_PowerMonitorCfgRt_t          txPowerMonitorCfg[],
                                                                   const uint32_t                                  numPowerProtectCfgs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    static const uint16_t MAX_PEAK_PRE_THRESHOLD = 511U;
    uint16_t peakPreThreShold = 0U;
    uint16_t peakThreshold = 0U;
    uint16_t avgThreshold = 0U;
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txPowerMonitorCfg, cleanup);
    
    if (numPowerProtectCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numPowerProtectCfgs, "Invalid Number of Tx Power Monitor Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numPowerProtectCfgs; ++cfgIdx)
    {
        recoveryAction = adrv903x_TxPowerMonitorCfgRangeCheck(device, &txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Power Monitor Range Check Error Reported");
            goto cleanup;
        }
        
        /*Check that if requested Tx Channel valid*/
        if (((txPowerMonitorCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (txPowerMonitorCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txPowerMonitorCfg[cfgIdx].txChannelMask,
                "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            goto cleanup;
        }
    }

    /* Write out the configurations to appropriate bitfields */
    for (cfgIdx = 0U; cfgIdx < numPowerProtectCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV903X_BF_EQUAL(txPowerMonitorCfg[cfgIdx].txChannelMask, chanSel))
            {
                recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                peakThreshold = txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakThreshold;
                avgThreshold = txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgThreshold;
                /* Peak Pre Threshold must be set to (peakThreshold/2) / 2^(16-9) to adjust bus width */
                peakPreThreShold = peakThreshold >> 8;
                if (peakPreThreShold > MAX_PEAK_PRE_THRESHOLD)
                {
                    /* Saturate pre threshold */
                    peakPreThreShold = MAX_PEAK_PRE_THRESHOLD;
                }

                recoveryAction = adrv903x_TxFuncs_PaProtectionOverloadThPre_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  peakPreThreShold);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Pre-Threshold");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_PaProtectionInputSel_BfSet(device,
                                                                             NULL,
                                                                             txBaseAddr,
                                                                             (uint8_t)txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.inputSel);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Input Select");
                    goto cleanup;
                }


                recoveryAction = adrv903x_TxFuncs_PaProtectionPeakThreshold_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  peakThreshold);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Threshold");
                    goto cleanup;
                }
                recoveryAction = adrv903x_TxFuncs_PaProtectionPeakDur_BfSet(device,
                                                                            NULL,
                                                                            txBaseAddr,
                                                                            txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.measDuration);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Duration");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionPeakCount_BfSet(device,
                                                                              NULL,
                                                                              txBaseAddr,
                                                                              txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakCount);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Count");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfSet(device,
                                                                                                NULL,
                                                                                                txBaseAddr,
                                                                                                txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakErrorClearRequired);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Error Clear Required");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerEn_BfSet(device,
                                                                                NULL,
                                                                                txBaseAddr,
                                                                                txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakPowerEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Power Enable");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerIrqEn_BfSet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakPowerIrqEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Power IRQ enable");
                    goto cleanup;
                }


                recoveryAction = adrv903x_TxFuncs_PaProtectionAverageThreshold_BfSet(device,
                                                                                     NULL,
                                                                                     txBaseAddr,
                                                                                     avgThreshold);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Threshold");
                    goto cleanup;
                }
                recoveryAction = adrv903x_TxFuncs_PaProtectionAverageDur_BfSet(device,
                                                                               NULL,
                                                                               txBaseAddr,
                                                                               txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.measDuration);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Duration");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfSet(device,
                                                                                               NULL,
                                                                                               txBaseAddr,
                                                                                               txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgErrorClearRequired);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Error Clear Required");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerEn_BfSet(device,
                                                                               NULL,
                                                                               txBaseAddr,
                                                                               txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgPowerEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Enable");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerIrqEn_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgPowerIrqEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average IRQ Enable");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_PaProtectionAprEn_BfSet(device,
                                                                          NULL,
                                                                          txBaseAddr,
                                                                          txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgPeakRatioEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Average to Peak Ratio Enable");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxPowerMonitorCfgGet(adi_adrv903x_Device_t * const device,
                                                                   adi_adrv903x_PowerMonitorCfgRt_t* const txPowerMonitorCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txPowerMonitorCfg, cleanup);

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)txPowerMonitorCfg->txChannelMask, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txPowerMonitorCfg->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionInputSel_BfGet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Input Select");
        goto cleanup;
    }
    txPowerMonitorCfg->txPowerMonitorCfg.inputSel = (adi_adrv903x_PaProtectionInputSel_e)tempByte;

    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakThreshold_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &txPowerMonitorCfg->txPowerMonitorCfg.peakThreshold);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Threshold");
        goto cleanup;
    }

    /* Peak and Average measurement durations are programmed same to HW */
    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakDur_BfGet(device,
                                                                NULL,
                                                                txBaseAddr,
                                                                &txPowerMonitorCfg->txPowerMonitorCfg.measDuration);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Duration");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakCount_BfGet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  &txPowerMonitorCfg->txPowerMonitorCfg.peakCount);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Count");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfGet(device,
                                                                                    NULL,
                                                                                    txBaseAddr,
                                                                                    &txPowerMonitorCfg->txPowerMonitorCfg.peakErrorClearRequired);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Error Clear Required");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerEn_BfGet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    &txPowerMonitorCfg->txPowerMonitorCfg.peakPowerEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Power Enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerIrqEn_BfGet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       &txPowerMonitorCfg->txPowerMonitorCfg.peakPowerIrqEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Power IRQ enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionAverageThreshold_BfGet(device,
                                                                         NULL,
                                                                         txBaseAddr,
                                                                         &txPowerMonitorCfg->txPowerMonitorCfg.avgThreshold);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average Threshold");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfGet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   &txPowerMonitorCfg->txPowerMonitorCfg.avgErrorClearRequired);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average Error Clear Required");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerEn_BfGet(device,
                                                                   NULL,
                                                                   txBaseAddr,
                                                                   &txPowerMonitorCfg->txPowerMonitorCfg.avgPowerEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average Enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerIrqEn_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &txPowerMonitorCfg->txPowerMonitorCfg.avgPowerIrqEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average IRQ Enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionAprEn_BfGet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              &txPowerMonitorCfg->txPowerMonitorCfg.avgPeakRatioEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average to Peak Ratio Enable");
        goto cleanup;
    }
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxProtectionErrorGet(adi_adrv903x_Device_t* const    device,
                                                                   const adi_adrv903x_TxChannels_e txChannel,
                                                                   uint32_t* const                 eventBits)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t bfValue = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, eventBits, cleanup);

    /* Clear the memory before setting individual bits for different events */
    *eventBits = 0U;

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerError_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &bfValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Average Power Error Bit");
        goto cleanup;
    }

    *eventBits |= (uint32_t) (bfValue & 0x01U);

    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerError_BfGet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       &bfValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Peak Power Error Bit");
        goto cleanup;
    }

    *eventBits |= (((uint32_t) (bfValue & 0x01U)) << 1U);

    recoveryAction = adrv903x_TxFuncs_SrdError_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &bfValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD Error Bit");
        goto cleanup;
    }

    *eventBits |= (((uint32_t) (bfValue & 0x01U)) << 2U);

    recoveryAction = adrv903x_TxFuncs_PllJesdProtEvent_BfGet(device,
                                                             NULL,
                                                             txBaseAddr,
                                                             &bfValue);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Pll Unlock/Dfrm Irq Error");
        goto cleanup;
    }

    *eventBits |= (((uint32_t) (bfValue & 0x01U)) << 3U);

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxProtectionErrorClear(adi_adrv903x_Device_t * const   device,
                                                                     const adi_adrv903x_TxChannels_e txChannel,
                                                                     const uint32_t                  eventBits)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    if (ADRV903X_BF_EQUAL(eventBits, 0x01U))
    {
        recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerErrorClear_BfSet(device,
                                                                               NULL,
                                                                               txBaseAddr,
                                                                               ADI_TRUE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set Tx Average Power Error Clear Bit");
            goto cleanup;
        }
    }

    if (ADRV903X_BF_EQUAL(eventBits, 0x02U))
    {
        recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerErrorClear_BfSet(device,
                                                                                NULL,
                                                                                txBaseAddr,
                                                                                ADI_TRUE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set Tx Peak Power Error Clear Bit");
            goto cleanup;
        }
    }

    if (ADRV903X_BF_EQUAL(eventBits, 0x04U))
    {
        recoveryAction = adrv903x_TxFuncs_SrdErrorClear_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              ADI_TRUE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set SRD Error Clear Bit");
            goto cleanup;
        }
    }

    if (ADRV903X_BF_EQUAL(eventBits, 0x08U))
    {
        recoveryAction = adrv903x_TxFuncs_PllJesdProtClr_BfSet(device,
                                                               NULL,
                                                               txBaseAddr,
                                                               ADI_TRUE);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set Pll Unlock/Dfrm Irq Error Clear Bit");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxSlewRateDetectorCfgSet(adi_adrv903x_Device_t* const                device,
                                                                       adi_adrv903x_SlewRateDetectorCfgRt_t        txSlewRateDetectorCfg[],
                                                                       const uint32_t                              numSlewRateDetCfgs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txSlewRateDetectorCfg, cleanup);

    if (numSlewRateDetCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numSlewRateDetCfgs, "Invalid Number of Tx SRD Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numSlewRateDetCfgs; ++cfgIdx)
    {
        recoveryAction = adrv903x_TxSlewRateDetectorCfgRangeCheck(device, &txSlewRateDetectorCfg[cfgIdx].srlCfg);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx SRD Range Check Error Reported");
            goto cleanup;
        }
        
        /*Check that if requested Tx Channel valid*/
        if (((txSlewRateDetectorCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (txSlewRateDetectorCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txSlewRateDetectorCfg[cfgIdx].txChannelMask,
                                   "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            goto cleanup;
        }
    }

    /* Write out the configurations to appropriate bitfields */
    for (cfgIdx = 0U; cfgIdx < numSlewRateDetCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV903X_BF_EQUAL(txSlewRateDetectorCfg[cfgIdx].txChannelMask, chanSel))
            {
                recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdDinSel_BfSet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  (uint8_t)txSlewRateDetectorCfg[cfgIdx].srlCfg.inputSel);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD input selection");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdSlewOffset_BfSet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      txSlewRateDetectorCfg[cfgIdx].srlCfg.srdOffset);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD threshold");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdEn_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              txSlewRateDetectorCfg[cfgIdx].srlCfg.srdEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD enable");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdIrqEn_BfSet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 txSlewRateDetectorCfg[cfgIdx].srlCfg.srdIrqEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD Irq enable");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdArvWait_BfSet(device,
                                                                   NULL,
                                                                   txBaseAddr,
                                                                   txSlewRateDetectorCfg[cfgIdx].srlCfg.autoRecoveryWaitTime);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write auto recovery wait time");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdArvEn_BfSet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 txSlewRateDetectorCfg[cfgIdx].srlCfg.autoRecoveryEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write auto recovery enable");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdArvTxonQual_BfSet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       txSlewRateDetectorCfg[cfgIdx].srlCfg.autoRecoveryDisableTimerWhenTxOff);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write auto recovery disable timer when Tx off");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdStatEn_BfSet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  txSlewRateDetectorCfg[cfgIdx].srlCfg.srdStatisticsEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write statistics enable");
                    goto cleanup;
                }

                /* Clear statistics before setting statistics mode */
                recoveryAction = adrv903x_TxFuncs_SrdStatMode_BfSet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    (2U | txSlewRateDetectorCfg[cfgIdx].srlCfg.srdStatisticsMode));
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to clear statistics");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_SrdStatMode_BfSet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    txSlewRateDetectorCfg[cfgIdx].srlCfg.srdStatisticsMode);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write statistics mode");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxSlewRateDetectorCfgGet(adi_adrv903x_Device_t * const               device,
                                                                       adi_adrv903x_SlewRateDetectorCfgRt_t * const txSlewRateDetectorCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txSlewRateDetectorCfg, cleanup);

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)txSlewRateDetectorCfg->txChannelMask, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txSlewRateDetectorCfg->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdDinSel_BfGet(device,
                                                      NULL,
                                                      txBaseAddr,
                                                      &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD input selection");
        goto cleanup;
    }
    txSlewRateDetectorCfg->srlCfg.inputSel = (adi_adrv903x_PaProtectionInputSel_e)tempByte;

    recoveryAction = adrv903x_TxFuncs_SrdSlewOffset_BfGet(device,
                                                          NULL,
                                                          txBaseAddr,
                                                          &txSlewRateDetectorCfg->srlCfg.srdOffset);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD threshold");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdEn_BfGet(device,
                                                  NULL,
                                                  txBaseAddr,
                                                  &txSlewRateDetectorCfg->srlCfg.srdEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdIrqEn_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &txSlewRateDetectorCfg->srlCfg.srdIrqEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD Irq enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdArvWait_BfGet(device,
                                                       NULL,
                                                       txBaseAddr,
                                                       &txSlewRateDetectorCfg->srlCfg.autoRecoveryWaitTime);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read auto recovery wait time");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdArvEn_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &txSlewRateDetectorCfg->srlCfg.autoRecoveryEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read auto recovery enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdArvTxonQual_BfGet(device,
                                                           NULL,
                                                           txBaseAddr,
                                                           &txSlewRateDetectorCfg->srlCfg.autoRecoveryDisableTimerWhenTxOff);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read auto recovery disable timer when Tx off");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdStatEn_BfGet(device,
                                                      NULL,
                                                      txBaseAddr,
                                                      &txSlewRateDetectorCfg->srlCfg.srdStatisticsEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics enable");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdStatMode_BfGet(device,
                                                        NULL,
                                                        txBaseAddr,
                                                        &txSlewRateDetectorCfg->srlCfg.srdStatisticsMode);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics mode");
        goto cleanup;
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxSlewRateStatisticsRead(adi_adrv903x_Device_t * const device,
                                                                       const adi_adrv903x_TxChannels_e txChannel,
                                                                       const uint8_t clearStats,
                                                                       uint16_t * const statisticsReadback)
{
        static const uint8_t SINGLE_BIT_MAX = 1U;
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, statisticsReadback, cleanup);

    if (clearStats > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clearStats,
                               "clearStats value is invalid. Valid values 0-1");
        goto cleanup;
    }

    /* Clear the memory to make sure statistics readback will be correct */
    *statisticsReadback = 0;

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_SrdStat_BfGet(device,
                                                    NULL,
                                                    txBaseAddr,
                                                    statisticsReadback);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics");
        goto cleanup;
    }

    if (clearStats == 1U)
    {
        recoveryAction = adrv903x_TxFuncs_SrdStatMode_BfGet(device,
                                                            NULL,
                                                            txBaseAddr,
                                                            &tempByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics mode");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_TxFuncs_SrdStatMode_BfSet(device,
                                                            NULL,
                                                            txBaseAddr,
                                                            (tempByte | 2U));
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to clear statistics");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_TxFuncs_SrdStatMode_BfSet(device,
                                                            NULL,
                                                            txBaseAddr,
                                                            tempByte);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to restore statistics mode");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxProtectionRampSampleHoldEnableSet(adi_adrv903x_Device_t* const device,
                                                                                  const uint32_t txChannelMask,
                                                                                  const uint8_t enable)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (enable > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enable,
                               "enable value is invalid. Valid values 0-1");
        goto cleanup;
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        if (ADRV903X_BF_EQUAL(txChannelMask, (uint32_t)(1U << chanIdx)))
        {
            chanSel = 1U << chanIdx;
            recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                goto cleanup;
            }


            recoveryAction = adrv903x_TxFuncs_AnaRampHoldSampleEn_BfSet(device,
                                                                        NULL,
                                                                        txBaseAddr,
                                                                        enable);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Analog sample hold and ramp down bit");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxProtectionRampSampleHoldEnableGet(adi_adrv903x_Device_t* const device,
                                                                                  const adi_adrv903x_TxChannels_e txChannel,
                                                                                  uint8_t* const enable)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, enable, cleanup);

    /* Retrieve the base address for selected tx channel. This will also range check the txChannelMask parameter */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_TxFuncs_AnaRampHoldSampleEn_BfGet(device,
                                                                NULL,
                                                                txBaseAddr,
                                                                enable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Analog sample hold and ramp down bit");
        goto cleanup;
    }


cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxProtectionRampCfgSet(adi_adrv903x_Device_t* const device,
                                                                     adi_adrv903x_ProtectionRampCfgRt_t txProtectionRampCfg[],
                                                                     const uint32_t numProtectionRampCfgs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t jesdDfrmMask = 0U;
    uint8_t pllUnlockMask = 0U;
    uint8_t srdRdnEn = 0U;
    uint8_t avgPowerRdnEn = 0U;
    uint8_t peakPowerRdnEn = 0U;

    uint32_t allRampDownMasks = (uint32_t)ADI_ADRV903X_RDT_PEAK_PA |
                                (uint32_t)ADI_ADRV903X_RDT_AVG_PA |
                                (uint32_t)ADI_ADRV903X_RDT_SRD |
                                (uint32_t)ADI_ADRV903X_RDT_CLK_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV903X_RDT_RF0_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV903X_RDT_RF1_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV903X_RDT_SERDES_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV903X_RDT_DFRM0_EVENT |
                                (uint32_t)ADI_ADRV903X_RDT_DFRM1_EVENT;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txProtectionRampCfg, cleanup);

    if (numProtectionRampCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numProtectionRampCfgs, "Invalid Number of Tx Protection Ramp Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numProtectionRampCfgs; ++cfgIdx)
    {
        if ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask & (~allRampDownMasks)) != 0U)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask,
                "Ramp down mask selection is invalid. Valid values are any combinations of bitmasks given in adi_adrv903x_RampDownTrigger_e");
            goto cleanup;
        }

        if (txProtectionRampCfg[cfgIdx].protectionRampCfg.altEventClearReqd > 1U)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                txProtectionRampCfg[cfgIdx].protectionRampCfg.altEventClearReqd,
                "Invalid altEventClearReqd selection. Valid values are 0-1");
            goto cleanup;
        }

        /*Check that if requested Tx Channel valid*/
        if(((txProtectionRampCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (txProtectionRampCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                txProtectionRampCfg[cfgIdx].txChannelMask,
                "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            goto cleanup;
        }
    }

    /* Write out the configurations to appropriate bitfields */
    for (cfgIdx = 0U; cfgIdx < numProtectionRampCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV903X_BF_EQUAL(txProtectionRampCfg[cfgIdx].txChannelMask, (uint32_t)(chanSel)))
            {
                recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                jesdDfrmMask = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x7U) & 0x3U);
                /* Setting these bits will disable the rampdown for dfrm events, so toggle user selection */
                jesdDfrmMask = ~jesdDfrmMask;
                jesdDfrmMask &= 0x3U;
                recoveryAction = adrv903x_TxFuncs_JesdDfrmMask_BfSet(device,
                                                                     NULL,
                                                                     txBaseAddr,
                                                                     jesdDfrmMask);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Jesd Dfrm Mask");
                    goto cleanup;
                }

                pllUnlockMask = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x3U) & 0xFU);
                /* Setting these bits will disable the rampdown for pll unlock events, so toggle user selection */
                pllUnlockMask = ~pllUnlockMask;
                pllUnlockMask &= 0xFU;
                recoveryAction = adrv903x_TxFuncs_PllUnlockMask_BfSet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      pllUnlockMask);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PLL Unlock Mask");
                    goto cleanup;
                }

                srdRdnEn = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x2U) & 0x1U);
                recoveryAction = adrv903x_TxFuncs_SrdRdnEn_BfSet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 srdRdnEn);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD Ramp Down Mask");
                    goto cleanup;
                }

                avgPowerRdnEn = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x1U) & 0x1U);
                recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerRdnEn_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  avgPowerRdnEn);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Average Power Ramp Down Mask");
                    goto cleanup;
                }

                peakPowerRdnEn = (txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask & 0x1U);
                recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerRdnEn_BfSet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   peakPowerRdnEn);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Peak Power Ramp Down Mask");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_PllJesdProtClrReqd_BfSet(device,
                                                                           NULL,
                                                                           txBaseAddr,
                                                                           txProtectionRampCfg[cfgIdx].protectionRampCfg.altEventClearReqd);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Power Protect Event Clear Required");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxProtectionRampCfgGet(adi_adrv903x_Device_t* const                device,
                                                                     adi_adrv903x_ProtectionRampCfgRt_t* const   txProtectionRampCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;
    uint32_t tmpRampDownMask = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txProtectionRampCfg, cleanup);

    /* Retrieve the base address for selected tx channel. This will also range check the txChannelMask parameter */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(txProtectionRampCfg->txChannelMask), &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txProtectionRampCfg->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_JesdDfrmMask_BfGet(device,
                                                         NULL,
                                                         txBaseAddr,
                                                         &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Jesd Dfrm Mask");
        goto cleanup;
    }
    /* bit = 1 means ramp down is disabled. So toggle the bits before returning it to user */
    tempByte = ~tempByte;
    tempByte &= 0x3U;
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0007U;

    recoveryAction = adrv903x_TxFuncs_PllUnlockMask_BfGet(device,
        NULL,
        txBaseAddr,
        &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PLL Unlock Mask");
        goto cleanup;
    }
    /* bit = 1 means ramp down is disabled. So toggle the bits before returning it to user */
    tempByte = ~tempByte;
    tempByte &= 0xFU;
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0003U;

    recoveryAction = adrv903x_TxFuncs_SrdRdnEn_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD Ramp Down Mask");
        goto cleanup;
    }
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0002U;

    recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerRdnEn_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Average Power Ramp Down Mask");
        goto cleanup;
    }
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0001U;
    
    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerRdnEn_BfGet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Peak Power Ramp Down Mask");
        goto cleanup;
    }
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0000U;

    recoveryAction = adrv903x_TxFuncs_PllJesdProtClrReqd_BfGet(device,
                                                               NULL,
                                                               txBaseAddr,
                                                               &txProtectionRampCfg->protectionRampCfg.altEventClearReqd);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Power Protect Event Clear Required");
        goto cleanup;
    }

    txProtectionRampCfg->protectionRampCfg.rampDownMask = tmpRampDownMask;

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxPowerMonitorStatusGet(adi_adrv903x_Device_t * const device,
                                                                      const adi_adrv903x_TxChannels_e txChannel,
                                                                      adi_adrv903x_TxPowerMonitorStatus_t * const powerMonitorStatus)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, powerMonitorStatus, cleanup);

    /* Retrieve the base address for selected tx channel. This will also range check the txChannel parameter */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_TxFuncs_PaProtectionReadbackUpdate_BfSet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set readback update bitfield");
        goto cleanup;
    }
    
    recoveryAction = adrv903x_TxFuncs_PaProtectionPeakpowerEn_BfGet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Power Enable");
        goto cleanup;
    }
    
    if (tempByte == 1U)
    {
        /* Read power levels only if peak power measurement block is enabled */
        recoveryAction = adrv903x_TxFuncs_PaProtectionPeakPower_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &powerMonitorStatus->peakPower);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read average power");
            goto cleanup;
        }
        
        recoveryAction = adrv903x_TxFuncs_PaProtectionPeakErrorPower_BfGet(device,
                                                                           NULL,
                                                                           txBaseAddr,
                                                                           &powerMonitorStatus->peakErrorPower);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read peak error power");
            goto cleanup;
        }
    }
    else
    {
        powerMonitorStatus->peakPower = 0U;
        powerMonitorStatus->peakErrorPower = 0U;
    }
    
    recoveryAction = adrv903x_TxFuncs_PaProtectionAvgpowerEn_BfGet(device,
                                                                   NULL,
                                                                   txBaseAddr,
                                                                   &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Avg Power Enable");
        goto cleanup;
    }

    if (tempByte == 1U)
    {
        recoveryAction = adrv903x_TxFuncs_PaProtectionAveragePower_BfGet(device,
                                                                         NULL,
                                                                         txBaseAddr,
                                                                         &powerMonitorStatus->avgPower);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read average power");
            goto cleanup;
        }

        recoveryAction = adrv903x_TxFuncs_PaProtectionAverageErrorPower_BfGet(device,
                                                                              NULL,
                                                                              txBaseAddr,
                                                                              &powerMonitorStatus->avgErrorPower);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read average error power");
            goto cleanup;
        }
    }
    else
    {
        powerMonitorStatus->avgPower = 0U;
        powerMonitorStatus->avgErrorPower = 0U;
    }

    recoveryAction = adrv903x_TxFuncs_PaProtectionAprEn_BfGet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              &tempByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average to Peak Ratio Enable");
        goto cleanup;
    }

    if (tempByte == 1U)
    {
        recoveryAction = adrv903x_TxFuncs_PaProtectionAveragePeakRatio_BfGet(device,
                                                                             NULL,
                                                                             txBaseAddr,
                                                                             &powerMonitorStatus->avgPeakRatio);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read average to peak ratio");
            goto cleanup;
        }
    }
    else
    {
        powerMonitorStatus->avgPeakRatio = 0U;
    }


cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenCfgSet(adi_adrv903x_Device_t* const device,
                                                            const uint32_t chanMask,
                                                            adi_adrv903x_TxAttenCfg_t* const txAttenCfg)
{
        const uint8_t TX_ATTEN_MODE_SPI = 1U;
    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t chanId = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxFuncsChanAddr_e txFuncsChanAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txAttenCfg, cleanup);
    
    if (chanMask > ADI_ADRV903X_TX_CHAN_MASK_MAX ||
        chanMask == 0U)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanMask, "Invalid channel mask.");
        goto cleanup;
    }
    
    if (txAttenCfg->txAttenStepSize >= ADI_ADRV903X_TXATTEN_INVALID)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
         ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txAttenCfg->txAttenStepSize, "Tx atten step size is invalid.");
        goto cleanup;        
    }
    
    /* adi_adrv903x_TxAttenUpdateCfgSet already validates everything within txAttenCfg->updateCfg */
    recoveryAction = adi_adrv903x_TxAttenUpdateCfgSet(device, chanMask, &txAttenCfg->updateCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx atten config failed to set update config");
        goto cleanup;
    }                                                              
       
    for (chanId = 0U; chanId < ADI_ADRV903X_MAX_TXCHANNELS; chanId++)
    {
        chanSel = 1U << chanId;
        if ((chanMask & chanSel) == 0)
        {
            /* chanId not set in chanMask - skip this channel */
            continue;
        }

        /* Convert the chanId to the base address value required by the bitfield functions */
        recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txFuncsChanAddr);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }
        
        /* Ensure mode is SPI mode */
        recoveryAction = adrv903x_TxFuncs_TxAttenMode_BfSet(device, NULL, txFuncsChanAddr, TX_ATTEN_MODE_SPI);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Atten Mode");
            goto cleanup;
        }
                                
        recoveryAction = adrv903x_TxFuncs_TxAttenConfig_BfSet(device, NULL, txFuncsChanAddr, txAttenCfg->txAttenStepSize);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Use Atten Step Size");
            goto cleanup;
        }
        device->devStateInfo.txAttenStepSize[chanId] = txAttenCfg->txAttenStepSize;        
    }
    
cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenCfgGet(adi_adrv903x_Device_t* const device,
                                                            const adi_adrv903x_TxChannels_e txChannel,
                                                            adi_adrv903x_TxAttenCfg_t* const txAttenCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txFuncsChanAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t chanId  = 0U;
    uint8_t tmpByte = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txAttenCfg, cleanup);
    
    
    chanId = adrv903x_TxChannelsToId(txChannel);
    if (chanId > ADI_ADRV903X_TX_CHAN_ID_MAX)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;   
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid channel id.");
        goto cleanup;        
    }
    
    recoveryAction = adi_adrv903x_TxAttenUpdateCfgGet(device, txChannel, &txAttenCfg->updateCfg);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get tx atten update config");
        goto cleanup;
    }                                                              
    
    /* Convert the chanId to the base address value required by the bitfield functions */
    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }
                        
    /* Fetch the const step mode config items */
    recoveryAction = adrv903x_TxFuncs_TxAttenConfig_BfGet(device, NULL, txFuncsChanAddr, &tmpByte);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Atten Step Size");
        goto cleanup;
    }

    txAttenCfg->txAttenStepSize = (adi_adrv903x_TxAttenStepSize_e)tmpByte;

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxDecimatedPowerCfgSet(adi_adrv903x_Device_t * const device,
                                                                     adi_adrv903x_TxDecimatedPowerCfg_t txDecPowerCfg[],
                                                                     const uint32_t numOfDecPowerCfgs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;     
    uint32_t chanSel = 0U;

    adrv903x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    adrv903x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv903x_BfTxDigChanAddr_e)0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txDecPowerCfg, cleanup);

    if (numOfDecPowerCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfDecPowerCfgs, "Invalid Number of Dec Power Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        recoveryAction = adrv903x_TxDecPowerCfgRangeCheck(device, &txDecPowerCfg[cfgIdx]);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Dec power range check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; chanIdx++)
        {
            chanSel = 1U << chanIdx;
            if ((txDecPowerCfg[cfgIdx].txChannelMask & chanSel) > 0)
            {
                /* Convert the chanId to the base address value required by the bitfield functions */
                recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txFuncsChanBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxFuncs_TxPowerInputSelect_BfSet(device,
                                                                           NULL,
                                                                           txFuncsChanBaseAddr,
                                                                           txDecPowerCfg[cfgIdx].powerInputSelect);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement input selection");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_TxPowerMeasurementDuration_BfSet(device,
                                                                                   NULL,
                                                                                   txFuncsChanBaseAddr,
                                                                                   txDecPowerCfg[cfgIdx].powerMeasurementDuration);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement duration");
                    goto cleanup;
                }
                
                recoveryAction = adrv903x_TxFuncs_TxPeakToPowerMode_BfSet(device,
                                                                          NULL,
                                                                          txFuncsChanBaseAddr,
                                                                          txDecPowerCfg[cfgIdx].peakToPowerMode);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting peak to power mode");
                    goto cleanup;
                }

                recoveryAction = adrv903x_TxDigBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txDigChanBaseAddr);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                if (txDecPowerCfg[cfgIdx].measurementEnable == 1U)
                {

                    recoveryAction = adrv903x_TxDig_StreamprocDuc0Hb3OutClkEnable_BfSet(device,
                                                                                        NULL,
                                                                                        txDigChanBaseAddr,
                                                                                        txDecPowerCfg[cfgIdx].measurementEnable);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting tx stream proc duc0 hb3 out clk enable bit");
                        goto cleanup;
                    }
                    recoveryAction = adrv903x_TxDig_StreamprocPfirClkEnable_BfSet(device,
                                                                                  NULL,
                                                                                  txDigChanBaseAddr,
                                                                                  txDecPowerCfg[cfgIdx].measurementEnable);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting tx stream proc pfir clk enable bit");
                        goto cleanup;
                    }
                }

                recoveryAction = adrv903x_TxFuncs_TxPowerMeasurementEnable_BfSet(device,
                                                                                 NULL,
                                                                                 txFuncsChanBaseAddr,
                                                                                 txDecPowerCfg[cfgIdx].measurementEnable);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement enable bit");
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxDecimatedPowerCfgGet(adi_adrv903x_Device_t * const device,
                                                                     const adi_adrv903x_TxChannels_e txChannel,
                                                                     adi_adrv903x_TxDecimatedPowerCfg_t * const txDecPowerCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;      
    adrv903x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txDecPowerCfg, cleanup);
   
    if ((txChannel != ADI_ADRV903X_TX0) &&
        (txChannel != ADI_ADRV903X_TX1) &&
        (txChannel != ADI_ADRV903X_TX2) &&
        (txChannel != ADI_ADRV903X_TX3) &&
        (txChannel != ADI_ADRV903X_TX4) &&
        (txChannel != ADI_ADRV903X_TX5) &&
        (txChannel != ADI_ADRV903X_TX6) &&
        (txChannel != ADI_ADRV903X_TX7))
    {
        /* Invalid tx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx channel selection");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_TxPowerInputSelect_BfGet(device,
                                                               NULL,
                                                               txFuncsChanBaseAddr,
                                                               &txDecPowerCfg->powerInputSelect);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement input selection");
        goto cleanup;
    }
                
    recoveryAction = adrv903x_TxFuncs_TxPowerMeasurementDuration_BfGet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       &txDecPowerCfg->powerMeasurementDuration);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement duration");
        goto cleanup;
    }
     
    recoveryAction = adrv903x_TxFuncs_TxPeakToPowerMode_BfGet(device,
                                                              NULL,
                                                              txFuncsChanBaseAddr,
                                                              &txDecPowerCfg->peakToPowerMode);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peak to power mode bit");
        goto cleanup;
    }
       
    recoveryAction = adrv903x_TxFuncs_TxPowerMeasurementEnable_BfGet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     &txDecPowerCfg->measurementEnable);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement enable bit");
        goto cleanup;
    }

    txDecPowerCfg->txChannelMask = (uint32_t)txChannel;
    
cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxDecimatedPowerGet(adi_adrv903x_Device_t * const device,
                                                                  const adi_adrv903x_TxChannels_e txChannel,
                                                                  uint8_t * const powerReadBack,
                                                                  uint8_t * const powerPeakReadBack)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_FUNCS;
    
    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, powerReadBack, cleanup);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, powerPeakReadBack, cleanup);

    if ((txChannel != ADI_ADRV903X_TX0) &&
        (txChannel != ADI_ADRV903X_TX1) &&
        (txChannel != ADI_ADRV903X_TX2) &&
        (txChannel != ADI_ADRV903X_TX3) &&
        (txChannel != ADI_ADRV903X_TX4) &&
        (txChannel != ADI_ADRV903X_TX5) &&
        (txChannel != ADI_ADRV903X_TX6) &&
        (txChannel != ADI_ADRV903X_TX7))
    {
        /* Invalid Tx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid channel selection");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxFuncs_TxPowerMeasurementReadback_BfSet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       1U);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting decimated power readback latch bit");
        goto cleanup;
    }
  
    recoveryAction = adrv903x_TxFuncs_TxPower_BfGet(device,
                                                    NULL,
                                                    txFuncsChanBaseAddr,
                                                    powerReadBack);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading decimated power for selected Tx channel");
        goto cleanup;
    }


    recoveryAction = adrv903x_TxFuncs_TxPowerLargestPeak_BfGet(device,
                                                               NULL,
                                                               txFuncsChanBaseAddr,
                                                               powerPeakReadBack);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading decimated peak power for selected Tx channel");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenPhaseSet(adi_adrv903x_Device_t* const device,
                                                              const adi_adrv903x_TxAttenPhaseCfg_t * const txAttenPhaseCfg)
{
        adi_adrv903x_ErrAction_e                recoveryAction     = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_SetTxAttenPhase_t       txAttenPhaseInfo;
    adrv903x_CpuCmd_SetTxAttenPhaseResp_t   cmdRsp;
    adrv903x_CpuCmdStatus_e                 cmdStatus           = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e                 cpuErrorCode        = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                i                   = 0U;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txAttenPhaseCfg, cleanup);
    ADI_LIBRARY_MEMSET(&txAttenPhaseInfo, 0, sizeof(txAttenPhaseInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (txAttenPhaseCfg->chanSelect == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txAttenPhaseCfg->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txAttenPhaseInfo.chanSelect = (uint8_t) txAttenPhaseCfg->chanSelect;
    for (i = 0U; i < ADI_ADRV903X_TX_ATTEN_PHASE_SIZE; i++)
    {
        txAttenPhaseInfo.txAttenPhase[i] = ADRV903X_HTOCS(txAttenPhaseCfg->txAttenPhase[i]);
    }

    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_SET_TX_ATTEN_PHASE,
                                            (void*)&txAttenPhaseInfo,
                                            sizeof(txAttenPhaseInfo),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_TxAttenPhaseGet(adi_adrv903x_Device_t* const device,
                                                              adi_adrv903x_TxAttenPhaseCfg_t* const txRbConfig)
{
        adi_adrv903x_ErrAction_e                recoveryAction              = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_CpuCmd_GetTxAttenPhase_t       txAttenPhaseCfgRbCmd;
    adrv903x_CpuCmd_GetTxAttenPhaseResp_t   txAttenPhaseCfgGetcmdRsp;
    adrv903x_CpuCmdStatus_e                 cmdStatus                   = ADRV903X_CPU_CMD_STATUS_GENERIC;
    adrv903x_CpuErrorCode_e                 cpuErrorCode                = ADRV903X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                i                           = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, txRbConfig, cleanup);
    ADI_LIBRARY_MEMSET(&txAttenPhaseCfgRbCmd, 0, sizeof(txAttenPhaseCfgRbCmd));
    ADI_LIBRARY_MEMSET(&txAttenPhaseCfgGetcmdRsp, 0, sizeof(txAttenPhaseCfgGetcmdRsp));

    /* Test if channel mask is 0 or has more than one channel set */
    if (txRbConfig->chanSelect == (uint8_t) ADI_ADRV903X_TXOFF ||
        txRbConfig->chanSelect > (uint8_t) ADI_ADRV903X_TX7    ||
        (((txRbConfig->chanSelect - 1) & txRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txAttenPhaseCfgRbCmd.chanSelect = (uint8_t) txRbConfig->chanSelect;

    /* Send command and receive response */
    recoveryAction = adrv903x_CpuCmdSend(   device,
                                            ADI_ADRV903X_CPU_TYPE_0,
                                            ADRV903X_LINK_ID_0,
                                            ADRV903X_CPU_CMD_ID_GET_TX_ATTEN_PHASE,
                                            (void*)&txAttenPhaseCfgRbCmd,
                                            sizeof(txAttenPhaseCfgRbCmd),
                                            (void*)&txAttenPhaseCfgGetcmdRsp,
                                            sizeof(txAttenPhaseCfgGetcmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_ADRV903X_CPU_CMD_RESP_CHECK_GOTO(txAttenPhaseCfgGetcmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    for (i = 0U; i < ADI_ADRV903X_TX_ATTEN_PHASE_SIZE; i++)
    {
        txRbConfig->txAttenPhase[i] = ADRV903X_CTOHS(txAttenPhaseCfgGetcmdRsp.txAttenPhase[i]);
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DtxCfgSet(adi_adrv903x_Device_t* const device,
                                                        const adi_adrv903x_DtxCfg_t  dtxCfg[],
                                                        const uint32_t               numDtxCfgs)
{
        static const uint32_t txScratchPad6[] = { (uint32_t)ADRV903X_ADDR_TX0_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX1_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX2_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX3_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX4_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX5_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX6_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX7_STREAM_SCRATCH6
                                            };
    static const uint32_t TX_SCRATCH_MASK      = 0xFFFF0103U;
    static const uint32_t DTX_MODE_MASK        = 0x00000003U;
    static const uint32_t DTX_MODE_SHIFT       = 0U;
    static const uint32_t DTX_ZERO_COUNT_MASK  = 0xFFFF0000U;
    static const uint32_t DTX_ZERO_COUNT_SHIFT = 16U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx  = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint32_t txScratch6Value = 0U;
    adi_adrv903x_DtxMode_e prvDtxMode[] = { ADI_ADRV903X_DTXMODE_DISABLE,
                                            ADI_ADRV903X_DTXMODE_DISABLE,
                                            ADI_ADRV903X_DTXMODE_DISABLE,
                                            ADI_ADRV903X_DTXMODE_DISABLE,
                                            ADI_ADRV903X_DTXMODE_DISABLE,
                                            ADI_ADRV903X_DTXMODE_DISABLE,
                                            ADI_ADRV903X_DTXMODE_DISABLE,
                                            ADI_ADRV903X_DTXMODE_DISABLE
                                          };

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dtxCfg, cleanup);

    if (numDtxCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numDtxCfgs, "Invalid Number of Tx DTX Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numDtxCfgs; ++cfgIdx)
    {
        /*Check that if requested Tx Channel valid*/
        if (((dtxCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (dtxCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dtxCfg[cfgIdx].txChannelMask,
                                   "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            goto cleanup;
        }
        
        /*Check that if requested DTX Mode is valid*/
        if (dtxCfg[cfgIdx].dtxMode > ADI_ADRV903X_DTXMODE_PIN)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dtxCfg[cfgIdx].dtxMode,
                                   "Invalid DTX mode selection. Valid values are 0/1/2/3");
            goto cleanup;
        }
    }

    /* Write out the configurations to appropriate bitfields */
    for (cfgIdx = 0U; cfgIdx < numDtxCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV903X_BF_EQUAL(dtxCfg[cfgIdx].txChannelMask, chanSel))
            {
                /* Update TX scratch register with DTX config for specific channel */
                recoveryAction = adi_adrv903x_Register32Read(device, NULL, txScratchPad6[chanIdx], &txScratch6Value, TX_SCRATCH_MASK);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 6");
                    goto cleanup;
                }

                prvDtxMode[chanIdx] = (adi_adrv903x_DtxMode_e)((txScratch6Value & DTX_MODE_MASK) >> DTX_MODE_SHIFT);

                txScratch6Value &= (~DTX_MODE_MASK);
                txScratch6Value |= ((uint32_t)(dtxCfg[cfgIdx].dtxMode) << DTX_MODE_SHIFT);
                txScratch6Value &= (~DTX_ZERO_COUNT_MASK);
                txScratch6Value |= ((uint32_t)(dtxCfg[cfgIdx].dtxZeroCounter) << DTX_ZERO_COUNT_SHIFT);

                recoveryAction = adi_adrv903x_Register32Write(device, NULL, txScratchPad6[chanIdx], txScratch6Value, TX_SCRATCH_MASK);
                if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 6");
                    goto cleanup;
                }
            }
        }

        /* For each cfgIdx, trigger stream to update DTX mode config registers while writing to DTX ClearStatus bits */
        recoveryAction = adrv903x_TxStreamTrigger(device, (uint8_t)dtxCfg[cfgIdx].txChannelMask, (uint8_t)ADRV903X_STREAM_TX_WRITE_DTX_MODE_CONFIG_CLRSTATUS);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dtxCfg[cfgIdx].txChannelMask,
                                   "Error while triggering TX DTX CFG stream");
            goto cleanup;
        }

        /* Check if DTX is requested to be disabled but this request was issued when DTX was activated */
        for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV903X_BF_EQUAL(dtxCfg[cfgIdx].txChannelMask, chanSel))
            {
                  if ((dtxCfg[cfgIdx].dtxMode == ADI_ADRV903X_DTXMODE_DISABLE) &&
                      (prvDtxMode[chanIdx] == ADI_ADRV903X_DTXMODE_AUTO))
                {
                    recoveryAction = adrv903x_TxStreamTrigger(device, (uint8_t)chanSel, (uint8_t)ADRV903X_STREAM_TX_DTX_POWER_UP);
                    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common,
                                               recoveryAction,
                                               chanSel,
                                               "Error while triggering TX DTX POWER UP stream");
                        goto cleanup;
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DtxCfgGet(adi_adrv903x_Device_t* const    device,
                                                        const adi_adrv903x_TxChannels_e txChannel,
                                                        adi_adrv903x_DtxCfg_t* const    dtxCfg)
{
        static const uint32_t txScratchPad6[] = { (uint32_t)ADRV903X_ADDR_TX0_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX1_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX2_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX3_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX4_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX5_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX6_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX7_STREAM_SCRATCH6 
                                            };
    static const uint32_t DTX_MODE_MASK        = 0x00000003U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv903x_BfTxDigChanAddr_e)0U;
    uint32_t txScratch6Value = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dtxCfg, cleanup);

    if ((txChannel != ADI_ADRV903X_TX0) &&
        (txChannel != ADI_ADRV903X_TX1) &&
        (txChannel != ADI_ADRV903X_TX2) &&
        (txChannel != ADI_ADRV903X_TX3) &&
        (txChannel != ADI_ADRV903X_TX4) &&
        (txChannel != ADI_ADRV903X_TX5) &&
        (txChannel != ADI_ADRV903X_TX6) &&
        (txChannel != ADI_ADRV903X_TX7))
    {
        /* Invalid tx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx channel selection");
        goto cleanup;
    }

    for (uint32_t chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        if ((1U << chanIdx) == txChannel)
        {
            /* Read DTX configuration stored in scratch register to retrieve DTX Mode */
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, txScratchPad6[chanIdx], &txScratch6Value, DTX_MODE_MASK);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 6");
                goto cleanup;
            }

            dtxCfg->dtxMode = (adi_adrv903x_DtxMode_e)(txScratch6Value);
        }
    }

    /* Retrieve Zero count via BfGet */
    recoveryAction = adrv903x_TxDigBitfieldAddressGet(device, txChannel, &txDigChanBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxDig_DtxZeroCounter_BfGet(device,
                                                         NULL,
                                                         txDigChanBaseAddr,
                                                         &dtxCfg->dtxZeroCounter);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting dtx zero counter ");
        goto cleanup;
    }

    dtxCfg->txChannelMask = (uint32_t)txChannel;

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DtxGpioCfgSet(adi_adrv903x_Device_t* const     device,
                                                            const adi_adrv903x_DtxGpioCfg_t* dtxGpioCfg)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adi_adrv903x_GpioPinSel_e gpioReadback = ADI_ADRV903X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);

    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dtxGpioCfg, cleanup);

    /* Check all Dtx GPIO if valid */
    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {            
        /* Check if pin is valid */
        if (dtxGpioCfg->dtxGpioTxSel[chanIdx] > ADI_ADRV903X_GPIO_INVALID)
        {
            recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid DTX GPIO pin selected. To disable, please select ADI_ADRV903X_GPIO_INVALID");
            goto cleanup;
        }

        /* Skip ADI_ADRV903X_GPIO_INVALID */
        if (dtxGpioCfg->dtxGpioTxSel[chanIdx] == ADI_ADRV903X_GPIO_INVALID)
        {
            continue;
        }

    }

    /* First release all GPIOs used for tx internal signals */
    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        recoveryAction = adrv903x_GpioSignalFind(device,
                                                 &gpioReadback,
                                                 ADI_ADRV903X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                 (adi_adrv903x_Channels_e)(chanSel));
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gpio for selected signal");
            goto cleanup;
        }

        if (gpioReadback != ADI_ADRV903X_GPIO_INVALID)
        {
            recoveryAction = adrv903x_GpioSignalRelease(device, 
                                                        gpioReadback,
                                                        ADI_ADRV903X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                        (adi_adrv903x_Channels_e)(1U <<chanIdx));
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to release gpio");
                goto cleanup;
            }
        }
    }

    /* Then assign new GPIOs for Tx internal signals */
    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        if (dtxGpioCfg->dtxGpioTxSel[chanIdx] != ADI_ADRV903X_GPIO_INVALID)
        {
            recoveryAction = adrv903x_GpioSignalSet(device,
                                                    dtxGpioCfg->dtxGpioTxSel[chanIdx],
                                                    ADI_ADRV903X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                    (adi_adrv903x_Channels_e)(1U << chanIdx));

            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Failure to assign selected signal to GPIO for chanIdx");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DtxGpioCfgGet(adi_adrv903x_Device_t* const     device,
                                                            adi_adrv903x_DtxGpioCfg_t* const dtxGpioCfg) 
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adi_adrv903x_GpioPinSel_e gpioReadback = ADI_ADRV903X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dtxGpioCfg, cleanup);

    /* Then assign new GPIOs for Tx internal signals */
    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        recoveryAction = adrv903x_GpioSignalFind(device,
                                                 &gpioReadback,
                                                 ADI_ADRV903X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                 (adi_adrv903x_Channels_e)chanSel);
        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gpio for selected signal");
            goto cleanup;
        }

        dtxGpioCfg->dtxGpioTxSel[chanIdx] = gpioReadback;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DtxForceSet(adi_adrv903x_Device_t* const   device,
                                                          const uint32_t txChannelMask,
                                                          const uint8_t dtxForce)
{
        static const uint32_t txScratchPad6[] = { (uint32_t)ADRV903X_ADDR_TX0_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX1_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX2_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX3_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX4_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX5_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX6_STREAM_SCRATCH6,
                                              (uint32_t)ADRV903X_ADDR_TX7_STREAM_SCRATCH6 
                                            };
    static const uint32_t TX_SCRATCH_MASK = 0xFFFF0103U;
    static const uint32_t DTX_FORCE_MASK  = 0x00000100U;
    static const uint32_t DTX_FORCE_SHIFT = 8U;

    adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM; 
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint32_t txScratch6Value = 0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
        
    /*Check that if requested DTX Force is valid*/
    if (dtxForce > ADI_ENABLE)
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dtxForce,
                               "Invalid DTX Force bit. Valid values are 0/1");
        goto cleanup;
    }


    /* Write out the configurations to appropriate bitfields */    
    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV903X_BF_EQUAL(txChannelMask, chanSel))
        {
            /* Update TX scratch register with DTX config for specific channel */
            recoveryAction = adi_adrv903x_Register32Read(device, NULL, txScratchPad6[chanIdx], &txScratch6Value, TX_SCRATCH_MASK);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 6");
                goto cleanup;
            }

            txScratch6Value = (txScratch6Value & (~DTX_FORCE_MASK)) | ((uint32_t)(dtxForce) << DTX_FORCE_SHIFT);

            recoveryAction = adi_adrv903x_Register32Write(device, NULL, txScratchPad6[chanIdx], txScratch6Value, TX_SCRATCH_MASK);
            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 6");
                goto cleanup;
            }
        }
    }

    recoveryAction = adrv903x_TxStreamTrigger(device, (uint8_t)txChannelMask, (uint8_t)ADRV903X_STREAM_TX_WRITE_DTX_MODE_CONFIG);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while triggering TX stream");
        goto cleanup;
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e adi_adrv903x_DtxStatusGet(adi_adrv903x_Device_t* const device,
                                                           const adi_adrv903x_TxChannels_e txChannel,
                                                           uint8_t * const dtxStatus)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    adrv903x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv903x_BfTxDigChanAddr_e)0U;

    /* Check device pointer is not null */
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_API_ENTRY(&device->common);
    
    ADI_ADRV903X_NULL_PTR_REPORT_GOTO(&device->common, dtxStatus, cleanup);

    if ((txChannel != ADI_ADRV903X_TX0) &&
        (txChannel != ADI_ADRV903X_TX1) &&
        (txChannel != ADI_ADRV903X_TX2) &&
        (txChannel != ADI_ADRV903X_TX3) &&
        (txChannel != ADI_ADRV903X_TX4) &&
        (txChannel != ADI_ADRV903X_TX5) &&
        (txChannel != ADI_ADRV903X_TX6) &&
        (txChannel != ADI_ADRV903X_TX7))
    {
        /* Invalid tx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx channel selection");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxDigBitfieldAddressGet(device, txChannel, &txDigChanBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxDig_DtxStatus_BfGet(device,
                                                    NULL,
                                                    txDigChanBaseAddr,
                                                    dtxStatus);

    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting dtx mode ");
        goto cleanup;

    }

cleanup :
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv903x_ErrAction_e  adi_adrv903x_TxPfirCoeffsWrite(adi_adrv903x_Device_t* const device,
                                                                 const uint32_t txChannelMask,
                                                                 adi_adrv903x_TxPfirCoeff_t* const pfirCoeffs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t i;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv903x_BfTxDatapathChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_DATAPATH;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(pfirCoeffs);
    ADI_ADRV903X_API_ENTRY(&device->common);

    /* Check that requested Tx Channel mask is valid */
    if (((txChannelMask & (~(uint32_t)ADI_ADRV903X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV903X_TXOFF))
    {
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel mask");
        goto cleanup;
    }

    /* Loop through each selected TX channel and set PFIR coeffs. */
    for (chanIdx = 0U; chanIdx < ADI_ADRV903X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if ((txChannelMask & chanSel) == 0)
        {
            /* TX channel is not selected - skip this channel */
            continue;
        }

        recoveryAction = adrv903x_TxDatapathBitfieldAddressGet(device, (adi_adrv903x_TxChannels_e)(chanSel), &txBaseAddr);

        /* Two 16 bit PFIR coefficients are written each loop */
        for (i = 0U; i < ADRV903X_NUM_PFIR_COEF/2; i++)
        {
            recoveryAction = adrv903x_TxDatapath_PfirCoeffDataI_BfSet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      i,
                                                                      (pfirCoeffs->pfirCoefI[2 * i] & 0xFFFFu) + (pfirCoeffs->pfirCoefI[2 * i + 1] << 16));

            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to Tx PFIR i coefficients");
                goto cleanup;
            }

            recoveryAction = adrv903x_TxDatapath_PfirCoeffDataQ_BfSet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      i,
                                                                      (pfirCoeffs->pfirCoefQ[2 * i] & 0xFFFFu) + (pfirCoeffs->pfirCoefQ[2 * i + 1] << 16));

            if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to Tx PFIR q coefficients");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv903x_ErrAction_e  adi_adrv903x_TxPfirCoeffsRead(adi_adrv903x_Device_t* const device,
                                                                const adi_adrv903x_TxChannels_e txChannel,
                                                                adi_adrv903x_TxPfirCoeff_t* const pfirCoeffs)
{
        adi_adrv903x_ErrAction_e recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
    uint32_t i;
    adrv903x_BfTxDatapathChanAddr_e txBaseAddr = ADRV903X_BF_SLICE_TX_0__TX_DATAPATH;

    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV903X_NULL_DEVICE_PTR_RETURN(pfirCoeffs);
    ADI_ADRV903X_API_ENTRY(&device->common);

    if ((txChannel != ADI_ADRV903X_TX0) &&
        (txChannel != ADI_ADRV903X_TX1) &&
        (txChannel != ADI_ADRV903X_TX2) &&
        (txChannel != ADI_ADRV903X_TX3) &&
        (txChannel != ADI_ADRV903X_TX4) &&
        (txChannel != ADI_ADRV903X_TX5) &&
        (txChannel != ADI_ADRV903X_TX6) &&
        (txChannel != ADI_ADRV903X_TX7))
    {
        /* Invalid tx channel selection */
        recoveryAction = ADI_ADRV903X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx channel selection");
        goto cleanup;
    }

    recoveryAction = adrv903x_TxDatapathBitfieldAddressGet(device, txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Error reading base address for txChannel.");
        goto cleanup; 
    }

    /* Two 16 bit PFIR coefficients are read each loop */
    for (i = 0U; i < ADRV903X_NUM_PFIR_COEF / 2; i++)
    {
        uint32_t readVal;

        /* Read two 16bit coefficients in one 32bit value */
        recoveryAction = adrv903x_TxDatapath_PfirCoeffDataI_BfGet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  i,
                                                                  &readVal);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Tx PFIR i coefficients");
            goto cleanup;
        }

        pfirCoeffs->pfirCoefI[2 * i]     = (int16_t)(readVal & 0xFFFFu) ;
        pfirCoeffs->pfirCoefI[2 * i + 1] = (int16_t)((readVal >> 16) & 0xFFFFu);

        /* Read two 16bit coefficients in one 32bit value */
        recoveryAction = adrv903x_TxDatapath_PfirCoeffDataQ_BfGet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  i,
                                                                  &readVal);

        if (recoveryAction != ADI_ADRV903X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Tx PFIR q coefficients");
            goto cleanup;
        }

        pfirCoeffs->pfirCoefQ[2 * i]     = (int16_t)(readVal & 0xFFFFu);
        pfirCoeffs->pfirCoefQ[2 * i + 1] = (int16_t)((readVal >> 16) & 0xFFFFu);
    }

cleanup:
    ADI_ADRV903X_API_EXIT(&device->common, recoveryAction);
}

