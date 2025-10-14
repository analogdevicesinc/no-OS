/**
* \file
* \brief Contains features related function implementation defined in
* adi_adrv910x_radioctrl.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adrv910x_init.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_user.h"

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_bf.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_validators.h"
#include "object_ids.h"

int32_t adi_adrv910x_Radio_State_Get(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_RadioState_t *radioState)
{
    uint8_t regValue = 0;

    /* Range checks */
    ADI_ENTRY_PTR_EXPECT(adrv910x, radioState);

    ADI_MUTEX_AQUIRE(adrv910x);

    ADRV910X_SPIREADBYTE(adrv910x, "arm_cmd_status_8", ADRV910X_ADDR_ARM_CMD_STATUS_8, &regValue);

    radioState->systemState         = regValue & 0x03;
    /* The same parts of the register are stored twice for monitor mode state and MCS substate.
       These bits have different definitions depending on systemState.
    */
    radioState->monitorModeState    = (regValue >> 2) & 0x03;
    radioState->mcsState            = (regValue >> 2) & 0x03;
    radioState->bootState           = (regValue >> 4) & 0x0F;

    ADRV910X_SPIREADBYTE(adrv910x, "arm_cmd_status_9", ADRV910X_ADDR_ARM_CMD_STATUS_9, &regValue);

    radioState->channelStates[0][0] = (regValue >> 0) & 0x03;   /* Rx1 */
    radioState->channelStates[0][1] = (regValue >> 2) & 0x03;   /* Rx2 */
    radioState->channelStates[1][0] = (regValue >> 4) & 0x03;   /* Tx1 */
    radioState->channelStates[1][1] = (regValue >> 6) & 0x03;   /* Tx2 */


    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Radio_Channel_State_Get_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                                            adi_common_Port_e port,
                                                                                            adi_common_ChannelNumber_e channel,
                                                                                            adi_adrv910x_ChannelState_e *channelState)

{
    ADI_RANGE_CHECK(adrv910x, port, ADI_RX, ADI_TX);
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, channelState);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Radio_Channel_State_Get(adi_adrv910x_Device_t *adrv910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv910x_ChannelState_e *channelState)
{
    uint8_t regValue = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Radio_Channel_State_Get_Validate, adrv910x, port, channel, channelState);

    ADI_MUTEX_AQUIRE(adrv910x);

    ADRV910X_SPIREADBYTE(adrv910x, "arm_cmd_status_9", ADRV910X_ADDR_ARM_CMD_STATUS_9, &regValue);

    if (ADI_RX == port && ADI_CHANNEL_1 == channel)
    {
        *channelState = (regValue >> 0) & 0x03;   /* Rx1 */
    }
    else if (ADI_RX == port && ADI_CHANNEL_2 == channel)
    {
        *channelState = (regValue >> 2) & 0x03;   /* Rx2 */
    }
    else if (ADI_TX == port && ADI_CHANNEL_1 == channel)
    {
        *channelState = (regValue >> 4) & 0x03;   /* Tx1 */
    }
    else if (ADI_TX == port && ADI_CHANNEL_2 == channel)
    {
        *channelState = (regValue >> 6) & 0x03;   /* Tx2 */
    }

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

uint8_t adi_adrv910x_Radio_MailboxChannelMask_Get(adi_common_Port_e ports[],
	                                              adi_common_ChannelNumber_e channels[],
	                                              uint32_t length)
{
	uint8_t i = 0;
	uint8_t channelMask = 0;

	for (i = 0; i < length; i++)
	{
		if (ports[i] == ADI_RX && channels[i] == ADI_CHANNEL_1)
		{
			channelMask |= ADI_ADRV910X_RX1;
		}
		else if (ports[i] == ADI_RX && channels[i] == ADI_CHANNEL_2)
		{
			channelMask |= ADI_ADRV910X_RXNB;
		}
		else if (ports[i] == ADI_TX && channels[i] == ADI_CHANNEL_1)
		{
			channelMask |= ADI_ADRV910X_TX1;
		}
		else if (ports[i] == ADI_TX && channels[i] == ADI_CHANNEL_2)
		{
			channelMask |= ADI_ADRV910X_TXNB;
		}
		else if (ports[i] == ADI_ORX && channels[i] == ADI_CHANNEL_1)
		{
			channelMask |= ADI_ADRV910X_ORX1;
		}
		else if (ports[i] == ADI_ORX && channels[i] == ADI_CHANNEL_2)
		{
			channelMask |= ADI_ADRV910X_ORXNB;
		}
		else
		{
		}
	}

	return channelMask;
}

