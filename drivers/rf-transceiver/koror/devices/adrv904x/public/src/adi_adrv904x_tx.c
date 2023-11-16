/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_tx.c
* \brief Contains ADRV904X features related function implementation defined in
* adi_adrv904x_tx.h
*
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_adrv904x_tx.h"

#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/include/adrv904x_stream_proc_types.h"
#include "../../private/bf/adrv904x_bf_tx_funcs.h"
#include "../../private/bf/adrv904x_bf_tx_dig.h"

#include "adi_adrv904x_dfe_cpu.h"
#include "../../private/bf/adrv904x_bf_tx_band_duc.h"
#include "../../private/include/adrv904x_dfe_cfr.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_t.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_dfe_pwr_mtr_t.h"
#include "../../private/include/adrv904x_dfe_svc_cmd_dfe_radio_t.h"
#include "../../private/bf/adrv904x_bf_tx_dfe_dig_regs.h"
#include "../../private/bf/adrv904x_bf_cduc_hb_dpath.h"
#include "../../private/bf/adrv904x_bf_tx_dpd_act_mmr.h"
#include "../../private/include/adrv904x_carrier_reconfigure.h"

#define ADI_FILE ADI_ADRV904X_FILE_PUBLIC_TX


#define ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET 0x8000U   /*!< DUC1 Playback RAM Offset */
#define ADI_ADVRV904X_TX0_PLAYBACK_RAM_BASEADDR 0x60800000U /*!< Base Address of Playback RAM for TX0 */
#define ADI_ADVRV904X_TX1_PLAYBACK_RAM_BASEADDR 0x60900000U /*!< Base Address of Playback RAM for TX1 */
#define ADI_ADVRV904X_TX2_PLAYBACK_RAM_BASEADDR 0x60A00000U /*!< Base Address of Playback RAM for TX2 */
#define ADI_ADVRV904X_TX3_PLAYBACK_RAM_BASEADDR 0x60B00000U /*!< Base Address of Playback RAM for TX3 */
#define ADI_ADVRV904X_TX4_PLAYBACK_RAM_BASEADDR 0x60C00000U /*!< Base Address of Playback RAM for TX4 */
#define ADI_ADVRV904X_TX5_PLAYBACK_RAM_BASEADDR 0x60D00000U /*!< Base Address of Playback RAM for TX5 */
#define ADI_ADVRV904X_TX6_PLAYBACK_RAM_BASEADDR 0x60E00000U /*!< Base Address of Playback RAM for TX6 */
#define ADI_ADVRV904X_TX7_PLAYBACK_RAM_BASEADDR 0x60F00000U /*!< Base Address of Playback RAM for TX7 */

#define TX_ATTEN_EDGE_TRIGGER 0U
#define TX_ATTEN_LEVEL_TRIGGER 1U
#define TX_ATTEN_TABLE_INDEX_RANGE_CHECK 33U


#define MAX_CDUC_NCO_FREQ_KHZ (8388607) /* 0x7FFFFF */
#define MAX_CDUC_NCO_PHASE_DEGREES (360)

/* Some tyepdefs and static fn ptrs to make the S0S1 Bf access manageable */
typedef adi_adrv904x_ErrAction_e(*txAttenSetFnPtr_t)(adi_adrv904x_Device_t *device,
    adi_adrv904x_SpiCache_t *spiCache,
    adrv904x_BfCoreChanAddr_e baseAddr,
    uint16_t bfValue);

typedef adi_adrv904x_ErrAction_e(*txAttenGetFnPtr_t)(adi_adrv904x_Device_t *device,
    adi_adrv904x_SpiCache_t *spiCache,
    adrv904x_BfCoreChanAddr_e baseAddr,
    uint16_t *bfValue);

typedef adi_adrv904x_ErrAction_e(*txAttenUpdateFnPtr_t)(adi_adrv904x_Device_t* const device,
    adi_adrv904x_SpiCache_t* const spiCache,
    const adrv904x_BfCoreChanAddr_e baseAddr,
    const uint8_t bfValue);

static const txAttenSetFnPtr_t txAttenChanS0S1SetFnPtrs[ADI_ADRV904X_MAX_TXCHANNELS][2] = 
{
    { adrv904x_Core_Tx0AttenS0_BfSet, adrv904x_Core_Tx0AttenS1_BfSet },
    { adrv904x_Core_Tx1AttenS0_BfSet, adrv904x_Core_Tx1AttenS1_BfSet },
    { adrv904x_Core_Tx2AttenS0_BfSet, adrv904x_Core_Tx2AttenS1_BfSet },
    { adrv904x_Core_Tx3AttenS0_BfSet, adrv904x_Core_Tx3AttenS1_BfSet },
    { adrv904x_Core_Tx4AttenS0_BfSet, adrv904x_Core_Tx4AttenS1_BfSet },
    { adrv904x_Core_Tx5AttenS0_BfSet, adrv904x_Core_Tx5AttenS1_BfSet },
    { adrv904x_Core_Tx6AttenS0_BfSet, adrv904x_Core_Tx6AttenS1_BfSet },        
    { adrv904x_Core_Tx7AttenS0_BfSet, adrv904x_Core_Tx7AttenS1_BfSet },        
};

static const txAttenGetFnPtr_t txAttenChanS0S1GetFnPtrs[ADI_ADRV904X_MAX_TXCHANNELS][2] = 
{
    { adrv904x_Core_Tx0AttenS0_BfGet, adrv904x_Core_Tx0AttenS1_BfGet },
    { adrv904x_Core_Tx1AttenS0_BfGet, adrv904x_Core_Tx1AttenS1_BfGet },
    { adrv904x_Core_Tx2AttenS0_BfGet, adrv904x_Core_Tx2AttenS1_BfGet },
    { adrv904x_Core_Tx3AttenS0_BfGet, adrv904x_Core_Tx3AttenS1_BfGet },
    { adrv904x_Core_Tx4AttenS0_BfGet, adrv904x_Core_Tx4AttenS1_BfGet },
    { adrv904x_Core_Tx5AttenS0_BfGet, adrv904x_Core_Tx5AttenS1_BfGet },
    { adrv904x_Core_Tx6AttenS0_BfGet, adrv904x_Core_Tx6AttenS1_BfGet },        
    { adrv904x_Core_Tx7AttenS0_BfGet, adrv904x_Core_Tx7AttenS1_BfGet },        
};

static const txAttenUpdateFnPtr_t txAttenChanS0S1UpdateFnPtrs[ADI_ADRV904X_MAX_TXCHANNELS][2] = 
{
    { adrv904x_Core_Tx0AttenUpdateS0_BfSet, adrv904x_Core_Tx0AttenUpdateS1_BfSet },
    { adrv904x_Core_Tx1AttenUpdateS0_BfSet, adrv904x_Core_Tx1AttenUpdateS1_BfSet },
    { adrv904x_Core_Tx2AttenUpdateS0_BfSet, adrv904x_Core_Tx2AttenUpdateS1_BfSet },
    { adrv904x_Core_Tx3AttenUpdateS0_BfSet, adrv904x_Core_Tx3AttenUpdateS1_BfSet },
    { adrv904x_Core_Tx4AttenUpdateS0_BfSet, adrv904x_Core_Tx4AttenUpdateS1_BfSet },
    { adrv904x_Core_Tx5AttenUpdateS0_BfSet, adrv904x_Core_Tx5AttenUpdateS1_BfSet },
    { adrv904x_Core_Tx6AttenUpdateS0_BfSet, adrv904x_Core_Tx6AttenUpdateS1_BfSet },
    { adrv904x_Core_Tx7AttenUpdateS0_BfSet, adrv904x_Core_Tx7AttenUpdateS1_BfSet },
};

ADI_API adi_adrv904x_ErrAction_e  adi_adrv904x_TxAttenTableRead(adi_adrv904x_Device_t* const device,
                                                                const adi_adrv904x_TxChannels_e txChannel,
                                                                const uint32_t txAttenIndexOffset,
                                                                adi_adrv904x_TxAttenTableRow_t txAttenTableRows[],
                                                                const uint32_t numTxAttenEntries)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t i = 0U;
    uint32_t tableAddr = 0U;
    /* Ensure alignment of cfgData */
    uint32_t cfgData32[ADI_ADRV904X_TX_ATTEN_TABLE_CACHE_MAX / 4U] = { 0U };
    uint8_t* cfgData = (uint8_t*) cfgData32;
    uint32_t start = 0U;
    uint32_t stop = 0U;
    uint32_t data = 0U;
    uint32_t offset;
    uint32_t numBytesToRead = 0U;
    static const uint8_t TX_ENTRY_SIZE = 4U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    tableAddr = adrv904x_txAttenAddrLookup(txChannel);
    if (tableAddr == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid TxChannel");
        goto cleanup;
    }

    if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & txChannel) != txChannel)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "TxChannel not initialized");
        goto cleanup;
    }

    if (numTxAttenEntries == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numTxAttenEntries, "Number of TxAttenuation Entries cannot be 0");
        goto cleanup;
    }

    if ((txAttenIndexOffset + numTxAttenEntries - 1U) > ADRV904X_TX_ATTEN_TABLE_MAX_IDX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numTxAttenEntries, "Request exceeds Attenuation Table Bounds");
        goto cleanup;
    }

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txAttenTableRows, cleanup);

    start = 0U;
    stop = 0U;
    offset = (txAttenIndexOffset * TX_ENTRY_SIZE) + tableAddr;

    numBytesToRead = numTxAttenEntries * TX_ENTRY_SIZE;
    if (numBytesToRead > ADI_ADRV904X_TX_ATTEN_TABLE_CACHE_MAX)
    {
        numBytesToRead = ADI_ADRV904X_TX_ATTEN_TABLE_CACHE_MAX;
    }

    recoveryAction = adi_adrv904x_Registers32bOnlyRead(device, NULL, (offset + stop), cfgData, numBytesToRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read Issue");
        goto cleanup;
    }

    for (i = 0; i < numTxAttenEntries; i++)
    {
        data = *((uint32_t*)(cfgData + start));
        txAttenTableRows[i].txAttenHp = ADRV904X_BF_DECODE(data, ADRV904X_TX_ATTEN_TABLE_HP_MASK, ADRV904X_TX_ATTEN_TABLE_HP_SHIFT);
        txAttenTableRows[i].txAttenMult = ADRV904X_BF_DECODE(data, ADRV904X_TX_ATTEN_TABLE_MULT_MASK, ADRV904X_TX_ATTEN_TABLE_MULT_SHIFT);

        start += TX_ENTRY_SIZE;

        if ((start >= ADI_ADRV904X_TX_ATTEN_TABLE_CACHE_MAX) &&
            ((i + 1U) < numTxAttenEntries))
        {
            numBytesToRead = (numTxAttenEntries - (i + 1U)) * TX_ENTRY_SIZE;
            if (numBytesToRead > ADI_ADRV904X_TX_ATTEN_TABLE_CACHE_MAX)
            {
                numBytesToRead = ADI_ADRV904X_TX_ATTEN_TABLE_CACHE_MAX;
            }

            stop += start;
            recoveryAction = adi_adrv904x_Registers32bOnlyRead(device, NULL, (offset + stop), cfgData, numBytesToRead);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read Issue");
                goto cleanup;
            }
            start = 0U;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenSet(adi_adrv904x_Device_t* const device,
                                                         const adi_adrv904x_TxAtten_t txAttenuation[],
                                                         const uint32_t               numTxAttenConfigs)
{
        static const uint8_t DATA_TRANSACTION_1_SIZE   = 9u;
    static const uint8_t DATA_TRANSACTION_2_SIZE   = 6U;
    static const uint8_t DATA_TRANSATCTION_FF_SIZE = 3U;
    static const uint8_t CH0_START_ADDRESS         = 0xB3U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txAttenuation, cleanup);

    if (numTxAttenConfigs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numTxAttenConfigs, "Invalid Number of TxAttentuation Configurations");
        goto cleanup;
    }
    
    /* Loop through the number of configurations and check attenuation mode */
    for (i = 0U; i < numTxAttenConfigs; ++i)
    {
        if (txAttenuation[i].txAttenuation_mdB > ADRV904X_TX_ATTEN_VALUE_MILLI_DB_MAX)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txAttenuation[i].txAttenuation_mdB,
                                   "Invalid txAttenuation_mdB value");
            goto cleanup;
        }
    
        /*Check that if requested Tx Channel valid*/
        if (((txAttenuation[i].txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
            (txAttenuation[i].txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        for (chanId = 0U; chanId <= ADI_ADRV904X_TX_CHAN_ID_MAX; ++chanId)
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
            attenIndex = (txAttenuation[i].txAttenuation_mdB / ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);
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
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   data2,
                                   "Error writing Attenuation Latch Value");
            goto cleanup;
        }
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction); 

}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenGet(adi_adrv904x_Device_t * const    device,
                                                         const adi_adrv904x_TxChannels_e  txChannel,
                                                         adi_adrv904x_TxAtten_t * const   txAttenuation)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint16_t txAttenReadBack = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txAttenuation, cleanup);

    switch (txChannel)
    {
        case ADI_ADRV904X_TX0:
            recoveryAction = adrv904x_Core_Tx0AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TX1:
            recoveryAction = adrv904x_Core_Tx1AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TX2:
            recoveryAction = adrv904x_Core_Tx2AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TX3:
            recoveryAction = adrv904x_Core_Tx3AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TX4:
            recoveryAction = adrv904x_Core_Tx4AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TX5:
            recoveryAction = adrv904x_Core_Tx5AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TX6:
            recoveryAction = adrv904x_Core_Tx6AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TX7:
            recoveryAction = adrv904x_Core_Tx7AttenS0_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &txAttenReadBack);
            break;

        case ADI_ADRV904X_TXOFF:    /* Fall Through */
        case ADI_ADRV904X_TXALL:
        
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    txChannel,
                                    "Invalid TxChannel");
            goto cleanup;
            break;

    }

    /* API Call Check for all Switch Statement */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(   &device->common,
                            ADI_ADRV904X_ERRSRC_API,
                            ADI_COMMON_ERRCODE_API,
                            recoveryAction,
                            txChannel,
                            "Failure to Read Tx Attenuation for Given Channel");
        goto cleanup;
    }


    /* Convert from atten table index to milli dB */
    txAttenuation->txAttenuation_mdB = (txAttenReadBack * ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);
    txAttenuation->txChannelMask = txChannel;
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction); 
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxNcoShifterSet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_TxNcoMixConfig_t * const txNcoConfig)
{
        adi_adrv904x_ErrAction_e        recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_TxNcoMixConfig_t   txNcoInfo;
    adrv904x_TxNcoMixConfigResp_t   cmdRsp;
    adrv904x_CpuCmdStatus_e         cmdStatus       = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e         cpuErrorCode    = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                        cpuTypeIdx      = 0U;
    uint8_t                         channelMaskRet  = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txNcoConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoInfo, 0, sizeof(txNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (txNcoConfig->chanSelect == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }


    if (txNcoConfig->bandSelect > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    if (txNcoConfig->enable > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->enable, "Invalid enable provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoInfo.chanSelect   = (uint8_t) txNcoConfig->chanSelect;

    txNcoInfo.bandSelect   = (uint8_t) txNcoConfig->bandSelect;

    txNcoInfo.enable       = (uint8_t) txNcoConfig->enable;
    txNcoInfo.phase        = (uint32_t) ADRV904X_HTOCL(txNcoConfig->phase);
    txNcoInfo.frequencyKhz = (int32_t) ADRV904X_HTOCL(txNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_SET_TX_MIX_NCO,
                                             (void*)&txNcoInfo,
                                             sizeof(txNcoInfo),
                                             (void*)&cmdRsp,
                                             sizeof(cmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == txNcoInfo.chanSelect)
        {
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV904X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxNcoShifterGet(adi_adrv904x_Device_t* const device,
                                                              adi_adrv904x_TxNcoMixConfigReadbackResp_t* const txRbConfig)
{
        adi_adrv904x_ErrAction_e                    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_TxNcoMixConfigReadback_t           txNcoCfgRbCmd;
    adi_adrv904x_TxNcoMixConfigReadbackResp_t   txNcoCfgGetCmdRsp;
    adrv904x_CpuCmdStatus_e                     cmdStatus           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                     cpuErrorCode        = ADRV904X_CPU_NO_ERROR;
    uint32_t                                    cpuType             = 0U;
    uint32_t                                    chIdx               = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoCfgRbCmd, 0, sizeof(txNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&txNcoCfgGetCmdRsp, 0, sizeof(txNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated*/
    if (txRbConfig->chanSelect == (uint8_t) ADI_ADRV904X_TXOFF ||
        txRbConfig->chanSelect > (uint8_t) ADI_ADRV904X_TX7    ||
        (((txRbConfig->chanSelect - 1) & txRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Get CPU assigned to this channel */
    for (chIdx = 0U; chIdx < ADI_ADRV904X_MAX_CHANNELS; chIdx++)
    {
        if (txRbConfig->chanSelect == ((uint8_t)1U << chIdx))
        {
            cpuType = device->initExtract.rxTxCpuConfig[chIdx];
            break;
        }
    }

    /* Prepare the command payload */
    txNcoCfgRbCmd.chanSelect = (uint8_t) txRbConfig->chanSelect;


    txNcoCfgRbCmd.bandSelect = (uint8_t) txRbConfig->bandSelect;
    if (txNcoCfgRbCmd.bandSelect > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoCfgRbCmd.bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         (adi_adrv904x_CpuType_e)cpuType,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_GET_TX_MIX_NCO,
                                         (void*)&txNcoCfgRbCmd,
                                         sizeof(txNcoCfgRbCmd),
                                         (void*)&txNcoCfgGetCmdRsp,
                                         sizeof(txNcoCfgGetCmdRsp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)txNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
    }

    if (txNcoCfgGetCmdRsp.chanSelect == txNcoCfgRbCmd.chanSelect)
    {
        /* Extract the command-specific response from the response payload */
        txRbConfig->enable       = (uint8_t) txNcoCfgGetCmdRsp.enable;
        txRbConfig->phase        = (uint32_t) ADRV904X_CTOHL(txNcoCfgGetCmdRsp.phase);
        txRbConfig->frequencyKhz = (int32_t) ADRV904X_CTOHL(txNcoCfgGetCmdRsp.frequencyKhz);
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Channel is not handled by the CPU");
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxTestToneSet(adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_TxTestNcoConfig_t * const txNcoConfig)
{
        adi_adrv904x_ErrAction_e        recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_TxTestNcoConfig_t  txNcoInfo;
    adrv904x_TxTestNcoConfigResp_t  cmdRsp;
    adrv904x_CpuCmdStatus_e         cmdStatus       = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e         cpuErrorCode    = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                        cpuTypeIdx      = 0U;
    uint8_t                         channelMaskRet  = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txNcoConfig, cleanup);
    ADI_LIBRARY_MEMSET(&txNcoInfo, 0, sizeof(txNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (txNcoConfig->chanSelect == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }


    if (txNcoConfig->bandSelect > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    if (txNcoConfig->ncoSelect >= ADI_ADRV904X_TX_TEST_NCO_MAX_NUM)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->ncoSelect, "Invalid ncoSelect provided.");
        goto cleanup;
    }

    if (txNcoConfig->enable > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->enable, "Invalid enable provided.");
        goto cleanup;
    }

    if (txNcoConfig->attenCtrl >= ADI_ADRV904X_TX_TEST_NCO_ATTEN_MAX_NUM)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txNcoConfig->attenCtrl, "Invalid attenCtrl provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoInfo.chanSelect   = (uint8_t) txNcoConfig->chanSelect;

    txNcoInfo.bandSelect   = (uint8_t) txNcoConfig->bandSelect;

    txNcoInfo.ncoSelect    = (adi_adrv904x_TxTestNcoSelect_t) txNcoConfig->ncoSelect;
    txNcoInfo.enable       = (uint8_t) txNcoConfig->enable;
    txNcoInfo.phase        = (uint32_t) ADRV904X_HTOCL(txNcoConfig->phase);
    txNcoInfo.frequencyKhz = (int32_t) ADRV904X_HTOCL(txNcoConfig->frequencyKhz);
    txNcoInfo.attenCtrl    = (adi_adrv904x_TxTestNcoAtten_t) txNcoConfig->attenCtrl;

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_SET_TX_TEST_NCO,
                                             (void*)&txNcoInfo,
                                             sizeof(txNcoInfo),
                                             (void*)&cmdRsp,
                                             sizeof(cmdRsp),
                                             &cmdStatus);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == txNcoInfo.chanSelect)
        {
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV904X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxTestToneGet(adi_adrv904x_Device_t* const device,
                                                            adi_adrv904x_TxTestNcoConfigReadbackResp_t* const txRbConfig)
{
        adi_adrv904x_ErrAction_e                    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_TxTestNcoConfigReadback_t          txNcoCfgRbCmd;
    adi_adrv904x_TxTestNcoConfigReadbackResp_t  txNcoCfgGetCmdRsp;
    adrv904x_CpuCmdStatus_e                     cmdStatus           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                     cpuErrorCode        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                    cpuTypeIdx          = 0U;
    uint8_t                                     channelMaskRet      = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoCfgRbCmd, 0, sizeof(txNcoCfgRbCmd));
    ADI_LIBRARY_MEMSET(&txNcoCfgGetCmdRsp, 0, sizeof(txNcoCfgGetCmdRsp));

    /* Testing  if channel mask has more than two channels activated*/
    if (txRbConfig->chanSelect == (uint8_t) ADI_ADRV904X_TXOFF || 
        txRbConfig->chanSelect > (uint8_t) ADI_ADRV904X_TX7    || 
        (((txRbConfig->chanSelect - 1) & txRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }



    if (txRbConfig->bandSelect > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->bandSelect, "Invalid bandSelect provided.");
        goto cleanup;
    }

    if (txRbConfig->ncoSelect >= ADI_ADRV904X_TX_TEST_NCO_MAX_NUM)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->ncoSelect, "Invalid ncoSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoCfgRbCmd.chanSelect = (uint8_t) txRbConfig->chanSelect;

    txNcoCfgRbCmd.bandSelect = (uint8_t) txRbConfig->bandSelect;
    txNcoCfgRbCmd.ncoSelect  = (adi_adrv904x_TxTestNcoSelect_t) txRbConfig->ncoSelect;

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_GET_TX_TEST_NCO,
                                             (void*)&txNcoCfgRbCmd,
                                             sizeof(txNcoCfgRbCmd),
                                             (void*)&txNcoCfgGetCmdRsp,
                                             sizeof(txNcoCfgGetCmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)txNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet = txNcoCfgGetCmdRsp.chanSelect;
        if (channelMaskRet == txNcoCfgRbCmd.chanSelect)
        {
            /* Extract the command-specific response from the response payload */
            txRbConfig->chanSelect   = (uint8_t)txNcoCfgGetCmdRsp.chanSelect;

            txRbConfig->bandSelect   = (uint8_t)txNcoCfgGetCmdRsp.bandSelect;
            txRbConfig->bandSelect   = 0;
            txRbConfig->ncoSelect    = (uint8_t) txNcoCfgGetCmdRsp.ncoSelect;
            txRbConfig->enabled      = (uint8_t) txNcoCfgGetCmdRsp.enabled;
            txRbConfig->attenCtrl    = txNcoCfgGetCmdRsp.attenCtrl;
            txRbConfig->phase        = (uint32_t) ADRV904X_CTOHL(txNcoCfgGetCmdRsp.phase);
            txRbConfig->frequencyKhz = (int32_t) ADRV904X_CTOHL(txNcoCfgGetCmdRsp.frequencyKhz);
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV904X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCducNcoSet(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_TxCducNcoConfig_t * const txCducNcoConfig)
{
        adi_adrv904x_ErrAction_e        recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_TxCducNcoConfig_t  txNcoInfo;
    adrv904x_TxNcoCducConfigResp_t  cmdRsp;
    adrv904x_CpuCmdStatus_e         cmdStatus       = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e         cpuErrorCode    = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                        cpuTypeIdx      = 0U;
    uint8_t                         channelMaskRet  = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCducNcoConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoInfo, 0, sizeof(txNcoInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (txCducNcoConfig->chanSelect == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCducNcoConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    if (txCducNcoConfig->carrierSelect == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCducNcoConfig->carrierSelect, "Invalid carrier provided.");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxCducCarrierCheck(device, txCducNcoConfig->chanSelect, txCducNcoConfig->carrierSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxCducCarrierCheck issue");
        goto cleanup;
    }

    if (txCducNcoConfig->enable > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCducNcoConfig->enable, "Invalid enable provided.");
        goto cleanup;
    }

    if (txCducNcoConfig->phase > MAX_CDUC_NCO_PHASE_DEGREES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCducNcoConfig->phase, "Invalid phase provided.");
        goto cleanup;
    }

    if ((txCducNcoConfig->frequencyKhz > MAX_CDUC_NCO_FREQ_KHZ) ||
        (txCducNcoConfig->frequencyKhz < -MAX_CDUC_NCO_FREQ_KHZ))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCducNcoConfig->frequencyKhz, "Invalid frequency provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoInfo.chanSelect    = (uint8_t) txCducNcoConfig->chanSelect;
    txNcoInfo.carrierSelect = (uint8_t) txCducNcoConfig->carrierSelect;
    txNcoInfo.enable        = (uint8_t) txCducNcoConfig->enable;
    txNcoInfo.phase         = (uint32_t) ADRV904X_HTOCL(txCducNcoConfig->phase);
    txNcoInfo.frequencyKhz  = (int32_t) ADRV904X_HTOCL(txCducNcoConfig->frequencyKhz);

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_SET_TX_CDUC_NCO,
                                             (void*)&txNcoInfo,
                                             sizeof(txNcoInfo),
                                             (void*)&cmdRsp,
                                             sizeof(cmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        channelMaskRet |= cmdRsp.chanSelect;
        if (channelMaskRet == txNcoInfo.chanSelect)
        {
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV904X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCducNcoGet(adi_adrv904x_Device_t* const device,
                                                           adi_adrv904x_TxCducNcoConfigReadbackResp_t* const txRbConfig)
{
        adi_adrv904x_ErrAction_e                    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_TxCducNcoConfig_t              txNcoCfgGetRbCmd;
    adi_adrv904x_TxCducNcoConfigReadbackResp_t  txNcoCfgGetCmdRsp;
    adrv904x_CpuCmdStatus_e                     cmdStatus           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                     cpuErrorCode        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                    cpuTypeIdx          = 0U;
    uint8_t                                     channelMaskRet      = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txRbConfig, cleanup);

    ADI_LIBRARY_MEMSET(&txNcoCfgGetRbCmd, 0, sizeof(txNcoCfgGetRbCmd));
    ADI_LIBRARY_MEMSET(&txNcoCfgGetCmdRsp, 0, sizeof(txNcoCfgGetCmdRsp));

    /* Test if channel mask contains one and only one channel */
    if (txRbConfig->chanSelect == (uint8_t) ADI_ADRV904X_TXOFF ||
        txRbConfig->chanSelect > (uint8_t) ADI_ADRV904X_TX7    ||
        (((txRbConfig->chanSelect - 1) & txRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Testing if carrier mask has more than two cariers activated */
    if ((txRbConfig->carrierSelect == (uint8_t)ADI_ADRV904X_TX_CARRIER_NONE) ||
        (txRbConfig->carrierSelect > (uint8_t)ADI_ADRV904X_TX_CARRIER_7)  ||
        (((txRbConfig->carrierSelect - 1) & txRbConfig->carrierSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->carrierSelect, "Invalid carrierSelect provided.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_TxCducCarrierCheck(device, txRbConfig->chanSelect, txRbConfig->carrierSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxCducCarrierCheck issue");
        goto cleanup;
    }

    /* Prepare the command payload */
    txNcoCfgGetRbCmd.chanSelect    = (uint8_t) txRbConfig->chanSelect;
    txNcoCfgGetRbCmd.carrierSelect = (uint8_t) txRbConfig->carrierSelect;

    /* For each CPU, send the command, wait for a response, and process any errors.
     * Only the CPU that owns this channel will set chanSelect in its response.
     */
    for (cpuTypeIdx = 0U; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                             (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                             ADRV904X_LINK_ID_0,
                                             ADRV904X_CPU_CMD_ID_GET_TX_CDUC_NCO,
                                             (void*)&txNcoCfgGetRbCmd,
                                             sizeof(txNcoCfgGetRbCmd),
                                             (void*)&txNcoCfgGetCmdRsp,
                                             sizeof(txNcoCfgGetCmdRsp),
                                             &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)txNcoCfgGetCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup) ;
        }

        channelMaskRet = txNcoCfgGetCmdRsp.chanSelect;
        if (channelMaskRet == txNcoCfgGetRbCmd.chanSelect)
        {
            /* Extract the command-specific response from the response payload */
            txRbConfig->enable       = (uint8_t) txNcoCfgGetCmdRsp.enable;
            txRbConfig->phase        = (uint32_t) ADRV904X_CTOHL(txNcoCfgGetCmdRsp.phase);
            txRbConfig->frequencyKhz = (int32_t) ADRV904X_CTOHL(txNcoCfgGetCmdRsp.frequencyKhz);
            break; /* Commands are done */
        }
    }

    /* The above loop should always break before cpuTypeIdx reaches ADI_ADRV904X_CPU_TYPE_MAX_RADIO */
    if (cpuTypeIdx >= ADI_ADRV904X_CPU_TYPE_MAX_RADIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMaskRet, "FW did not execute cmd on all channels requested");
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPfirFracDelaySet(adi_adrv904x_Device_t* const device,
                                                                 const uint8_t txChan,
                                                                 const uint8_t fracDelay)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    adrv904x_DfeSvctxPfirFracDelaySet_t      cmdStruct;
    adrv904x_DfeSvctxPfirFracDelaySetResp_t    respStruct;

    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    if (txChan >= ADI_ADRV904X_MAX_TXCHANNELS)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChan, "Invalid txChan provided.");
        goto cleanup;
    }

    if (fracDelay < 64)
    {
        /* Do nothing, fracDelay is valid. */
    }
    else
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, fracDelay, "Invalid fracDelay provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    cmdStruct.txChannel = txChan;
    cmdStruct.fracDelay  = fracDelay;

    /* Send command and receive response to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_RADIO_TX_PFIR_FRAC_DELAY_SET, 
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DfeSvcCmdSend - ctc2 frac delay set");
        goto cleanup;
    }

    if (respStruct.status != ADI_ADRV904X_DFE_SVC_ERR_CODE_NO_ERROR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE; 
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respStruct.status, "Ctc2 frac delay set status");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenS0S1Set(adi_adrv904x_Device_t* const   device,
                                                             const uint32_t                 chanMask,
                                                             const uint32_t                 levelMilliDB,
                                                             const uint8_t                  isS0)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t chanId = 0U;
    uint8_t isS1 = !isS0;
    uint32_t attenRegVal = 0U;

    /* Variables for minimum Tx attenuation range check - only for A0 and A1 silicon  */
    uint16_t minAttenIndexSet = UINT16_MAX;
    uint16_t attenIndex = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    if ((chanMask > ADI_ADRV904X_TX_CHAN_MASK_MAX) ||
        (chanMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                chanMask,
                                "Invalid channel mask");
        goto cleanup;
    }

    if (levelMilliDB > ADRV904X_TX_ATTEN_VALUE_MILLI_DB_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                levelMilliDB,
                                "Value exceeds max attenuation value");
        goto cleanup;
    }

    /* for each Tx channel in chanMask */
    for (chanId = 0U; chanId <= ADI_ADRV904X_TX_CHAN_ID_MAX; ++chanId)
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
        attenIndex = (levelMilliDB / ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);
        attenRegVal = attenIndex;

        if (attenIndex  < minAttenIndexSet)
        {
            minAttenIndexSet = attenIndex;
        }

        /* Write to the S0/S1 register */
        recoveryAction = txAttenChanS0S1SetFnPtrs[chanId][isS1](device,
                                                                NULL,
                                                                ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                attenRegVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_ADRV904X_ERRSRC_API,
                                ADI_COMMON_ERRCODE_API,
                                recoveryAction,
                                chanId,
                                "Failure to write S0/S1 TxAttenuation");
            goto cleanup;
        }

        /* Latch in the S0/S1 register */
        recoveryAction = txAttenChanS0S1UpdateFnPtrs[chanId][isS1]( device,
                                                                    NULL,
                                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                    1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ERROR_REPORT(   &device->common,
                                ADI_ADRV904X_ERRSRC_API,
                                ADI_COMMON_ERRCODE_API,
                                recoveryAction,
                                chanId,
                                "Failure to update S0/S1 TxAttenuation");
            goto cleanup;
        }
    }   /* for each Tx channel */
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);   
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenS0S1Get(adi_adrv904x_Device_t* const device,
                                                             const adi_adrv904x_TxChannels_e txChannel,
                                                             uint32_t* const levelMilliDB,
                                                             const uint8_t isS0)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t isS1 = !isS0;
    uint16_t attenRegVal = 0U;
    uint8_t chanId = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, levelMilliDB, cleanup);

    ADI_ADRV904X_API_ENTRY(&device->common);

    chanId = adrv904x_TxChannelsToId(txChannel);
    if (chanId > ADI_ADRV904X_TX_CHAN_ID_MAX)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
            chanId,                        
            "Invalid channel id");
        goto cleanup;
    }
        
    /* Read back the S0/S1 register */
    recoveryAction = (adi_adrv904x_ErrAction_e)txAttenChanS0S1GetFnPtrs[chanId][isS1](device,
        NULL,
        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
        &attenRegVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_ADRV904X_ERRSRC_API,
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
    *levelMilliDB = attenRegVal * ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05;
                
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);   
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenUpdateCfgSet(adi_adrv904x_Device_t* const device,
                                                                  const uint32_t chanMask,
                                                                  const adi_adrv904x_TxAttenUpdateCfg_t* const cfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t chanId = 0;
    uint32_t chanSel = 0U;
    adrv904x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv904x_BfTxDigChanAddr_e)0U;
    adrv904x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = (adrv904x_BfTxFuncsChanAddr_e)0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfg, cleanup);

    if ((chanMask > ADI_ADRV904X_TX_CHAN_MASK_MAX) || 
        (chanMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
            chanMask,                        
            "Invalid channel mask");
        goto cleanup;
    }
    
    if (cfg->trgCfg.updateTrg != ADI_ADRV904X_TXATTEN_UPD_TRG_NONE &&
        cfg->trgCfg.updateTrg != ADI_ADRV904X_TXATTEN_UPD_TRG_SPI &&
        cfg->trgCfg.updateTrg != ADI_ADRV904X_TXATTEN_UPD_TRG_GPIO)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            cfg->trgCfg.updateTrg,
            "Invalid ADI_ADRV904X_TXATTEN_UPD_TRG value");
        goto cleanup;
    }
    
    if (cfg->srcCfg.updateSrc != ADI_ADRV904X_TXATTEN_UPD_SRC_S0 &&
        cfg->srcCfg.updateSrc != ADI_ADRV904X_TXATTEN_UPD_SRC_S0_OR_S1)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            cfg->srcCfg.updateSrc,
            "Invalid ADI_ADRV904X_TXATTEN_UPD_SRC value");
        goto cleanup;        
    }
        
    /* If update is via gpio verify the trigger gpio is valid.
     * Note: Due to h/w pinmux limitations pin0 cannot be routed for use as the trigger. */    
    if (cfg->trgCfg.updateTrg == ADI_ADRV904X_TXATTEN_UPD_TRG_GPIO && 
        (cfg->trgCfg.triggerGpio >= ADI_ADRV904X_GPIO_INVALID ||
         cfg->trgCfg.triggerGpio == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
            cfg->trgCfg.triggerGpio,                        
            "Invalid triggerGpio");
        goto cleanup;
    }
    
    /* If use of S0/S1 is determined via gpio ensure gpio is valid.
     * Note: Due to h/w pinmux limitations pin0 cannot be routed for use as the S0_S1 src */
    if (cfg->srcCfg.updateSrc == ADI_ADRV904X_TXATTEN_UPD_SRC_S0_OR_S1 &&
        (cfg->srcCfg.s0OrS1Gpio >= ADI_ADRV904X_GPIO_INVALID ||
         cfg->srcCfg.s0OrS1Gpio == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
            cfg->srcCfg.s0OrS1Gpio,                        
            "Invalid s0OrS1Gpio");
        goto cleanup;
    }
    
    /* If update is via gpio and S1/S0 is determined by gpio verify
     * the gpios are different */ 
    if (cfg->trgCfg.updateTrg == ADI_ADRV904X_TXATTEN_UPD_TRG_GPIO && 
        cfg->srcCfg.updateSrc == ADI_ADRV904X_TXATTEN_UPD_SRC_S0_OR_S1 &&        
        cfg->trgCfg.triggerGpio == cfg->srcCfg .s0OrS1Gpio )
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
            ADI_NO_VARIABLE,                        
            "Cannot use same GPIO for trigger gpio and S0/S1 selection");
        goto cleanup;
    }    
            
    /* The global CoreSpiEnable bit is set for SPI trigger; Unset for any other trigger. */
    recoveryAction =  adrv904x_Core_TxAttenUpdCoreSpiEn_BfSet(device,
                                                              NULL,
                                                              ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                              (cfg->trgCfg.updateTrg == ADI_ADRV904X_TXATTEN_UPD_TRG_SPI));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv904x_Core_TxAttenUpdCoreSpiEn_BfSet failed.");
        goto cleanup;
    } 
    
    /* Routing of GPIO signals to TxChans for update triggering 
     * must be done for all channels in a single call not channel by channel */    
    if (cfg->trgCfg.updateTrg == ADI_ADRV904X_TXATTEN_UPD_TRG_GPIO)
    {        
        /* Route selected trigger GPIO to TxChans*/
        recoveryAction = adrv904x_GpioSignalSet(device,
            cfg->trgCfg.triggerGpio, 
            ADI_ADRV904X_GPIO_SIGNAL_TX_ATTEN_UPD_GPIO,
            chanMask);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                recoveryAction,
                "Call to adrv904x_GpioSignalSet failed for trigger GPIO");
            goto cleanup;
        }
    }
    
    /* Routing of GPIO signals to TxChans for S0/S1 selection  
     * must be done for all channels in a single call not channel by channel */
    if (cfg->srcCfg.updateSrc == ADI_ADRV904X_TXATTEN_UPD_SRC_S0_OR_S1)
    {
        /* Configuring which GPIO selects S0/S1. */
        recoveryAction = adrv904x_GpioSignalSet(device,
            cfg->srcCfg.s0OrS1Gpio,
            ADI_ADRV904X_GPIO_SIGNAL_SELECT_S1,
            chanMask);
            
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                recoveryAction,
                "Call to adrv904x_GpioSignalSet failed for S0/S1 gpio.");
            goto cleanup;
        }
    }

    /* for each Tx channel in chanMask */
    for (chanId = 0U; chanId <= ADI_ADRV904X_TX_CHAN_ID_MAX; chanId++)
    {
        chanSel = 1U << chanId;

        if ((chanMask & (1 << chanId)) == 0)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        /* Convert the chanId to the base address value required by the bitfield functions */
        recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txFuncsChanBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txDigChanBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }
                
        switch (cfg->trgCfg.updateTrg)
        {
        case ADI_ADRV904X_TXATTEN_UPD_TRG_NONE:
            /* Do not trigger update from GPIO */
            recoveryAction = adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet failed.");
                goto cleanup;
            }
            break;
        case ADI_ADRV904X_TXATTEN_UPD_TRG_SPI:
            /* Hardware supports level trigger for SPI but we don't expose that. Is always edge triggered. */
            recoveryAction = adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     TX_ATTEN_EDGE_TRIGGER);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet failed.");
                goto cleanup;
            }
            break;
        case ADI_ADRV904X_TXATTEN_UPD_TRG_GPIO:
            /* Do trigger update from GPIO */
            recoveryAction = adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv904x_TxFuncs_TxAttenUpdGpioEn_BfSet failed.");
                goto cleanup;
            }
            
            /*  Always set edge triggered. Hardware supports level trigger for GPIO but we don't expose that. */
            recoveryAction = adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     TX_ATTEN_EDGE_TRIGGER);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv904x_TxFuncs_TxAttenUpdTrgSel_BfSet failed.");
                goto cleanup;
            }
            
            /* Setting the GPIO that triggers the update is done by adrv904x_GpioSignalSet above. */
            
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                cfg->trgCfg.updateTrg,
                "Invalid ADI_ADRV904X_TXATTEN_UPD_TRG value");
            goto cleanup;
            break;
        }
        
        switch (cfg->srcCfg.updateSrc)
        {
        case ADI_ADRV904X_TXATTEN_UPD_SRC_S0:
            /* Turn off use of per-slice tx_attenuation register as that would override S0 */
            recoveryAction = adrv904x_TxFuncs_UseSliceAttenValue_BfSet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv904x_TxFuncs_UseSliceAttenValue_BfSet failed.");
                goto cleanup;
            }
            
            /* Turn off use of GPIO to select between S0/S1 */
            recoveryAction = adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet(device,
                                                                             NULL,
                                                                             txFuncsChanBaseAddr,
                                                                             0U);
            break;            
        case ADI_ADRV904X_TXATTEN_UPD_SRC_S0_OR_S1:
            /* Turn off use of per-slice tx_attenuation register that would override S0 */            
            recoveryAction = adrv904x_TxFuncs_UseSliceAttenValue_BfSet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv904x_TxFuncs_UseSliceAttenValue_BfSet failed.");
                goto cleanup;
            }
            
            /* Turn on use of GPIO to select between S0/S1 */
            recoveryAction = adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet(device,
                                                                             NULL,
                                                                             txFuncsChanBaseAddr,
                                                                             1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common,
                    recoveryAction,
                    "Call to adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfSet failed.");
                goto cleanup;
            }
            
            /* Configuring which GPIO selects S0/S1 is done by adrv904x_GpioSignalSet above. */            
            break;            
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                cfg->srcCfg.updateSrc,
                "Invalid ADI_ADRV904X_TXATTEN_UPD_SRC value");
            goto cleanup;
            break;
        }
    }
    
