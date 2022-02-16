/**
 * \file
 * \brief Contains DPD features related function implementation
 *
 * Copyright 2019-2021 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_adrv9001_dpd.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_tx.h"

#include "adi_adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_nvs_regmap_tx.h"
#include "object_ids.h"

#define MAX_PRELUTSCALE 15

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_Initial_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          adi_adrv9001_DpdInitCfg_t *dpdConfig)
{
    adi_adrv9001_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&adrv9001->common, dpdConfig);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->amplifierType, ADI_ADRV9001_DPDAMPLIFIER_NONE, ADI_ADRV9001_DPDAMPLIFIER_GAN);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->lutSize, ADI_ADRV9001_DPDLUTSIZE_256, ADI_ADRV9001_DPDLUTSIZE_512);
    switch (dpdConfig->model)
    {
    case ADI_ADRV9001_DPDMODEL_0:  /* Falls through */
    case ADI_ADRV9001_DPDMODEL_1:  /* Falls through */
    case ADI_ADRV9001_DPDMODEL_3:  /* Falls through */
    case ADI_ADRV9001_DPDMODEL_4:
        break;
    default:
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdConfig->model,
                         "Invalid parameter value. dpdConfig->model must be a valid adi_adrv9001_DpdModel_e");
        ADI_ERROR_RETURN(adrv9001->common.error.newAction);
    }
    ADI_RANGE_CHECK(adrv9001, dpdConfig->preLutScale, 0, MAX_PRELUTSCALE);
	ADI_RANGE_CHECK(adrv9001, dpdConfig->clgcEnable, 0, 1);

    /* Validate state is STANDBY */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (ADI_ADRV9001_CHANNEL_STANDBY != state.channelStates[port_index][chan_index])
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel must be in STANDBY state");
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_Initial_Configure(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv9001_DpdInitCfg_t *dpdConfig)
{
	uint8_t armData[27] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_Initial_Configure_Validate, adrv9001, channel, dpdConfig);

    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = dpdConfig->enable;
    armData[offset++] = dpdConfig->amplifierType;
    armData[offset++] = dpdConfig->lutSize;
    armData[offset++] = dpdConfig->model;
    armData[offset++] = dpdConfig->changeModelTapOrders;
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[0]);
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[1]);
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[2]);
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[3]);
    armData[offset++] = dpdConfig->preLutScale;
	armData[offset++] = dpdConfig->clgcEnable;

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_DPD_PRE_INIT_CAL;

	ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

	if (dpdConfig->clgcEnable != 0)
	{
		ADI_EXPECT(adi_adrv9001_Tx_AttenuationMode_Set, adrv9001, channel, ADI_ADRV9001_TX_ATTENUATION_CONTROL_MODE_CLGC)
	}

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_Initial_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                    adi_common_ChannelNumber_e channel,
                                    adi_adrv9001_DpdInitCfg_t *dpdConfig)
{
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, dpdConfig);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_Initial_Inspect(adi_adrv9001_Device_t *adrv9001,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv9001_DpdInitCfg_t *dpdConfig)
{
	uint8_t armReadBack[23] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_Initial_Inspect_Validate, adrv9001, channel, dpdConfig);

    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_DPD_PRE_INIT_CAL, channelMask, offset, armReadBack, sizeof(armReadBack))

    dpdConfig->enable = (bool)armReadBack[offset++];
    dpdConfig->amplifierType = armReadBack[offset++];
    dpdConfig->lutSize = armReadBack[offset++];
    dpdConfig->model = armReadBack[offset++];
    dpdConfig->changeModelTapOrders = (bool)armReadBack[offset++];
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[0]);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[1]);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[2]);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[3]);
    dpdConfig->preLutScale = armReadBack[offset++];
	dpdConfig->clgcEnable = armReadBack[offset++];

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                  adi_common_ChannelNumber_e channel,
                                                                  adi_adrv9001_DpdCfg_t *dpdConfig)
{
    static const uint32_t DPD_MAX_SAMPLES = 4096;
    static const uint32_t MAX_RX_TX_NORMALIZATION_THRESHOLD_U2D30 = 1 << 30;    // 1.0 in U2.30
    static const uint32_t MAX_TIME_FILTER_COEFFICIENT = 0x7FFFFFFF;      // 0.999... in U1.31

    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&adrv9001->common, dpdConfig);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->numberOfSamples, 3, DPD_MAX_SAMPLES);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->rxTxNormalizationLowerThreshold, 0, MAX_RX_TX_NORMALIZATION_THRESHOLD_U2D30);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->rxTxNormalizationUpperThreshold, 0, MAX_RX_TX_NORMALIZATION_THRESHOLD_U2D30);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->countsLessThanPowerThreshold, 0, 4096);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->countsGreaterThanPeakThreshold, 0, 4096);
    ADI_RANGE_CHECK(adrv9001, dpdConfig->timeFilterCoefficient, 0, MAX_TIME_FILTER_COEFFICIENT);
	ADI_RANGE_CHECK(adrv9001, dpdConfig->clgcLoopOpen, 0, 1);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_Configure(adi_adrv9001_Device_t *adrv9001,
                                   adi_common_ChannelNumber_e channel,
                                   adi_adrv9001_DpdCfg_t *dpdConfig)
{
	uint8_t armData[64] = { 0 };

    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_Configure_Validate, adrv9001, channel, dpdConfig);

    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->numberOfSamples);
    
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->additionalPowerScale);
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->rxTxNormalizationLowerThreshold);
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->rxTxNormalizationUpperThreshold);
    
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->detectionPowerThreshold);
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->detectionPeakThreshold);
    adrv9001_LoadTwoBytes( &offset, armData, dpdConfig->countsLessThanPowerThreshold);
    adrv9001_LoadTwoBytes( &offset, armData, dpdConfig->countsGreaterThanPeakThreshold);
    
    armData[offset++] = dpdConfig->immediateLutSwitching;
    armData[offset++] = dpdConfig->useSpecialFrame;
    armData[offset++] = dpdConfig->resetLuts;
    offset++;    /* 1 byte padding for word alignment */
    offset += 4; /* Leave space for 'dpdSampleRate_Hz', which is a read-only parameter */
    adrv9001_LoadFourBytes(&offset, armData, dpdConfig->timeFilterCoefficient);
	/* CLGC */
	armData[offset++] = dpdConfig->clgcLoopOpen;
	offset += 3;				/* struct padding */
	adrv9001_LoadFourBytes(&offset, armData, dpdConfig->clgcGainTarget_HundredthdB);
	adrv9001_LoadFourBytes(&offset, armData, dpdConfig->clgcFilterAlpha);
	offset += 8; /* space for clgcLastGain_HundredthdB & clgcFilteredGain_HundredthdB , which are read-only */

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_TC_TX_DPD;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                adi_common_ChannelNumber_e channel,
                                adi_adrv9001_DpdCfg_t *dpdConfig)
{
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, dpdConfig);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_Inspect(adi_adrv9001_Device_t *adrv9001,
                                 adi_common_ChannelNumber_e channel,
                                 adi_adrv9001_DpdCfg_t *dpdConfig)
{
	uint8_t armReadBack[60] = { 0 };

    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_Inspect_Validate, adrv9001, channel, dpdConfig);

    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);

    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_TC_TX_DPD, channelMask, offset, armReadBack, sizeof(armReadBack))

    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->numberOfSamples);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->additionalPowerScale);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->rxTxNormalizationLowerThreshold);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->rxTxNormalizationUpperThreshold);
    
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->detectionPowerThreshold);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->detectionPeakThreshold);
    adrv9001_ParseTwoBytes( &offset, armReadBack, &dpdConfig->countsLessThanPowerThreshold);
    adrv9001_ParseTwoBytes( &offset, armReadBack, &dpdConfig->countsGreaterThanPeakThreshold);

    dpdConfig->immediateLutSwitching    = (bool)armReadBack[offset++];
    dpdConfig->useSpecialFrame          = (bool)armReadBack[offset++];
    dpdConfig->resetLuts                = (bool)armReadBack[offset++];
    offset++;
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->dpdSamplingRate_Hz);
    adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->timeFilterCoefficient);
	/* CLGC */
	dpdConfig->clgcLoopOpen             = (bool)armReadBack[offset++];
	offset += 3;			/* struct padding */
	adrv9001_ParseFourBytes(&offset, armReadBack, (uint32_t*)&dpdConfig->clgcGainTarget_HundredthdB);
	adrv9001_ParseFourBytes(&offset, armReadBack, &dpdConfig->clgcFilterAlpha);
	adrv9001_ParseFourBytes(&offset, armReadBack, (uint32_t*)&dpdConfig->clgcLastGain_HundredthdB);
	adrv9001_ParseFourBytes(&offset, armReadBack, (uint32_t*)&dpdConfig->clgcFilteredGain_HundredthdB);

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_coefficients_Set_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv9001_DpdCoefficients_t *coefficients)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, coefficients);
    ADI_RANGE_CHECK(adrv9001, coefficients->region, 0, 7);
    
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, ADI_TX, channel, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (ADI_ADRV9001_CHANNEL_CALIBRATED != state)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel must be in CALIBRATED state");
    }
    
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_coefficients_Set(adi_adrv9001_Device_t *adrv9001,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_DpdCoefficients_t *coefficients)
{
    uint8_t armData[216] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    uint8_t i = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_coefficients_Set_Validate, adrv9001, channel, coefficients);
    
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = coefficients->region;
    offset += 3;    // Reserved
    for(i = 0 ; i < ADI_ADRV9001_DPD_NUM_COEFFICIENTS; i++)
    {
        armData[offset++] = coefficients->coefficients[i];
    }
    
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_DPD_LUT_INITIALIZATION;
    
    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))
    
    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_coefficients_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv9001_DpdCoefficients_t *coefficients)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, coefficients);
    ADI_RANGE_CHECK(adrv9001, coefficients->region, 0, 7);
 
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, ADI_TX, channel, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (ADI_ADRV9001_CHANNEL_CALIBRATED != state)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel must be in CALIBRATED state");
    }
    
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_coefficients_Get(adi_adrv9001_Device_t *adrv9001,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv9001_DpdCoefficients_t *coefficients)
{
    uint8_t armReadBack[212] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;
    uint8_t i = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_coefficients_Get_Validate, adrv9001, channel, coefficients);

    /* Write region to GET buffer */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_MAILBOX_GET + 4, &coefficients->region, 1, ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    
    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_DPD_LUT_INITIALIZATION, channelMask, 0, armReadBack, sizeof(armReadBack));
    
    coefficients->region = armReadBack[offset++];
    offset += 3;    // Reserved
    for(i = 0 ; i < ADI_ADRV9001_DPD_NUM_COEFFICIENTS ; i++)
    {
        coefficients->coefficients[i] = armReadBack[offset++];
    }
    
    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t adi_adrv9001_dpd_CaptureData_Read_Validate(adi_adrv9001_Device_t *adrv9001,
                                                          adi_common_ChannelNumber_e channel,
                                                          int32_t iData_tx[],
                                                          int32_t qData_tx[],
                                                          int32_t iData_elb[],
                                                          int32_t qData_elb[],
                                                          uint32_t length)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    static const uint32_t DPD_MAX_SAMPLES = 4096;

    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(adrv9001, length, 1, DPD_MAX_SAMPLES);
    ADI_NULL_PTR_RETURN(&adrv9001->common, iData_tx);
    ADI_NULL_PTR_RETURN(&adrv9001->common, qData_tx);
    ADI_NULL_PTR_RETURN(&adrv9001->common, iData_elb);
    ADI_NULL_PTR_RETURN(&adrv9001->common, qData_elb);
   
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, iData_tx, length);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, qData_tx, length);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, iData_elb, length);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, qData_elb, length);

    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, ADI_TX, channel, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (state != ADI_ADRV9001_CHANNEL_CALIBRATED &&
        state != ADI_ADRV9001_CHANNEL_PRIMED)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Invalid channel state. Channel must be in CALIBRATED or PRIMED state");
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_CaptureData_Read(adi_adrv9001_Device_t *adrv9001,
                                          adi_common_ChannelNumber_e channel,
                                          int32_t iData_tx[],
                                          int32_t qData_tx[],
                                          int32_t iData_elb[],
                                          int32_t qData_elb[],
                                          uint32_t length,
                                          bool autoIncrement)
{
    uint32_t i = 0;
    int32_t dpdTxCaptureDataAddr_I = 0;
    int32_t dpdTxCaptureDataAddr_Q = 0;
    int32_t dpdRxCaptureDataAddr_I = 0;
    int32_t dpdRxCaptureDataAddr_Q = 0;
    
    adrv9001_BfNvsRegmapTx_e baseAddr = ADRV9001_BF_TX1_CORE;
    uint8_t bfValue = 0;

    /* Base address table for DPD registers */
    static const uint32_t txDPDTopBaseAddr[2] =
    {
        INST_TX1_DPD_TOP,
        INST_TX2_DPD_TOP,
    };

    if (ADI_CHANNEL_2 == channel)
    {
        baseAddr = ADRV9001_BF_TX2_CORE;
    }
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_CaptureData_Read_Validate, adrv9001, channel, iData_tx, qData_tx, iData_elb, qData_elb, length);

    ADI_EXPECT(adrv9001_NvsRegmapTx_TxDpdRamClkSel_Get, adrv9001, baseAddr, &bfValue)
    ADI_EXPECT(adrv9001_NvsRegmapTx_TxDpdRamClkSel_Set, adrv9001, baseAddr, 1)

    dpdTxCaptureDataAddr_I = ((int32_t) pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_I_N(txDPDTopBaseAddr[channel - 1], 0));
    dpdTxCaptureDataAddr_Q = ((int32_t) pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_Q_N(txDPDTopBaseAddr[channel - 1], 0));
    dpdRxCaptureDataAddr_I = ((int32_t) pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_I_N(txDPDTopBaseAddr[channel - 1], 0));
    dpdRxCaptureDataAddr_Q = ((int32_t) pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_Q_N(txDPDTopBaseAddr[channel - 1], 0));

    ADI_EXPECT(adrv9001_DmaMemRead, adrv9001, dpdTxCaptureDataAddr_I, (uint8_t *)iData_tx, length * 4, (uint8_t)autoIncrement)
    ADI_EXPECT(adrv9001_DmaMemRead, adrv9001, dpdTxCaptureDataAddr_Q, (uint8_t *)qData_tx, length * 4, (uint8_t)autoIncrement)
    ADI_EXPECT(adrv9001_DmaMemRead, adrv9001, dpdRxCaptureDataAddr_I, (uint8_t *)iData_elb, length * 4, (uint8_t)autoIncrement)
    ADI_EXPECT(adrv9001_DmaMemRead, adrv9001, dpdRxCaptureDataAddr_Q, (uint8_t *)qData_elb, length * 4, (uint8_t)autoIncrement)

    for(i = 0 ; i < length; i++)
    {
        iData_tx[i] = iData_tx[i] >> 14;
        qData_tx[i] = qData_tx[i] >> 14;
        iData_elb[i] = iData_elb[i] >> 14;
        qData_elb[i] = qData_elb[i] >> 14;
    }
    ADI_EXPECT(adrv9001_NvsRegmapTx_TxDpdRamClkSel_Set, adrv9001, baseAddr, bfValue)
    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_fh_regions_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                             adi_common_ChannelNumber_e channel,
                                                                             adi_adrv9001_DpdFhRegions_t dpdFhRegions[],
                                                                             uint32_t size)
{
    adi_adrv9001_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    uint32_t i = 0;
    
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, dpdFhRegions);
    ADI_RANGE_CHECK(adrv9001, size, 1, 7);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, dpdFhRegions, size);
    for (i = 0; i < size; i++)
    {
        if (dpdFhRegions[i].startFrequency_Hz == 0)
        {
            if (dpdFhRegions[i].endFrequency_Hz != 0)
            {
                ADI_ERROR_REPORT(&adrv9001->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channel,
                                 "Start and End frequency are a pair for a region. If start frequency is initialized with 0 then end frequency also must be 0.");		
                ADI_API_RETURN(adrv9001);
            }
        }
        else // non-zero start frequency
            {
                if (dpdFhRegions[i].endFrequency_Hz <= dpdFhRegions[i].startFrequency_Hz)
                {
                    ADI_ERROR_REPORT(&adrv9001->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     channel,
                                     "End frequency for a region must be greater than the start frequency.");	
                    ADI_API_RETURN(adrv9001);
                }		
            }
    }
    /* Validate state are STANDBY and CALIBRATED */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if ((ADI_ADRV9001_CHANNEL_PRIMED == state.channelStates[port_index][chan_index]) ||
        (ADI_ADRV9001_CHANNEL_RF_ENABLED == state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel state must be either STANDBY or CALIBRATED");
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_fh_regions_Configure(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_DpdFhRegions_t dpdFhRegions[],
                                              uint32_t size)
{
    static const uint8_t OBJID_CFG_DPD_FH_REGIONS = 0xA9;
    uint8_t armData[116] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    uint32_t i = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_fh_regions_Configure_Validate, adrv9001, channel, dpdFhRegions, size);

    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    for (i = 0; i < size; i++)
    {
        adrv9001_LoadEightBytes(&offset, armData, dpdFhRegions[i].startFrequency_Hz);
        adrv9001_LoadEightBytes(&offset, armData, dpdFhRegions[i].endFrequency_Hz);
    }
    
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_DPD_FH_REGIONS;
    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_dpd_fh_regions_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                           adi_common_ChannelNumber_e channel,
                                                                           adi_adrv9001_DpdFhRegions_t dpdFhRegions[],
                                                                           uint32_t size)
{
    adi_adrv9001_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, dpdFhRegions);
    ADI_RANGE_CHECK(adrv9001, size, 1, 7);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv9001, dpdFhRegions, size);
    /* Validate state are STANDBY and CALIBRATED */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if ((ADI_ADRV9001_CHANNEL_PRIMED == state.channelStates[port_index][chan_index]) ||
        (ADI_ADRV9001_CHANNEL_RF_ENABLED == state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel state must be either STANDBY or CALIBRATED");
    }
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_dpd_fh_regions_Inspect(adi_adrv9001_Device_t *adrv9001,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv9001_DpdFhRegions_t dpdFhRegions[],
                                            uint32_t size)
{
    static const uint8_t OBJID_CFG_DPD_FH_REGIONS = 0xA9;
    uint8_t armReadBack[112] = { 0 };
    uint8_t channelMask = 0;
    uint32_t i = 0;
    uint32_t offset = 0;
    uint32_t byteCount = size * 16;

    ADI_PERFORM_VALIDATION(adi_adrv9001_dpd_fh_regions_Inspect_Validate, adrv9001, channel, dpdFhRegions, size);
    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_DPD_FH_REGIONS, channelMask, offset, armReadBack, byteCount)
    for(i = 0 ; i < size ; i++)
    {
        adrv9001_ParseEightBytes(&offset, armReadBack, &(dpdFhRegions[i].startFrequency_Hz));
        adrv9001_ParseEightBytes(&offset, armReadBack, &(dpdFhRegions[i].endFrequency_Hz));
    }

    ADI_API_RETURN(adrv9001);
}