uint8_t adi_adrv910x_Radio_MailboxChannel_Get(adi_common_Port_e port, adi_common_ChannelNumber_e channel)
{
	return adi_adrv910x_Radio_MailboxChannelMask_Get(&port, &channel, 1);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Radio_Carrier_Configure_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                            adi_common_Port_e port,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            adi_adrv910x_Carrier_t *carrier)
{
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;

    ADI_EXPECT(adi_adrv910x_Port_Validate, adrv910x, port);
    ADI_EXPECT(adi_adrv910x_Channel_Validate, adrv910x, channel);

    ADI_RANGE_CHECK(adrv910x, carrier->loGenOptimization, ADI_ADRV910X_LO_GEN_OPTIMIZATION_PHASE_NOISE, ADI_ADRV910X_LO_GEN_OPTIMIZATION_POWER_CONSUMPTION);
    ADI_RANGE_CHECK_X(adrv910x, carrier->carrierFrequency_Hz, ADI_ADRV910X_CARRIER_FREQUENCY_MIN_HZ, ADI_ADRV910X_CARRIER_FREQUENCY_MAX_HZ, "%llu");
	ADI_RANGE_CHECK_X(adrv910x, carrier->extVcoConfig.extVcoPllRefClkDivider, ADI_ADRV910X_CARRIER_EXTVCO_PLL_REF_DIV_MIN, ADI_ADRV910X_CARRIER_EXTVCO_PLL_REF_DIV_MAX, "%d");
	ADI_RANGE_CHECK_X(adrv910x, carrier->extVcoConfig.extVcoPllChargePumpCurrent, ADI_ADRV910X_CARRIER_EXTVCO_PLL_CHARGE_PUMP_MIN, ADI_ADRV910X_CARRIER_EXTVCO_PLL_CHARGE_PUMP_MAX, "%d");
	ADI_RANGE_CHECK_X(adrv910x, carrier->extVcoConfig.extVcoPllSettle, ADI_ADRV910X_CARRIER_EXTVCO_PLL_SETTLE_TIME_MIN, ADI_ADRV910X_CARRIER_EXTVCO_PLL_SETTLE_TIME_MAX, "%d");
	ADI_RANGE_CHECK_X(adrv910x, carrier->extVcoConfig.extVcoPllSpeedUpChargePumpCurrent, ADI_ADRV910X_CARRIER_EXTVCO_PLL_CHARGE_PUMP_MIN, ADI_ADRV910X_CARRIER_EXTVCO_PLL_CHARGE_PUMP_MAX, "%d");
	ADI_RANGE_CHECK_X(adrv910x, carrier->extVcoConfig.extVcoPllSpeedUpCounter, 0, 0xFFF, "%d");

	if (carrier->extVcoConfig.extVcoPllSpeedUpMode &&
		carrier->extVcoConfig.extVcoPllSpeedUpChargePumpCurrent < carrier->extVcoConfig.extVcoPllChargePumpCurrent)
	{
		ADI_ERROR_REPORT(adrv910x, ADI_COMMON_ERRSRC_API,
						 ADI_COMMON_ERR_API_FAIL,
						 ADI_COMMON_ACT_ERR_CHECK_PARAM,
						 carrier->extVcoConfig.extVcoPllSpeedUpChargePumpCurrent,
						 "extVcoPllSpeedUpChargePumpCurrent must be greater than extVcoPllChargePumpCurrent");
	}

	if (0 != carrier->intermediateFrequency_Hz)
    {
        ADI_RANGE_CHECK(adrv910x, carrier->intermediateFrequency_Hz, -ADI_ADRV910X_INTERMEDIATE_FREQUENCY_MAX_HZ, ADI_ADRV910X_INTERMEDIATE_FREQUENCY_MAX_HZ);
    }

    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, adrv910x, port, channel, &state);
    switch (state)
    {
    case ADI_ADRV910X_CHANNEL_STANDBY:  /* Falls through */
    case ADI_ADRV910X_CHANNEL_CALIBRATED:
        break;
    case ADI_ADRV910X_CHANNEL_PRIMED:
        if (!carrier->enableFrequencyChangeInPrimed){
            ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         state,
                         "Invalid channel state. State must be STANDBY or CALIBRATED. Channel state can be PRIMED if enableFrequencyChangeInPrimed parameter is enabled.");
        }
        break;
    default:
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         state,
                         "Invalid channel state. State must be STANDBY or CALIBRATED. Channel state can be PRIMED if enableFrequencyChangeInPrimed parameter is enabled.");
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Radio_Carrier_Configure(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv910x_Carrier_t *carrier)
{
    uint8_t armData[22] = { 0 };
    uint8_t extData[2] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Radio_Carrier_Configure_Validate, adrv910x, port, channel, carrier);

    /* Loading byte array with parsed bytes from carrierFrequency_Hz word */
    adrv910x_LoadEightBytes(&offset, armData, carrier->carrierFrequency_Hz);
    armData[offset++] = carrier->enableFrequencyChangeInPrimed;
    armData[offset++] = carrier->loGenOptimization;
    /* Loading byte array with parsed bytes from intermediateFrequency_Hz word */
    adrv910x_LoadFourBytes(&offset, armData, carrier->intermediateFrequency_Hz);
	armData[offset++] = carrier->extVcoConfig.forceExtVcoPllRefClkDivider;
	armData[offset++] = carrier->extVcoConfig.extVcoPllRefClkDivider;
	armData[offset++] = carrier->extVcoConfig.extVcoPllChargePumpCurrent;
	armData[offset++] = carrier->extVcoConfig.extVcoPllSettle;
	armData[offset++] = carrier->extVcoConfig.extVcoPllSpeedUpMode;
	armData[offset++] = carrier->extVcoConfig.extVcoPllSpeedUpChargePumpCurrent;
	adrv910x_LoadTwoBytes(&offset, armData, carrier->extVcoConfig.extVcoPllSpeedUpCounter);

	ADI_MUTEX_AQUIRE(adrv910x);

    /* Write carrier Frequency to ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, adrv910x, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(port, channel);
	extData[1] = OBJID_GS_CHANNEL_CARRIER_FREQUENCY;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
                                        (uint8_t)ADRV910X_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_SETCARRIER_FREQUENCY_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_SETCARRIER_FREQUENCY_INTERVAL_US);

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Radio_Carrier_Inspect_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                          adi_common_Port_e port,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          adi_adrv910x_Carrier_t *carrier)
{
    ADI_EXPECT(adi_adrv910x_Port_Validate, adrv910x, port);
    ADI_EXPECT(adi_adrv910x_Channel_Validate, adrv910x, channel);
    ADI_NULL_PTR_RETURN(&adrv910x->common, carrier);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Radio_Carrier_Inspect(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv910x_Carrier_t *carrier)
{
    uint8_t armData[24] = { 0 };
    uint8_t extData[2] = { 0 };
    uint32_t offset = 0;
    uint32_t intermediateFrequency_Hz = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_Radio_Carrier_Inspect_Validate, adrv910x, port, channel, carrier);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(port, channel);
	extData[1] = OBJID_GS_CHANNEL_CARRIER_FREQUENCY;

    ADI_MUTEX_AQUIRE(adrv910x);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write,
                   adrv910x,
                   (uint8_t)ADRV910X_ARM_GET_OPCODE,
                   &extData[0],
                   sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
                                        (uint8_t)ADRV910X_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_GETCARRIER_FREQUENCY_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_GETCARRIER_FREQUENCY_INTERVAL_US);

    /* Read PLL Frequency from ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               adrv910x,
               (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
               &armData[0],
               sizeof(armData),
               ADRV910X_ARM_MEM_READ_AUTOINCR,
               ADI_PS1);

    /*Form pllFrequency word with data read back from ARM mailbox*/
    adrv910x_ParseEightBytes(&offset, armData, &carrier->carrierFrequency_Hz);
    carrier->enableFrequencyChangeInPrimed = (armData[offset++] != 0);
    carrier->loGenOptimization = (adi_adrv910x_LoGenOptimization_e)armData[offset++];
    if (ADI_RX == port)
    {
        adrv910x_ParseFourBytes(&offset, armData, &intermediateFrequency_Hz);
        carrier->intermediateFrequency_Hz = intermediateFrequency_Hz;
    }
    else
    {
        carrier->intermediateFrequency_Hz = 0;
	    offset += 4;
    }
	/* PLL User config data when using an external VCO */
	carrier->extVcoConfig.forceExtVcoPllRefClkDivider = (armData[offset++] != 0);
	carrier->extVcoConfig.extVcoPllRefClkDivider = armData[offset++];
	carrier->extVcoConfig.extVcoPllChargePumpCurrent = armData[offset++];
	carrier->extVcoConfig.extVcoPllSettle = armData[offset++];
	carrier->extVcoConfig.extVcoPllSpeedUpMode = armData[offset++];
	carrier->extVcoConfig.extVcoPllSpeedUpChargePumpCurrent = armData[offset++];
	adrv910x_ParseTwoBytes(&offset, armData, &carrier->extVcoConfig.extVcoPllSpeedUpCounter);

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Radio_ChannelEnableMode_Set_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            adi_common_Port_e port,
                                                                            adi_adrv910x_ChannelEnableMode_e mode)
{
    ADI_EXPECT(adi_adrv910x_Channel_Validate, adrv910x, channel);
    ADI_EXPECT(adi_adrv910x_Port_Validate, adrv910x, port);
    ADI_RANGE_CHECK(adrv910x, mode, ADI_ADRV910X_SPI_MODE, ADI_ADRV910X_PIN_MODE);

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Radio_ChannelEnableMode_Set(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv910x_ChannelEnableMode_e mode)
{
    ADI_PERFORM_VALIDATION(adi_adrv910x_Radio_ChannelEnableMode_Set_Validate, adrv910x, channel, port, mode);

    ADI_MUTEX_AQUIRE(adrv910x);

    if (port == ADI_RX && channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicRx1PinMode_Set, adrv910x, mode);
    }
    else if (port == ADI_RX && channel == ADI_CHANNEL_2)
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicRx2PinMode_Set, adrv910x, mode);
    }
    else if (port == ADI_TX && channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicTx1PinMode_Set, adrv910x, mode);
    }
	else if (port == ADI_TX && channel == ADI_CHANNEL_2)
	{
		ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicTx1PinMode_Set, adrv910x, mode);
	}
    else
    {
        ADI_SHOULD_NOT_EXECUTE(adrv910x);
    }

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_Radio_ChannelEnableMode_Get_Validate(adi_adrv910x_Device_t *adrv910x,
                                        adi_common_Port_e port,
                                        adi_common_ChannelNumber_e channel,
                                        adi_adrv910x_ChannelEnableMode_e *mode)
{
    ADI_EXPECT(adi_adrv910x_Channel_Validate, adrv910x, channel);
    ADI_EXPECT(adi_adrv910x_Port_Validate, adrv910x, port);
    ADI_NULL_PTR_RETURN(&adrv910x->common, mode);

    ADI_API_RETURN(adrv910x);
}
int32_t adi_adrv910x_Radio_ChannelEnableMode_Get(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv910x_ChannelEnableMode_e *mode)
{
    uint8_t regVal = 0;
    ADI_PERFORM_VALIDATION(adi_adrv910x_Radio_ChannelEnableMode_Get_Validate, adrv910x, port, channel, mode);

    ADI_MUTEX_AQUIRE(adrv910x);

    if (port == ADI_RX && channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicRx1PinMode_Get, adrv910x, &regVal);
    }
    else if (port == ADI_RX && channel == ADI_CHANNEL_2)
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicRx2PinMode_Get, adrv910x, &regVal);
    }
    else if (port == ADI_TX && (channel == ADI_CHANNEL_1 || channel == ADI_CHANNEL_2))
    {
        ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicTx1PinMode_Get, adrv910x, &regVal);
    }
    else
    {
        ADI_SHOULD_NOT_EXECUTE(adrv910x);
    }

    *mode = (adi_adrv910x_ChannelEnableMode_e)regVal;

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Radio_Channel_Prime(adi_adrv910x_Device_t *adrv910x,
                                         adi_common_Port_e port,
                                         adi_common_ChannelNumber_e channel,
                                         bool prime)
{
    return adi_adrv910x_Radio_Channels_Prime(adrv910x, &port, &channel, 1, prime);
}