cleanup:
        ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenUpdateCfgGet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                  adi_adrv904x_TxAttenUpdateCfg_t* const cfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv904x_BfTxDigChanAddr_e)0U;
    adrv904x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = (adrv904x_BfTxFuncsChanAddr_e)0U;    
    uint8_t chanId = 0U;

    uint8_t updCoreSpiEn = 0U;       /* global config that overrides the per-channel updGpioEn */
    uint8_t updGpioEn = 0U;          /* enables gpio update trigger */
    uint8_t trgCfgTriggerGpio = 0U;  /* identifies gpio update trigger */
    uint8_t useS1ViaGpioPin = 0U;    /* enables S0/S1 selection via GPIO */
    uint8_t srcCfgS0OrS1Gpio = 0U;   /* identifies S0/S1 selection GPIO */
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);    

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cfg, cleanup);
        
    chanId = adrv904x_TxChannelsToId(txChannel);
    if (chanId > ADI_ADRV904X_TX_CHAN_ID_MAX)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
            ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
            chanId,                        
            "Invalid channel id");
        goto cleanup;
    }
    
    /* Convert the chanId to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, txChannel, &txDigChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }    
    
    recoveryAction =  adrv904x_Core_TxAttenUpdCoreSpiEn_BfGet(device,
                                                              NULL,
                                                              ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                              &updCoreSpiEn);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv904x_Core_TxAttenUpdCoreSpiEn_BfGet failed.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_TxFuncs_TxAttenUpdGpioEn_BfGet(device,
                                                             NULL,
                                                             txFuncsChanBaseAddr,
                                                             &updGpioEn);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv904x_TxFuncs_TxAttenUpdGpioEn_BfGet failed.");
        goto cleanup;
    }
            
    recoveryAction = adrv904x_TxDig_TxAttenUpdGpioSelect_BfGet(device,
                                                               NULL,
                                                               txDigChanBaseAddr,
                                                               &trgCfgTriggerGpio);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv904x_TxDig_TxAttenUpdGpioSelect_BfGet failed.");
        goto cleanup;
    }
        
    /* Turn off use of GPIO to select between S0/S1 */
    recoveryAction = adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfGet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     &useS1ViaGpioPin);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv904x_TxFuncs_UseAttenWordS1ViaGpioPin_BfGet failed.");
        goto cleanup;
    }
        
    recoveryAction = adrv904x_TxDig_Spi2TxAttenGpioSelect_BfGet(device,
                                                                NULL,
                                                                txDigChanBaseAddr,
                                                                &srcCfgS0OrS1Gpio);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Call to adrv904x_TxDig_Spi2TxAttenGpioSelect_BfGet failed.");
        goto cleanup;
    }
    
    /* Analyze the device settings and populate the cfg structure accordingly */    
    if (useS1ViaGpioPin == 0)
    {
        cfg->srcCfg.updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0;            
    }
    else
    {
        cfg->srcCfg.updateSrc = ADI_ADRV904X_TXATTEN_UPD_SRC_S0_OR_S1;
        cfg->srcCfg.s0OrS1Gpio = (adi_adrv904x_GpioPinSel_e)srcCfgS0OrS1Gpio;
    }
    
    if (updCoreSpiEn > 0)
    {
        cfg->trgCfg.updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_SPI;
    }
    else if (updGpioEn > 0)
    {
        cfg->trgCfg.updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_GPIO;
        cfg->trgCfg.triggerGpio = (adi_adrv904x_GpioPinSel_e)trgCfgTriggerGpio ;
    }
    else
    {
        cfg->trgCfg.updateTrg = ADI_ADRV904X_TXATTEN_UPD_TRG_NONE;
    }                
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenUpdate(adi_adrv904x_Device_t *const device,
                                                            const uint32_t chanMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
        
    if ((chanMask > ADI_ADRV904X_TX_CHAN_MASK_MAX) ||
        (chanMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,                          
                               ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                               chanMask,                        
                               "Invalid channel mask");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_TxAttenUpdCoreSpi_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                           chanMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to update TxAtten");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_Core_TxAttenUpdCoreSpi_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                           0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to reset update TxAtten bits");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxLoSourceGet(adi_adrv904x_Device_t * const   device,
                                                            const adi_adrv904x_TxChannels_e txChannel,
                                                            adi_adrv904x_LoSel_e * const    txLoSource)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Check Tx data format config pointer is not NULL*/
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txLoSource, cleanup);
    
    /* Check that the requested txChannel is valid */
    if ((txChannel != ADI_ADRV904X_TX0) &&
        (txChannel != ADI_ADRV904X_TX1) &&
        (txChannel != ADI_ADRV904X_TX2) &&
        (txChannel != ADI_ADRV904X_TX3) &&
        (txChannel != ADI_ADRV904X_TX4) &&
        (txChannel != ADI_ADRV904X_TX5) &&
        (txChannel != ADI_ADRV904X_TX6) &&
        (txChannel != ADI_ADRV904X_TX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, 
                                 recoveryAction, 
                                 txChannel, 
                                 "Invalid Tx channel selected for LO source mapping read back. Valid Tx channels are Tx0-Tx7.");
        goto cleanup;
    }
    
    /* Check that Tx profile is valid in current config */
    if (((device->devStateInfo.profilesValid & ADI_ADRV904X_TX_PROFILE_VALID) != ADI_ADRV904X_TX_PROFILE_VALID)
        || ((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & (uint32_t)txChannel) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 txChannel,
                                 "LO source read back requested for an Tx channel but Tx profile is invalid or channel not initialized in the device structure");
        goto cleanup;
    }
    
    /* Readback Tx LO source selection */
    recoveryAction = adrv904x_TxLoSourceGet(device, txChannel, txLoSource);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving Lo Source for Tx channel.");
        goto cleanup;
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPowerMonitorCfgSet(adi_adrv904x_Device_t* const                    device,
                                                                   const adi_adrv904x_PowerMonitorCfgRt_t          txPowerMonitorCfg[],
                                                                   const uint32_t                                  numPowerProtectCfgs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    static const uint16_t MAX_PEAK_PRE_THRESHOLD = 511U;
    uint16_t peakPreThreShold = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txPowerMonitorCfg, cleanup);
    
    if (numPowerProtectCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numPowerProtectCfgs, "Invalid Number of Tx Power Monitor Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numPowerProtectCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_TxPowerMonitorCfgRangeCheck(device, &txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Power Monitor Range Check Error Reported");
            goto cleanup;
        }
        
        /*Check that if requested Tx Channel valid*/
        if (((txPowerMonitorCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txPowerMonitorCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txPowerMonitorCfg[cfgIdx].txChannelMask,
                "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            goto cleanup;
        }
    }

    /* Write out the configurations to appropriate bitfields */
    for (cfgIdx = 0U; cfgIdx < numPowerProtectCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(txPowerMonitorCfg[cfgIdx].txChannelMask, chanSel))
            {
                recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                /* Peak Pre Threshold must be set to (peakThreshold/2) / 2^(16-9) to adjust bus width */
                peakPreThreShold = txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakThreshold >> 8;
                if (peakPreThreShold > MAX_PEAK_PRE_THRESHOLD)
                {
                    /* Saturate pre threshold */
                    peakPreThreShold = MAX_PEAK_PRE_THRESHOLD;
                }

                recoveryAction = adrv904x_TxFuncs_PaProtectionOverloadThPre_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  peakPreThreShold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Pre-Threshold");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_PaProtectionInputSel_BfSet(device,
                                                                             NULL,
                                                                             txBaseAddr,
                                                                             (uint8_t)txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.inputSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Input Select");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_PaProtectionPeakThreshold_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakThreshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionPeakDur_BfSet(device,
                                                                            NULL,
                                                                            txBaseAddr,
                                                                            txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.measDuration);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Duration");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionPeakCount_BfSet(device,
                                                                              NULL,
                                                                              txBaseAddr,
                                                                              txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakCount);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Count");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfSet(device,
                                                                                                NULL,
                                                                                                txBaseAddr,
                                                                                                txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakErrorClearRequired);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Error Clear Required");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfSet(device,
                                                                                NULL,
                                                                                txBaseAddr,
                                                                                txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakPowerEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Power Enable");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfSet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.peakPowerIrqEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Peak Power IRQ enable");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionAverageThreshold_BfSet(device,
                                                                                     NULL,
                                                                                     txBaseAddr,
                                                                                     txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgThreshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Threshold");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionAverageDur_BfSet(device,
                                                                               NULL,
                                                                               txBaseAddr,
                                                                               txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.measDuration);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Duration");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfSet(device,
                                                                                               NULL,
                                                                                               txBaseAddr,
                                                                                               txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgErrorClearRequired);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Error Clear Required");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfSet(device,
                                                                               NULL,
                                                                               txBaseAddr,
                                                                               txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgPowerEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average Enable");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgPowerIrqEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Average IRQ Enable");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_PaProtectionAprEn_BfSet(device,
                                                                          NULL,
                                                                          txBaseAddr,
                                                                          txPowerMonitorCfg[cfgIdx].txPowerMonitorCfg.avgPeakRatioEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Average to Peak Ratio Enable");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPowerMonitorCfgGet(adi_adrv904x_Device_t * const device,
                                                                   adi_adrv904x_PowerMonitorCfgRt_t* const txPowerMonitorCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txPowerMonitorCfg, cleanup);

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)txPowerMonitorCfg->txChannelMask, &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txPowerMonitorCfg->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionInputSel_BfGet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Input Select");
        goto cleanup;
    }
    txPowerMonitorCfg->txPowerMonitorCfg.inputSel = (adi_adrv904x_PaProtectionInputSel_e)tempByte;

    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakThreshold_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &txPowerMonitorCfg->txPowerMonitorCfg.peakThreshold);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Threshold");
        goto cleanup;
    }

    /* Peak and Average measurement durations are programmed same to HW */
    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakDur_BfGet(device,
                                                                NULL,
                                                                txBaseAddr,
                                                                &txPowerMonitorCfg->txPowerMonitorCfg.measDuration);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Duration");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakCount_BfGet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  &txPowerMonitorCfg->txPowerMonitorCfg.peakCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Count");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerErrorClearRequired_BfGet(device,
                                                                                    NULL,
                                                                                    txBaseAddr,
                                                                                    &txPowerMonitorCfg->txPowerMonitorCfg.peakErrorClearRequired);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Error Clear Required");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfGet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    &txPowerMonitorCfg->txPowerMonitorCfg.peakPowerEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Power Enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerIrqEn_BfGet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       &txPowerMonitorCfg->txPowerMonitorCfg.peakPowerIrqEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Power IRQ enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionAverageThreshold_BfGet(device,
                                                                         NULL,
                                                                         txBaseAddr,
                                                                         &txPowerMonitorCfg->txPowerMonitorCfg.avgThreshold);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average Threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerErrorClearRequired_BfGet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   &txPowerMonitorCfg->txPowerMonitorCfg.avgErrorClearRequired);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average Error Clear Required");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfGet(device,
                                                                   NULL,
                                                                   txBaseAddr,
                                                                   &txPowerMonitorCfg->txPowerMonitorCfg.avgPowerEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average Enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerIrqEn_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &txPowerMonitorCfg->txPowerMonitorCfg.avgPowerIrqEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average IRQ Enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionAprEn_BfGet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              &txPowerMonitorCfg->txPowerMonitorCfg.avgPeakRatioEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average to Peak Ratio Enable");
        goto cleanup;
    }


cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionErrorGet(adi_adrv904x_Device_t* const    device,
                                                                   const adi_adrv904x_TxChannels_e txChannel,
                                                                   uint32_t* const                 eventBits)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t bfValue = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, eventBits, cleanup);

    /* Clear the memory before setting individual bits for different events */
    *eventBits = 0U;

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerError_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Average Power Error Bit");
        goto cleanup;
    }

    *eventBits |= (uint32_t) (bfValue & 0x01U);

    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerError_BfGet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Peak Power Error Bit");
        goto cleanup;
    }

    *eventBits |= (((uint32_t) (bfValue & 0x01U)) << 1U);

    recoveryAction = adrv904x_TxFuncs_SrdError_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD Error Bit");
        goto cleanup;
    }

    *eventBits |= (((uint32_t) (bfValue & 0x01U)) << 2U);

    recoveryAction = adrv904x_TxFuncs_PllJesdProtEvent_BfGet(device,
                                                             NULL,
                                                             txBaseAddr,
                                                             &bfValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Pll Unlock/Dfrm Irq Error");
        goto cleanup;
    }

    *eventBits |= (((uint32_t) (bfValue & 0x01U)) << 3U);

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionErrorClear(adi_adrv904x_Device_t * const   device,
                                                                     const adi_adrv904x_TxChannels_e txChannel,
                                                                     const uint32_t                  eventBits)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    if (ADRV904X_BF_EQUAL(eventBits, 0x01U))
    {
        recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerErrorClear_BfSet(device,
                                                                               NULL,
                                                                               txBaseAddr,
                                                                               ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set Tx Average Power Error Clear Bit");
            goto cleanup;
        }
    }

    if (ADRV904X_BF_EQUAL(eventBits, 0x02U))
    {
        recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerErrorClear_BfSet(device,
                                                                                NULL,
                                                                                txBaseAddr,
                                                                                ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set Tx Peak Power Error Clear Bit");
            goto cleanup;
        }
    }

    if (ADRV904X_BF_EQUAL(eventBits, 0x04U))
    {
        recoveryAction = adrv904x_TxFuncs_SrdErrorClear_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set SRD Error Clear Bit");
            goto cleanup;
        }
    }

    if (ADRV904X_BF_EQUAL(eventBits, 0x08U))
    {
        recoveryAction = adrv904x_TxFuncs_PllJesdProtClr_BfSet(device,
                                                               NULL,
                                                               txBaseAddr,
                                                               ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set Pll Unlock/Dfrm Irq Error Clear Bit");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxSlewRateDetectorCfgSet(adi_adrv904x_Device_t* const                device,
                                                                       adi_adrv904x_SlewRateDetectorCfgRt_t        txSlewRateDetectorCfg[],
                                                                       const uint32_t                              numSlewRateDetCfgs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txSlewRateDetectorCfg, cleanup);

    if (numSlewRateDetCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numSlewRateDetCfgs, "Invalid Number of Tx SRD Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numSlewRateDetCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_TxSlewRateDetectorCfgRangeCheck(device, &txSlewRateDetectorCfg[cfgIdx].srlCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx SRD Range Check Error Reported");
            goto cleanup;
        }
        
        /*Check that if requested Tx Channel valid*/
        if (((txSlewRateDetectorCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txSlewRateDetectorCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(txSlewRateDetectorCfg[cfgIdx].txChannelMask, chanSel))
            {
                recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdDinSel_BfSet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  (uint8_t)txSlewRateDetectorCfg[cfgIdx].srlCfg.inputSel);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD input selection");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdSlewOffset_BfSet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      txSlewRateDetectorCfg[cfgIdx].srlCfg.srdOffset);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD threshold");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdEn_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              txSlewRateDetectorCfg[cfgIdx].srlCfg.srdEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD enable");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdIrqEn_BfSet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 txSlewRateDetectorCfg[cfgIdx].srlCfg.srdIrqEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD Irq enable");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdArvWait_BfSet(device,
                                                                   NULL,
                                                                   txBaseAddr,
                                                                   txSlewRateDetectorCfg[cfgIdx].srlCfg.autoRecoveryWaitTime);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write auto recovery wait time");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdArvEn_BfSet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 txSlewRateDetectorCfg[cfgIdx].srlCfg.autoRecoveryEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write auto recovery enable");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdArvTxonQual_BfSet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       txSlewRateDetectorCfg[cfgIdx].srlCfg.autoRecoveryDisableTimerWhenTxOff);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write auto recovery disable timer when Tx off");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdStatEn_BfSet(device,
                                                                  NULL,
                                                                  txBaseAddr,
                                                                  txSlewRateDetectorCfg[cfgIdx].srlCfg.srdStatisticsEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write statistics enable");
                    goto cleanup;
                }

                /* Clear statistics before setting statistics mode */
                recoveryAction = adrv904x_TxFuncs_SrdStatMode_BfSet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    (2U | txSlewRateDetectorCfg[cfgIdx].srlCfg.srdStatisticsMode));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to clear statistics");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_SrdStatMode_BfSet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    txSlewRateDetectorCfg[cfgIdx].srlCfg.srdStatisticsMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write statistics mode");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxSlewRateDetectorCfgGet(adi_adrv904x_Device_t * const               device,
                                                                       adi_adrv904x_SlewRateDetectorCfgRt_t * const txSlewRateDetectorCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txSlewRateDetectorCfg, cleanup);

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)txSlewRateDetectorCfg->txChannelMask, &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txSlewRateDetectorCfg->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdDinSel_BfGet(device,
                                                      NULL,
                                                      txBaseAddr,
                                                      &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD input selection");
        goto cleanup;
    }
    txSlewRateDetectorCfg->srlCfg.inputSel = (adi_adrv904x_PaProtectionInputSel_e)tempByte;

    recoveryAction = adrv904x_TxFuncs_SrdSlewOffset_BfGet(device,
                                                          NULL,
                                                          txBaseAddr,
                                                          &txSlewRateDetectorCfg->srlCfg.srdOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD threshold");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdEn_BfGet(device,
                                                  NULL,
                                                  txBaseAddr,
                                                  &txSlewRateDetectorCfg->srlCfg.srdEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdIrqEn_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &txSlewRateDetectorCfg->srlCfg.srdIrqEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD Irq enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdArvWait_BfGet(device,
                                                       NULL,
                                                       txBaseAddr,
                                                       &txSlewRateDetectorCfg->srlCfg.autoRecoveryWaitTime);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read auto recovery wait time");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdArvEn_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &txSlewRateDetectorCfg->srlCfg.autoRecoveryEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read auto recovery enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdArvTxonQual_BfGet(device,
                                                           NULL,
                                                           txBaseAddr,
                                                           &txSlewRateDetectorCfg->srlCfg.autoRecoveryDisableTimerWhenTxOff);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read auto recovery disable timer when Tx off");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdStatEn_BfGet(device,
                                                      NULL,
                                                      txBaseAddr,
                                                      &txSlewRateDetectorCfg->srlCfg.srdStatisticsEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics enable");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdStatMode_BfGet(device,
                                                        NULL,
                                                        txBaseAddr,
                                                        &txSlewRateDetectorCfg->srlCfg.srdStatisticsMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics mode");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxSlewRateStatisticsRead(adi_adrv904x_Device_t * const device,
                                                                       const adi_adrv904x_TxChannels_e txChannel,
                                                                       const uint8_t clearStats,
                                                                       uint16_t * const statisticsReadback)
{
        static const uint8_t SINGLE_BIT_MAX = 1U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, statisticsReadback, cleanup);

    if (clearStats > SINGLE_BIT_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clearStats,
                               "clearStats value is invalid. Valid values 0-1");
        goto cleanup;
    }

    /* Clear the memory to make sure statistics readback will be correct */
    *statisticsReadback = 0;

    /* Retrieve the base address of selected channel. This function call will also range check the txChannelMask */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_SrdStat_BfGet(device,
                                                    NULL,
                                                    txBaseAddr,
                                                    statisticsReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics");
        goto cleanup;
    }

    if (clearStats == 1U)
    {
        recoveryAction = adrv904x_TxFuncs_SrdStatMode_BfGet(device,
                                                            NULL,
                                                            txBaseAddr,
                                                            &tempByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read statistics mode");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_TxFuncs_SrdStatMode_BfSet(device,
                                                            NULL,
                                                            txBaseAddr,
                                                            (tempByte | 2U));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to clear statistics");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_TxFuncs_SrdStatMode_BfSet(device,
                                                            NULL,
                                                            txBaseAddr,
                                                            tempByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to restore statistics mode");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampSampleHoldEnableSet(adi_adrv904x_Device_t* const device,
                                                                                  const uint32_t txChannelMask,
                                                                                  const uint8_t enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (enable > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enable,
                               "enable value is invalid. Valid values 0-1");
        goto cleanup;
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        if (ADRV904X_BF_EQUAL(txChannelMask, (uint32_t)(1U << chanIdx)))
        {
            chanSel = 1U << chanIdx;
            recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                goto cleanup;
            }


            recoveryAction = adrv904x_TxFuncs_AnaRampHoldSampleEn_BfSet(device,
                                                                        NULL,
                                                                        txBaseAddr,
                                                                        enable);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Analog sample hold and ramp down bit");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampSampleHoldEnableGet(adi_adrv904x_Device_t* const device,
                                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                                  uint8_t* const enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, enable, cleanup);

    /* Retrieve the base address for selected tx channel. This will also range check the txChannelMask parameter */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_TxFuncs_AnaRampHoldSampleEn_BfGet(device,
                                                                NULL,
                                                                txBaseAddr,
                                                                enable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Analog sample hold and ramp down bit");
        goto cleanup;
    }


cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampCfgSet(adi_adrv904x_Device_t* const device,
                                                                     adi_adrv904x_ProtectionRampCfgRt_t txProtectionRampCfg[],
                                                                     const uint32_t numProtectionRampCfgs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t jesdDfrmMask = 0U;
    uint8_t pllUnlockMask = 0U;
    uint8_t srdRdnEn = 0U;
    uint8_t avgPowerRdnEn = 0U;
    uint8_t peakPowerRdnEn = 0U;

    uint32_t allRampDownMasks = (uint32_t)ADI_ADRV904X_RDT_PEAK_PA |
                                (uint32_t)ADI_ADRV904X_RDT_AVG_PA |
                                (uint32_t)ADI_ADRV904X_RDT_SRD |
                                (uint32_t)ADI_ADRV904X_RDT_CLK_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV904X_RDT_RF0_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV904X_RDT_RF1_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV904X_RDT_SERDES_PLL_UNLOCK |
                                (uint32_t)ADI_ADRV904X_RDT_DFRM0_EVENT |
                                (uint32_t)ADI_ADRV904X_RDT_DFRM1_EVENT;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txProtectionRampCfg, cleanup);

    if (numProtectionRampCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numProtectionRampCfgs, "Invalid Number of Tx Protection Ramp Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numProtectionRampCfgs; ++cfgIdx)
    {
        if ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask & (~allRampDownMasks)) != 0U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask,
                "Ramp down mask selection is invalid. Valid values are any combinations of bitmasks given in adi_adrv904x_RampDownTrigger_e");
            goto cleanup;
        }

        if (txProtectionRampCfg[cfgIdx].protectionRampCfg.altEventClearReqd > 1U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                txProtectionRampCfg[cfgIdx].protectionRampCfg.altEventClearReqd,
                "Invalid altEventClearReqd selection. Valid values are 0-1");
            goto cleanup;
        }

        /*Check that if requested Tx Channel valid*/
        if(((txProtectionRampCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txProtectionRampCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(txProtectionRampCfg[cfgIdx].txChannelMask, (uint32_t)(chanSel)))
            {
                recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                jesdDfrmMask = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x7U) & 0x3U);
                /* Setting these bits will disable the rampdown for dfrm events, so toggle user selection */
                jesdDfrmMask = ~jesdDfrmMask;
                jesdDfrmMask &= 0x3U;
                recoveryAction = adrv904x_TxFuncs_JesdDfrmMask_BfSet(device,
                                                                     NULL,
                                                                     txBaseAddr,
                                                                     jesdDfrmMask);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Jesd Dfrm Mask");
                    goto cleanup;
                }

                pllUnlockMask = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x3U) & 0xFU);
                /* Setting these bits will disable the rampdown for pll unlock events, so toggle user selection */
                pllUnlockMask = ~pllUnlockMask;
                pllUnlockMask &= 0xFU;
                recoveryAction = adrv904x_TxFuncs_PllUnlockMask_BfSet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      pllUnlockMask);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PLL Unlock Mask");
                    goto cleanup;
                }

                srdRdnEn = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x2U) & 0x1U);
                recoveryAction = adrv904x_TxFuncs_SrdRdnEn_BfSet(device,
                                                                 NULL,
                                                                 txBaseAddr,
                                                                 srdRdnEn);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx SRD Ramp Down Mask");
                    goto cleanup;
                }

                avgPowerRdnEn = ((txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask >> 0x1U) & 0x1U);
                recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfSet(device,
                                                                                  NULL,
                                                                                  txBaseAddr,
                                                                                  avgPowerRdnEn);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Average Power Ramp Down Mask");
                    goto cleanup;
                }

                peakPowerRdnEn = (txProtectionRampCfg[cfgIdx].protectionRampCfg.rampDownMask & 0x1U);
                recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfSet(device,
                                                                                   NULL,
                                                                                   txBaseAddr,
                                                                                   peakPowerRdnEn);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Peak Power Ramp Down Mask");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_PllJesdProtClrReqd_BfSet(device,
                                                                           NULL,
                                                                           txBaseAddr,
                                                                           txProtectionRampCfg[cfgIdx].protectionRampCfg.altEventClearReqd);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Power Protect Event Clear Required");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxProtectionRampCfgGet(adi_adrv904x_Device_t* const                device,
                                                                     adi_adrv904x_ProtectionRampCfgRt_t* const   txProtectionRampCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;
    uint32_t tmpRampDownMask = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txProtectionRampCfg, cleanup);

    /* Retrieve the base address for selected tx channel. This will also range check the txChannelMask parameter */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txProtectionRampCfg->txChannelMask), &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txProtectionRampCfg->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_JesdDfrmMask_BfGet(device,
                                                         NULL,
                                                         txBaseAddr,
                                                         &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Jesd Dfrm Mask");
        goto cleanup;
    }
    /* bit = 1 means ramp down is disabled. So toggle the bits before returning it to user */
    tempByte = ~tempByte;
    tempByte &= 0x3U;
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0007U;

    recoveryAction = adrv904x_TxFuncs_PllUnlockMask_BfGet(device,
        NULL,
        txBaseAddr,
        &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PLL Unlock Mask");
        goto cleanup;
    }
    /* bit = 1 means ramp down is disabled. So toggle the bits before returning it to user */
    tempByte = ~tempByte;
    tempByte &= 0xFU;
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0003U;

    recoveryAction = adrv904x_TxFuncs_SrdRdnEn_BfGet(device,
                                                     NULL,
                                                     txBaseAddr,
                                                     &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx SRD Ramp Down Mask");
        goto cleanup;
    }
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0002U;

    recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerRdnEn_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Average Power Ramp Down Mask");
        goto cleanup;
    }
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0001U;
    
    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerRdnEn_BfGet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Peak Power Ramp Down Mask");
        goto cleanup;
    }
    tmpRampDownMask |=  (uint32_t)tempByte << 0x0000U;

    recoveryAction = adrv904x_TxFuncs_PllJesdProtClrReqd_BfGet(device,
                                                               NULL,
                                                               txBaseAddr,
                                                               &txProtectionRampCfg->protectionRampCfg.altEventClearReqd);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Power Protect Event Clear Required");
        goto cleanup;
    }

    txProtectionRampCfg->protectionRampCfg.rampDownMask = tmpRampDownMask;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPowerMonitorStatusGet(adi_adrv904x_Device_t * const device,
                                                                      const adi_adrv904x_TxChannels_e txChannel,
                                                                      adi_adrv904x_TxPowerMonitorStatus_t * const powerMonitorStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t tempByte = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, powerMonitorStatus, cleanup);

    /* Retrieve the base address for selected tx channel. This will also range check the txChannel parameter */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, txChannel, &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_TxFuncs_PaProtectionReadbackUpdate_BfSet(device,
                                                                       NULL,
                                                                       txBaseAddr,
                                                                       1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to set readback update bitfield");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_TxFuncs_PaProtectionPeakpowerEn_BfGet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Peak Power Enable");
        goto cleanup;
    }
    
    if (tempByte == 1U)
    {
        /* Read power levels only if peak power measurement block is enabled */
        recoveryAction = adrv904x_TxFuncs_PaProtectionPeakPower_BfGet(device,
                                                                      NULL,
                                                                      txBaseAddr,
                                                                      &powerMonitorStatus->peakPower);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read average power");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_TxFuncs_PaProtectionPeakErrorPower_BfGet(device,
                                                                           NULL,
                                                                           txBaseAddr,
                                                                           &powerMonitorStatus->peakErrorPower);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
    
    recoveryAction = adrv904x_TxFuncs_PaProtectionAvgpowerEn_BfGet(device,
                                                                   NULL,
                                                                   txBaseAddr,
                                                                   &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Avg Power Enable");
        goto cleanup;
    }

    if (tempByte == 1U)
    {
        recoveryAction = adrv904x_TxFuncs_PaProtectionAveragePower_BfGet(device,
                                                                         NULL,
                                                                         txBaseAddr,
                                                                         &powerMonitorStatus->avgPower);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read average power");
            goto cleanup;
        }

        recoveryAction = adrv904x_TxFuncs_PaProtectionAverageErrorPower_BfGet(device,
                                                                              NULL,
                                                                              txBaseAddr,
                                                                              &powerMonitorStatus->avgErrorPower);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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

    recoveryAction = adrv904x_TxFuncs_PaProtectionAprEn_BfGet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              &tempByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx PA Protect Average to Peak Ratio Enable");
        goto cleanup;
    }

    if (tempByte == 1U)
    {
        recoveryAction = adrv904x_TxFuncs_PaProtectionAveragePeakRatio_BfGet(device,
                                                                             NULL,
                                                                             txBaseAddr,
                                                                             &powerMonitorStatus->avgPeakRatio);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
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
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenCfgSet(adi_adrv904x_Device_t* const device,
                                                            const uint32_t chanMask,
                                                            adi_adrv904x_TxAttenCfg_t* const txAttenCfg)
{
        const uint8_t TX_ATTEN_MODE_SPI = 1U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanId = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txFuncsChanAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txAttenCfg, cleanup);
    
    if (chanMask > ADI_ADRV904X_TX_CHAN_MASK_MAX ||
        chanMask == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanMask, "Invalid channel mask.");
        goto cleanup;
    }
    
    if (txAttenCfg->txAttenStepSize >= ADI_ADRV904X_TXATTEN_INVALID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
         ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txAttenCfg->txAttenStepSize, "Tx atten step size is invalid.");
        goto cleanup;        
    }
    
    /* adi_adrv904x_TxAttenUpdateCfgSet already validates everything within txAttenCfg->updateCfg */
    recoveryAction = adi_adrv904x_TxAttenUpdateCfgSet(device, chanMask, &txAttenCfg->updateCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx atten config failed to set update config");
        goto cleanup;
    }                                                              
       
    for (chanId = 0U; chanId < ADI_ADRV904X_MAX_TXCHANNELS; chanId++)
    {
        chanSel = 1U << chanId;
        if ((chanMask & chanSel) == 0)
        {
            /* chanId not set in chanMask - skip this channel */
            continue;
        }

        /* Convert the chanId to the base address value required by the bitfield functions */
        recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txFuncsChanAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }
        
        /* Ensure mode is SPI mode */
        recoveryAction = adrv904x_TxFuncs_TxAttenMode_BfSet(device, NULL, txFuncsChanAddr, TX_ATTEN_MODE_SPI);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Atten Mode");
            goto cleanup;
        }
                                
        recoveryAction = adrv904x_TxFuncs_TxAttenConfig_BfSet(device, NULL, txFuncsChanAddr, txAttenCfg->txAttenStepSize);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Use Atten Step Size");
            goto cleanup;
        }
        device->devStateInfo.txAttenStepSize[chanId] = txAttenCfg->txAttenStepSize;        
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenCfgGet(adi_adrv904x_Device_t* const device,
                                                            const adi_adrv904x_TxChannels_e txChannel,
                                                            adi_adrv904x_TxAttenCfg_t* const txAttenCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txFuncsChanAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    uint8_t chanId  = 0U;
    uint8_t tmpByte = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txAttenCfg, cleanup);
    
    
    chanId = adrv904x_TxChannelsToId(txChannel);
    if (chanId > ADI_ADRV904X_TX_CHAN_ID_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;   
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid channel id.");
        goto cleanup;        
    }
    
    recoveryAction = adi_adrv904x_TxAttenUpdateCfgGet(device, txChannel, &txAttenCfg->updateCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get tx atten update config");
        goto cleanup;
    }                                                              
    
    /* Convert the chanId to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }
                        
    /* Fetch the const step mode config items */
    recoveryAction = adrv904x_TxFuncs_TxAttenConfig_BfGet(device, NULL, txFuncsChanAddr, &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Atten Step Size");
        goto cleanup;
    }

    txAttenCfg->txAttenStepSize = (adi_adrv904x_TxAttenStepSize_e)tmpByte;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDecimatedPowerCfgSet(adi_adrv904x_Device_t * const device,
                                                                     adi_adrv904x_TxDecimatedPowerCfg_t txDecPowerCfg[],
                                                                     const uint32_t numOfDecPowerCfgs)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx = 0U;
    uint32_t chanIdx = 0U;     
    uint32_t chanSel = 0U;

    adrv904x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    adrv904x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv904x_BfTxDigChanAddr_e)0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txDecPowerCfg, cleanup);

    if (numOfDecPowerCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numOfDecPowerCfgs, "Invalid Number of Dec Power Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        recoveryAction = adrv904x_TxDecPowerCfgRangeCheck(device, &txDecPowerCfg[cfgIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Tx Dec power range check Error Reported");
            goto cleanup;
        }
    }

    /* Write out the configurations */
    for (cfgIdx = 0U; cfgIdx < numOfDecPowerCfgs; ++cfgIdx)
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; chanIdx++)
        {
            chanSel = 1U << chanIdx;
            if ((txDecPowerCfg[cfgIdx].txChannelMask & chanSel) > 0)
            {
                /* Convert the chanId to the base address value required by the bitfield functions */
                recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txFuncsChanBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxFuncs_TxPowerInputSelect_BfSet(device,
                                                                           NULL,
                                                                           txFuncsChanBaseAddr,
                                                                           txDecPowerCfg[cfgIdx].powerInputSelect);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement input selection");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_TxPowerMeasurementDuration_BfSet(device,
                                                                                   NULL,
                                                                                   txFuncsChanBaseAddr,
                                                                                   txDecPowerCfg[cfgIdx].powerMeasurementDuration);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement duration");
                    goto cleanup;
                }
                
                recoveryAction = adrv904x_TxFuncs_TxPeakToPowerMode_BfSet(device,
                                                                          NULL,
                                                                          txFuncsChanBaseAddr,
                                                                          txDecPowerCfg[cfgIdx].peakToPowerMode);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting peak to power mode");
                    goto cleanup;
                }

                recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txDigChanBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine SPI address");
                    goto cleanup;
                }


                {
                    /* For ADRV904X, calculate the clock divide rate to match the powerInputSelect */
                    uint8_t  clkDivRatio = 0;
                    uint32_t outputRate;
                    uint32_t clkDiv = 0;

                    switch (txDecPowerCfg[cfgIdx].powerInputSelect)
                    {
                        case 0: /* Band DUC output */
                            outputRate = device->initExtract.dfeCduc.cducOutputRate_kHz[chanIdx];
                            break;
	                    case 1: /* DPD output is the same as PFIR */ 
                        case 2: /* QEC output is the same as PFIR */
                        case 3: /* PFIR output */
                        default:
                            outputRate = device->initExtract.tx.txChannelCfg[chanIdx].pfirRate_kHz;
                            break;
                    }

                    if (outputRate == 0)
                    {
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "outputRate is 0");
                        goto cleanup;
                    }

                    clkDiv = device->initExtract.clocks.hsDigClk_kHz / outputRate;

                    switch (clkDiv)
                    {
                        case   1: clkDivRatio = 0u; break;
                        case   2: clkDivRatio = 1u; break;
                        case   4: clkDivRatio = 2u; break;
                        case   8: clkDivRatio = 3u; break;
                        case  16: clkDivRatio = 4u; break;
                        case  32: clkDivRatio = 5u; break;
                        case  64: clkDivRatio = 6u; break;
                        case 128: clkDivRatio = 7u; break;
                        default:
                            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "clocks not divisable by power of 2");
                            goto cleanup;
                            break;
                    }

                    recoveryAction = adrv904x_TxDig_PowerMeasClkDivideRatio_BfSet(device,
                                                                                  NULL,
                                                                                  txDigChanBaseAddr,
                                                                                  clkDivRatio);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Error while setting power meas clock div ratio");
                        goto cleanup;
                    }

                    /* Enable/disable the clock */
                    recoveryAction = adrv904x_TxDig_PowerMeasClkEnable_BfSet(device,
                                                                             NULL,
                                                                             txDigChanBaseAddr,
                                                                             txDecPowerCfg[cfgIdx].measurementEnable);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Error while enabling power meas clock");
                        goto cleanup;
                    }
                }

                if (txDecPowerCfg[cfgIdx].measurementEnable == 1U)
                {
                    recoveryAction = adrv904x_TxDig_StreamprocPfirClkEnable_BfSet(device,
                                                                                  NULL,
                                                                                  txDigChanBaseAddr,
                                                                                  txDecPowerCfg[cfgIdx].measurementEnable);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting tx stream proc pfir clk enable bit");
                        goto cleanup;
                    }
                }

                recoveryAction = adrv904x_TxFuncs_TxPowerMeasurementEnable_BfSet(device,
                                                                                 NULL,
                                                                                 txFuncsChanBaseAddr,
                                                                                 txDecPowerCfg[cfgIdx].measurementEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting power measurement enable bit");
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDecimatedPowerCfgGet(adi_adrv904x_Device_t * const device,
                                                                     const adi_adrv904x_TxChannels_e txChannel,
                                                                     adi_adrv904x_TxDecimatedPowerCfg_t * const txDecPowerCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;      
    adrv904x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txDecPowerCfg, cleanup);
   
    if ((txChannel != ADI_ADRV904X_TX0) &&
        (txChannel != ADI_ADRV904X_TX1) &&
        (txChannel != ADI_ADRV904X_TX2) &&
        (txChannel != ADI_ADRV904X_TX3) &&
        (txChannel != ADI_ADRV904X_TX4) &&
        (txChannel != ADI_ADRV904X_TX5) &&
        (txChannel != ADI_ADRV904X_TX6) &&
        (txChannel != ADI_ADRV904X_TX7))
    {
        /* Invalid tx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx channel selection");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_TxPowerInputSelect_BfGet(device,
                                                               NULL,
                                                               txFuncsChanBaseAddr,
                                                               &txDecPowerCfg->powerInputSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement input selection");
        goto cleanup;
    }
                
    recoveryAction = adrv904x_TxFuncs_TxPowerMeasurementDuration_BfGet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       &txDecPowerCfg->powerMeasurementDuration);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement duration");
        goto cleanup;
    }
     
    recoveryAction = adrv904x_TxFuncs_TxPeakToPowerMode_BfGet(device,
                                                              NULL,
                                                              txFuncsChanBaseAddr,
                                                              &txDecPowerCfg->peakToPowerMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peak to power mode bit");
        goto cleanup;
    }
       
    recoveryAction = adrv904x_TxFuncs_TxPowerMeasurementEnable_BfGet(device,
                                                                     NULL,
                                                                     txFuncsChanBaseAddr,
                                                                     &txDecPowerCfg->measurementEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading power measurement enable bit");
        goto cleanup;
    }

    txDecPowerCfg->txChannelMask = (uint32_t)txChannel;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDecimatedPowerGet(adi_adrv904x_Device_t * const device,
                                                                  const adi_adrv904x_TxChannels_e txChannel,
                                                                  uint8_t * const powerReadBack,
                                                                  uint8_t * const powerPeakReadBack)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txFuncsChanBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, powerReadBack, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, powerPeakReadBack, cleanup);

    if ((txChannel != ADI_ADRV904X_TX0) &&
        (txChannel != ADI_ADRV904X_TX1) &&
        (txChannel != ADI_ADRV904X_TX2) &&
        (txChannel != ADI_ADRV904X_TX3) &&
        (txChannel != ADI_ADRV904X_TX4) &&
        (txChannel != ADI_ADRV904X_TX5) &&
        (txChannel != ADI_ADRV904X_TX6) &&
        (txChannel != ADI_ADRV904X_TX7))
    {
        /* Invalid Tx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid channel selection");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, txChannel, &txFuncsChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxFuncs_TxPowerMeasurementReadback_BfSet(device,
                                                                       NULL,
                                                                       txFuncsChanBaseAddr,
                                                                       1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting decimated power readback latch bit");
        goto cleanup;
    }
  
    recoveryAction = adrv904x_TxFuncs_TxPower_BfGet(device,
                                                    NULL,
                                                    txFuncsChanBaseAddr,
                                                    powerReadBack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading decimated power for selected Tx channel");
        goto cleanup;
    }


    recoveryAction = adrv904x_TxFuncs_TxPowerLargestPeak_BfGet(device,
                                                               NULL,
                                                               txFuncsChanBaseAddr,
                                                               powerPeakReadBack);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading decimated peak power for selected Tx channel");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenPhaseSet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_TxAttenPhaseCfg_t * const txAttenPhaseCfg)
{
        adi_adrv904x_ErrAction_e                recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_SetTxAttenPhase_t       txAttenPhaseInfo;
    adrv904x_CpuCmd_SetTxAttenPhaseResp_t   cmdRsp;
    adrv904x_CpuCmdStatus_e                 cmdStatus           = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                 cpuErrorCode        = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                i                   = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txAttenPhaseCfg, cleanup);
    ADI_LIBRARY_MEMSET(&txAttenPhaseInfo, 0, sizeof(txAttenPhaseInfo));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(cmdRsp));

    if (txAttenPhaseCfg->chanSelect == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txAttenPhaseCfg->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txAttenPhaseInfo.chanSelect = (uint8_t) txAttenPhaseCfg->chanSelect;
    for (i = 0U; i < ADI_ADRV904X_TX_ATTEN_PHASE_SIZE; i++)
    {
        txAttenPhaseInfo.txAttenPhase[i] = ADRV904X_HTOCS(txAttenPhaseCfg->txAttenPhase[i]);
    }

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            ADI_ADRV904X_CPU_TYPE_0,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_SET_TX_ATTEN_PHASE,
                                            (void*)&txAttenPhaseInfo,
                                            sizeof(txAttenPhaseInfo),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxAttenPhaseGet(adi_adrv904x_Device_t* const device,
                                                              adi_adrv904x_TxAttenPhaseCfg_t* const txRbConfig)
{
        adi_adrv904x_ErrAction_e                recoveryAction              = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetTxAttenPhase_t       txAttenPhaseCfgRbCmd;
    adrv904x_CpuCmd_GetTxAttenPhaseResp_t   txAttenPhaseCfgGetcmdRsp;
    adrv904x_CpuCmdStatus_e                 cmdStatus                   = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e                 cpuErrorCode                = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t                                i                           = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txRbConfig, cleanup);
    ADI_LIBRARY_MEMSET(&txAttenPhaseCfgRbCmd, 0, sizeof(txAttenPhaseCfgRbCmd));
    ADI_LIBRARY_MEMSET(&txAttenPhaseCfgGetcmdRsp, 0, sizeof(txAttenPhaseCfgGetcmdRsp));

    /* Test if channel mask is 0 or has more than one channel set */
    if (txRbConfig->chanSelect == (uint8_t) ADI_ADRV904X_TXOFF ||
        txRbConfig->chanSelect > (uint8_t) ADI_ADRV904X_TX7    ||
        (((txRbConfig->chanSelect - 1) & txRbConfig->chanSelect) != 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txRbConfig->chanSelect, "Invalid chanSelect provided.");
        goto cleanup;
    }

    /* Prepare the command payload */
    txAttenPhaseCfgRbCmd.chanSelect = (uint8_t) txRbConfig->chanSelect;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            ADI_ADRV904X_CPU_TYPE_0,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_GET_TX_ATTEN_PHASE,
                                            (void*)&txAttenPhaseCfgRbCmd,
                                            sizeof(txAttenPhaseCfgRbCmd),
                                            (void*)&txAttenPhaseCfgGetcmdRsp,
                                            sizeof(txAttenPhaseCfgGetcmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(txAttenPhaseCfgGetcmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    for (i = 0U; i < ADI_ADRV904X_TX_ATTEN_PHASE_SIZE; i++)
    {
        txRbConfig->txAttenPhase[i] = ADRV904X_CTOHS(txAttenPhaseCfgGetcmdRsp.txAttenPhase[i]);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxCfgSet(adi_adrv904x_Device_t* const device,
                                                        const adi_adrv904x_DtxCfg_t  dtxCfg[],
                                                        const uint32_t               numDtxCfgs)
{
        static const uint32_t txScratchPad6[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH6 
                                            };
    static const uint32_t TX_SCRATCH_MASK      = 0xFFFF0103U;
    static const uint32_t DTX_MODE_MASK        = 0x00000003U;
    static const uint32_t DTX_MODE_SHIFT       = 0U;
    static const uint32_t DTX_ZERO_COUNT_MASK  = 0xFFFF0000U;
    static const uint32_t DTX_ZERO_COUNT_SHIFT = 16U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t cfgIdx  = 0U;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint32_t txScratch6Value = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dtxCfg, cleanup);

    if (numDtxCfgs == 0U)
    {
        /* no valid configs */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numDtxCfgs, "Invalid Number of Tx DTX Configurations");
        goto cleanup;
    }

    /* Loop through the number of configurations and perform range checks */
    for (cfgIdx = 0U; cfgIdx < numDtxCfgs; ++cfgIdx)
    {
        /*Check that if requested Tx Channel valid*/
        if (((dtxCfg[cfgIdx].txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (dtxCfg[cfgIdx].txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dtxCfg[cfgIdx].txChannelMask,
                                   "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
            goto cleanup;
        }
        
        /*Check that if requested DTX Mode is valid*/
        if (dtxCfg[cfgIdx].dtxMode > ADI_ADRV904X_DTXMODE_PIN)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
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
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(dtxCfg[cfgIdx].txChannelMask, chanSel))
            {
                /* Update TX scratch register with DTX config for specific channel */
                recoveryAction = adi_adrv904x_Register32Read(device, NULL, txScratchPad6[chanIdx], &txScratch6Value, TX_SCRATCH_MASK);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 6");
                    goto cleanup;
                }

                txScratch6Value &= (~DTX_MODE_MASK);
                txScratch6Value |= ((uint32_t)(dtxCfg[cfgIdx].dtxMode) << DTX_MODE_SHIFT);
                txScratch6Value &= (~DTX_ZERO_COUNT_MASK);
                txScratch6Value |= ((uint32_t)(dtxCfg[cfgIdx].dtxZeroCounter) << DTX_ZERO_COUNT_SHIFT);

                recoveryAction = adi_adrv904x_Register32Write(device, NULL, txScratchPad6[chanIdx], txScratch6Value, TX_SCRATCH_MASK);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 6");
                    goto cleanup;
                }
            }
        }

        /* For each cfgIdx, trigger stream to update DTX mode config registers while writing to DTX ClearStatus bits */
        recoveryAction = adrv904x_TxStreamTrigger(device, (uint8_t)dtxCfg[cfgIdx].txChannelMask, (uint8_t)ADRV904X_STREAM_TX_WRITE_DTX_MODE_CONFIG_CLRSTATUS);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dtxCfg[cfgIdx].txChannelMask,
                                   "Error while triggering TX stream");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxCfgGet(adi_adrv904x_Device_t* const    device,
                                                        const adi_adrv904x_TxChannels_e txChannel,
                                                        adi_adrv904x_DtxCfg_t* const    dtxCfg)
{
        static const uint32_t txScratchPad6[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH6 
                                            };
    static const uint32_t DTX_MODE_MASK        = 0x00000003U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv904x_BfTxDigChanAddr_e)0U;
    uint32_t txScratch6Value = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dtxCfg, cleanup);

    if ((txChannel != ADI_ADRV904X_TX0) &&
        (txChannel != ADI_ADRV904X_TX1) &&
        (txChannel != ADI_ADRV904X_TX2) &&
        (txChannel != ADI_ADRV904X_TX3) &&
        (txChannel != ADI_ADRV904X_TX4) &&
        (txChannel != ADI_ADRV904X_TX5) &&
        (txChannel != ADI_ADRV904X_TX6) &&
        (txChannel != ADI_ADRV904X_TX7))
    {
        /* Invalid tx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx channel selection");
        goto cleanup;
    }

    for (uint32_t chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        if ((1U << chanIdx) == txChannel)
        {
            /* Read DTX configuration stored in scratch register to retrieve DTX Mode */
            recoveryAction = adi_adrv904x_Register32Read(device, NULL, txScratchPad6[chanIdx], &txScratch6Value, DTX_MODE_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 6");
                goto cleanup;
            }

            dtxCfg->dtxMode = (adi_adrv904x_DtxMode_e)(txScratch6Value);
        }
    }

    /* Retrieve Zero count via BfGet */
    recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, txChannel, &txDigChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxDig_DtxZeroCounter_BfGet(device,
                                                         NULL,
                                                         txDigChanBaseAddr,
                                                         &dtxCfg->dtxZeroCounter);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting dtx zero counter ");
        goto cleanup;
    }

    dtxCfg->txChannelMask = (uint32_t)txChannel;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxGpioCfgSet(adi_adrv904x_Device_t* const     device,
                                                            const adi_adrv904x_DtxGpioCfg_t* dtxGpioCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;

    uint8_t profileCfgOk = 0U;
    uint8_t dtxPinChannelMasks[ADI_ADRV904X_DTX_INPUT_MAX] = { 0U };
    adi_adrv904x_GpioPinSel_e gpioReadback = ADI_ADRV904X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dtxGpioCfg, cleanup);

    /* Check all Dtx GPIO if valid */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {            
        /* Check if pin is valid */
        if (dtxGpioCfg->dtxGpioTxSel[chanIdx] > ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid DTX GPIO pin selected. To disable, please select ADI_ADRV904X_GPIO_INVALID");
            goto cleanup;
        }

        /* Skip ADI_ADRV904X_GPIO_INVALID */
        if (dtxGpioCfg->dtxGpioTxSel[chanIdx] == ADI_ADRV904X_GPIO_INVALID)
        {
            continue;
        }


        /* Confirm Dtx GPIO has been configured as Dtx input in profile and build channel mask per DTX input pin */
        profileCfgOk = 0U;
        for (int dtxIdx = 0U; dtxIdx < ADI_ADRV904X_DTX_INPUT_MAX; dtxIdx++)
        {
            if (dtxGpioCfg->dtxGpioTxSel[chanIdx] == device->devStateInfo.dtxInputMapping.dtxInput[dtxIdx])
            {
                dtxPinChannelMasks[dtxIdx] |= (1U << chanIdx);
                profileCfgOk = 1U;
            }
        }
        if (profileCfgOk == 0U)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "DTX GPIO pin selected has not been configured as DTX input in device profile");
            goto cleanup;
        }
    }

    /* First release all GPIOs used for tx internal signals */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        recoveryAction = adrv904x_GpioSignalFind(device,
                                                 &gpioReadback,
                                                 ADI_ADRV904X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                 (adi_adrv904x_Channels_e)(chanSel));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gpio for selected signal");
            goto cleanup;
        }

        if (gpioReadback != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease(device, 
                                                        gpioReadback,
                                                        ADI_ADRV904X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                        (adi_adrv904x_Channels_e)(1U <<chanIdx));
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to release gpio");
                goto cleanup;
            }
        }
    }

    /* Then assign new GPIOs for Tx internal signals */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        if (dtxGpioCfg->dtxGpioTxSel[chanIdx] != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalSet(device,
                                                    dtxGpioCfg->dtxGpioTxSel[chanIdx],
                                                    ADI_ADRV904X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                    (adi_adrv904x_Channels_e)(1U << chanIdx));

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Failure to assign selected signal to GPIO for chanIdx");
                goto cleanup;
            }
        }
    }


    /* Write DTX input channel masks to core stream scratch registers */
    recoveryAction = adi_adrv904x_RegistersByteWrite(device, 
                                                     NULL, 
                                                     (uint32_t)ADRV904X_ADDR_CORE_STREAM_SCRATCH481, 
                                                     dtxPinChannelMasks, 
                                                     ADI_ADRV904X_DTX_INPUT_MAX);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Failure to write channel masks to scratch registers");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxGpioCfgGet(adi_adrv904x_Device_t* const     device,
                                                            adi_adrv904x_DtxGpioCfg_t* const dtxGpioCfg) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    adi_adrv904x_GpioPinSel_e gpioReadback = ADI_ADRV904X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dtxGpioCfg, cleanup);

    /* Then assign new GPIOs for Tx internal signals */
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        recoveryAction = adrv904x_GpioSignalFind(device,
                                                 &gpioReadback,
                                                 ADI_ADRV904X_GPIO_SIGNAL_DTX_FORCE_PIN,
                                                 (adi_adrv904x_Channels_e)chanSel);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read gpio for selected signal");
            goto cleanup;
        }

        dtxGpioCfg->dtxGpioTxSel[chanIdx] = gpioReadback;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxForceSet(adi_adrv904x_Device_t* const   device,
                                                          const uint32_t txChannelMask,
                                                          const uint8_t dtxForce)
{
        static const uint32_t txScratchPad6[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH6,
                                              (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH6 
                                            };
    static const uint32_t TX_SCRATCH_MASK = 0xFFFF0103U;
    static const uint32_t DTX_FORCE_MASK  = 0x00000100U;
    static const uint32_t DTX_FORCE_SHIFT = 8U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM; 
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    uint32_t txScratch6Value = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
        
    /*Check that if requested DTX Force is valid*/
    if (dtxForce > ADI_ENABLE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               dtxForce,
                               "Invalid DTX Force bit. Valid values are 0/1");
        goto cleanup;
    }


    /* Write out the configurations to appropriate bitfields */    
    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(txChannelMask, chanSel))
        {
            /* Update TX scratch register with DTX config for specific channel */
            recoveryAction = adi_adrv904x_Register32Read(device, NULL, txScratchPad6[chanIdx], &txScratch6Value, TX_SCRATCH_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 6");
                goto cleanup;
            }

            txScratch6Value = (txScratch6Value & (~DTX_FORCE_MASK)) | ((uint32_t)(dtxForce) << DTX_FORCE_SHIFT);

            recoveryAction = adi_adrv904x_Register32Write(device, NULL, txScratchPad6[chanIdx], txScratch6Value, TX_SCRATCH_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 6");
                goto cleanup;
            }
        }
    }

    recoveryAction = adrv904x_TxStreamTrigger(device, (uint8_t)txChannelMask, (uint8_t)ADRV904X_STREAM_TX_WRITE_DTX_MODE_CONFIG);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while triggering TX stream");
        goto cleanup;
    }


    /* Trigger stream to execute manual DTX activate/deactivate */
    if (dtxForce == ADI_ENABLE)
    {
        recoveryAction = adrv904x_TxStreamTrigger(device, (uint8_t)txChannelMask, (uint8_t)ADRV904X_STREAM_TX_WRITE_DTX_MANUAL_ACTIVATE);
    }
    else
    {
        recoveryAction = adrv904x_TxStreamTrigger(device, (uint8_t)txChannelMask, (uint8_t)ADRV904X_STREAM_TX_WRITE_DTX_MANUAL_DEACTIVATE);
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while triggering TX stream");
        goto cleanup;
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DtxStatusGet(adi_adrv904x_Device_t* const device,
                                                           const adi_adrv904x_TxChannels_e txChannel,
                                                           uint8_t * const dtxStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDigChanAddr_e txDigChanBaseAddr = (adrv904x_BfTxDigChanAddr_e)0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dtxStatus, cleanup);

    if ((txChannel != ADI_ADRV904X_TX0) &&
        (txChannel != ADI_ADRV904X_TX1) &&
        (txChannel != ADI_ADRV904X_TX2) &&
        (txChannel != ADI_ADRV904X_TX3) &&
        (txChannel != ADI_ADRV904X_TX4) &&
        (txChannel != ADI_ADRV904X_TX5) &&
        (txChannel != ADI_ADRV904X_TX6) &&
        (txChannel != ADI_ADRV904X_TX7))
    {
        /* Invalid tx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx channel selection");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, txChannel, &txDigChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxDig_DtxStatus_BfGet(device,
                                                    NULL,
                                                    txDigChanBaseAddr,
                                                    dtxStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting dtx mode ");
        goto cleanup;

    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiConfigSet(adi_adrv904x_Device_t* const                 device,
                                              const uint32_t channelMask,
                                              const adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t *pPwrMtrTssiCfg,
                                              const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter,
                                              const adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e source)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrTssiConfigSet_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrTssiCfg, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate channelMask, meter, source */
    recoveryAction = adrv904x_ValidateMask((uint8_t)channelMask, 0u); /* allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "Invalid channel mask");
        goto cleanup;
    }

    /* Validate pPwrMtrTssiCfg parameters: */
    if (meter >= ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_TSSI_METERS)
    {
        /* 0: TSSI_1, 1: TSSI_2, 2: ORx, 3~10: CDUC TSSI 1~8 */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            meter,
            "TSSI meter is invalid. Valid values defined by adi_adrv904x_DfeSvcDfePwrMtrTssi_e");

        goto cleanup;
    }

    if (meter == ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_TX_1) /* TSSI_1 (See TxSliceUserGuide 9.4) */
    {
        if (source > ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_PRE_DPD_HB2_OUT_AFTER_HC)
        {
            /* This is a 2 bit register field:
             * Pre-DPD HB1 (0) --> 2'b00
             * Pre-DPD HB2 (1) --> 2'b01 NOTE: output after hard clipper
             */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                pPwrMtrTssiCfg->sourceSelect,
                "TSSI sourceSelect is invalid. Valid values are 0,1 (adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e)");

            goto cleanup;
        }
    }
    if (meter == ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_TX_2) /* TSSI_2 (See TxSliceUserGuide 9.4) */
    {
        if ((source < ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_CFR_IN) ||
            (source > ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_POST_DPD_HB1_OUT))
        {
            /* This is a 2 bit register field.
             * CFR input   (2) --> 2'b01
             * CFR output  (3) --> 2'b01
             * DPD HB1 out (4) --> 2'b10
             */
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                pPwrMtrTssiCfg->sourceSelect,
                "TSSI sourceSelect is invalid. Valid values are 2,3,4 (adi_adrv904x_DfeSvcDfePwrMtrTssiSrc_e)");

            goto cleanup;
        }
    }

    if (pPwrMtrTssiCfg->opMode > 2u)
    {
        /* 2'b00 - the duration of measurement is non-power of 2. In this case,
         *         two or more of the tssi_meas_duration0/1/2/3/_duc* are used.
         *         The power from each duration is weighted by tx_tssi_mul_factor0/1/2/3_duc*.
         *         If a given multiplication factor is 0, that duration is not included in the
         *         calculation. It is required that the lower most multiplication factors can be 0.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->opMode,
            "TSSI default mode is invalid. Valid values are 0 and 1.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->measMode > 2u)
    {
        /* 2'b00 - Power measurement starts immediately when tx_tssi_duc*_enable bit-field is set.
         * 2'b01 - Power measurement starts at the next TxOn. There is a delay after the TxOn configured
         *           by tx_tssi_start_delay that has to expire before measurement starts. 
         *           Once measurement is complete, the block waits for next TxOn.
         * 2'b10 - Power measurement starts at the next TxOn. There is a delay after the TxOn configured
         *           by tx_tssi_start_delay that has to expire before measurement starts.
         *           Once measurement is complete, the block waits for a period specified by
         * tx_tssi_wait_delay and then starts the next measurement.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->measMode,
            "TSSI measurement mode is invalid. Valid values are 0 - 2.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->radioSeqPinModeEnable > 1u)
    {
        /* When set, this bitfield allows the radio sequencer to restart the power measurement. */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->radioSeqPinModeEnable,
            "TSSI radio seq pin mode enable invalid. Valid values are 0 and 1.");

        goto cleanup;
    }

    if ((pPwrMtrTssiCfg->startDelay > 0x1FFFU) || /* 13 bits */
        ((pPwrMtrTssiCfg->measMode != 0u) && (pPwrMtrTssiCfg->startDelay == 0U))) /* 0 only allowed in FDD mode */
    {
        /* This delay is measured from the rising edge of TxOn signal and is used to delay start of
         * measurement in modes described by pPwrMtrTssiCfg->measMode. */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->startDelay,
            "TSSI startDelay is invalid. Valid values are 0 or 1 - 0x1FFF. FDD allows 0.");

        goto cleanup;
    }

    if ((pPwrMtrTssiCfg->waitDelay > 0xFFFFFFU) || /* 24 bits */
        ((pPwrMtrTssiCfg->measMode != 0u) && (pPwrMtrTssiCfg->waitDelay == 0U))) /* 0 only allowed in FDD mode */
    {
        /* This delay is measured at the end of measurement. 
         * It delays the start of next measurement in the meas mode = 0 and 2.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->waitDelay,
            "TSSI waitDelay is invalid. Valid values are 0 or 1 - 0xFFFFFF. FDD allows 0.");

        goto cleanup;
    }

    if ((pPwrMtrTssiCfg->contDelay > 0x3FFFU) || /* 14 bits */
        ((pPwrMtrTssiCfg->measMode != 0u) &&  (pPwrMtrTssiCfg->contDelay == 0U)))/* 0 only allowed in FDD mode */
    {
        /* This delay is measured from the rising edge of TxOn signal and is used to delay start of
         * measurement in modes described by pPwrMtrTssiCfg->measMode. This delay comes in to play
         * only in certain mode when a measurement is not completed during TxOn.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->contDelay,
            "TSSI contDelay is invalid. Valid values are 0 or 1 - 0x3FFF. FDD allows 0.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->fddPinMode > 1U) /* 1 bit */
    {
        /* Enable pin mode for FDD operation.
         * When enabled, a GPIO pin can be used to trigger the start of power measurement.
         * When the GPIO pin goes high, the start delay counter is enabled.
         * Power measurement starts at the expiry of that counter.
         * See GPIO documentation on how to configure a specific pin as a trigger.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->fddPinMode,
            "TSSI fddPinMode is invalid. Valid values are 0 and 1.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->duration0 > 28U)
    {
        /* Measurement duration for TSSI power.
         * Number of samples = 2 ^ (tx_tss_measurement_duration_duc* +4)
         * This is the only duration that is used when meas mode = 1.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->duration0,
            "TSSI duration0 is invalid. Valid values are 0 - 28.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->duration1 > 28U)
    {
        /* Measurement duration for TSSI power.
         * Number of samples = 2 ^ (tx_tss_measurement_duration_duc* +4)
         * This is the only duration that is used when meas mode = 0.
         * It is required to configure the multiplier factor correctly to use this duration.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->duration1,
            "TSSI duration1 is invalid. Valid values are 0 - 28.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->duration2 > 28U)
    {
        /* Measurement duration for TSSI power.
         * Number of samples = 2 ^ (tx_tss_measurement_duration_duc* +4)
         * This is the only duration that is used when meas mode = 0.
         * It is required to configure the multiplier factor correctly to use this duration.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->duration2,
            "TSSI duration2 is invalid. Valid values are 0 - 28.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->duration3 > 28U)
    {
        /* Measurement duration for TSSI power.
         * Number of samples = 2 ^ (tx_tss_measurement_duration_duc* +4)
         * This is the only duration that is used when meas mode = 0.
         * It is required to configure the multiplier factor correctly to use this duration.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->duration3,
            "TSSI duration3 is invalid. Valid values are 0 - 28.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->mulFactor0 > 0x3FFU)
    {
        /* This bit-field is used when tx_tssi_default_mode = 0.
         * This is a 0.8 number. This is the weight of power calculated during duration 0.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->mulFactor0,
            "TSSI mulFactor0 is invalid. Valid values are 0 - 0x3FF.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->mulFactor1 > 0x3FFU)
    {
        /* This bit-field is used when tx_tssi_default_mode = 0.
         * This is a 0.8 number. This is the weight of power calculated during duration 1.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->mulFactor1,
            "TSSI mulFactor1 is invalid. Valid values are 0 - 0x3FF.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->mulFactor2 > 0x3FFU)
    {
        /* This bit-field is used when tx_tssi_default_mode = 0.
         * This is a 0.8 number. This is the weight of power calculated during duration 2.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->mulFactor2,
            "TSSI mulFactor2 is invalid. Valid values are 0 - 0x3FF.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->mulFactor3 > 0x3FFU)
    {
        /* This bit-field is used when tx_tssi_default_mode = 0.
         * This is a 0.8 number. This is the weight of power calculated during duration 3.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->mulFactor3,
            "TSSI mulFactor3 is invalid. Valid values are 0 - 0x3FF.");

        goto cleanup;
    }

    if (pPwrMtrTssiCfg->captureTimeUsec > 500000U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrTssiCfg->captureTimeUsec,
            "TSSI captureTimeUsec is invalid. Valid values are < 500000 (0.5 sec)");

        goto cleanup;
    }

    cmdStruct.chanMask = channelMask;
    cmdStruct.meter    = meter;
    cmdStruct.source   = source;

    cmdStruct.pwrMtrTssiCfg.resetAtGainChange     = (uint8_t)0; /* deprecated and unused so always set to 0 */
    cmdStruct.pwrMtrTssiCfg.radioSeqPinModeEnable = (uint8_t)pPwrMtrTssiCfg->radioSeqPinModeEnable;
    cmdStruct.pwrMtrTssiCfg.opMode                = (uint8_t)pPwrMtrTssiCfg->opMode;
    cmdStruct.pwrMtrTssiCfg.measMode              = (uint8_t)pPwrMtrTssiCfg->measMode;
    cmdStruct.pwrMtrTssiCfg.startDelay            = (uint16_t)ADRV904X_HTOCS(pPwrMtrTssiCfg->startDelay);
    cmdStruct.pwrMtrTssiCfg.contDelay             = (uint16_t)ADRV904X_HTOCS(pPwrMtrTssiCfg->contDelay);
    cmdStruct.pwrMtrTssiCfg.waitDelay             = (uint32_t)ADRV904X_HTOCL(pPwrMtrTssiCfg->waitDelay);
    cmdStruct.pwrMtrTssiCfg.fddPinMode            = (uint8_t)pPwrMtrTssiCfg->fddPinMode;
    cmdStruct.pwrMtrTssiCfg.duration0             = (uint8_t)pPwrMtrTssiCfg->duration0;
    cmdStruct.pwrMtrTssiCfg.duration1             = (uint8_t)pPwrMtrTssiCfg->duration1;
    cmdStruct.pwrMtrTssiCfg.duration2             = (uint8_t)pPwrMtrTssiCfg->duration2;
    cmdStruct.pwrMtrTssiCfg.duration3             = (uint8_t)pPwrMtrTssiCfg->duration3;
    cmdStruct.pwrMtrTssiCfg.mulFactor0            = (uint16_t)ADRV904X_HTOCS(pPwrMtrTssiCfg->mulFactor0);
    cmdStruct.pwrMtrTssiCfg.mulFactor1            = (uint16_t)ADRV904X_HTOCS(pPwrMtrTssiCfg->mulFactor1);
    cmdStruct.pwrMtrTssiCfg.mulFactor2            = (uint16_t)ADRV904X_HTOCS(pPwrMtrTssiCfg->mulFactor2);
    cmdStruct.pwrMtrTssiCfg.mulFactor3            = (uint16_t)ADRV904X_HTOCS(pPwrMtrTssiCfg->mulFactor3);
    cmdStruct.pwrMtrTssiCfg.setLinearThreshold    = (uint8_t)pPwrMtrTssiCfg->setLinearThreshold;
    cmdStruct.pwrMtrTssiCfg.linearThreshold       = (uint32_t)ADRV904X_HTOCL(pPwrMtrTssiCfg->linearThreshold);
    cmdStruct.pwrMtrTssiCfg.enabled               = (uint8_t)pPwrMtrTssiCfg->enabled;
    cmdStruct.pwrMtrTssiCfg.debug                 = (uint32_t)ADRV904X_HTOCL(pPwrMtrTssiCfg->debug);
    cmdStruct.pwrMtrTssiCfg.captureTimeUsec       = (uint32_t)ADRV904X_HTOCL(pPwrMtrTssiCfg->captureTimeUsec);

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_CONFIG_SET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiConfigGet(adi_adrv904x_Device_t* const             device,
                                                                     const uint32_t                           channelSel,
                                                                     adi_adrv904x_DfeSvcDfePwrMtrTssiCfg_t *pPwrMtrTssiCfg,
                                                                     const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrTssiConfigGet_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrTssiConfigGetResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrTssiCfg, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate channelSel */
    recoveryAction = adrv904x_ValidateMask((uint8_t)channelSel, 1u); /* don't allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSel, "Invalid channel mask");
        goto cleanup;
    }

    /* Validate meter parameter: */
    if (meter >= ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_TSSI_METERS)
    {
        /* 0: Tx TSSI (1), 1: Tx TSSI (2), 2: ORx TSSI, 3~10: CDUC TSSI 1~8 */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            meter,
            "TSSI meter is invalid. Valid values defined by adi_adrv904x_DfeSvcDfePwrMtrTssi_e");

        goto cleanup;
    }

    cmdStruct.chanMask = (uint8_t)channelSel;
    cmdStruct.meter    = (uint8_t)meter;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_CONFIG_GET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Copy response parameters to pPwrMtrTssiCfg */
    pPwrMtrTssiCfg->resetAtGainChange      = (uint8_t)respStruct.pwrMtrTssiCfg.resetAtGainChange; /* deprecated and unused */
    pPwrMtrTssiCfg->sourceSelect           = (uint8_t)respStruct.pwrMtrTssiCfg.sourceSelect;
    pPwrMtrTssiCfg->radioSeqPinModeEnable  = (uint8_t)respStruct.pwrMtrTssiCfg.radioSeqPinModeEnable;
    pPwrMtrTssiCfg->opMode                 = (uint8_t)respStruct.pwrMtrTssiCfg.opMode;
    pPwrMtrTssiCfg->measMode               = (uint8_t)respStruct.pwrMtrTssiCfg.measMode;
    pPwrMtrTssiCfg->startDelay             = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrTssiCfg.startDelay);
    pPwrMtrTssiCfg->contDelay              = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrTssiCfg.contDelay);
    pPwrMtrTssiCfg->waitDelay              = (uint32_t)ADRV904X_CTOHL(respStruct.pwrMtrTssiCfg.waitDelay);
    pPwrMtrTssiCfg->fddPinMode             = (uint8_t)respStruct.pwrMtrTssiCfg.fddPinMode;
    pPwrMtrTssiCfg->duration0              = (uint8_t)respStruct.pwrMtrTssiCfg.duration0;
    pPwrMtrTssiCfg->duration1              = (uint8_t)respStruct.pwrMtrTssiCfg.duration1;
    pPwrMtrTssiCfg->duration2              = (uint8_t)respStruct.pwrMtrTssiCfg.duration2;
    pPwrMtrTssiCfg->duration3              = (uint8_t)respStruct.pwrMtrTssiCfg.duration3;
    pPwrMtrTssiCfg->mulFactor0             = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrTssiCfg.mulFactor0);
    pPwrMtrTssiCfg->mulFactor1             = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrTssiCfg.mulFactor1);
    pPwrMtrTssiCfg->mulFactor2             = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrTssiCfg.mulFactor2);
    pPwrMtrTssiCfg->mulFactor3             = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrTssiCfg.mulFactor3);
    pPwrMtrTssiCfg->setLinearThreshold     = (uint8_t)respStruct.pwrMtrTssiCfg.setLinearThreshold;
    pPwrMtrTssiCfg->linearThreshold        = (uint32_t)ADRV904X_CTOHL(respStruct.pwrMtrTssiCfg.linearThreshold);
    pPwrMtrTssiCfg->captureTimeUsec        = (uint32_t)ADRV904X_HTOCL(respStruct.pwrMtrTssiCfg.captureTimeUsec);

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostCfrGainSet(adi_adrv904x_Device_t* const device,
                                                             const uint32_t txChannelMask,
                                                             const uint32_t gain_milli)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    static const uint32_t MAX_GAIN_MDB = 4000U;
    static const uint32_t MAX_GAIN_BFV = 255U;
    uint32_t gainBfVal32 = 0U;
    uint8_t gainBfVal = 0U;
    adrv904x_BfTxDfeDigRegsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    if (gain_milli > MAX_GAIN_MDB) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               gain_milli,
                               "Gain value exceed the limit. Range is [0-4000]");
        goto cleanup;
    }

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    gainBfVal32 = (gain_milli * (uint32_t)64U) / (uint32_t)1000U;
    if (gainBfVal32 > MAX_GAIN_BFV)
    {
        gainBfVal32 = MAX_GAIN_BFV;
    }
    gainBfVal = (uint8_t)gainBfVal32;

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(txChannelMask, chanSel))
        {
            recoveryAction =  adrv904x_TxDfeDigBitfieldAddressGet(device,
                                                                  (adi_adrv904x_TxChannels_e)chanSel,
                                                                  &txBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine base address");
                goto cleanup;
            }

            recoveryAction = adrv904x_TxDfeDigRegs_PostCfrDigitalGain_BfSet(device,
                                                                            NULL,
                                                                            txBaseAddr,
                                                                            gainBfVal);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx Post CFR gain value");
                goto cleanup;
            }
        }
    }
    
cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostCfrGainGet(adi_adrv904x_Device_t* const device,
                                                             const uint32_t txChannelSel,
                                                             uint32_t * const gain_milli)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    uint8_t gainBfVal = 0U;
    adrv904x_BfTxDfeDigRegsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gain_milli, cleanup);

    recoveryAction =  adrv904x_TxDfeDigBitfieldAddressGet(device,
                                                            (adi_adrv904x_TxChannels_e)txChannelSel,
                                                            &txBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine base address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxDfeDigRegs_PostCfrDigitalGain_BfGet(device,
                                                                    NULL,
                                                                    txBaseAddr,
                                                                    &gainBfVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx Post CFR gain value");
        goto cleanup;
    }

    *gain_milli = ((uint32_t)gainBfVal * (uint32_t)1000U) / (uint32_t)64U;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
    
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiEnable(adi_adrv904x_Device_t* const             device,
                                                                  const uint32_t                           channelMask,
                                                                  const uint8_t                            enable,
                                                                  const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrTssiEnable_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate channelMask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)channelMask, 0u); /* allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "Invalid channel mask");
        goto cleanup;
    }

    /* Validate meter */
    if (meter >= ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_TSSI_METERS)
    {
        /* 0: Tx TSSI (1), 1: Tx TSSI (2); 2: ORx TSSI, 3~10: CDUC TSSI 1~8 */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            meter,
            "TSSI meter is invalid. Valid values defined by adi_adrv904x_DfeSvcDfePwrMtrTssi_e");

        goto cleanup;
    }

    cmdStruct.chanMask = (uint8_t)channelMask;
    cmdStruct.enable   = (uint8_t)enable;
    cmdStruct.meter    = (uint8_t)meter;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_ENABLE,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrClgcTssiEnable(adi_adrv904x_Device_t* const device,
                                                                      const uint32_t               txChannelMask,
                                                                      const uint8_t                enable,
                                                                      const uint8_t                useTxMeter1,
                                                                      const uint8_t                useTxMeter2)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrClgcTssiEnable_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate channelMask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelMask, 0u); /* allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelMask, "Invalid tx channel mask");
        goto cleanup;
    }

    /* Validate meter flags. At least one of them must be used */
    if ((useTxMeter1 == 0u) && (useTxMeter2 == 0u))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            useTxMeter1,
            "Both Tx TSSI meters are not used. At least one must be used.");

        goto cleanup;
    }

    cmdStruct.chanMask    = txChannelMask;
    cmdStruct.enable      = enable;
    cmdStruct.useTxMeter1 = useTxMeter1;
    cmdStruct.useTxMeter2 = useTxMeter2;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_CLGC_TSSI_ENABLE,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiManualStart(adi_adrv904x_Device_t* const             device,
                                                                       const uint32_t                           channelMask,
                                                                       const uint8_t                            start,
                                                                       const adi_adrv904x_DfeSvcDfePwrMtrTssi_e meter)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrTssiManualStart_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate channelMask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)channelMask, 0u); /* allow multiples */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelMask, "Invalid channel mask");
        goto cleanup;
    }

    /* Validate meter */
    if (meter > ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_TSSI_ORX)
    {
        /* 0: Tx TSSI (1), 1: Tx TSSI (2); 2: ORx TSSI */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            meter,
            "TSSI meter is invalid. Valid values defined by adi_adrv904x_DfeSvcDfePwrMtrTssi_e");

        goto cleanup;
    }

    cmdStruct.chanMask = (uint8_t)channelMask;
    cmdStruct.start    = (uint8_t)start;
    cmdStruct.meter    = (uint8_t)meter;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_MANUAL_START,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTssiReadBack(adi_adrv904x_Device_t* const              device,
                                                                    const uint32_t                            channelSel,
                                                                    const adi_adrv904x_DfeSvcDfePwrMtrTssi_e  meter,
                                                                    adi_adrv904x_DfeSvcDfePwrMtrTssiMeasResult_t *pPwrMeas)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrTssiReadback_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrTssiReadbackResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMeas, cleanup);

    /* Validate channelSel */
    recoveryAction = adrv904x_ValidateMask((uint8_t)channelSel, 1u); /* Only one allowed */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSel, "Invalid channel mask");
        goto cleanup;
    }

    /* Validate meter */
    if (meter >= ADI_ADRV904X_DFE_SVC_DFE_PWR_MTR_NUM_TSSI_METERS)
    {
        /* 0: Tx TSSI (1), 1: Tx TSSI (2); 2: ORx TSSI 3~10: CDUC TSSI (1~8) */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            meter,
            "TSSI meter is invalid. Valid values defined by adi_adrv904x_DfeSvcDfePwrMtrTssi_e");

        goto cleanup;
    }

    cmdStruct.chanMask = (uint8_t)channelSel;
    cmdStruct.meter    = (uint8_t)meter;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_TSSI_READBACK,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    if (   (respStruct.chanMask != cmdStruct.chanMask)
        || (respStruct.meter != cmdStruct.meter))
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "response mismatch.");
        goto cleanup;
    }

    pPwrMeas->powerMeasDb     = respStruct.pwrMeasDb;
    pPwrMeas->powerMeasLinear = respStruct.pwrMeasLinear;
    pPwrMeas->thresExceeded   = respStruct.thresExceeded;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrClgcTssiReadBack(adi_adrv904x_Device_t* const device,
                                                                        const uint32_t               txChannelSel,
                                                                        const uint8_t                useTxMeter1,
                                                                        const uint8_t                useTxMeter2,
                                                                        uint64_t                     *pPwrMeasTx1,
                                                                        uint64_t                     *pPwrMeasTx2,
                                                                        uint64_t                     *pPwrMeasOrx)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrClgcTssiReadback_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrClgcTssiReadbackResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMeasTx1, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMeasTx2, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMeasOrx, cleanup);

    /* Validate txChannelSel */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelSel, 1u); /* Only one allowed */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid channel mask");
        goto cleanup;
    }

    /* Validate meter flags. At least one of them must be used */
    if ((useTxMeter1 == 0u) && (useTxMeter2 == 0u))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            useTxMeter1,
            "Both Tx TSSI meters are not used. At least one must be used.");

        goto cleanup;
    }

    cmdStruct.txChanMask  = txChannelSel;
    cmdStruct.useTxMeter1 = useTxMeter1;
    cmdStruct.useTxMeter2 = useTxMeter2;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_CLGC_TSSI_READBACK,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    if (respStruct.txChanMask != cmdStruct.txChanMask)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "chan mask response mismatch.");
        goto cleanup;
    }

    if ((respStruct.useTxMeter1 != cmdStruct.useTxMeter1) ||
        (respStruct.useTxMeter2 != cmdStruct.useTxMeter2))
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "response mismatch.");
        goto cleanup;
    }

    *pPwrMeasTx1 = respStruct.pwrMeasTx1;
    *pPwrMeasTx2 = respStruct.pwrMeasTx2;
    *pPwrMeasOrx = respStruct.pwrMeasOrx;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInConfigSet(adi_adrv904x_Device_t* const                device,
                                                                      const uint32_t                              txChannelMask,
                                                                      const adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t *pPwrMtrDpdIn)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrDpdConfig_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrDpdIn, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate txChannelMask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    /* Validate pPwrMtrDpdIn parameters */
    if (pPwrMtrDpdIn->dpdInOutPwrMeasDuration > 24u)
    {
        /* Power measurement duration.
         * The number of samples over which power is averaged is given by 2^(power_measurement_duration).
         * The range for this bit-field is 0 to 24.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrDpdIn->dpdInOutPwrMeasDuration,
            "dpdInPwrMeasDuration is invalid. Valid values are 0 - 24");

        goto cleanup;
    }

    if (pPwrMtrDpdIn->dpdInOutPwrMeasTDDModeEn > 1U)
    {
        /* Set to 1 to enable TDD mode for the power measurement block.
         * Measurement is paused when txon is low.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrDpdIn->dpdInOutPwrMeasTDDModeEn,
            "dpdInPwrMeasTDDModeEn is invalid. Valid values are 0 and 1");

        goto cleanup;
    }

    if (pPwrMtrDpdIn->dpdInOutPeakToPowerMode > 1U)
    {
        /* Enable peak-to-power mode.
         * When this mode is enabled, power for a given measurement duration is available
         * in the tx_power bit-field. Also, the largest peak (I^2+Q^2) within that
         * duration is available in the tx_power_largest_peak bit-field.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrDpdIn->dpdInOutPeakToPowerMode,
            "dpdInPeakToPowerMode is invalid. Valid values are 0 and 1");

        goto cleanup;
    }

    /* NOTE: The FW is intentionally not setting the enable or pause bits here.
     * There are separate API commands for that.
     */

    cmdStruct.txChanMask = (uint8_t)txChannelMask;

    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasDuration    = (uint8_t)pPwrMtrDpdIn->dpdInOutPwrMeasDuration;
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasContDlyCntr = (uint16_t)ADRV904X_HTOCS(pPwrMtrDpdIn->dpdInOutPwrMeasContDlyCntr);
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasTDDModeEn   = (uint8_t)pPwrMtrDpdIn->dpdInOutPwrMeasTDDModeEn;
    cmdStruct.pwrMtrDpdConfig.dpdInOutPeakToPowerMode    = (uint8_t)pPwrMtrDpdIn->dpdInOutPeakToPowerMode;
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasPause       = (uint8_t)0; /* not used */
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasEn          = (uint8_t)0; /* not used */

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_CONFIG_SET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInConfigGet(adi_adrv904x_Device_t* const          device,
                                                                      const uint32_t                        txChannelSel,
                                                                      adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdIn)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrDpdConfigGet_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrDpdConfig_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrDpdIn, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate txChannelSel */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelSel, 1u); /* only one allowed */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx channel mask");
        goto cleanup;
    }

    cmdStruct.txChanMask = (uint8_t)txChannelSel;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_CONFIG_GET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Copy response parameters to pPwrMtrDpdIn */
    pPwrMtrDpdIn->dpdInOutPwrMeasDuration    = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasDuration;
    pPwrMtrDpdIn->dpdInOutPwrMeasContDlyCntr = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasContDlyCntr);
    pPwrMtrDpdIn->dpdInOutPwrMeasTDDModeEn   = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasTDDModeEn;
    pPwrMtrDpdIn->dpdInOutPeakToPowerMode    = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPeakToPowerMode;
    pPwrMtrDpdIn->dpdInOutPwrMeasPause       = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasPause;
    pPwrMtrDpdIn->dpdInOutPwrMeasEn          = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasEn;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdOutConfigSet(adi_adrv904x_Device_t* const                device,
                                                                       const uint32_t                              txChannelMask,
                                                                       const adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t *pPwrMtrDpdOut)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrDpdConfig_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrDpdOut, cleanup);

    /* Validate txChannelMask*/
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    if (pPwrMtrDpdOut->dpdInOutPwrMeasDuration > 24u)
    {
        /* Power measurement duration.
         * The number of samples over which power is averaged is given by 2^(power_measurement_duration).
         * The range for this bit-field is 0 to 24.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrDpdOut->dpdInOutPwrMeasDuration,
            "dpdOutPwrMeasDuration is invalid. Valid values are 0 - 24");

        goto cleanup;
    }

    if (pPwrMtrDpdOut->dpdInOutPwrMeasTDDModeEn > 1U)
    {
        /* Set to 1 to enable TDD mode for the power measurement block.
         * Measurement is paused when txon is low.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrDpdOut->dpdInOutPwrMeasTDDModeEn,
            "dpdOutPwrMeasTDDModeEn is invalid. Valid values are 0 and 1");

        goto cleanup;
    }

    if (pPwrMtrDpdOut->dpdInOutPeakToPowerMode > 1U)
    {
        /* Enable peak-to-power mode.
         * When this mode is enabled, power for a given measurement duration is available
         * in the tx_power bit-field. Also, the largest peak (I^2+Q^2) within that
         * duration is available in the tx_power_largest_peak bit-field.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrMtrDpdOut->dpdInOutPeakToPowerMode,
            "dpdOutPeakToPowerMode is invalid. Valid values are 0 and 1");

        goto cleanup;
    }

    /* NOTE: The API is intentionally not setting the enable or pause bits here.
     * There are separate API commands for that.
     */
    cmdStruct.txChanMask = (uint8_t)txChannelMask;

    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasDuration    = (uint8_t)pPwrMtrDpdOut->dpdInOutPwrMeasDuration;
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasContDlyCntr = (uint16_t)ADRV904X_HTOCS(pPwrMtrDpdOut->dpdInOutPwrMeasContDlyCntr);
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasTDDModeEn   = (uint8_t)pPwrMtrDpdOut->dpdInOutPwrMeasTDDModeEn;
    cmdStruct.pwrMtrDpdConfig.dpdInOutPeakToPowerMode    = (uint8_t)pPwrMtrDpdOut->dpdInOutPeakToPowerMode;
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasPause       = (uint8_t)0; /* not used */
    cmdStruct.pwrMtrDpdConfig.dpdInOutPwrMeasEn          = (uint8_t)0; /* not used */

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_CONFIG_SET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdOutConfigGet(adi_adrv904x_Device_t* const          device,
                                                                       const uint32_t                        txChannelSel,
                                                                       adi_adrv904x_DfeSvcDfePwrMtrDpdCfg_t* pPwrMtrDpdOut)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrDpdConfigGet_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrDpdConfig_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrMtrDpdOut, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate txChannelSel */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelSel, 1u); /* only one allowed */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx channel mask");
        goto cleanup;
    }

    cmdStruct.txChanMask = (uint8_t)txChannelSel;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_CONFIG_GET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Copy response parameters to pPwrMtrDpdOut */
    pPwrMtrDpdOut->dpdInOutPwrMeasDuration    = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasDuration;
    pPwrMtrDpdOut->dpdInOutPwrMeasContDlyCntr = (uint16_t)ADRV904X_CTOHS(respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasContDlyCntr);
    pPwrMtrDpdOut->dpdInOutPwrMeasTDDModeEn   = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasTDDModeEn;
    pPwrMtrDpdOut->dpdInOutPeakToPowerMode    = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPeakToPowerMode;
    pPwrMtrDpdOut->dpdInOutPwrMeasPause       = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasPause;
    pPwrMtrDpdOut->dpdInOutPwrMeasEn          = (uint8_t)respStruct.pwrMtrDpdConfig.dpdInOutPwrMeasEn;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInEnable(adi_adrv904x_Device_t* const device,
                                                                   const uint32_t               txChannelMask,
                                                                   const uint8_t                enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrDpdEnable_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate txChannelMask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    cmdStruct.txChanMask = (uint8_t)txChannelMask;
    cmdStruct.enable     = (uint8_t)enable;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_ENABLE,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdOutEnable(adi_adrv904x_Device_t* const device,
                                                                    const uint32_t               txChannelMask,
                                                                    const uint8_t                enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrDpdEnable_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate txChannelMask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    cmdStruct.txChanMask = (uint8_t)txChannelMask;
    cmdStruct.enable     = (uint8_t)enable;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_OUT_ENABLE,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrDpdInOutReadBack(adi_adrv904x_Device_t* const              device,
                                                                        const adi_adrv904x_TxChannels_e           txChannelSel,
                                                                        adi_adrv904x_DfeSvcDfePwrMtrDpdResults_t *pPwrDpdInOut)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcCmdDfeMtrDpdReadback_t cmdStruct;
    adrv904x_DfeSvcCmdDfeMtrDpdReadbackResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrDpdInOut, cleanup);

    /* Validate Tx channel */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txChannelSel, 1u); /* Only one allowed */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx channel mask");
        goto cleanup;
    }

    cmdStruct.txChannel = (uint8_t)txChannelSel;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_PWR_MTR_DPD_IN_OUT_READBACK,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    if (respStruct.txChannel != cmdStruct.txChannel)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "response mismatch for txChannel");
        goto cleanup;
    }

    /* Copy response parameters to pPwrDpdInOut */
    pPwrDpdInOut->pwrDpdIn      = (uint8_t)respStruct.pwrMtrResp.pwrDpdIn;
    pPwrDpdInOut->peakPwrDpdIn  = (uint8_t)respStruct.pwrMtrResp.peakPwrDpdIn;
    pPwrDpdInOut->pwrDpdOut     = (uint8_t)respStruct.pwrMtrResp.pwrDpdOut;
    pPwrDpdInOut->peakPwrDpdOut = (uint8_t)respStruct.pwrMtrResp.peakPwrDpdOut;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierCfgSet(adi_adrv904x_Device_t* const                          device,
                                                                       const adi_adrv904x_TxCarrierMask_t * const            txCarrierMask,
                                                                       const adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t * pPwrTxCarrierCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfig_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrTxCarrierCfg, cleanup);

    /* Validate Tx channel mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierMask->txChannelMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierMask->txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    /* Validate Tx carrier mask (same rules as Tx channel) */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierMask->carrierMask, 0u); /* allow multiples but not 0 */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierMask->carrierMask, "Invalid Tx carrier mask");
        goto cleanup;
    }

    /* Scan through each channel to validate the carrier mask */
    recoveryAction = adrv904x_TxCducCarrierCheck(device, txCarrierMask->txChannelMask, txCarrierMask->carrierMask);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting enabled carriers");
        goto cleanup;
    }

    cmdStruct.channelMask = (uint32_t)ADRV904X_HTOCL(txCarrierMask->txChannelMask);
    cmdStruct.carrierMask = (uint32_t)ADRV904X_HTOCL(txCarrierMask->carrierMask);

    /* Validate pPwrTxCarrierCfg parameters */
    if (pPwrTxCarrierCfg->measDuration > 18U)
    {
        /* Power measurement duration.
         * The number of samples over which power is averaged is given by 2^(power_measurement_duration).
         * The range for this bit-field is 0 to 18.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrTxCarrierCfg->measDuration,
            "measDuration is invalid. Valid values are 0 - 18");

        goto cleanup;
    }

    if (pPwrTxCarrierCfg->peakToPowerMode > 1u)
    {
        /* Enable peak-to-power mode.
         * When this mode is enabled, power for a given measurement duration is available
         * in the tx_power bit-field. Also, the largest peak (I^2+Q^2) within that duration
         * is available in the tx_power_largest_peak bit-field
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrTxCarrierCfg->peakToPowerMode,
            "peakToPowerMode is invalid. Valid values are 0 and 1");

        goto cleanup;
    }

    if (pPwrTxCarrierCfg->inputSelect > 2U)
    {
        /* This bit-field allows selection of the data path point for power calculations.
         * 4'b00 - channel filter out 4'b01 - delay matcher out 4'b10 - multiplier out
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrTxCarrierCfg->inputSelect,
            "inputSelect is invalid. Valid values are 0 - 2");

        goto cleanup;
    }

    if (pPwrTxCarrierCfg->tddModeEnable > 1U)
    {
        /* Set to 1 to enable TDD mode for the power measurement block.
         * Measurement is paused when txon is low.
         */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
            recoveryAction,
            pPwrTxCarrierCfg->tddModeEnable,
            "tddModeEnable is invalid. Valid values are 0 and 1");

        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY((void*)&cmdStruct.cfg, (const void*)pPwrTxCarrierCfg, sizeof(*pPwrTxCarrierCfg));

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_CONFIG_SET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierCfgGet(adi_adrv904x_Device_t* const                   device,
                                                                       const adi_adrv904x_TxCarrierMask_t* const      txCarrierSel,
                                                                       adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasCfg_t *pPwrTxCarrrierCfgResp)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfigGet_t cmdStruct;
    adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasConfigGetResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierSel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrTxCarrrierCfgResp, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate Tx channel mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierSel->txChannelMask, 1u); /* Only one allowed */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierSel->txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    /* Validate Tx carrier mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierSel->carrierMask, 1u); /* Only one allowed */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierSel->carrierMask, "Invalid Tx carrier mask");
        goto cleanup;
    }

    /* Scan through each channel to validate the carrier mask */
    recoveryAction = adrv904x_TxCducCarrierCheck(device, txCarrierSel->txChannelMask, txCarrierSel->carrierMask);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting enabled carriers");
        goto cleanup;
    }

    cmdStruct.channelMask = (uint32_t)ADRV904X_HTOCL(txCarrierSel->txChannelMask);
    cmdStruct.carrierMask = (uint32_t)ADRV904X_HTOCL(txCarrierSel->carrierMask);

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_CONFIG_GET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    if (respStruct.channelMask != txCarrierSel->txChannelMask)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respStruct.channelMask, "Tx carrier config get status - bad channel");
        goto cleanup;
    }

    if (respStruct.carrierMask != txCarrierSel->carrierMask)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, respStruct.carrierMask, "Tx carrier config get status - bad carrier");
        goto cleanup;
    }

    /* Copy response parameters to pPwrTxCarrrierCfgResp */
    pPwrTxCarrrierCfgResp->measDuration              = (uint8_t)respStruct.cfg.measDuration;
    pPwrTxCarrrierCfgResp->measContinueDelayDuration = (uint16_t)ADRV904X_CTOHS(respStruct.cfg.measContinueDelayDuration);
    pPwrTxCarrrierCfgResp->peakToPowerMode           = (uint16_t)ADRV904X_CTOHS(respStruct.cfg.peakToPowerMode);
    pPwrTxCarrrierCfgResp->inputSelect               = (uint32_t)ADRV904X_CTOHL(respStruct.cfg.inputSelect);
    pPwrTxCarrrierCfgResp->tddModeEnable             = (uint8_t)respStruct.cfg.tddModeEnable;
    pPwrTxCarrrierCfgResp->enable                    = (uint8_t)respStruct.cfg.enable;
    pPwrTxCarrrierCfgResp->pause                     = (uint8_t)respStruct.cfg.pause;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierMeasEnable(adi_adrv904x_Device_t* const               device,
                                                                           const adi_adrv904x_TxCarrierMask_t * const txCarrierMask,
                                                                           const uint8_t                              enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasEnable_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierMask, cleanup);
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate Tx channel mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierMask->txChannelMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierMask->txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    /* Validate Tx carrier mask (multiples ok) */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierMask->carrierMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Tx carrier mask for enable");
        goto cleanup;
    }

    /* Scan through each channel to validate the carrier mask */
    recoveryAction = adrv904x_TxCducCarrierCheck(device, txCarrierMask->txChannelMask, txCarrierMask->carrierMask);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting enabled carriers");
        goto cleanup;
    }

    cmdStruct.channelMask = (uint32_t)ADRV904X_HTOCL(txCarrierMask->txChannelMask);
    cmdStruct.carrierMask = (uint32_t)ADRV904X_HTOCL(txCarrierMask->carrierMask);
    cmdStruct.enable      = (uint8_t)enable;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_ENABLE,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierManualStart(adi_adrv904x_Device_t* const               device,
                                                                            const adi_adrv904x_TxCarrierMask_t * const txCarrierMask,
                                                                            const uint8_t                              start)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasManualStart_t cmdStruct;
    adrv904x_DfeSvcCmdGenericResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierMask, cleanup);
    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate Tx channel mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierMask->txChannelMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierMask->txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    /* Validate Tx carrier mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierMask->carrierMask, 0u); /* allow multiples */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierMask->carrierMask, "Invalid Tx carrier mask");
        goto cleanup;
    }

    /* Scan through each channel to validate the carrier mask */
    recoveryAction = adrv904x_TxCducCarrierCheck(device, txCarrierMask->txChannelMask, txCarrierMask->carrierMask);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting enabled carriers");
        goto cleanup;
    }

    cmdStruct.channelMask = (uint32_t)ADRV904X_HTOCL(txCarrierMask->txChannelMask);
    cmdStruct.carrierMask = (uint32_t)ADRV904X_HTOCL(txCarrierMask->carrierMask);
    cmdStruct.start       = (uint8_t)start;

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_MANUAL_START,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfePwrMtrTxCarrierMeasGet(adi_adrv904x_Device_t* const                      device,
                                                                        const adi_adrv904x_TxCarrierMask_t* const         txCarrierSel,
                                                                        adi_adrv904x_DfeSvcDfeTxCarrierPowerMeasResult_t *pPwrTxCarrrierMeasResult)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasGet_t cmdStruct;
    adrv904x_DfeSvcDfeAppCmd_TxCarrierPwrMeasGetResp_t respStruct;
    adrv904x_DfeSvcCmdStatus_t cmdStatus = ADRV904X_DFE_SVC_CMD_STATUS_GENERIC;
    adi_adrv904x_DfeSvcErrCode_e cpuErrorCode = ADI_ADRV904X_DFE_SVC_ERR_CODE_DFE_EVENT_UNEXPECTED;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierSel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pPwrTxCarrrierMeasResult, cleanup);

    ADI_LIBRARY_MEMSET(&cmdStruct, 0, sizeof(cmdStruct));
    ADI_LIBRARY_MEMSET(&respStruct, 0, sizeof(respStruct));

    /* Validate Tx channel mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierSel->txChannelMask, 1u); /* Only one allowed */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierSel->txChannelMask, "Invalid Tx channel mask");
        goto cleanup;
    }

    /* Validate Tx carrier mask */
    recoveryAction = adrv904x_ValidateMask((uint8_t)txCarrierSel->carrierMask, 1u); /* Only one allowed */

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierSel->carrierMask, "Invalid Tx carrier mask");
        goto cleanup;
    }

    /* Scan through each channel to validate the carrier mask */
    recoveryAction = adrv904x_TxCducCarrierCheck(device, txCarrierSel->txChannelMask, txCarrierSel->carrierMask);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting enabled carriers");
        goto cleanup;
    }

    cmdStruct.channelMask = (uint32_t)ADRV904X_HTOCL(txCarrierSel->txChannelMask);
    cmdStruct.carrierMask = (uint32_t)ADRV904X_HTOCL(txCarrierSel->carrierMask);

    /* Send command to DFE */
    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_TX_CARRIER_PWR_MEAS_GET,
                                            (uint8_t *)&cmdStruct,
                                            sizeof(cmdStruct),
                                            (uint8_t *)&respStruct,
                                            sizeof(respStruct),
                                            &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_DFE_SVC_CMD_RESP_CHECK_GOTO(respStruct.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Copy response results to pPwrTxCarrrierMeasResult */
    ADI_LIBRARY_MEMCPY((void*)pPwrTxCarrrierMeasResult, (void*)&respStruct.result, sizeof(*pPwrTxCarrrierMeasResult));

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierGainAdjustSet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_TxCarrierMask_t * const txCarrierMask,
                                                                     const int32_t                              gain_mdB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t carrierIdx = 0U;
    uint32_t carrierSel = 0U;
    uint32_t txIdx = 0U;     
    uint32_t txSel = 0U;
    uint32_t  bfValue = 0U;
    const int32_t DIG_GAIN_MIN = -90000;
    const int32_t DIG_GAIN_MAX = 36000;
    const uint32_t DIG_GAIN_MULT = 65535U;
    adrv904x_BfCducHbDpathChanAddr_e cducHbDpathChanBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierMask, cleanup);

    if (((txCarrierMask->carrierMask & (~(uint32_t)ADI_ADRV904X_TX_CARRIER_ALL)) != 0U) || 
         (txCarrierMask->carrierMask == (uint32_t)ADI_ADRV904X_TX_CARRIER_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txCarrierMask->carrierMask,
                               "Invalid Tx carrier mask is selected. Valid values are any combinations of TxCarrier0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((txCarrierMask->txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || 
       (txCarrierMask->txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txCarrierMask->txChannelMask,
                               "Invalid Tx channel mask is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if ((gain_mdB < DIG_GAIN_MIN) || (gain_mdB > DIG_GAIN_MAX))
    {
        /* Invalid gain_mdB selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, gain_mdB, "Invalid gain_mdB selection. Valid value are from -90000mdB to 36000mdB");
        goto cleanup;
    }

    /* Convert from mdB to 7.16. (reg value = 10**(value in mdB/1000/20)) * 2^16) */
    //bfValue = (uint32_t)((double)pow(10, (double)gain_mdB / 1000U / 20U) * DIG_GAIN_MULT);
    bfValue = 100;

    /* Write out the enable */
    for (txIdx = 0U; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; txIdx++)
    {
        txSel = 1U << txIdx;
        if ((txCarrierMask->txChannelMask & txSel) > 0U)
        {
            /* Convert the txSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CducHbDpathBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txSel), &cducHbDpathChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txSel, "Invalid Tx Carrier used to determine SPI address");
                goto cleanup;
            }

            for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_TX_CARRIERS; carrierIdx++)
            {
                carrierSel = 1U << carrierIdx;
                if ((txCarrierMask->carrierMask & carrierSel) > 0U)
                {
                    switch (carrierSel)
                    {
                        case ADI_ADRV904X_TX_CARRIER_0:            
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain0_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_1:
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain1_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_2:
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain2_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_3:
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain3_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_4:
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain4_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_5:
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain5_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_6:
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain6_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_7:
                            recoveryAction = adrv904x_CducHbDpath_CarrierGain7_BfSet(device,
                                                                                     NULL,
                                                                                     cducHbDpathChanBaseAddr,
                                                                                     bfValue);
                            break;
                        default:
                            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                            ADI_PARAM_ERROR_REPORT(&device->common,
                                                   recoveryAction,
                                                   txCarrierMask->carrierMask,
                                                   "Invalid Tx carrier selection");
                            goto cleanup;
                            break;
                    }

                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting carrier gain");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_CducHbDpath_CarrierGainEnable_BfSet(device,
                                                                                  NULL,
                                                                                  cducHbDpathChanBaseAddr,
                                                                                  carrierIdx,
                                                                                  1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting carrier gain enable");
                        goto cleanup;
                    }

                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierGainAdjustGet(adi_adrv904x_Device_t * const              device,
                                                                     const adi_adrv904x_TxCarrierMask_t * const txCarrierSel,
                                                                     int32_t * const                            gain_mdB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t  bfValue = 0U;
    const uint32_t DIG_GAIN_MULT = 65535U;

    adrv904x_BfCducHbDpathChanAddr_e cducHbDpathChanBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierSel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gain_mdB, cleanup);

    *gain_mdB = 0;

    if ((txCarrierSel->txChannelMask != ADI_ADRV904X_TX0) &&
        (txCarrierSel->txChannelMask != ADI_ADRV904X_TX1) &&
        (txCarrierSel->txChannelMask != ADI_ADRV904X_TX2) &&
        (txCarrierSel->txChannelMask != ADI_ADRV904X_TX3) &&
        (txCarrierSel->txChannelMask != ADI_ADRV904X_TX4) &&
        (txCarrierSel->txChannelMask != ADI_ADRV904X_TX5) &&
        (txCarrierSel->txChannelMask != ADI_ADRV904X_TX6) &&
        (txCarrierSel->txChannelMask != ADI_ADRV904X_TX7))
    {
        /* Invalid Tx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierSel->txChannelMask, "Invalid Tx channel selection");
        goto cleanup;
    }

    /* Read out the configurations */ 
    /* Convert the txCarrierSel->txChannelMask to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CducHbDpathBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txCarrierSel->txChannelMask), &cducHbDpathChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierSel->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    switch (txCarrierSel->carrierMask)
    {
        case ADI_ADRV904X_TX_CARRIER_0:            
            recoveryAction = adrv904x_CducHbDpath_CarrierGain0_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_1:
            recoveryAction = adrv904x_CducHbDpath_CarrierGain1_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_2:
            recoveryAction = adrv904x_CducHbDpath_CarrierGain2_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_3:
            recoveryAction = adrv904x_CducHbDpath_CarrierGain3_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_4:
            recoveryAction = adrv904x_CducHbDpath_CarrierGain4_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_5:
            recoveryAction = adrv904x_CducHbDpath_CarrierGain5_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_6:
            recoveryAction = adrv904x_CducHbDpath_CarrierGain6_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_7:
            recoveryAction = adrv904x_CducHbDpath_CarrierGain7_BfGet(device,
                                                                     NULL,
                                                                     cducHbDpathChanBaseAddr,
                                                                     &bfValue);
            break;
        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txCarrierSel->carrierMask,
                                   "Invalid Tx carrier selection");
            goto cleanup;
            break;
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Tx Carrier Gain.");
        goto cleanup;
    }

    if (bfValue != 0U)
    {
        /* Convert from 7.16 to mdB.  value in mdB = (1000*20*log10(reg value/2^16)) */
        //*gain_mdB = (int32_t)(1000U * 20U * log10((double)bfValue / DIG_GAIN_MULT));
    	*gain_mdB = 12;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierBandAttenSet(adi_adrv904x_Device_t * const           device,
                                                                    const adi_adrv904x_TxBandMask_t * const txBandMask,
                                                                    const uint32_t                          atten_mdB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bandIdx = 0U;
    uint32_t bandSel = 0U;
    uint32_t txIdx = 0U;     
    uint32_t txSel = 0U;
    uint32_t attenRegVal = 0U;
    const uint32_t DIG_GAIN_MULT = 256U;
    adrv904x_BfCducHbDpathChanAddr_e cducHbDpathChanBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txBandMask, cleanup);

    if (((txBandMask->bandMask & (~(uint32_t)ADI_ADRV904X_TX_CARRIER_BAND_ALL)) != 0U) || 
         (txBandMask->bandMask == (uint32_t)ADI_ADRV904X_TX_CARRIER_BAND_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txBandMask->bandMask,
                               "Invalid Tx carrier band mask is selected. Valid values are any combinations of TxCarrierBand0/1");
        goto cleanup;
    }

    if (((txBandMask->txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || 
        (txBandMask->txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txBandMask->txChannelMask,
                               "Invalid Tx channel mask is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    for (txIdx = 0U; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; txIdx++)
    {
        txSel = 1U << txIdx;
        if ((txBandMask->txChannelMask & txSel) > 0U)
        {
            /* Convert the txSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_CducHbDpathBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txSel), &cducHbDpathChanBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txSel, "Invalid Tx Carrier used to determine SPI address");
                goto cleanup;
            }

            for (bandIdx = 0U; bandIdx < ADI_ADRV904X_MAX_TX_CARRIER_BANDS; bandIdx++)
            {
                bandSel = 1U << bandIdx;
                if ((txBandMask->bandMask & bandSel) > 0U)
                {
                    /* Convert from the requested atten level (milli-dB) to equivalent */
                    /* Convert from mdB to 0.8. (reg value = 10**(value in mdB/1000/20)) * 2^8) */
                    //attenRegVal = (uint32_t)((double)pow(10, (0.0 - (double)atten_mdB) / 1000U / 20U) * DIG_GAIN_MULT);
                    attenRegVal = 12;
                   
                    if (attenRegVal > 255U)
                    {
                        /* Invalid attenRegVal selection */
                        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, attenRegVal, "Invalid attenRegVal selection.");
                        goto cleanup;
                    }

                    switch (bandSel)
                    {
                        case ADI_ADRV904X_TX_CARRIER_BAND_0:            
                            recoveryAction = adrv904x_CducHbDpath_CarrBand0Attn_BfSet(device,
                                                                                      NULL,
                                                                                      cducHbDpathChanBaseAddr,
                                                                                      attenRegVal);
                            break;
                        case ADI_ADRV904X_TX_CARRIER_BAND_1:            
                            recoveryAction = adrv904x_CducHbDpath_CarrBand1Attn_BfSet(device,
                                                                                      NULL,
                                                                                      cducHbDpathChanBaseAddr,
                                                                                      attenRegVal);
                            break;
                       
                        default:
                            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                            ADI_PARAM_ERROR_REPORT(&device->common,
                                                   recoveryAction,
                                                   txBandMask->bandMask,
                                                   "Invalid Tx carrier band selection");
                            goto cleanup;
                            break;
                    }

                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting carrier band attn");
                        goto cleanup;
                    }

                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierBandAttenGet(adi_adrv904x_Device_t * const      device,
                                                                    const adi_adrv904x_TxBandMask_t * const txBandSel,
                                                                    uint32_t * const atten_mdB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t  bfValue = 0U;
    const uint32_t DIG_GAIN_MULT = 256U;
    adrv904x_BfCducHbDpathChanAddr_e cducHbDpathChanBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_CDUC_TX_CDUC_HB_DPATH;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txBandSel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, atten_mdB, cleanup);

    if ((txBandSel->txChannelMask != ADI_ADRV904X_TX0) &&
        (txBandSel->txChannelMask != ADI_ADRV904X_TX1) &&
        (txBandSel->txChannelMask != ADI_ADRV904X_TX2) &&
        (txBandSel->txChannelMask != ADI_ADRV904X_TX3) &&
        (txBandSel->txChannelMask != ADI_ADRV904X_TX4) &&
        (txBandSel->txChannelMask != ADI_ADRV904X_TX5) &&
        (txBandSel->txChannelMask != ADI_ADRV904X_TX6) &&
        (txBandSel->txChannelMask != ADI_ADRV904X_TX7))
    {
        /* Invalid Tx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txBandSel->txChannelMask, "Invalid Tx channel selection");
        goto cleanup;
    }

    /* Convert the txBandSel->txChannelMask to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_CducHbDpathBitfieldAddressGet(device,
                                                            (adi_adrv904x_TxChannels_e)(txBandSel->txChannelMask),
                                                            &cducHbDpathChanBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txBandSel->txChannelMask, "Invalid Tx Channel used to determine SPI address");
        goto cleanup;
    }

    switch (txBandSel->bandMask)
    {
        case ADI_ADRV904X_TX_CARRIER_BAND_0:            
            recoveryAction = adrv904x_CducHbDpath_CarrBand0Attn_BfGet(device,
                                                                      NULL,
                                                                      cducHbDpathChanBaseAddr,
                                                                      &bfValue);
            break;
        case ADI_ADRV904X_TX_CARRIER_BAND_1:
            recoveryAction = adrv904x_CducHbDpath_CarrBand1Attn_BfGet(device,
                                                                      NULL,
                                                                      cducHbDpathChanBaseAddr,
                                                                      &bfValue);
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   txBandSel->bandMask,
                                   "Invalid Tx carrier band selection");
            goto cleanup;
            break;
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Tx Carrier Band Atten.");
        goto cleanup;
    }
          
    /* Convert from 0.8 to mdB.  value in mdB = (1000*20*log10(reg value/2^8)) */
    if (bfValue > 0)
    {        
        //*atten_mdB = 0 - (1000U * 20U * (double)log10((double)bfValue / DIG_GAIN_MULT));
    	*atten_mdB = -12;
    }
    else
    {
        *atten_mdB = 0xFFFFFFFFU;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostDpdAttenSet(adi_adrv904x_Device_t * const              device,
                                                              const adi_adrv904x_TxDpdMask_t * const     txDpdMask,
                                                              const adi_adrv904x_PostDpdAtten_t * const  attenParams)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t txIdx = 0U;     
    uint32_t txSel = 0U;
    uint32_t dpdIdx = 0U;     
    uint32_t dpdSel = 0U;
    adrv904x_BfTxDpdActMmrChanAddr_e baseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DPD_ACT_MMR;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txDpdMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, attenParams, cleanup);

    if (((txDpdMask->txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) || 
        (txDpdMask->txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txDpdMask->txChannelMask,
                               "Invalid Tx channel mask is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (((txDpdMask->dpdIdMask & (~(uint32_t)ADI_ADRV904X_TX_DPD_ID_ALL)) != 0U) || 
         (txDpdMask->dpdIdMask == (uint32_t)ADI_ADRV904X_TX_DPD_ID_NONE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txDpdMask->dpdIdMask,
                               "Invalid Tx dpd id mask is selected. Valid values are any combinations of TxDdpId0/1/2/3");
        goto cleanup;
    }

    if ((attenParams->gmpOutShift > 15U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, attenParams->gmpOutShift, "Invalid gmpOutShift selection. Valid value are from 0 to 15");
        goto cleanup;
    }

    if ((attenParams->ddrOutShift > 15U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, attenParams->ddrOutShift, "Invalid ddrOutShift selection. Valid value are from 0 to 15");
        goto cleanup;
    }

    for (txIdx = 0U; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; txIdx++)
    {
        txSel = 1U << txIdx;
        if ((txDpdMask->txChannelMask & txSel) > 0U)
        {
            /* Convert the txSel to the base address value required by the bitfield functions */
            recoveryAction = adrv904x_DpdActMmrBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txSel), &baseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txSel, "Invalid Tx Carrier used to determine SPI address");
                goto cleanup;
            }

            for (dpdIdx = 0U; dpdIdx < ADI_ADRV904X_MAX_TX_DPD_IDS; dpdIdx++)
            {
                dpdSel = 1U << dpdIdx;
                if ((txDpdMask->dpdIdMask & dpdSel) > 0U)
                {
                    recoveryAction = adrv904x_TxDpdActMmr_MagGain_BfSet(device,
                                                                        NULL,
                                                                        baseAddr,
                                                                        dpdIdx,
                                                                        attenParams->magGain);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Tx Dpd Mag Gain.");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_TxDpdActMmr_DpdGmpOutShift_BfSet(device,
                                                                               NULL,
                                                                               baseAddr,
                                                                               dpdIdx,
                                                                               attenParams->gmpOutShift);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Tx Dpd Gmp Out Shift.");
                        goto cleanup;
                    }

                    recoveryAction = adrv904x_TxDpdActMmr_DpdDdrOutShift_BfSet(device,
                                                                               NULL,
                                                                               baseAddr,
                                                                               dpdIdx,
                                                                               attenParams->ddrOutShift);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Tx Dpd Ddr Out Shift.");
                        goto cleanup;
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PostDpdAttenGet(adi_adrv904x_Device_t * const           device,
                                                              const adi_adrv904x_TxDpdMask_t * const  txDpdSel,
                                                              adi_adrv904x_PostDpdAtten_t * const     attenParams)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t dpdIdx = 0U;     
    uint32_t dpdSel = 0U;
    adrv904x_BfTxDpdActMmrChanAddr_e baseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DPD_ACT_MMR;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txDpdSel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, attenParams, cleanup);

    if ((txDpdSel->txChannelMask != ADI_ADRV904X_TX0) &&
        (txDpdSel->txChannelMask != ADI_ADRV904X_TX1) &&
        (txDpdSel->txChannelMask != ADI_ADRV904X_TX2) &&
        (txDpdSel->txChannelMask != ADI_ADRV904X_TX3) &&
        (txDpdSel->txChannelMask != ADI_ADRV904X_TX4) &&
        (txDpdSel->txChannelMask != ADI_ADRV904X_TX5) &&
        (txDpdSel->txChannelMask != ADI_ADRV904X_TX6) &&
        (txDpdSel->txChannelMask != ADI_ADRV904X_TX7))
    {
        /* Invalid Tx channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txDpdSel->txChannelMask, "Invalid Tx channel selection");
        goto cleanup;
    }

    if ((txDpdSel->dpdIdMask != ADI_ADRV904X_TX_DPD_ID_0) &&
        (txDpdSel->dpdIdMask != ADI_ADRV904X_TX_DPD_ID_1) &&
        (txDpdSel->dpdIdMask != ADI_ADRV904X_TX_DPD_ID_2) &&
        (txDpdSel->dpdIdMask != ADI_ADRV904X_TX_DPD_ID_3))
    {
        /* Invalid Tx dpd id selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txDpdSel->dpdIdMask, "Invalid Tx dpd id selection");
        goto cleanup;
    }

    /* Convert the txDpdSel->txChannelMask to the base address value required by the bitfield functions */
    recoveryAction = adrv904x_DpdActMmrBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txDpdSel->txChannelMask), &baseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txDpdSel->txChannelMask, "Invalid Tx Carrier used to determine SPI address");
        goto cleanup;
    }

    for (dpdIdx = 0U; dpdIdx < ADI_ADRV904X_MAX_TX_DPD_IDS; dpdIdx++)
    {
        dpdSel = 1U << dpdIdx;
        if ((txDpdSel->dpdIdMask & dpdSel) > 0U)
        {
            recoveryAction = adrv904x_TxDpdActMmr_MagGain_BfGet(device,
                                                                NULL,
                                                                baseAddr,
                                                                dpdIdx,
                                                                &attenParams->magGain);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Tx Dpd Mag Gain.");
                goto cleanup;
            }

            recoveryAction = adrv904x_TxDpdActMmr_DpdGmpOutShift_BfGet(device,
                                                                       NULL,
                                                                       baseAddr,
                                                                       dpdIdx,
                                                                       &attenParams->gmpOutShift);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Tx Dpd Gmp Out Shift.");
                goto cleanup;
            }

            recoveryAction = adrv904x_TxDpdActMmr_DpdDdrOutShift_BfGet(device,
                                                                       NULL,
                                                                       baseAddr,
                                                                       dpdIdx,
                                                                       &attenParams->ddrOutShift);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting Tx Dpd Ddr Out Shift.");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
        
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HardClipperConfigSet(adi_adrv904x_Device_t* const device,
                                                                   const adi_adrv904x_HardClipperConfig_t * const hardClipperConfig,
                                                                   const uint32_t hcTxChannelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDatapathChanAddr_e txDatapathHcBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DATAPATH;
    adrv904x_BfTxDfeDigRegsChanAddr_e txDfeDigHcBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    uint32_t chanIdx = 0U;
    uint32_t chanSel = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, hardClipperConfig, cleanup);

    /*Check that if requested Tx Channel valid*/
    if (((hcTxChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (hcTxChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               hcTxChannelMask,
                               "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    /* Check hc select parameter is valid */
    if (hardClipperConfig->hardClipperSel > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               hardClipperConfig->hardClipperSel,
                               "hardClipperSel value is invalid. Valid value 0-1");
        goto cleanup;
    }
    
    /* Check if window size parameter is valid */
    if (hardClipperConfig->hardClipperEnable > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               hardClipperConfig->hardClipperEnable,
                               "hardClipperEnable value is invalid. Valid value 0-1");
        goto cleanup;
    }
    
    if (hardClipperConfig->hardClipperSel == 0) /* PostAtten hardClipper mode*/
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(hcTxChannelMask, chanSel))
            {
                recoveryAction = adrv904x_CfrDatapathBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txDatapathHcBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine data path base address");
                    goto cleanup;
                }

                /* Write hard clipper threshold value */
                recoveryAction = adrv904x_TxDatapath_PostAttenHcThreshold_BfSet(device,
                                                                                NULL,
                                                                                txDatapathHcBaseAddr,
                                                                                hardClipperConfig->hardClipperThreshold); /* TODO: Apply threshold transformation if needed */
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing hard clipper threshold");
                    goto cleanup;
                }
                
                /* Write hard clipper enable bit */
                recoveryAction = adrv904x_TxDatapath_PostAttenHcEn_BfSet(device,
                                                                         NULL,
                                                                         txDatapathHcBaseAddr,
                                                                         hardClipperConfig->hardClipperEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing hard clipper enable");
                    goto cleanup;
                }

            }
        }
    }
    else /* PreDpd hardClipper mode*/
    {
        for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
        {
            chanSel = 1U << chanIdx;
            if (ADRV904X_BF_EQUAL(hcTxChannelMask, chanSel))
            {
                recoveryAction = adrv904x_TxDfeDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txDfeDigHcBaseAddr);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanIdx, "Invalid Tx Channel used to determine Tx Dfe Dig base address");
                    goto cleanup;
                }
                
                /* Write hard clipper threshold value */
                recoveryAction = adrv904x_TxDfeDigRegs_PreDpdHcThreshold_BfSet(device,
                                                                               NULL,
                                                                               txDfeDigHcBaseAddr,
                                                                               hardClipperConfig->hardClipperThreshold);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing hard clipper threshold");
                    goto cleanup;
                }
                
                /* Write hard clipper enable bit */
                recoveryAction = adrv904x_TxDfeDigRegs_PreDpdHcEn_BfSet(device,
                                                                        NULL,
                                                                        txDfeDigHcBaseAddr,
                                                                        hardClipperConfig->hardClipperEnable);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing hard clipper enable");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HardClipperConfigGet(adi_adrv904x_Device_t* const device,
                                                                   adi_adrv904x_HardClipperConfig_t * const hardClipperConfig,
                                                                   const uint32_t hcTxChannelSel) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDatapathChanAddr_e txDatapathHcBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DATAPATH;
    adrv904x_BfTxDfeDigRegsChanAddr_e txDfeDigHcBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, hardClipperConfig, cleanup);

    /* Check hc mode parameter is valid */
    if (hardClipperConfig->hardClipperSel > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               hardClipperConfig->hardClipperSel,
                               "hardClipperSel value is invalid. Valid value 0-1");
        goto cleanup;
    }
    
    if (hardClipperConfig->hardClipperSel == 0) /* PostAtten hardClipper mode*/
    {
        recoveryAction = adrv904x_CfrDatapathBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(hcTxChannelSel), &txDatapathHcBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, hcTxChannelSel, "Invalid Tx Channel used to determine data path base address");
            goto cleanup;
        }

        /* Read hard clipper enable bit */
        recoveryAction = adrv904x_TxDatapath_PostAttenHcEn_BfGet(device,
                                                                 NULL,
                                                                 txDatapathHcBaseAddr,
                                                                 &hardClipperConfig->hardClipperEnable);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading PostAtten hard clipper enable");
            goto cleanup;
        }

        /* Read hard clipper threshold value */
        recoveryAction = adrv904x_TxDatapath_PostAttenHcThreshold_BfGet(device,
                                                                        NULL,
                                                                        txDatapathHcBaseAddr,
                                                                        &hardClipperConfig->hardClipperThreshold); /* TODO: Apply threshold transformation if needed */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading PostAtten hard clipper threshold");
            goto cleanup;
        }
    }
    else /* PreDpd hardClipper mode*/
    {
        recoveryAction = adrv904x_TxDfeDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(hcTxChannelSel), &txDfeDigHcBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, hcTxChannelSel, "Invalid Tx Channel used to determine Tx Dfe Dig base address");
            goto cleanup;
        }
       
        /* Read hard clipper enable bit */
        recoveryAction = adrv904x_TxDfeDigRegs_PreDpdHcEn_BfGet(device,
                                                                NULL,
                                                                txDfeDigHcBaseAddr,
                                                                &hardClipperConfig->hardClipperEnable);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing PreDpd hard clipper enable");
            goto cleanup;
        }
        
        /* Read hard clipper threshold value */
        recoveryAction = adrv904x_TxDfeDigRegs_PreDpdHcThreshold_BfGet(device,
                                                                       NULL,
                                                                       txDfeDigHcBaseAddr,
                                                                       &hardClipperConfig->hardClipperThreshold);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing PreDpd hard clipper threshold");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HardClipperStatisticsGet(adi_adrv904x_Device_t* const device,
                                                                       const adi_adrv904x_TxChannels_e txChannelSel,
                                                                       adi_adrv904x_HardClipperStatistics_t * const hardClipperStat,
                                                                       uint8_t clearStats)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDatapathChanAddr_e txDatapathHcBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DATAPATH;
    adrv904x_BfTxDfeDigRegsChanAddr_e txDfeDigHcBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_DFE_DIG_REGS;
    uint16_t postAttenHardClipperStat = 0;
    uint16_t preDpdHardClipperStat = 0;
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, hardClipperStat, cleanup);
    
    /* Check if clearStats parameter is valid */
    if (clearStats > ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               clearStats,
                               "clearStats value is invalid. Valid values 0-1");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_TxDfeDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txDfeDigHcBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Dfe Dig base address");
        goto cleanup;
    }
    
    /* Update PreDpdHC statistics */
    recoveryAction = adrv904x_TxDfeDigRegs_PreDpdHcCntUpd_BfSet(device,
                                                                NULL,
                                                                txDfeDigHcBaseAddr,
                                                                ADI_TRUE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while updating PreDpdHC statistics");
        goto cleanup;
    }

    /* Read PreDpd hard clipper statistics */
    recoveryAction = adrv904x_TxDfeDigRegs_PreDpdHcClipCntStat_BfGet(device,
                                                                     NULL,
                                                                     txDfeDigHcBaseAddr,
                                                                     &preDpdHardClipperStat);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks clipped at PreDpdHC");
        goto cleanup;
    }

    hardClipperStat->preDpdHardClipperStat = preDpdHardClipperStat;
    
    recoveryAction = adrv904x_CfrDatapathBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txDatapathHcBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine data path base address");
        goto cleanup;
    }
    
    /* Update PostAttenHC statistics */
    recoveryAction = adrv904x_TxDatapath_PostAttenHcCntUpd_BfSet(device,
                                                                 NULL,
                                                                 txDatapathHcBaseAddr,
                                                                 ADI_TRUE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while updating PostAttenHC statistics");
        goto cleanup;
    }
    
    /* Read PostAtten hard clipper statistics */
    recoveryAction = adrv904x_TxDatapath_PostAttenHcClipCntStat_BfGet(device,
                                                                      NULL,
                                                                      txDatapathHcBaseAddr,
                                                                      &postAttenHardClipperStat);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading peaks clipped at PostAttenHC");
        goto cleanup;
    }
    
    hardClipperStat->postAttenHardClipperStat = postAttenHardClipperStat;
    
    if (clearStats == ADI_TRUE) 
    {
        /* Clear PreDpdHC statistics */
        recoveryAction = adrv904x_TxDfeDigRegs_PreDpdHcCntClear_BfSet(device,
                                                                      NULL,
                                                                      txDfeDigHcBaseAddr,
                                                                      ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing PreDpdHC statistics");
            goto cleanup;
        }
        /* Clear PostAttenHC statistics */
        recoveryAction = adrv904x_TxDatapath_PostAttenHcCntClear_BfSet(device,
                                                                       NULL,
                                                                       txDatapathHcBaseAddr,
                                                                       ADI_TRUE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing PostAttenHC statistics");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackDataWrite(adi_adrv904x_Device_t* const device,
                                                                  const uint32_t txChannelMask,
                                                                  const uint8_t  bandSelect,
                                                                  const adi_adrv904x_TxPlaybackData_t* const playbackData)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanIdx           = 0U;
    uint32_t chanSel           = 0U;
    uint32_t i                 = 0U;
    uint32_t dataToWrite       = 0U;
    uint8_t captureEnableRead  = 0U;
    adrv904x_BfTxDigChanAddr_e txDigBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DIG;
    uint32_t dataToWriteArr[ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES];
    uint32_t txPlaybackDuc0BaseAddressArr[ADI_ADRV904X_MAX_TXCHANNELS] = {
        ADI_ADVRV904X_TX0_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX1_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX2_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX3_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX4_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX5_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX6_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX7_PLAYBACK_RAM_BASEADDR 
    };
    uint32_t txPlaybackDuc1BaseAddressArr[ADI_ADRV904X_MAX_TXCHANNELS] = {
        (ADI_ADVRV904X_TX0_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET),
        (ADI_ADVRV904X_TX1_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET),
        (ADI_ADVRV904X_TX2_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET),
        (ADI_ADVRV904X_TX3_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET),
        (ADI_ADVRV904X_TX4_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET),
        (ADI_ADVRV904X_TX5_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET),
        (ADI_ADVRV904X_TX6_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET),
        (ADI_ADVRV904X_TX7_PLAYBACK_RAM_BASEADDR + ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET) 
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, playbackData, cleanup);
    ADI_LIBRARY_MEMSET(&dataToWriteArr[0U], 0, sizeof(dataToWriteArr));

    /*Check that if requested Tx Channel valid*/
    if (((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U) ||
         (txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                              recoveryAction,
                              txChannelMask,
                              "Invalid Tx channel is selected. Valid values are any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if (bandSelect > 1) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               bandSelect,
                               "Band value cannot be greater than 1");
        goto cleanup;
    }

    if (playbackData->numSamples > ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               playbackData->numSamples,
                               "Number of samples cannot be greater than ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES");
                               goto cleanup;
    }

    if ((playbackData->numSamples % 2) != 0) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               playbackData->numSamples,
                               "Number of samples is not even");
                               goto cleanup;
    }

    for (chanIdx = 0U; chanIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++chanIdx)
    {
        chanSel = 1U << chanIdx;
        if (ADRV904X_BF_EQUAL(txChannelMask, chanSel))
        {
            recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txDigBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Tx Channel used to determine Tx Dig base address");
                goto cleanup;
            }

            /* Check if TX DPD capture is enabled before writing a new dataset*/
            recoveryAction = adrv904x_TxDig_TxDpdCptEn_BfGet(device,
                                                             NULL,
                                                             txDigBaseAddr,
                                                             &captureEnableRead);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX DPD capture state");
                goto cleanup;
            }

            if (captureEnableRead == ADI_TRUE) 
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX DPD capture should be disabled while writing to playback RAM");
                goto cleanup;
            }

            /* Check if TX FSC capture is enabled before writing a new dataset*/
            recoveryAction = adrv904x_TxDig_TxFscCptEn_BfGet(device,
                                                             NULL,
                                                             txDigBaseAddr,
                                                             &captureEnableRead);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX FSC capture state");
                goto cleanup;
            }
            
            if (captureEnableRead == ADI_TRUE) 
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX FSC capture should be disabled while writing to playback RAM");
                goto cleanup;
            }

            /* Check if TX Loopback capture is enabled before writing a new dataset*/
            recoveryAction = adrv904x_TxDig_TxLpbCptEn_BfGet(device,
                                                             NULL,
                                                             txDigBaseAddr,
                                                             &captureEnableRead);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX Loopback capture state");
                goto cleanup;
            }

            if (captureEnableRead == ADI_TRUE) 
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX Loopback capture should be disabled while writing to playback RAM");
                goto cleanup;
            }

            /* Check if TX RX ADC capture is enabled before writing a new dataset*/
            recoveryAction = adrv904x_TxDig_TxRadCptEn_BfGet(device,
                                                             NULL,
                                                             txDigBaseAddr,
                                                             &captureEnableRead);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX RX ADC capture state");
                goto cleanup;
            }

            if (captureEnableRead == ADI_TRUE) 
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX RX ADC capture should be disabled while writing to playback RAM");
                goto cleanup;
            }

            for (i = 0; i < playbackData->numSamples; ++i) 
            {
                int j;
                // swap the order of samples
                if ((i % 2) == 0)
                {
                    j = i + 1;
                }
                else
                {
                    j = i - 1;
                }

                dataToWrite = ((((uint32_t)playbackData->playbackDataReal[i] << 0U) & 0x0000FFFFU) |
                               (((uint32_t)playbackData->playbackDataImag[i] << 16U) & 0xFFFF0000U));
                
                dataToWriteArr[j] = dataToWrite;
            }

            if (bandSelect == 0)
            {
                recoveryAction = adi_adrv904x_Registers32Write(device,
                                                               NULL,
                                                               &txPlaybackDuc0BaseAddressArr[chanIdx],
                                                               dataToWriteArr,
                                                               NULL,
                                                               ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES);
            }
            else
            {
                recoveryAction = adi_adrv904x_Registers32Write(device,
                                                               NULL,
                                                               &txPlaybackDuc1BaseAddressArr[chanIdx],
                                                               dataToWriteArr,
                                                               NULL,
                                                               ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES);
            }

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Write Issue");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackDataRead(adi_adrv904x_Device_t* const device,
                                                                 const adi_adrv904x_TxChannels_e txChannelSel,
                                                                 const uint8_t  bandSelect,
                                                                 adi_adrv904x_TxPlaybackData_t* const playbackData)
{
        adi_adrv904x_ErrAction_e recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t i                 = 0U;
    uint32_t baseAddrToRead    = 0U;
    uint8_t captureEnableRead  = 0U;
    adrv904x_BfTxDigChanAddr_e txDigBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DIG;
    uint32_t dataToReadArr[ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES];

    uint32_t txPlaybackBaseAddressArr[ADI_ADRV904X_MAX_TXCHANNELS] = {
        ADI_ADVRV904X_TX0_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX1_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX2_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX3_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX4_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX5_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX6_PLAYBACK_RAM_BASEADDR,
        ADI_ADVRV904X_TX7_PLAYBACK_RAM_BASEADDR 
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, playbackData, cleanup);
    ADI_LIBRARY_MEMSET(&dataToReadArr[0U], 0, sizeof(dataToReadArr));

    if (bandSelect > 1) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               bandSelect,
                               "Band value cannot be greater than 1");
        goto cleanup;
    }

    /*Check that if requested Tx Channel valid*/
    switch (txChannelSel) 
    {
    case ADI_ADRV904X_TX0:
        baseAddrToRead = txPlaybackBaseAddressArr[0];
        break;
    case ADI_ADRV904X_TX1:
        baseAddrToRead = txPlaybackBaseAddressArr[1];
        break;
    case ADI_ADRV904X_TX2:
        baseAddrToRead = txPlaybackBaseAddressArr[2];
        break;
    case ADI_ADRV904X_TX3:
        baseAddrToRead = txPlaybackBaseAddressArr[3];
        break;
    case ADI_ADRV904X_TX4:
        baseAddrToRead = txPlaybackBaseAddressArr[4];
        break;
    case ADI_ADRV904X_TX5:
        baseAddrToRead = txPlaybackBaseAddressArr[5];
        break;
    case ADI_ADRV904X_TX6:
        baseAddrToRead = txPlaybackBaseAddressArr[6];
        break;
    case ADI_ADRV904X_TX7:
        baseAddrToRead = txPlaybackBaseAddressArr[7];
        break;
    default:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, baseAddrToRead, "Invalid Tx Channel used to determine TX playback address");
        goto cleanup;
        break;
    }
    
    if (bandSelect == 1)
    {
        baseAddrToRead += ADI_ADVRV904X_TX_DUC1_PLAYBACK_RAM_OFFSET;
    }

    ADI_LIBRARY_MEMSET(playbackData->playbackDataReal, 0, sizeof(playbackData->playbackDataReal));
    ADI_LIBRARY_MEMSET(playbackData->playbackDataImag, 0, sizeof(playbackData->playbackDataImag));

    recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txDigBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Dfe base address");
        goto cleanup;
    }

    /* Check if TX DPD capture is enabled before reading a dataset*/
    recoveryAction = adrv904x_TxDig_TxDpdCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX DPD capture state");
        goto cleanup;
    }
            
    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX DPD capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    /* Check if TX FSC capture is enabled before reading a dataset*/
    recoveryAction = adrv904x_TxDig_TxFscCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX FSC capture state");
        goto cleanup;
    }

    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX FSC capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    /* Check if TX Loopback capture is enabled before reading a dataset*/
    recoveryAction = adrv904x_TxDig_TxLpbCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX Loopback capture state");
        goto cleanup;
    }

    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX Loopback capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    /* Check if TX RX ADC capture is enabled before reading a dataset*/
    recoveryAction = adrv904x_TxDig_TxRadCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX RX ADC capture state");
        goto cleanup;
    }

    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX RX ADC capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Registers32Read(device,
                                                  NULL,
                                                  baseAddrToRead,
                                                  dataToReadArr,
                                                  NULL,
                                                  ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Register Read Issue");
        goto cleanup;
    }

    for (i = 0; i < ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES; ++i) 
    {
        // swap the order of samples
        int j;
        if ((i % 2) == 0)
        {
            j = i + 1;
        }
        else
        {
            j = i - 1;
        }

        playbackData->playbackDataReal[i] =  (int16_t)((dataToReadArr[j] >> 0U) & 0x0000FFFFU);
        playbackData->playbackDataImag[i] =  (int16_t)((dataToReadArr[j] >> 16U) & 0x0000FFFFU);
    }
    playbackData->numSamples = ADI_ADRV904X_TX_PLAYBACK_DATA_MAX_NUM_SAMPLES;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackStart(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_TxChannels_e txChannelSel,
                                                              const uint8_t  bandSelect,
                                                              const uint32_t dataRate_kHz)
{
        adi_adrv904x_ErrAction_e recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t captureEnableRead  = 0U;
    uint8_t radClkDiv          = 0U; 
    uint8_t radClkDivLog2      = 0U;
    adrv904x_BfTxDigChanAddr_e txDigBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DIG;
    adrv904x_BfTxBandDucChanAddr_e txBandDucBaseAddrDuc0 = ADRV904X_BF_SLICE_TX_0__TX_BAND_DUC0;
    adrv904x_BfTxBandDucChanAddr_e txBandDucBaseAddrDuc1 = ADRV904X_BF_SLICE_TX_0__TX_BAND_DUC0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (bandSelect > 1) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               bandSelect,
                               "Band value cannot be greater than 1");
        goto cleanup;
    }

    /* Get the base address for DUC0. This function checks the provided TX channel */
    recoveryAction = adrv904x_TxBandDuc0BitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txBandDucBaseAddrDuc0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Band DUC0 base address");
        goto cleanup;
    }

    /* Get the base address for DUC1. This function checks the provided TX channel */
    recoveryAction = adrv904x_TxBandDuc1BitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txBandDucBaseAddrDuc1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Band DUC1 base address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txDigBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Dig base address");
        goto cleanup;
    }

    /* Check if TX DPD capture is enabled before starting a playback*/
    recoveryAction = adrv904x_TxDig_TxDpdCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX DPD capture state");
        goto cleanup;
    }
            
    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX DPD capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    /* Check if TX FSC capture is enabled before before starting a playback*/
    recoveryAction = adrv904x_TxDig_TxFscCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX FSC capture state");
        goto cleanup;
    }

    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX FSC capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    /* Check if TX Loopback capture is enabled before before starting a playback*/
    recoveryAction = adrv904x_TxDig_TxLpbCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX Loopback capture state");
        goto cleanup;
    }

    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX Loopback capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    /* Check if TX RX ADC capture is enabled before before starting a playback*/
    recoveryAction = adrv904x_TxDig_TxRadCptEn_BfGet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     &captureEnableRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading TX RX ADC capture state");
        goto cleanup;
    }

    if (captureEnableRead == ADI_TRUE) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX RX ADC capture should be disabled while reading from playback RAM");
        goto cleanup;
    }

    if (device->devStateInfo.hsDigClk_kHz == 0U)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid HS DIG clock frequency");
        goto cleanup;
    }

    if ((device->devStateInfo.hsDigClk_kHz < dataRate_kHz) || (device->devStateInfo.hsDigClk_kHz % dataRate_kHz))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid data rate");
        goto cleanup;
    }

    if(bandSelect == 0)
    {
        /* Set the DUC0 Input to RAM Playback input */
        recoveryAction = adrv904x_TxBandDuc_DucInputSelect_BfSet(device,
                                                                 NULL,
                                                                 txBandDucBaseAddrDuc0,
                                                                 0x4U); /* TxAd-hocTesting(RAM Playback) input */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting DUC0 Input");
            goto cleanup;
        }

        /* Mask Input I data for DUC1 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputI_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc1,
                                                                0x1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking I data for DUC1");
            goto cleanup;
        }

        /* Mask Input Q data for DUC1 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputQ_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc1,
                                                                0x1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking Q data for DUC1");
            goto cleanup;
        }
    }
    else
    {
        /* Set the DUC1 Input to RAM Playback input */
        recoveryAction = adrv904x_TxBandDuc_DucInputSelect_BfSet(device,
                                                                 NULL,
                                                                 txBandDucBaseAddrDuc1,
                                                                 0x4U); /* TxAd-hocTesting(RAMPlayback) input */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting DUC0 Input");
            goto cleanup;
        }

        /* Mask Input I data for DUC0 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputI_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc0,
                                                                0x1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking I data for DUC0");
            goto cleanup;
        }

        /* Mask Input Q data for DUC0 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputQ_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc0,
                                                                0x1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking Q data for DUC0");
            goto cleanup;
        }
    }

    radClkDiv = device->devStateInfo.hsDigClk_kHz / dataRate_kHz;
    while (((radClkDiv) >>= 1) > 0U)
    {
        ++radClkDivLog2;
    }

    /* Program the SRAM playback clock (rad_clk) */
    recoveryAction = adrv904x_TxDig_RadClkDivideRatio_BfSet(device,
                                                             NULL,
                                                             txDigBaseAddr,
                                                             radClkDivLog2);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RAD Clock Divide Ratio");
        goto cleanup;
    }

    /* Enable the RAD Clock */
    recoveryAction = adrv904x_TxDig_RadClkEnable_BfSet(device,
                                                       NULL,
                                                       txDigBaseAddr,
                                                       1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RAD Clock Enable");
        goto cleanup;
    }

    /* Set tx_dig.tx_rad_cpt_mode to 8 */
    recoveryAction = adrv904x_TxDig_TxRadCptMode_BfSet(device,
                                                       NULL,
                                                       txDigBaseAddr,
                                                       8U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RAD CPT mode bitfield");
        goto cleanup;
    }

    /* Set the tx_rad_cpt_en bitfield to 1 */
    recoveryAction = adrv904x_TxDig_TxRadCptEn_BfSet(device,
                                                     NULL,
                                                     txDigBaseAddr,
                                                     1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting TX RX ADC capture state");
        goto cleanup;
    }

    /* To start playing back the data from memory, set cpt_trigger bitfield.  */
    recoveryAction = adrv904x_TxDig_TxCptTrigger_BfSet(device,
                                                       NULL,
                                                       txDigBaseAddr,
                                                       1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting capture trigger");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxPlaybackStop(adi_adrv904x_Device_t* const device, 
                                                             const adi_adrv904x_TxChannels_e txChannelSel,
                                                             const uint8_t  bandSelect)
{
        adi_adrv904x_ErrAction_e recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxDigChanAddr_e txDigBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DIG;
    adrv904x_BfTxBandDucChanAddr_e txBandDucBaseAddrDuc0 = ADRV904X_BF_SLICE_TX_0__TX_BAND_DUC0;
    adrv904x_BfTxBandDucChanAddr_e txBandDucBaseAddrDuc1 = ADRV904X_BF_SLICE_TX_0__TX_BAND_DUC0;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (bandSelect > 1) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               bandSelect,
                               "Band value cannot be greater than 1");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxBandDuc0BitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txBandDucBaseAddrDuc0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Band DUC0 base address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxBandDuc1BitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txBandDucBaseAddrDuc1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Band DUC1 base address");
        goto cleanup;
    }

    recoveryAction = adrv904x_TxDigBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(txChannelSel), &txDigBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannelSel, "Invalid Tx Channel used to determine Tx Dig base address");
        goto cleanup;
    }

    if (bandSelect == 0)
    {
        /* Set the DUC0 Input to default (JESD data) */
        recoveryAction = adrv904x_TxBandDuc_DucInputSelect_BfSet(device,
                                                                 NULL,
                                                                 txBandDucBaseAddrDuc0,
                                                                 0x0U); /* JESD data - default setting */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting DUC0 Input");
            goto cleanup;
        }

        /* Unmask Input I data for DUC1 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputI_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc1,
                                                                0x0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking I data for DUC1");
            goto cleanup;
        }

        /* Unmask Input Q data for DUC1 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputQ_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc1,
                                                                0x0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking Q data for DUC1");
            goto cleanup;
        }
    }
    else
    {
        /* Set the DUC1 Input to default (JESD data) */
        recoveryAction = adrv904x_TxBandDuc_DucInputSelect_BfSet(device,
                                                                 NULL,
                                                                 txBandDucBaseAddrDuc1,
                                                                 0x0U); /* JESD data - default setting */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting DUC0 Input");
            goto cleanup;
        }

        /* Unmask Input I data for DUC0 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputI_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc0,
                                                                0x0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking I data for DUC0");
            goto cleanup;
        }

        /* Unmask Input Q data for DUC0 */
        recoveryAction = adrv904x_TxBandDuc_MaskDucInputQ_BfSet(device,
                                                                NULL,
                                                                txBandDucBaseAddrDuc0,
                                                                0x0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while unmasking Q data for DUC0");
            goto cleanup;
        }
    }

    /* Set tx_dig.tx_rad_cpt_mode to 9 */
    recoveryAction = adrv904x_TxDig_TxRadCptMode_BfSet(device,
                                                       NULL,
                                                       txDigBaseAddr,
                                                       9U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RAD CPT mode");
        goto cleanup;
    }

    /* Set the tx_rad_cpt_en bitfield to 0 */
    recoveryAction = adrv904x_TxDig_TxRadCptEn_BfSet(device,
        NULL,
        txDigBaseAddr,
        0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting TX RX ADC capture state");
        goto cleanup;
    }

    /* Clear cpt_trigger bitfield */
    recoveryAction = adrv904x_TxDig_TxCptTrigger_BfSet(device,
        NULL,
        txDigBaseAddr,
        0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting TX RX ADC capture state");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDynamicCarrierReconfigureWithFilterSelect(  adi_adrv904x_Device_t* const                                device,
                                                                                            adi_adrv904x_CarrierJesdCfg_t* const                        jesdCfg,
                                                                                            adi_adrv904x_CarrierRadioCfg_t                              txCarrierConfigs[],
                                                                                            const adi_adrv904x_CarrierChannelFilterApplicationSel_t     txCarrierChannelFilterApplicationSel[],
                                                                                            const uint32_t                                              numCarrierProfiles )
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ChannelFilterCfg_t txCarrierChannelFilter[ADI_ADRV904X_MAX_NUM_PROFILES];
    uint32_t carrierIdx = 0U;
    uint32_t profileIdx = 0U;
    uint32_t coeffIdx = 0U;
    int32_t coeffTableIdx = 0U;
    const int16_t *coeffTable = NULL;
    int16_t coeffTableSize = 0;
    uint32_t numberOfFilterTaps = 0U;
    uint8_t asymmetricFilterTaps = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierConfigs, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierChannelFilterApplicationSel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, jesdCfg, cleanup);
    
    ADI_LIBRARY_MEMSET(&txCarrierChannelFilter, 0, sizeof(txCarrierChannelFilter));
 
    if (numCarrierProfiles > ADI_ADRV904X_MAX_NUM_PROFILES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numCarrierProfiles, "Up to four profiles allowed.");
        goto cleanup;
    }
    
    for (profileIdx = 0U; profileIdx < numCarrierProfiles; profileIdx++)
    {
        /* Reset coefficient index for each profile */
        coeffIdx = 0U;
        
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            if (txCarrierConfigs[profileIdx].carriers[carrierIdx].enable == 1U)
            {
                recoveryAction = adrv904x_ChannelFilterCoefsGet(    device,
                                                                    txCarrierChannelFilterApplicationSel[profileIdx].channelFilterApplicationSel[carrierIdx],
                                                                    &txCarrierConfigs[profileIdx].carriers[carrierIdx],
                                                                    &coeffTable,
                                                                    &coeffTableSize,
                                                                    &numberOfFilterTaps,
                                                                    &asymmetricFilterTaps,
                                                                    ADI_FALSE);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting carrier chanel filter");
                    goto cleanup;
                }
        
                for (coeffTableIdx = 0U; coeffTableIdx < coeffTableSize; coeffTableIdx++)
                {
                    txCarrierChannelFilter[profileIdx].coeffs[coeffIdx] = coeffTable[coeffTableIdx];
                    coeffIdx++;
                    if (coeffIdx >= ADI_ADRV904X_NUM_CF_COEFFICIENTS)
                    {
                        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, coeffIdx, "Carrier coefficients did not fit in the space provided");
                        goto cleanup;
                    }
                }
                txCarrierChannelFilter[profileIdx].carrierFilterCfg.numberOfFilterTaps[carrierIdx] = numberOfFilterTaps;
                txCarrierChannelFilter[profileIdx].carrierFilterCfg.asymmetricFilterTaps[carrierIdx] = asymmetricFilterTaps; 
            }
        }
    }
    
    recoveryAction = adi_adrv904x_TxDynamicCarrierReconfigure( device,
                                                                jesdCfg,
                                                                &txCarrierConfigs[0],
                                                                &txCarrierChannelFilter[0],
                                                                numCarrierProfiles);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
        goto cleanup;
    }

