/**
 * \file
 * \brief Contains DPD features related function implementation
 *
 * Copyright 2019-2025 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_adrv910x_dpd.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_tx.h"

#include "adrv910x_arm.h"
#include "adi_adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_nvs_regmap_tx.h"
#include "object_ids.h"

#define MAX_PRELUTSCALE 15

static __maybe_unused int32_t adi_adrv910x_dpd_Initial_Configure_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          adi_adrv910x_DpdInitCfg_t *dpdConfig)
{
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&adrv910x->common, dpdConfig);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->amplifierType, ADI_ADRV910X_DPDAMPLIFIER_NONE, ADI_ADRV910X_DPDAMPLIFIER_GAN);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->lutSize, ADI_ADRV910X_DPDLUTSIZE_256, ADI_ADRV910X_DPDLUTSIZE_512);
    switch (dpdConfig->model)
    {
    case ADI_ADRV910X_DPDMODEL_0:  /* Falls through */
    case ADI_ADRV910X_DPDMODEL_1:  /* Falls through */
    case ADI_ADRV910X_DPDMODEL_3:  /* Falls through */
    case ADI_ADRV910X_DPDMODEL_4:
        break;
    default:
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         dpdConfig->model,
                         "Invalid parameter value. dpdConfig->model must be a valid adi_adrv910x_DpdModel_e");
        ADI_ERROR_RETURN(adrv910x->common.error.newAction);
    }
    ADI_RANGE_CHECK(adrv910x, dpdConfig->preLutScale, 0, MAX_PRELUTSCALE);
	ADI_RANGE_CHECK(adrv910x, dpdConfig->clgcEnable, 0, 1);

    /* Validate state is STANDBY */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (ADI_ADRV910X_CHANNEL_STANDBY != state.channelStates[port_index][chan_index])
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel must be in STANDBY state");
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_Initial_Configure(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv910x_DpdInitCfg_t *dpdConfig)
{
	uint8_t armData[27] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_Initial_Configure_Validate, adrv910x, channel, dpdConfig);

    adrv910x_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = dpdConfig->enable;
    armData[offset++] = dpdConfig->amplifierType;
    armData[offset++] = dpdConfig->lutSize;
    armData[offset++] = dpdConfig->model;
    armData[offset++] = dpdConfig->changeModelTapOrders;
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[0]);
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[1]);
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[2]);
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->modelOrdersForEachTap[3]);
    armData[offset++] = dpdConfig->preLutScale;
	armData[offset++] = dpdConfig->clgcEnable;

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_DPD_PRE_INIT_CAL;

	ADI_EXPECT(adi_adrv910x_arm_Config_Write, adrv910x, armData, sizeof(armData), extData, sizeof(extData))

	if (dpdConfig->clgcEnable != 0)
	{
		ADI_EXPECT(adi_adrv910x_Tx_AttenuationMode_Set, adrv910x, ADI_ADRV910X_TX_ATTENUATION_CONTROL_MODE_CLGC)
	}

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_dpd_Initial_Inspect_Validate(adi_adrv910x_Device_t *adrv910x,
                                    adi_common_ChannelNumber_e channel,
                                    adi_adrv910x_DpdInitCfg_t *dpdConfig)
{
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, dpdConfig);

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_Initial_Inspect(adi_adrv910x_Device_t *adrv910x,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv910x_DpdInitCfg_t *dpdConfig)
{
	uint8_t armReadBack[23] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_Initial_Inspect_Validate, adrv910x, channel, dpdConfig);

    channelMask = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
	ADI_EXPECT(adi_adrv910x_arm_MailBox_Get, adrv910x, OBJID_GS_CONFIG, OBJID_CFG_DPD_PRE_INIT_CAL, channelMask, offset, armReadBack, sizeof(armReadBack))

    dpdConfig->enable = (bool)armReadBack[offset++];
    dpdConfig->amplifierType = armReadBack[offset++];
    dpdConfig->lutSize = armReadBack[offset++];
    dpdConfig->model = armReadBack[offset++];
    dpdConfig->changeModelTapOrders = (bool)armReadBack[offset++];
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[0]);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[1]);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[2]);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->modelOrdersForEachTap[3]);
    dpdConfig->preLutScale = armReadBack[offset++];
	dpdConfig->clgcEnable = armReadBack[offset++];

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_dpd_Configure_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                  adi_common_ChannelNumber_e channel,
                                                                  adi_adrv910x_DpdCfg_t *dpdConfig)
{
    static const uint32_t DPD_MAX_SAMPLES = 4096;
    static const uint32_t MAX_RX_TX_NORMALIZATION_THRESHOLD_U2D30 = 1 << 30;    // 1.0 in U2.30
    static const uint32_t MAX_TIME_FILTER_COEFFICIENT = 0x7FFFFFFF;      // 0.999... in U1.31
	static const uint32_t DPD_MAX_CAPTURE_DELAY_US = 1000000;

    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ADI_NULL_PTR_RETURN(&adrv910x->common, dpdConfig);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->numberOfSamples, 3, DPD_MAX_SAMPLES);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->rxTxNormalizationLowerThreshold, 0, MAX_RX_TX_NORMALIZATION_THRESHOLD_U2D30);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->rxTxNormalizationUpperThreshold, 0, MAX_RX_TX_NORMALIZATION_THRESHOLD_U2D30);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->countsLessThanPowerThreshold, 0, 4096);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->countsGreaterThanPeakThreshold, 0, 4096);
    ADI_RANGE_CHECK(adrv910x, dpdConfig->timeFilterCoefficient, 0, MAX_TIME_FILTER_COEFFICIENT);
	ADI_RANGE_CHECK(adrv910x, dpdConfig->clgcLoopOpen, 0, 1);
	ADI_RANGE_CHECK(adrv910x, dpdConfig->captureDelay_us, 0, DPD_MAX_CAPTURE_DELAY_US);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_Configure(adi_adrv910x_Device_t *adrv910x,
                                   adi_common_ChannelNumber_e channel,
                                   adi_adrv910x_DpdCfg_t *dpdConfig)
{
	uint8_t armData[72] = { 0 };

    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_Configure_Validate, adrv910x, channel, dpdConfig);

    adrv910x_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->numberOfSamples);
    
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->additionalPowerScale);
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->rxTxNormalizationLowerThreshold);
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->rxTxNormalizationUpperThreshold);
    
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->detectionPowerThreshold);
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->detectionPeakThreshold);
    adrv910x_LoadTwoBytes( &offset, armData, dpdConfig->countsLessThanPowerThreshold);
    adrv910x_LoadTwoBytes( &offset, armData, dpdConfig->countsGreaterThanPeakThreshold);
    
    armData[offset++] = dpdConfig->immediateLutSwitching;
    armData[offset++] = dpdConfig->useSpecialFrame;
    armData[offset++] = dpdConfig->resetLuts;
    offset++;    /* 1 byte padding for word alignment */
    offset += 4; /* Leave space for 'dpdSampleRate_Hz', which is a read-only parameter */
    adrv910x_LoadFourBytes(&offset, armData, dpdConfig->timeFilterCoefficient);
	/* CLGC */
	armData[offset++] = dpdConfig->clgcLoopOpen;
	offset += 3;				/* struct padding */
	adrv910x_LoadFourBytes(&offset, armData, dpdConfig->clgcGainTarget_HundredthdB);
	adrv910x_LoadFourBytes(&offset, armData, dpdConfig->clgcFilterAlpha);
	offset += 8; /* space for clgcLastGain_HundredthdB & clgcFilteredGain_HundredthdB , which are read-only */
	adrv910x_LoadFourBytes(&offset, armData, dpdConfig->captureDelay_us);
	armData[offset++] = dpdConfig->enableRepeatedEstimationInTDD;
	offset += 3; /* struct padding */
	
    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_TC_TX_DPD;

	ADI_EXPECT(adi_adrv910x_arm_Config_Write, adrv910x, armData, sizeof(armData), extData, sizeof(extData));

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_dpd_Inspect_Validate(adi_adrv910x_Device_t *adrv910x,
                                adi_common_ChannelNumber_e channel,
                                adi_adrv910x_DpdCfg_t *dpdConfig)
{
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, dpdConfig);

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_Inspect(adi_adrv910x_Device_t *adrv910x,
                                 adi_common_ChannelNumber_e channel,
                                 adi_adrv910x_DpdCfg_t *dpdConfig)
{
	uint8_t armReadBack[72] = { 0 };

    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_Inspect_Validate, adrv910x, channel, dpdConfig);

    channelMask = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);

	ADI_EXPECT(adi_adrv910x_arm_MailBox_Get, adrv910x, OBJID_GS_CONFIG, OBJID_TC_TX_DPD, channelMask, offset, armReadBack, sizeof(armReadBack))

    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->numberOfSamples);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->additionalPowerScale);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->rxTxNormalizationLowerThreshold);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->rxTxNormalizationUpperThreshold);
    
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->detectionPowerThreshold);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->detectionPeakThreshold);
    adrv910x_ParseTwoBytes( &offset, armReadBack, &dpdConfig->countsLessThanPowerThreshold);
    adrv910x_ParseTwoBytes( &offset, armReadBack, &dpdConfig->countsGreaterThanPeakThreshold);

    dpdConfig->immediateLutSwitching    = (bool)armReadBack[offset++];
    dpdConfig->useSpecialFrame          = (bool)armReadBack[offset++];
    dpdConfig->resetLuts                = (bool)armReadBack[offset++];
    offset++;
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->dpdSamplingRate_Hz);
    adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->timeFilterCoefficient);
	/* CLGC */
	dpdConfig->clgcLoopOpen             = (bool)armReadBack[offset++];
	offset += 3;			/* struct padding */
	adrv910x_ParseFourBytes(&offset, armReadBack, (uint32_t*)&dpdConfig->clgcGainTarget_HundredthdB);
	adrv910x_ParseFourBytes(&offset, armReadBack, &dpdConfig->clgcFilterAlpha);
	adrv910x_ParseFourBytes(&offset, armReadBack, (uint32_t*)&dpdConfig->clgcLastGain_HundredthdB);
	adrv910x_ParseFourBytes(&offset, armReadBack, (uint32_t*)&dpdConfig->clgcFilteredGain_HundredthdB);
	adrv910x_ParseFourBytes(&offset, armReadBack, (uint32_t*)&dpdConfig->captureDelay_us);
	dpdConfig->enableRepeatedEstimationInTDD = (bool)armReadBack[offset++];
	offset += 3; /* struct padding */

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_dpd_coefficients_Set_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv910x_DpdCoefficients_t *coefficients)
{
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, coefficients);
    ADI_RANGE_CHECK(adrv910x, coefficients->region, 0, 2);
    
    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, adrv910x, ADI_TX, channel, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (ADI_ADRV910X_CHANNEL_CALIBRATED != state)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel must be in CALIBRATED state");
    }
    
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_coefficients_Set(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_DpdCoefficients_t *coefficients)
{
    uint8_t armData[216] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    uint8_t i = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_coefficients_Set_Validate, adrv910x, channel, coefficients);
    
    adrv910x_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    armData[offset++] = coefficients->region;
    offset += 3;    // Reserved
    for(i = 0 ; i < ADI_ADRV910X_DPD_NUM_COEFFICIENTS; i++)
    {
        armData[offset++] = coefficients->coefficients[i];
    }
    
    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_DPD_LUT_INITIALIZATION;
    
    ADI_EXPECT(adi_adrv910x_arm_Config_Write, adrv910x, armData, sizeof(armData), extData, sizeof(extData))
    
    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_dpd_coefficients_Get_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                         adi_common_ChannelNumber_e channel,
                                                                         adi_adrv910x_DpdCoefficients_t *coefficients)
{
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, coefficients);
    ADI_RANGE_CHECK(adrv910x, coefficients->region, 0, 2);
 
    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, adrv910x, ADI_TX, channel, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (ADI_ADRV910X_CHANNEL_CALIBRATED != state)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Invalid channel state. Channel must be in CALIBRATED state");
    }
    
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_coefficients_Get(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_ChannelNumber_e channel,
                                          adi_adrv910x_DpdCoefficients_t *coefficients)
{
    uint8_t armReadBack[212] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;
    uint8_t i = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_coefficients_Get_Validate, adrv910x, channel, coefficients);

    /* Write region to GET buffer */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, adrv910x, ADRV910X_ADDR_ARM_MAILBOX_GET + 4, &coefficients->region, 1, ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    
    channelMask = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
	ADI_EXPECT(adi_adrv910x_arm_MailBox_Get, adrv910x, OBJID_GS_CONFIG, OBJID_CFG_DPD_LUT_INITIALIZATION, channelMask, 0, armReadBack, sizeof(armReadBack));
    
    coefficients->region = armReadBack[offset++];
    offset += 3;    // Reserved
    for(i = 0 ; i < ADI_ADRV910X_DPD_NUM_COEFFICIENTS ; i++)
    {
        coefficients->coefficients[i] = armReadBack[offset++];
    }
    
    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_dpd_CaptureData_Read_Validate(adi_adrv910x_Device_t *adrv910x,
                                                          adi_common_ChannelNumber_e channel,
                                                          int32_t iData_tx[],
                                                          int32_t qData_tx[],
                                                          int32_t iData_elb[],
                                                          int32_t qData_elb[],
                                                          uint32_t length)
{
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    static const uint32_t DPD_MAX_SAMPLES = 4096;

    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(adrv910x, length, 1, DPD_MAX_SAMPLES);
    ADI_NULL_PTR_RETURN(&adrv910x->common, iData_tx);
    ADI_NULL_PTR_RETURN(&adrv910x->common, qData_tx);
    ADI_NULL_PTR_RETURN(&adrv910x->common, iData_elb);
    ADI_NULL_PTR_RETURN(&adrv910x->common, qData_elb);
   
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv910x, iData_tx, length);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv910x, qData_tx, length);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv910x, iData_elb, length);
    ADI_ENTRY_PTR_ARRAY_EXPECT(adrv910x, qData_elb, length);

    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, adrv910x, ADI_TX, channel, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);
    if (state != ADI_ADRV910X_CHANNEL_CALIBRATED &&
        state != ADI_ADRV910X_CHANNEL_PRIMED)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Invalid channel state. Channel must be in CALIBRATED or PRIMED state");
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_CaptureData_Read(adi_adrv910x_Device_t *adrv910x,
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
    
    uint8_t bfValue = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_CaptureData_Read_Validate, adrv910x, channel, iData_tx, qData_tx, iData_elb, qData_elb, length);

    ADI_EXPECT(adrv910x_NvsRegmapTx_TxDpdRamClkSel_Get, adrv910x, &bfValue)
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxDpdRamClkSel_Set, adrv910x, 1)

	dpdTxCaptureDataAddr_I = ((int32_t)pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_I_N(INST_TX1_DPD_TOP, 0));
	dpdTxCaptureDataAddr_Q = ((int32_t)pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_TX_Q_N(INST_TX1_DPD_TOP, 0));
	dpdRxCaptureDataAddr_I = ((int32_t)pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_I_N(INST_TX1_DPD_TOP, 0));
	dpdRxCaptureDataAddr_Q = ((int32_t)pREG_NVS_AHB_TX_DPD_TOP_DPD_TOP_RX_Q_N(INST_TX1_DPD_TOP, 0));

	ADI_EXPECT(adrv910x_DmaMemRead, adrv910x, dpdTxCaptureDataAddr_I, (uint8_t *)iData_tx, length * 4, (uint8_t)autoIncrement, ADI_PS1)
	ADI_EXPECT(adrv910x_DmaMemRead, adrv910x, dpdTxCaptureDataAddr_Q, (uint8_t *)qData_tx, length * 4, (uint8_t)autoIncrement, ADI_PS1)
	ADI_EXPECT(adrv910x_DmaMemRead, adrv910x, dpdRxCaptureDataAddr_I, (uint8_t *)iData_elb, length * 4, (uint8_t)autoIncrement, ADI_PS1)
	ADI_EXPECT(adrv910x_DmaMemRead, adrv910x, dpdRxCaptureDataAddr_Q, (uint8_t *)qData_elb, length * 4, (uint8_t)autoIncrement, ADI_PS1)

    for(i = 0 ; i < length; i++)
    {
        iData_tx[i] = iData_tx[i] >> 14;
        qData_tx[i] = qData_tx[i] >> 14;
        iData_elb[i] = iData_elb[i] >> 14;
        qData_elb[i] = qData_elb[i] >> 14;
    }
    ADI_EXPECT(adrv910x_NvsRegmapTx_TxDpdRamClkSel_Set, adrv910x, bfValue)
    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_dpd_channel_Status_Get_Validate(adi_adrv910x_Device_t* adrv910x,
                                                                                         adi_common_ChannelNumber_e channel,
                                                                                         adi_adrv910x_DpdChannelStatus_t* dpdChannelStatus)
{
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    /* Check that channelMask is valid */
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, dpdChannelStatus);

    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, adrv910x, ADI_TX, channel, &state);
    adi_common_port_to_index(ADI_TX, &port_index);
    adi_common_channel_to_index(channel, &chan_index);

	if (state != ADI_ADRV910X_CHANNEL_CALIBRATED &&
	    state != ADI_ADRV910X_CHANNEL_PRIMED &&
        state != ADI_ADRV910X_CHANNEL_RF_ENABLED)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
		    "Invalid channel state. Channel must be in CALIBRATED or PRIMED or RF_ENABLED state");
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_dpd_channel_Status_Get(adi_adrv910x_Device_t* adrv910x, adi_common_ChannelNumber_e channel, adi_adrv910x_DpdChannelStatus_t* dpdChannelStatus)
{
    /* Per-channel ARM-padded size of dpdChannelStatus_t (13 bytes packed, padded to 16). */
    static const uint32_t DPD_CH_STATUS_SIZE = 16u;
    uint8_t channelMask = 0;
    uint8_t descriptorBuf[8] = { 0 };
    uint8_t rawBuf[32] = { 0 };
    uint32_t statusLen = 0u;
    uint32_t statusAddr = 0u;
    uint32_t readLen = 0u;
    uint32_t requiredLen = 0u;
    uint8_t channelIndex = 0u;
    uint32_t base = 0u;

    ADI_PERFORM_VALIDATION(adi_adrv910x_dpd_channel_Status_Get_Validate, adrv910x, channel, dpdChannelStatus);
    channelMask = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);

    /* Firmware writes a calStatusGet_t descriptor { uint32_t len, void *pCalStatus } for
     * OBJID_GO_CAL_STATUS requests; the actual status struct must be read separately from
     * ARM memory at pCalStatus, then filtered down to the requested channel. */
    ADI_EXPECT(adi_adrv910x_arm_MailBox_Get, adrv910x, OBJID_GO_CAL_STATUS, OBJID_TC_TX_DPD, channelMask, 0u, &descriptorBuf[0], sizeof(descriptorBuf))

    statusLen  = (uint32_t)descriptorBuf[0] |
                 ((uint32_t)descriptorBuf[1] << 8) |
                 ((uint32_t)descriptorBuf[2] << 16) |
                 ((uint32_t)descriptorBuf[3] << 24);

    statusAddr = (uint32_t)descriptorBuf[4] |
                 ((uint32_t)descriptorBuf[5] << 8) |
                 ((uint32_t)descriptorBuf[6] << 16) |
                 ((uint32_t)descriptorBuf[7] << 24);

    if ((statusLen == 0u) || (statusAddr == 0u))
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         statusLen,
                         "Firmware returned an empty DPD channel status descriptor");
        ADI_API_RETURN(adrv910x);
    }

    adi_common_channel_to_index(channel, &channelIndex);
    requiredLen = ((uint32_t)channelIndex + 1u) * DPD_CH_STATUS_SIZE;

    if (statusLen < requiredLen)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         statusLen,
                         "Firmware DPD status buffer is too small to contain the requested channel");
        ADI_API_RETURN(adrv910x);
    }

    /* Only the requested channel is needed; ARM memory reads must be word aligned */
    readLen = (requiredLen + 3u) & ~3u;

    ADI_EXPECT(adi_adrv910x_arm_Memory_Read, adrv910x, statusAddr, &rawBuf[0], readLen, ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);

    base = (uint32_t)channelIndex * DPD_CH_STATUS_SIZE;
    adrv910x_ParseFourBytes(&base, rawBuf, &dpdChannelStatus->numberOfSuccessfulIterations);

    ADI_API_RETURN(adrv910x);
}