int32_t adi_adrv910x_Radio_Channels_Prime(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_Port_e ports[],
                                          adi_common_ChannelNumber_e channels[],
                                          uint32_t length,
                                          bool prime)
{
    uint8_t i = 0;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    uint8_t opCode = 0;
    uint8_t mailboxChannelMask = 0;
    adi_adrv910x_RadioState_t currentState = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Channel_State_GenericValidate, adrv910x, ports, channels, length);

    ADI_MUTEX_AQUIRE(adrv910x);

    // Validate current state
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    for (i = 0; i < length; i++)
    {
        adi_common_port_to_index(ports[i], &port_index);
        adi_common_channel_to_index(channels[i], &chan_index);
        if(currentState.channelStates[port_index][chan_index] != ADI_ADRV910X_CHANNEL_CALIBRATED &&
           currentState.channelStates[port_index][chan_index] != ADI_ADRV910X_CHANNEL_PRIMED)
        {
            if (prime)
            {
                ADI_ERROR_REPORT(&adrv910x->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channelState,
                                 "Error while attempting to prime channel. Channel must be in the CALIBRATED state to be primed.");
            }
            else
            {
                ADI_ERROR_REPORT(&adrv910x->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channelState,
                                 "Error while attempting to un-prime channel. Channel must be in the PRIMED state to be un-primed.");
            }
            ADI_MUTEX_RELEASE(adrv910x);
            ADI_API_RETURN(adrv910x);
        }
    }

    /* TODO: What if 1 or more channels are already primed? */
    mailboxChannelMask = adi_adrv910x_Radio_MailboxChannelMask_Get(ports, channels, length);

    if (true == prime)
    {
        opCode = ADRV910X_ARM_RADIOON_OPCODE;
    }
    else
    {
        opCode = ADRV910X_ARM_RADIOOFF_OPCODE;
    }

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, opCode, &mailboxChannelMask, 1);

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
                                        opCode,
                                        0,
                                        (uint32_t)ADI_ADRV910X_RADIOONOFF_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_RADIOONOFF_INTERVAL_US);

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Radio_Channel_EnableRf(adi_adrv910x_Device_t *adrv910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            bool enable)
{
    return adi_adrv910x_Radio_Channels_EnableRf(adrv910x, &port, &channel, 1, enable);
}

