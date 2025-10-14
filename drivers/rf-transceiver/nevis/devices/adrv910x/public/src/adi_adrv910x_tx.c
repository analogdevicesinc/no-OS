/**
* \file
* \brief Contains Tx features related function implementation defined in
* adi_adrv910x_tx.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv910x_user.h"

#include "adi_adrv910x.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_spi.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_tx.h"
#include "adi_adrv910x_dpd.h"
/* #include "adi_adrv910x_dpd.h" */

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adi_common_macros.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_bf.h"

/* System header files */
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/math64.h>
#else
#include <stdlib.h>
#include <math.h>
#endif

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Tx_AttenuationTable_Write_Validate(adi_adrv910x_Device_t *device,
                                                                              uint32_t channelMask,
                                                                              uint32_t indexOffset,
                                                                              adi_adrv910x_TxAttenTableRow_t attenTableRows[],
                                                                              uint32_t arraySize)
{
    static const uint8_t ATTEN_HP_MIN = 0;
    static const uint8_t ATTEN_HP_MAX = 63;
    static const uint16_t ATTEN_MULT_MIN = 0;
    static const uint16_t ATTEN_MULT_MAX = 4095;
    uint16_t idx = 0;

    /* Check that the index offset is within range */
    ADI_RANGE_CHECK(device, indexOffset, 0, ADRV910X_TX_ATTEN_TABLE_MAX);

    /* Check for a valid array */
    ADI_ENTRY_PTR_ARRAY_EXPECT(device, attenTableRows, arraySize);

    /* Check that the entire array fits */
    if (arraySize > ((ADRV910X_TX_ATTEN_TABLE_MAX - indexOffset)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid parameter value. arraySize must be less than ADRV910X_TX_ATTEN_TABLE_MAX - indexOffset");
    }

    /* Check that Tx Profile is valid */
    if ((device->devStateInfo.profilesValid & ADI_ADRV910X_TX_PROFILE_VALID) == 0)
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

int32_t adi_adrv910x_Tx_AttenuationTable_Write(adi_adrv910x_Device_t *device,
                                               uint32_t channelMask,
                                               uint32_t indexOffset,
                                               adi_adrv910x_TxAttenTableRow_t attenTableRows[],
                                               uint32_t arraySize)
{
    static const uint32_t ATTEN_HP_MASK = 0x0003F000;
    static const uint32_t ATTEN_HP_SHIFT = 12;
    static const uint32_t ATTEN_MULT_MASK = 0x00000FFF;
    static const uint32_t ATTEN_MULT_SHIFT = 0;

	uint32_t baseAddr = ADRV910X_ADDR_TX_ATTEN_TABLE;
    uint32_t txAttenEntry = 0;
    uint32_t start = 0;
    uint32_t stop = 0;
    uint32_t data = 0;
    uint32_t offset = 0;

    /* FIXME: Get rid of CACHE_MAX and +10 */
    uint8_t  cfgData[ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX + 10] = { 0 };
    static const uint8_t TX_ENTRY_SIZE = 4;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Tx_AttenuationTable_Write_Validate, device, channelMask, indexOffset, attenTableRows, arraySize);

    ADI_MUTEX_AQUIRE(device);

    /* Enable ARM clock to access attenuation table memory */
	ADI_EXPECT(adrv910x_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, true);
    ADI_EXPECT(adrv910x_NvsRegmapCore2_Tx1GainTableClkEn_Set, device, true);

	if (ADRV910X_BF_EQUAL(channelMask, ADI_ADRV910X_TX1) | ADRV910X_BF_EQUAL(channelMask, ADI_ADRV910X_TXNB))
	{
		/* TODO: Refactor this bitwise logic to a Format function a la RxGainTableFormat */
		offset = (indexOffset * TX_ENTRY_SIZE) + baseAddr;
		for (txAttenEntry = 0; txAttenEntry < arraySize; txAttenEntry++)
		{
			data = ADRV910X_BF_ENCODE((uint32_t)(attenTableRows[txAttenEntry].txAttenHp), ATTEN_HP_MASK, ATTEN_HP_SHIFT);
			data |= ADRV910X_BF_ENCODE((uint32_t)(attenTableRows[txAttenEntry].txAttenMult), ATTEN_MULT_MASK, ATTEN_MULT_SHIFT);

			cfgData[start + 0] = (uint8_t)(data & 0x000000FF);
			cfgData[start + 1] = (uint8_t)((data >> 8) & 0x000000FF);
			cfgData[start + 2] = (uint8_t)((data >> 16) & 0x000000FF);
			cfgData[start + 3] = (uint8_t)((data >> 24) & 0x000000FF);

			start += TX_ENTRY_SIZE;
			if (start >= ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX)
			{
				ADI_EXPECT(adrv910x_DmaMemWrite, device, offset + stop, &cfgData[0], start, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_CACHED_BYTES_N, ADI_PS1);

				stop += start;
				start = 0;
			}
		}

		if (start > 0)
		{
			ADI_EXPECT(adrv910x_DmaMemWrite, device, offset + stop, &cfgData[0], start, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
		}
	}

	ADI_EXPECT(adrv910x_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, false);

	/* Initialize TX Attenuation Mode */
	device->devStateInfo.txAttenMode = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI;

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Tx_AttenuationTable_Read_Validate(adi_adrv910x_Device_t *device,
                                                                             uint32_t indexOffset,
                                                                             adi_adrv910x_TxAttenTableRow_t attenTableRows[],
                                                                             uint32_t arraySize)
{
    ADI_RANGE_CHECK(device, indexOffset, 0, ADRV910X_TX_ATTEN_TABLE_MAX);

    ADI_NULL_PTR_RETURN(&device->common, attenTableRows);

    ADI_ENTRY_PTR_ARRAY_EXPECT(device, attenTableRows, arraySize);
    if (arraySize > ((ADRV910X_TX_ATTEN_TABLE_MAX - indexOffset)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         arraySize,
                         "Invalid parameter value. arraySize must be less than ADRV910X_TX_ATTEN_TABLE_MAX - indexOffset");
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_AttenuationTable_Read(adi_adrv910x_Device_t *device,
                                              uint32_t indexOffset,
                                              adi_adrv910x_TxAttenTableRow_t attenTableRows[],
                                              uint32_t arraySize)
{
    static const uint32_t ATTEN_HP_MASK = 0x0003F000;
    static const uint32_t ATTEN_HP_SHIFT = 12;
    static const uint32_t ATTEN_MULT_MASK = 0x00000FFF;
    static const uint32_t ATTEN_MULT_SHIFT = 0;

	uint32_t baseAddr = ADRV910X_ADDR_TX_ATTEN_TABLE;
	uint32_t idx = 0;
    uint32_t start = 0;
    uint32_t stop = 0;
    uint32_t data = 0;
    uint32_t offset = 0;
    uint32_t numTxAttenEntriesRead = 0;

    /* FIXME: Get rid of CACHE_MAX and +10 */
    uint8_t cfgData[ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX + 10] = { 0 };
    static const uint8_t TX_ENTRY_SIZE = 4;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Tx_AttenuationTable_Read_Validate,
                           device,
                           indexOffset,
                           attenTableRows,
                           arraySize);

    offset = (indexOffset * TX_ENTRY_SIZE) + baseAddr;

    numTxAttenEntriesRead = arraySize * TX_ENTRY_SIZE;
    if (numTxAttenEntriesRead > ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX)
    {
        numTxAttenEntriesRead = ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX;
    }

    ADI_MUTEX_AQUIRE(device);

    /* Enable ARM clock to access attenuation table memory */
    ADI_EXPECT(adrv910x_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, true);
    ADI_EXPECT(adrv910x_NvsRegmapCore2_Tx1GainTableClkEn_Set, device, true);

    ADI_EXPECT(adrv910x_DmaMemRead, device, offset + stop, &cfgData[0], numTxAttenEntriesRead, ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);
	
    for (idx = 0; idx < arraySize; idx++)
    {
        data = (((uint32_t)cfgData[start + 3] << 24) |
                ((uint32_t)cfgData[start + 2] << 16) |
                ((uint32_t)cfgData[start + 1] <<  8) |
                 (uint32_t)cfgData[start + 0]);

        attenTableRows[idx].txAttenHp   = ADRV910X_BF_DECODE(data, ATTEN_HP_MASK, ATTEN_HP_SHIFT);
        attenTableRows[idx].txAttenMult = ADRV910X_BF_DECODE(data, ATTEN_MULT_MASK, ATTEN_MULT_SHIFT);

        start += TX_ENTRY_SIZE;

        if ((start >= ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX) &&
            ((idx + 1) < arraySize))
        {
            numTxAttenEntriesRead = (arraySize - (idx + 1)) * TX_ENTRY_SIZE;

            if (numTxAttenEntriesRead > ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX)
            {
                numTxAttenEntriesRead = ADI_ADRV910X_TX_ATTEN_TABLE_CACHE_MAX;
            }

            stop += start;

	        ADI_EXPECT(adrv910x_DmaMemRead, device, offset + stop, &cfgData[0], numTxAttenEntriesRead, ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);

            start = 0;
        }
    }

	/* Disable ARM clock to Tx1 atten table memory access */
	ADI_EXPECT(adrv910x_NvsRegmapCore2_Tx1GainTableClkEn_Set, device, false);
	ADI_EXPECT(adrv910x_NvsRegmapTxb_TxAlgArmOrGroup11ClkSel_Set, device, false);
	
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Tx_AttenuationMode_Set_Validate(adi_adrv910x_Device_t *device,
                                                                           adi_adrv910x_TxAttenuationControlMode_e mode)
{
    static const uint32_t TX_CHANNELS[] = {ADI_ADRV910X_TX1, ADI_ADRV910X_TXNB};
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;
    adi_common_ChannelNumber_e channel = ADI_CHANNEL_1;
    adi_adrv910x_TxAttenuationControlMode_e txModeRead = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_BYPASS;
    uint8_t chan_index = 0;

    /* Validate mode parameter */
    switch (mode)
    {
    case ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_BYPASS:
    case ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI:
    case ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_PIN:
	case ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC:
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                        ADI_COMMON_ERRSRC_API,
                        ADI_COMMON_ERR_API_FAIL,
                        ADI_COMMON_ACT_ERR_CHECK_PARAM,
                        mode,
                        "Invalid parameter value. mode must be a valid adi_adrv910x_TxAttenuationControlMode_e");
        ADI_API_RETURN(device);
    }

    /* Validate calibration state for initialized Tx channels */
    for (channel = ADI_CHANNEL_1; channel <= ADI_CHANNEL_2; channel++)
    {
        adi_common_channel_to_index(channel, &chan_index);
        if (ADRV910X_BF_EQUAL(device->devStateInfo.initializedChannels, TX_CHANNELS[chan_index]))
        {
            ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, device, ADI_TX, channel, &state);
            switch (state)
            {
            case ADI_ADRV910X_CHANNEL_STANDBY:  
            case ADI_ADRV910X_CHANNEL_CALIBRATED:
                break;
            default:
                ADI_ERROR_REPORT(&device->common,
                                ADI_COMMON_ERRSRC_API,
                                ADI_COMMON_ERR_API_FAIL,
                                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                currentState.channelStates[port_index][chan_index],
                                "Error while attempting to set attenuation mode. Channel must be in STANDBY or CALIBRATED.");
            }
        }
    }

	/* Retrieve attenuation mode */
	ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Get, device, &txModeRead);
	if (txModeRead == ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
	{
		ADI_ERROR_REPORT(&device->common,
						 ADI_COMMON_ERRSRC_API,
						 ADI_COMMON_ERR_API_FAIL,
						 ADI_COMMON_ACT_ERR_CHECK_PARAM,
						 txModeRead,
						 "Invalid TxAttenuation Control Mode. Cannot control when mode CLGC is enabled");
	}
	
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_AttenuationMode_Set(adi_adrv910x_Device_t *device,
                                            adi_adrv910x_TxAttenuationControlMode_e mode)
{
    ADI_PERFORM_VALIDATION(adi_adrv910x_Tx_AttenuationMode_Set_Validate, device, mode);

    ADI_MUTEX_AQUIRE(device);

	device->devStateInfo.txAttenMode = mode;

	if (mode == ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
	{
		mode = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI;
	}
	
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenMode_Set, device, (uint8_t)mode);
    
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_AttenuationMode_Get(adi_adrv910x_Device_t *device,
                                            adi_adrv910x_TxAttenuationControlMode_e *mode)
{
    uint8_t regData = 0;
	adi_adrv910x_RadioState_t radioState = {0};
    ADI_NULL_PTR_RETURN(&device->common, mode);
    ADI_MUTEX_AQUIRE(device);
	
	ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &radioState);
	if (radioState.channelStates[ADI_TX][0] == ADI_ADRV910X_CHANNEL_PRIMED ||
		radioState.channelStates[ADI_TX][1] == ADI_ADRV910X_CHANNEL_PRIMED)
	{
		*mode = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI;
	}
	else
	{
		ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenMode_Get, device, &regData);
		*mode = (adi_adrv910x_TxAttenuationControlMode_e)regData;
	
		if (device->devStateInfo.txAttenMode == ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
			*mode = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC;
	}
	
	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Tx_Attenuation_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                            adi_adrv910x_TxAttenuationConfig_t *config)
{
    ADI_NULL_PTR_RETURN(&device->common, config);
    ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Set_Validate, device, config->attenMode);
    ADI_RANGE_CHECK(device, config->txAttenStepSize, ADI_ADRV910X_TXATTEN_0P05_DB, ADI_ADRV910X_TXATTEN_0P4_DB);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_Attenuation_Configure(adi_adrv910x_Device_t *device,
                                            adi_adrv910x_TxAttenuationConfig_t *config)
{
    ADI_PERFORM_VALIDATION(adi_adrv910x_Tx_Attenuation_Configure_Validate, device, config);
    ADI_MUTEX_AQUIRE(device);
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenConfig_Set, device, config->txAttenStepSize);
    ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Set, device, config->attenMode);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_Attenuation_Inspect(adi_adrv910x_Device_t *device,
                                            adi_adrv910x_TxAttenuationConfig_t *config)
{
    uint8_t disableTxOnPllUnlock = 0;
    uint8_t txAttenStepRead = ADI_ADRV910X_TXATTEN_0P05_DB;
    adi_adrv910x_TxAttenuationControlMode_e txModeRead = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI;
    
    ADI_NULL_PTR_RETURN(&device->common, config);
    ADI_MUTEX_AQUIRE(device);
    /* RFPLL unlock setting for Tx attenuation ramp */
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxDataRampDownOnPllUnlock_Get, device, &disableTxOnPllUnlock);

    /* Attenuation Step Size */
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenConfig_Get, device, &txAttenStepRead);

    /* Retrieve attenuation mode */
    ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Get, device, &txModeRead);

    config->disableTxOnPllUnlock = (bool)disableTxOnPllUnlock;
    config->attenMode = txModeRead;
    config->txAttenStepSize = (adi_adrv910x_TxAttenStepSize_e)txAttenStepRead;

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);    
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Tx_Attenuation_Set_Validate(adi_adrv910x_Device_t *device,
                                                                                        uint16_t attenuation_mdB)
{
    adi_adrv910x_TxAttenuationControlMode_e txModeRead = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_BYPASS;
    ADI_RANGE_CHECK(device, attenuation_mdB, 0, ADRV910X_TX_ATTEN_MDB_MAX);

    if (attenuation_mdB % ADRV910X_TX_ATTEN_MDB_RESOLUTION != 0)
    {
        ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             attenuation_mdB,
                             "Invalid attenuation_mdB value. The resolution of adi_adrv910X_Tx_Attenuation_Set() is only 0.05dB");
    }

	/* Do not allow setting attenuation when CLGC is active and loop is closed */
	ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Get, device, &txModeRead);
	if (txModeRead == ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
	{
		/* TODO: we will need a channel parameter inorder to validate */
		adi_adrv910x_DpdCfg_t dpdCfgRead = {0};
		ADI_EXPECT(adi_adrv910x_dpd_Inspect, device, ADI_CHANNEL_1, &dpdCfgRead);
		if ((dpdCfgRead.clgcLoopOpen != true))
		    ADI_ERROR_REPORT(&device->common,
						     ADI_COMMON_ERRSRC_API,
						     ADI_COMMON_ERR_API_FAIL,
						     ADI_COMMON_ACT_ERR_CHECK_PARAM,
						     txModeRead,
						     "Cannot set TxAtten when CLGC is enabled and loop is closed");
	}
	
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_Attenuation_Set(adi_adrv910x_Device_t *device,
                                        uint16_t attenuation_mdB)
{
    uint16_t regData = 0;
    adi_adrv910x_TxAttenuationControlMode_e attenMode = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_BYPASS;
    adi_adrv910x_TxAttenStepSize_e stepSize = ADI_ADRV910X_TXATTEN_0P05_DB;
    uint8_t bfVal = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Tx_Attenuation_Set_Validate, device, attenuation_mdB);
    ADI_MUTEX_AQUIRE(device);
    /* Save the current attenuation mode and set to the required mode */
    ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Get, device, &attenMode);
	if (attenMode != ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI &&
	    attenMode != ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
    {
        ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Set, device, ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI);
    }

    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenConfig_Get, device, &bfVal);
    stepSize = (adi_adrv910x_TxAttenStepSize_e)bfVal;

    /* Attenuation register value depends on txAttenStepSize */
    switch (stepSize)
        {
        case ADI_ADRV910X_TXATTEN_0P05_DB:
            regData = (attenuation_mdB / 50);
            break;
        case ADI_ADRV910X_TXATTEN_0P1_DB:
            regData = (attenuation_mdB / 100);
            break;
        case ADI_ADRV910X_TXATTEN_0P2_DB:
            regData = (attenuation_mdB / 200);
            break;
        case ADI_ADRV910X_TXATTEN_0P4_DB:
            regData = (attenuation_mdB / 400);
            break;
        default:
            {
                ADI_SHOULD_NOT_EXECUTE(device);
            }
        }
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenuation_Set, device, regData);

    /* Restore Atten Mode */
	if (attenMode != ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_SPI &&
		attenMode != ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
    {
        ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Set, device, attenMode);
    }
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_Attenuation_Get(adi_adrv910x_Device_t *device,
                                        uint16_t *attenuation_mdB)
{
    int32_t halError = 0;
    uint16_t txAttenReadBack = 0;
    uint16_t attenStepSizeDiv = 0;
    uint16_t regData = 0;
    adi_adrv910x_TxAttenStepSize_e stepSize = ADI_ADRV910X_TXATTEN_0P05_DB;
    uint8_t bfVal = 0;
    uint32_t waitInterval_us = 100;

    ADI_NULL_PTR_RETURN(&device->common, attenuation_mdB);

    ADI_MUTEX_AQUIRE(device);
    /* Strobe read register to update value, delay for update */
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenuationRead_Set, device, regData);

    halError = adi_common_hal_Wait_us(&device->common, waitInterval_us);
    ADI_ERROR_REPORT(&device->common,
                    ADI_COMMON_ERRSRC_ADI_HAL,
                    halError,
                    ADI_COMMON_ACT_ERR_CHECK_TIMER,
                    device,
                    "Timer not working");
    ADI_MUTEX_ERROR_RELEASE(device);
    ADI_ERROR_RETURN(device->common.error.newAction);

    /* Get attenuation value and step size */
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenuationRead_Get, device, &txAttenReadBack);
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxAttenConfig_Get, device, &bfVal);
    stepSize = (adi_adrv910x_TxAttenStepSize_e)bfVal;

    switch (stepSize)
        {
        case ADI_ADRV910X_TXATTEN_0P05_DB:
            attenStepSizeDiv = 50;
            break;
        case ADI_ADRV910X_TXATTEN_0P1_DB:
            attenStepSizeDiv = 100;
            break;
        case ADI_ADRV910X_TXATTEN_0P2_DB:
            attenStepSizeDiv = 200;
            break;
        case ADI_ADRV910X_TXATTEN_0P4_DB:
            attenStepSizeDiv = 400;
            break;
        default:
            {
                ADI_SHOULD_NOT_EXECUTE(device);
            }
        }
    *attenuation_mdB = (txAttenReadBack * attenStepSizeDiv);
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Tx_OutputPowerBoost_Set_Validate(adi_adrv910x_Device_t *device)
{
    static const uint32_t TX_CHANNELS[] = {ADI_ADRV910X_TX1, ADI_ADRV910X_TXNB};
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;
    adi_common_ChannelNumber_e channel = ADI_CHANNEL_1;
	adi_adrv910x_TxAttenuationControlMode_e txModeRead = ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_BYPASS;
    uint8_t chan_idx = 0;

	/* Retrieve attenuation mode */
	ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Get, device, &txModeRead);
	if (txModeRead == ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
	{
				ADI_ERROR_REPORT(&device->common,
								 ADI_COMMON_ERRSRC_API,
								 ADI_COMMON_ERR_API_FAIL,
								 ADI_COMMON_ACT_ERR_CHECK_PARAM,
								 txModeRead,
								 "Invalid TxAttenuation Control Mode. Cannot control when mode CLGC is enabled");
	}
	
    /* Validate calibration state for initialized Tx channels */
    for (channel = ADI_CHANNEL_1; channel <= ADI_CHANNEL_2; channel++)
    {
        adi_common_channel_to_index(channel, &chan_idx);
        if (ADRV910X_BF_EQUAL(device->devStateInfo.initializedChannels, TX_CHANNELS[chan_idx]))
        {
            ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, device, ADI_TX, channel, &state);
            if (state != ADI_ADRV910X_CHANNEL_STANDBY)
            {
                ADI_ERROR_REPORT(&device->common,
                                ADI_COMMON_ERRSRC_API,
                                ADI_COMMON_ERR_INV_PARAM,
                                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                state,
                                "Invalid channel state. Channel must be in STANDBY");
            }
        }
    }
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_OutputPowerBoost_Set(adi_adrv910x_Device_t *device,
                                            bool boostEnable)
{
    ADI_PERFORM_VALIDATION(adi_adrv910x_Tx_OutputPowerBoost_Set_Validate, device);

    ADI_MUTEX_AQUIRE(device);
    /* DAC full scale setting */
    ADI_EXPECT(adrv910x_TxdacMemMap_TxdacFscBoostI_Set, device, boostEnable);
    ADI_EXPECT(adrv910x_TxdacMemMap_TxdacFscBoostQ_Set, device, boostEnable);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Tx_OutputPowerBoost_Get(adi_adrv910x_Device_t *device,
                                            bool *boostEnable)
{
    uint8_t bfValue = 0;
    ADI_NULL_PTR_RETURN(&device->common, boostEnable);

    ADI_MUTEX_AQUIRE(device);
    ADI_EXPECT(adrv910x_TxdacMemMap_TxdacFscBoostI_Get, device, &bfValue);
    *boostEnable = (bool)bfValue;

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}