cleanup:

    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxDynamicCarrierReconfigure(  adi_adrv904x_Device_t* const            device,
                                                                            adi_adrv904x_CarrierJesdCfg_t* const    jesdCfg,
                                                                            adi_adrv904x_CarrierRadioCfg_t          txCarrierConfigs[],
                                                                            adi_adrv904x_ChannelFilterCfg_t         txCarrierChannelFilter[],
                                                                            const uint32_t                          numCarrierProfiles)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    char coeffLoadBuf[sizeof(adi_adrv904x_ChannelFilterLoadMaxSize_t)];
    adi_adrv904x_ChannelFilterLoad_t* const channelFilterLoad = (adi_adrv904x_ChannelFilterLoad_t*)&coeffLoadBuf;
    adrv904x_CarrierDynamicReconfigInternalCfg_t txCarrierInternalCfg;
    adi_adrv904x_ChannelFilterResp_t chFilterCmdRsp;
    adi_adrv904x_ChannelFilterLoadResp_t chFilterLoadCmdRsp;
    uint32_t txIdx = 0U;
    uint32_t bandIdx = 0U;
    uint32_t txSel = 0U;
    uint32_t carrierIdx = 0U;
    uint32_t coeffLoadIdx = 0U;
    uint32_t profileIdx = 0U;
    uint32_t coeffAddressOffset = 0U;
    uint32_t coeffIdx = 0U;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adi_adrv904x_RxTxLoFreqReadback_t rxTxLoReadback;
    adi_adrv904x_TxNcoMixConfigReadbackResp_t txRbConfig;
    uint32_t currentBandCenter_kHz = 0U;
    adrv904x_CarrierDynamicReconfigProfileCfg_t* pProfileCfg = NULL;
    adrv904x_CarrierInitialCfg_t* pInitialCfg = NULL;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierConfigs, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierChannelFilter, cleanup);

    ADI_LIBRARY_MEMSET(&chFilterLoadCmdRsp, 0, sizeof(chFilterLoadCmdRsp));
    ADI_LIBRARY_MEMSET(&chFilterCmdRsp, 0, sizeof(chFilterCmdRsp));
    ADI_LIBRARY_MEMSET(&rxTxLoReadback, 0, sizeof(rxTxLoReadback));
    ADI_LIBRARY_MEMSET(&txRbConfig, 0, sizeof(txRbConfig));
 
    if (numCarrierProfiles > ADI_ADRV904X_MAX_NUM_PROFILES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, numCarrierProfiles, "Up to four profiles allowed.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_InternalReconfigStructInit(   device,
                                                            &txCarrierInternalCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Could not initialize struct");
        goto cleanup;
    }
    
    recoveryAction = adi_adrv904x_RxTxLoFreqGet(device,
                                                &rxTxLoReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RxTxLoFreqGet failed");
        goto cleanup;
    }
    
    for (profileIdx = 0U; profileIdx < numCarrierProfiles; profileIdx++)
    {
        pProfileCfg = &(txCarrierInternalCfg.profileCfgs[profileIdx]);
        pInitialCfg = &(pProfileCfg->initialCfg);
        
        if ((txCarrierConfigs[profileIdx].channelMask == 0U) ||
            ((txCarrierConfigs[profileIdx].channelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierConfigs[profileIdx].channelMask, "Invalid chanSelect provided.");
            goto cleanup;
        }
        
        for (txIdx = 0U; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; txIdx++)
        {
            txSel = 1U << txIdx;
            if ((txCarrierConfigs[profileIdx].channelMask & txSel) > 0U)
            {
                for (bandIdx = 0U; bandIdx < ADI_ADRV904X_DUC_NUM_BAND; bandIdx++)
                {
                    if (device->initExtract.tx.txChannelCfg[txIdx].bandSettings[bandIdx].enabled == ADI_TRUE)
                    {
                        ADI_LIBRARY_MEMSET(&txRbConfig, 0, sizeof(txRbConfig));
                        
                        txRbConfig.bandSelect = bandIdx;
                        txRbConfig.chanSelect = txSel;
    
                        recoveryAction = adi_adrv904x_TxNcoShifterGet(  device, &txRbConfig);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_TxNcoShifterGet failed");
                            goto cleanup;
                        }
                        
                        currentBandCenter_kHz = rxTxLoReadback.txFreq_Khz[txIdx] + txRbConfig.frequencyKhz; 
                        if (pInitialCfg->bandSettings[bandIdx].rfCenterFreq_kHz == 0U)
                        {
                            /* Copy all settings and overwrite centerFreq with current NCO values */
                            ADI_LIBRARY_MEMCPY( &(pInitialCfg->bandSettings[bandIdx]), 
                                                &(device->initExtract.tx.txChannelCfg[txIdx].bandSettings[bandIdx]), 
                                                sizeof(pInitialCfg->bandSettings[bandIdx]));
                            pInitialCfg->bandSettings[bandIdx].rfCenterFreq_kHz = currentBandCenter_kHz;
                            pInitialCfg->maxSlot = device->initExtract.tx.txChannelCfg[txIdx].maxSlot;
                        }
                        else if (pInitialCfg->bandSettings[bandIdx].rfCenterFreq_kHz != currentBandCenter_kHz)
                        {
                            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txCarrierConfigs[profileIdx].channelMask, "Each channel in a single profile must have matching band center frequencies, e.g. all NCOs in a single profile must be the same value.");
                            goto cleanup;
                        }
                    }
                }
            }
        } 
        
        recoveryAction = adrv904x_TxCarrierBandSorting( device,
                                                        &txCarrierConfigs[profileIdx],
                                                        pProfileCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid band selection");
            goto cleanup;
        }
            
        recoveryAction = adrv904x_TxCarrierBandAttenConfig( device,
                                                            &txCarrierConfigs[profileIdx],
                                                            pProfileCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid band atten settings");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_TxCarrierNcoReconfig( device,
                                                        &txCarrierConfigs[profileIdx],
                                                        pProfileCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid NCO settings");
            goto cleanup;
        }
             
        recoveryAction = adrv904x_CarrierJesdParametersCalculate(   device,
                                                                    &txCarrierConfigs[profileIdx],
                                                                    pProfileCfg,
                                                                    ADI_FALSE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid JESD settings");
            goto cleanup;
        }    
		
		recoveryAction = adrv904x_CarrierTxJesdConfigSet(   device,
															txCarrierConfigs[profileIdx].channelMask,
															pProfileCfg);
		if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
		{
			ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
			goto cleanup;
		}
    
        /* Reset the buffer each profile. channelFilterLoad maps to this buffer */
        ADI_LIBRARY_MEMSET(&coeffLoadBuf, 0, sizeof(coeffLoadBuf));
    
        /* Perform endianness correction and load channel filter coefficients */
        channelFilterLoad->coeffIdx = 0U;
        for (coeffIdx = 0U; coeffIdx < ADI_ADRV904X_NUM_CF_COEFFICIENTS; coeffIdx++)
        {
            coeffAddressOffset = coeffLoadIdx * sizeof(int16_t);
            ADI_LIBRARY_MEMCPY((void*)((uint8_t*)channelFilterLoad + sizeof(adi_adrv904x_ChannelFilterLoad_t) + coeffAddressOffset), (void*)(&txCarrierChannelFilter[profileIdx].coeffs[coeffIdx]), sizeof(int16_t));
            coeffLoadIdx++;
        
            /* Once we fill up enough coefficients or we're on the last coefficient send to fw and clear out the buffer */
            if ((coeffLoadIdx == ADI_ADRV904X_CHAN_FILTER_COEFF_LOAD_LEN) || (coeffIdx == (ADI_ADRV904X_NUM_CF_COEFFICIENTS - 1U)))
            {
                /* Using coeffLoadIdx after incremented to give us the number of coefficients actually loaded */
                channelFilterLoad->coeffNum = (uint16_t)ADRV904X_HTOCL(coeffLoadIdx);
    
                /* Send command and receive response */
                recoveryAction = adrv904x_CpuCmdSend(   device,
                                                        ADI_ADRV904X_CPU_TYPE_0,
                                                        ADRV904X_LINK_ID_0,
                                                        ADRV904X_CPU_CMD_ID_LOAD_CHANNEL_FILTER_COEFFS,
                                                        (void*)channelFilterLoad,
                                                        sizeof(coeffLoadBuf),
                                                        (void*)&chFilterLoadCmdRsp,
                                                        sizeof(chFilterLoadCmdRsp),
                                                        &cmdStatus);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)chFilterLoadCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
                }
            
                ADI_LIBRARY_MEMSET(&coeffLoadBuf, 0, sizeof(coeffLoadBuf));
            
                channelFilterLoad->coeffIdx = coeffIdx + 1;
                coeffLoadIdx = 0U;
            }
        }
        
        /* Perform endianness correction and load channel filter configs */
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            txCarrierChannelFilter[profileIdx].carrierFilterCfg.numberOfFilterTaps[carrierIdx] = (uint32_t)ADRV904X_HTOCL(txCarrierChannelFilter[profileIdx].carrierFilterCfg.numberOfFilterTaps[carrierIdx]);
            txCarrierChannelFilter[profileIdx].carrierFilterCfg.asymmetricFilterTaps[carrierIdx] = txCarrierChannelFilter[profileIdx].carrierFilterCfg.asymmetricFilterTaps[carrierIdx];
        }
    
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_LOAD_CHANNEL_FILTER_CFG,
                                                (void*)&(txCarrierChannelFilter[profileIdx].carrierFilterCfg),
                                                sizeof(txCarrierChannelFilter[profileIdx].carrierFilterCfg),
                                                (void*)&chFilterLoadCmdRsp,
                                                sizeof(chFilterLoadCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)chFilterLoadCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        
        /* Endianness correction */
        txCarrierConfigs[profileIdx].channelMask = (uint32_t)ADRV904X_HTOCL(txCarrierConfigs[profileIdx].channelMask);
    
        for (carrierIdx = 0U; carrierIdx < ADI_ADRV904X_MAX_CARRIERS; carrierIdx++)
        {
            txCarrierConfigs[profileIdx].carriers[carrierIdx].enable = txCarrierConfigs[profileIdx].carriers[carrierIdx].enable;
            txCarrierConfigs[profileIdx].carriers[carrierIdx].sampleRate_kHz = (uint32_t)ADRV904X_HTOCL(txCarrierConfigs[profileIdx].carriers[carrierIdx].sampleRate_kHz);
            txCarrierConfigs[profileIdx].carriers[carrierIdx].ibw_kHz = (uint32_t)ADRV904X_HTOCL(txCarrierConfigs[profileIdx].carriers[carrierIdx].ibw_kHz);
            txCarrierConfigs[profileIdx].carriers[carrierIdx].centerFrequency_kHz = (uint32_t)ADRV904X_HTOCL(txCarrierConfigs[profileIdx].carriers[carrierIdx].centerFrequency_kHz);
        }
    
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_TX_CARRIER_RECONFIGURE,
                                                (void*)&txCarrierConfigs[profileIdx],
                                                sizeof(adi_adrv904x_CarrierRadioCfg_t),
                                                (void*)&chFilterCmdRsp,
                                                sizeof(chFilterCmdRsp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO((adrv904x_CpuErrorCode_e)chFilterCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }

        recoveryAction = adrv904x_CducDelayCalculate(   device,
                                                        &txCarrierConfigs[profileIdx],
                                                        pProfileCfg,
                                                        &chFilterCmdRsp.carrierFilterOutCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with delay calculation");
            goto cleanup;
        }

        for (txIdx = 0U; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; txIdx++)
        {
            txSel = 1U << txIdx;
            if ((txCarrierConfigs[profileIdx].channelMask & txSel) > 0U)
            {
                recoveryAction = adrv904x_TxCarrierConfigSet(   device,
                                                                txIdx,
                                                                pProfileCfg);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
                    goto cleanup;
                }
            
	            /* TPGSWE-7944: Skip following write due to BfSet bugs */

                /* Note: The following API TxCarrierDelaySet() is reported not working. 
                recoveryAction = adrv904x_TxCarrierDelaySet(    device,
                                                                txIdx,
                                                                &txCarrierInternalCfg.internalCfg.delayCfg);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring delay params");
                    goto cleanup;
                }
                */
            
                /* Update stored carrier settings */
                ADI_LIBRARY_MEMCPY(&device->initExtract.txCarrierConfigs[txIdx], &txCarrierConfigs[profileIdx], sizeof(adi_adrv904x_CarrierRadioCfg_t));
                ADI_LIBRARY_MEMCPY(&device->initExtract.tx.txChannelCfg[txIdx].carrierRuntimeSettings, &(pProfileCfg->carrierCfgs), sizeof(adi_adrv904x_CarrierRuntime_t));
            }
        }
    }
    
    recoveryAction = adrv904x_CalculateTxSampleXbarSlotConfig(device, jesdCfg, &txCarrierInternalCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        return recoveryAction;
    }
	
    recoveryAction = adrv904x_TxJesdConfigSet(  device,
												&txCarrierInternalCfg);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
	{
		ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reconfiguring carriers");
		goto cleanup;
	}
	
	/* Update stored JESD settings */
	ADI_LIBRARY_MEMCPY(&device->initExtract.txCarrierJesdCfg, jesdCfg, sizeof(adi_adrv904x_CarrierJesdCfg_t));

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierSettingsGet( adi_adrv904x_Device_t* const    device,
                                                                    const uint32_t txChannel,
                                                                    adi_adrv904x_CarrierRadioCfg_t* const txCarrierConfigs,
                                                                    adi_adrv904x_CarrierRuntime_t* const carrierRuntimeSettings)
{
    
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierConfigs, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, carrierRuntimeSettings, cleanup);
    
    ADI_LIBRARY_MEMCPY(txCarrierConfigs, &device->initExtract.txCarrierConfigs[txChannel], sizeof(adi_adrv904x_CarrierRadioCfg_t));
    ADI_LIBRARY_MEMCPY(carrierRuntimeSettings, &device->initExtract.tx.txChannelCfg[txChannel].carrierRuntimeSettings, sizeof(adi_adrv904x_CarrierRuntime_t));
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierJesdCfgGet(  adi_adrv904x_Device_t* const    device,
                                                                    adi_adrv904x_CarrierJesdCfg_t* const txCarrierJesdCfg)
{
    
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierJesdCfg, cleanup);
    
    ADI_LIBRARY_MEMCPY(txCarrierJesdCfg, &device->initExtract.txCarrierJesdCfg, sizeof(adi_adrv904x_CarrierJesdCfg_t));
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxCarrierLatencyGet(  adi_adrv904x_Device_t* const device,
                                                                    const adi_adrv904x_TxChannels_e txChannel,
                                                                    adi_adrv904x_CarrierReconfigLatencyCfg_t* const txCarrierLatencyCfg)
{

        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txCarrierLatencyCfg, cleanup);

    if ((txChannel != ADI_ADRV904X_TX0) &&
        (txChannel != ADI_ADRV904X_TX1) &&
        (txChannel != ADI_ADRV904X_TX2) &&
        (txChannel != ADI_ADRV904X_TX3) &&
        (txChannel != ADI_ADRV904X_TX4) &&
        (txChannel != ADI_ADRV904X_TX5) &&
        (txChannel != ADI_ADRV904X_TX6) &&
        (txChannel != ADI_ADRV904X_TX7))
    {
        /* Invalid Channel selection */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel Selection");
        goto cleanup;
    }


    ADI_LIBRARY_MEMCPY((adi_adrv904x_CarrierReconfigLatencyCfg_t*)txCarrierLatencyCfg,
                       (adi_adrv904x_CarrierReconfigLatencyCfg_t*)&(device->devStateInfo.txCarrierLatencyCfg[adrv904x_TxChannelsToId(txChannel)]),
                       sizeof(adi_adrv904x_CarrierReconfigLatencyCfg_t));

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