int32_t adi_adrv910x_Radio_Channels_EnableRf(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_Port_e ports[],
                                             adi_common_ChannelNumber_e channels[],
                                             uint32_t length,
                                             bool enable)
{
    uint8_t i = 0;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    adi_adrv910x_ChannelEnableMode_e enableMode = ADI_ADRV910X_SPI_MODE;
    adi_adrv910x_RadioState_t currentState = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_Channel_State_GenericValidate, adrv910x, ports, channels, length);

    ADI_MUTEX_AQUIRE(adrv910x);

    // Validate current state
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    for (i = 0; i < length; i++)
    {
        ADI_EXPECT(adi_adrv910x_Radio_ChannelEnableMode_Get, adrv910x, ports[i], channels[i], &enableMode);
        if (ADI_ADRV910X_SPI_MODE != enableMode)
        {
            ADI_ERROR_REPORT(&adrv910x->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             enableMode,
                             "Error while attempting to enable/disable RF for channel. Channel enable mode must be ADI_ADRV910x_SPI_MODE");
            ADI_MUTEX_RELEASE(adrv910x);
            ADI_API_RETURN(adrv910x);
        }

        adi_common_port_to_index(ports[i], &port_index);
        adi_common_channel_to_index(channels[i], &chan_index);
        if (currentState.channelStates[port_index][chan_index] != ADI_ADRV910X_CHANNEL_PRIMED &&
            currentState.channelStates[port_index][chan_index] != ADI_ADRV910X_CHANNEL_RF_ENABLED)
        {
            if (enable)
            {
                ADI_ERROR_REPORT(&adrv910x->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channelState,
                                 "Error while attempting to enable RF for channel. Channel must be in the PRIMED state to enable RF.");
            }
            else
            {
                ADI_ERROR_REPORT(&adrv910x->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_API_FAIL,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channelState,
                                 "Error while attempting to disable RF for channel. Channel must be in the RF_ENABLED state to disable RF.");
            }
            ADI_API_RETURN(adrv910x)
        }

        /* Set the enable field for the specified channel */
        if (ports[i] == ADI_RX && channels[i] == ADI_CHANNEL_1)
        {
            ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicRx1Enable_Set, adrv910x, enable);
        }
        else if (ports[i] == ADI_RX && channels[i] == ADI_CHANNEL_2)
        {
            ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicRx2Enable_Set, adrv910x, enable);
        }
        else if (ports[i] == ADI_TX && (channels[i] == ADI_CHANNEL_1 || channels[i] == ADI_CHANNEL_2))
        {
            ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicTx1Enable_Set, adrv910x, enable);
        }
        /* TODO: Is ORX necessary? */
        else if(ports[i] == ADI_ORX && channels[i] == ADI_CHANNEL_1)
        {
            ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicOrx1Enable_Set, adrv910x, enable);
        }
        else if(ports[i] == ADI_ORX && channels[i] == ADI_CHANNEL_2)
        {
            ADI_EXPECT(adrv910x_NvsRegmapCore2_BbicOrx2Enable_Set, adrv910x, enable);
        }
        else
        {
            ADI_SHOULD_NOT_EXECUTE(adrv910x);
        }
    }

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}


/* Ported from adrv910x API */
static __maybe_unused int32_t __maybe_unused adi_adrv910x_Radio_PllLoopFilter_Set_Validate(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_Pll_e pll,
	adi_adrv910x_PllLoopFilterCfg_t *pllLoopFilterConfig)
{
	static const uint8_t  MINIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES = 40;
	static const uint8_t  MAXIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES = 85;
	static const uint16_t MINIMUM_LOOP_FILTER_BANDWIDTH_KHZ = 50;
	static const uint16_t MAXIMUM_LOOP_FILTER_BANDWIDTH_KHZ = 1500;
	static const uint8_t  MINIMUM_POWER_SCALE_FACTOR = 0;
	static const uint8_t  MAXIMUM_POWER_SCALE_FACTOR = 10;

	/* Check adrv910x pointer is not null */
	ADI_ENTRY_PTR_EXPECT(adrv910x, pllLoopFilterConfig);

	/*Check that PLL selected is valid*/
	ADI_RANGE_CHECK(adrv910x, pll, ADI_ADRV910X_PLL_LO1, ADI_ADRV910X_PLL_LO2); /* LO1 = 0, LO2 = 1, PLL_AUX = 2*/

	/*Check that Loop Filter phase margin is between 40-85 degrees*/
	ADI_RANGE_CHECK(adrv910x,
		pllLoopFilterConfig->phaseMargin_degrees,
		MINIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES,
		MAXIMUM_PLL_LOOP_FILTER_PHASE_MARGIN_DEGREES);

	/*Check that loop filter bandwidth is between 50Khz - 1500Khz*/
	ADI_RANGE_CHECK(adrv910x,
		pllLoopFilterConfig->loopBandwidth_kHz,
		MINIMUM_LOOP_FILTER_BANDWIDTH_KHZ,
		MAXIMUM_LOOP_FILTER_BANDWIDTH_KHZ);

	/*Check that power scale factor is between 0-10*/
	ADI_RANGE_CHECK(adrv910x,
		pllLoopFilterConfig->powerScale,
		MINIMUM_POWER_SCALE_FACTOR,
		MAXIMUM_POWER_SCALE_FACTOR);

	ADI_API_RETURN(adrv910x);
}

/* Ported from adrv910x API */
int32_t adi_adrv910x_Radio_PllLoopFilter_Set(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_Pll_e     pll,
	adi_adrv910x_PllLoopFilterCfg_t *pllLoopFilterConfig)
{
	uint8_t armData[4] = { 0 };
	uint8_t extData[3] = { 0 };
	int offset = 0;
#if !ADI_ADRV910X_PRE_MCS_BROADCAST_DISABLE > 0
	int32_t halError = ADI_COMMON_ACT_NO_ACTION;
#endif

	ADI_PERFORM_VALIDATION(adi_adrv910x_Radio_PllLoopFilter_Set_Validate, adrv910x, pll, pllLoopFilterConfig);

	/* Loading byte array with parsed bytes from pllLoopFilterConfig struct */
	armData[offset++] = pllLoopFilterConfig->phaseMargin_degrees;
	adrv910x_LoadTwoBytes(&offset, armData, pllLoopFilterConfig->loopBandwidth_kHz);
	armData[offset++] = pllLoopFilterConfig->powerScale;

	/* Write PLL Frequency to ARM mailbox */
	ADI_EXPECT( adi_adrv910x_arm_Memory_Write,
		adrv910x,
		(uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET,
		&armData[0],
		sizeof(armData),
		ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4,
		ADI_PS1);

	/* Executing the SET PLL Freq command */
	extData[0] = 0;
	extData[1] = OBJID_GS_PLL_LOOPFILTER;
	extData[2] = (uint8_t)pll;

	ADI_EXPECT( adi_adrv910x_arm_Cmd_Write,
		adrv910x,
		(uint8_t)ADRV910X_ARM_SET_OPCODE,
		&extData[0],
		sizeof(extData));

#if ADI_ADRV910X_PRE_MCS_BROADCAST_DISABLE > 0
	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
		(uint8_t)ADRV910X_ARM_SET_OPCODE,
		extData[1],
		(uint32_t)ADI_ADRV910X_SETLOOPFILTER_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_SETLOOPFILTER_INTERVAL_US);
#else
	halError = adi_common_hal_Wait_us(&adrv910x->common, ADI_ADRV910X_ARM_SET_OPCODE_WAIT_INTERVAL_US);
	if (halError != ADI_COMMON_ACT_NO_ACTION)
	{
		ADI_ERROR_REPORT(&adrv910x->common,
			ADI_COMMON_ERRSRC_ADI_HAL,
			halError,
			ADI_COMMON_ACT_ERR_CHECK_TIMER,
			device,
			"Timer not working");
		ADI_ERROR_RETURN(adrv910x->common.error.newAction);
	}
#endif

	ADI_API_RETURN(adrv910x);
}

/* Ported from adrv910x API */
static __maybe_unused int32_t __maybe_unused adi_adrv910x_Radio_PllLoopFilter_Get_Validate(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_Pll_e pll,
	adi_adrv910x_PllLoopFilterCfg_t *pllLoopFilterConfig)
{
	ADI_RANGE_CHECK(adrv910x, pll, ADI_ADRV910X_PLL_LO1, ADI_ADRV910X_PLL_LO2);
	ADI_NULL_PTR_RETURN(&adrv910x->common, pllLoopFilterConfig);
	ADI_API_RETURN(adrv910x);
}

/* Ported from adrv910x API */
int32_t adi_adrv910x_Radio_PllLoopFilter_Get(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_Pll_e pll,
	adi_adrv910x_PllLoopFilterCfg_t *pllLoopFilterConfig)
{
	uint8_t armData[6] = { 0 };
	uint8_t extData[3] = { 0 };

	ADI_PERFORM_VALIDATION(adi_adrv910x_Radio_PllLoopFilter_Get_Validate, adrv910x, pll, pllLoopFilterConfig);

	/* Check adrv910x pointer is not null */
	ADI_ENTRY_PTR_EXPECT(adrv910x, pllLoopFilterConfig);

	/* Executing the GET PLL Freq command */
	extData[0] = 0;
	extData[1] = OBJID_GS_PLL_LOOPFILTER;
	extData[2] = (uint8_t)pll;

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
		(uint8_t)ADRV910X_ARM_GET_OPCODE,
		extData[1],
		(uint32_t)ADI_ADRV910X_GETLOOPFILTER_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_GETLOOPFILTER_INTERVAL_US);

	/* Read PLL Loop Filter from ARM mailbox */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		adrv910x,
		(uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armData[0],
		sizeof(armData),
		false,
		ADI_PS1);

	/*Deserialize ARM Data into pllLoopFilterConfig Structure*/
	pllLoopFilterConfig->phaseMargin_degrees = armData[0];
	pllLoopFilterConfig->loopBandwidth_kHz = (((uint16_t)armData[1]) |
	                                          ((uint16_t)armData[2] << 8));
	pllLoopFilterConfig->powerScale = armData[3];
	pllLoopFilterConfig->effectiveLoopBandwidth_kHz = (((uint16_t)armData[4]) |
	                                                   ((uint16_t)armData[5] << 8));

	ADI_API_RETURN(adrv910x);
}