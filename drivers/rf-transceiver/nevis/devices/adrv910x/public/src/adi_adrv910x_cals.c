/**
* \file
* \brief Contains Calibration features related function implementation defined in
* adi_adrv910x_cals.h
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
#include "adi_adrv910x_cals.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_radio.h"

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_validators.h"
#include "object_ids.h"

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

int32_t adi_adrv910x_cals_InitCals_Run(adi_adrv910x_Device_t *adrv910x,
                                       initCals_t *initCals,
                                       uint32_t timeout_ms,
                                       uint8_t *errorFlag)
{
    uint8_t payloadMailbox[12] = { 0 };
    uint8_t payload[3] = { 0 };
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t cmdStatusByte = 0;
    uint8_t errFlag = 0;

    static const uint16_t TIMEOUT_MS_FACTOR = 1000;
    static const uint32_t ADRV910X_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
    static const uint32_t ADRV910X_RXNB_TXNB  = 1; /*!< RxNB/TxNB channels */

    ADI_ENTRY_PTR_EXPECT(adrv910x, initCals);
    ADI_NULL_PTR_RETURN(&adrv910x->common, errorFlag);

    /* Bit mask info for non-channel related Init calibrations */
    payloadMailbox[0] = (uint8_t)(initCals->sysInitCalMask);
    payloadMailbox[1] = (uint8_t)(initCals->sysInitCalMask >> 8);
    payloadMailbox[2] = (uint8_t)(initCals->sysInitCalMask >> 16);
    payloadMailbox[3] = (uint8_t)(initCals->sysInitCalMask >> 24);

    /* Bit mask info for Rx1 and Tx1 channels */
    payloadMailbox[4] = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RX1_TX1]);
    payloadMailbox[5] = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RX1_TX1] >> 8);
    payloadMailbox[6] = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RX1_TX1] >> 16);
    payloadMailbox[7] = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RX1_TX1] >> 24);

    /* Bit mask info for RxNB and TxNB channels */
	payloadMailbox[8]  = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RXNB_TXNB]);
	payloadMailbox[9]  = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RXNB_TXNB] >> 8);
	payloadMailbox[10] = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RXNB_TXNB] >> 16);
	payloadMailbox[11] = (uint8_t)(initCals->chanInitCalMask[ADRV910X_RXNB_TXNB] >> 24);
	
	ADI_MUTEX_AQUIRE(adrv910x);
    
    ADI_MSG_EXPECT("Failed to write ARM mem",
                   adi_adrv910x_arm_Memory_Write,
                   adrv910x,
                   ADRV910X_ADDR_ARM_MAILBOX_RUN_INIT,
                   &payloadMailbox[0],
                   ADI_ARRAY_LEN(payloadMailbox),
                   ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    /* channelMask is ignored for RUN_INIT */
    payload[0] = 0;

    /* Mode to select the Init calibration algorithms to run */
    payload[1] = (uint8_t)(initCals->calMode);
    
    /* A value of true will force all enabled calibrations to re-run */
    payload[2] = (uint8_t)(initCals->force);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, ADRV910X_ARM_RUNINIT_OPCODE, &payload[0], ADI_ARRAY_LEN(payload));

    recoveryAction = adi_adrv910x_arm_CmdStatus_Wait(adrv910x,
                                                     ADRV910X_ARM_RUNINIT_OPCODE,
                                                     &cmdStatusByte,
                                                     (timeout_ms * TIMEOUT_MS_FACTOR),
                                                     ADI_ADRV910X_INITCALSWAIT_INTERVAL_US);

    errFlag = (cmdStatusByte >> 1);

    /* Don't update errorFlag if SPI error because errorFlag could be a random */
    /* value but update error flag for other recovery action types */
    if (recoveryAction == ADI_COMMON_ACT_ERR_RESET_INTERFACE)
    {
        *errorFlag = 0;
    }
    else
    {
        *errorFlag = errFlag;
    }

    /* ARM error handler to provide valid recovery action based on ARM error code */
    if (errFlag > 0)
    {
        ADI_EXPECT(adrv910x_ArmCmdErrorHandler,
                   adrv910x,
                   ADRV910X_ARMCMD_ERRCODE(ADRV910X_ARM_RUNINIT_OPCODE, 0, cmdStatusByte));
    }

    adrv910x->devStateInfo.devState = (adi_adrv910x_ApiStates_e)(adrv910x->devStateInfo.devState | ADI_ADRV910X_STATE_INITCALS_RUN);

    /* TODO: Uncomment when FW requires this */
#if 0
    /* TODO: Add "MCS required" flag to profile? */
    /* After some predefined delay, perform multi-chip synchronization */
    recoveryAction = adi_common_hal_Wait_us(&adrv910x->common, ADI_ADRV910X_READY_FOR_MCS_DELAY_US);
    ADI_ERROR_REPORT(&adrv910x->common,
                     ADI_COMMON_ERRSRC_ADI_HAL,
                     recoveryAction,
                     ADI_COMMON_ACT_ERR_CHECK_TIMER,
                     device,
                     "Timer not working");
    ADI_ERROR_RETURN(adrv910x->common.error.newAction);

    ADI_EXPECT(adi_adrv910x_Mcs_Execute, adrv910x, ADI_ADRV910X_MCS_EXT_ANALOG_ALL, true);
#endif
    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_InitCalsBuildDefault(initCals_t *initCals)
{
    int ii = 0;

    initCals->calMode = INIT_CAL_MODE_ALL;
    initCals->sysInitCalMask = INIT_CAL_SYSTEM_ALL;

	for (ii = 0; ii < NUM_RX_CHANNELS; ii++)
    {
        initCals->chanInitCalMask[ii] = INIT_CAL_RX_TX_ALL;
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

static __maybe_unused int32_t adi_adrv910x_TrackingCals_Channel_State_Validate(adi_adrv910x_Device_t *adrv910x,
                                                                trackingCals_t *trackingCals)
{
    uint8_t i = 0;

    adi_adrv910x_RadioState_t currentState = { 0 };

    /* Validate current state */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    for (i = 0; i < ADI_ARRAY_LEN(trackingCals->chanTrackingCalMask); i++)
    {
        /* Check whether RX1/2 and TX1/2 are in CALIBRATED state to run tracking calibrations */
        if (trackingCals->chanTrackingCalMask[i] != 0)
        {
            if (!(currentState.channelStates[0][i] == ADI_ADRV910X_CHANNEL_STANDBY) &&
                !(currentState.channelStates[0][i] == ADI_ADRV910X_CHANNEL_CALIBRATED))
            {
                ADI_ERROR_REPORT(&adrv910x->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_API_FAIL,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    currentState.channelStates[0][i],
                    "Error while attempting to send tracking calmask mailbox command to ARM firmware. RX Channel must be in CALIBRATED state.");
                ADI_API_RETURN(adrv910x)
            }
            else if (!(currentState.channelStates[1][i] == ADI_ADRV910X_CHANNEL_STANDBY) &&
                     !(currentState.channelStates[1][i] == ADI_ADRV910X_CHANNEL_CALIBRATED))
            {
                ADI_ERROR_REPORT(&adrv910x->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_API_FAIL,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    currentState.channelStates[1][i],
                    "Error while attempting to send tracking calmask mailbox command to ARM firmware. TX Channel must be in CALIBRATED state.");
                ADI_API_RETURN(adrv910x)
            }
            else
            {
                /* Channel is in desired state */
            }
        }
    }

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_cals_Tracking_Set_Validate(adi_adrv910x_Device_t *adrv910x,
                                      trackingCals_t *trackingCals)
{
    static const uint32_t TRACING_CAL_MASK_MAX = 0xF8133F;
    static const uint32_t ADRV910X_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
    static const uint32_t ADRV910X_RXNB_TXNB  = 1; /*!< Rx2/Tx2 channels */

    ADI_ENTRY_PTR_EXPECT(adrv910x, trackingCals);

    ADI_RANGE_CHECK(adrv910x, trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1], 0, TRACING_CAL_MASK_MAX);
	ADI_RANGE_CHECK(adrv910x, trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB], 0, TRACING_CAL_MASK_MAX);

    adi_adrv910x_TrackingCals_Channel_State_Validate(adrv910x, trackingCals);

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_Tracking_Set(adi_adrv910x_Device_t *adrv910x,
                                       trackingCals_t *trackingCals)
{
    uint8_t payloadMailbox[8] = { 0 };
    uint8_t payload[2] = { 0 };

    static const uint32_t ADRV910X_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
	static const uint32_t ADRV910X_RXNB_TXNB  = 1; /*!< Rx2/Tx2 channels */

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_Tracking_Set_Validate, adrv910x, trackingCals);

    /* Bit mask info for RX1-TX1 tracking calibrations */
    payloadMailbox[0] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1] >> 0);
    payloadMailbox[1] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1] >> 8);
    payloadMailbox[2] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1] >> 16);
    payloadMailbox[3] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1] >> 24);

    /* Bit mask info for RXNB-TXNB tracking calibrations */
	payloadMailbox[4] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB] >> 0);
	payloadMailbox[5] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB] >> 8);
	payloadMailbox[6] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB] >> 16);
	payloadMailbox[7] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB] >> 24);
	
	ADI_MUTEX_AQUIRE(adrv910x);
    
    ADI_MSG_EXPECT("Failed to write ARM mem",
                   adi_adrv910x_arm_Memory_Write,
                   adrv910x,
                   ADRV910X_ADDR_ARM_MAILBOX_SET,
                   &payloadMailbox[0],
                   ADI_ARRAY_LEN(payloadMailbox),
                   ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    payload[0] = 0; /* channel mask is ignored */
	payload[1] = OBJID_GS_TRACKCALS_ENABLE;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_SET_OPCODE, &payload[0], sizeof(payload));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
        (uint8_t)ADRV910X_ARM_SET_OPCODE,
        payload[1],
        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_INTERVAL_US);

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

static uint32_t __maybe_unused adi_adrv910x_cals_Tracking_Get_Validate(adi_adrv910x_Device_t *adrv910x,
                                       trackingCals_t *trackingCals)
{
    ADI_NULL_PTR_RETURN(&adrv910x->common, trackingCals);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_Tracking_Get(adi_adrv910x_Device_t *adrv910x,
                                       trackingCals_t *trackingCals)
{
    uint8_t payloadMailbox[8] = { 0 };
    uint8_t payload[2] = { 0 };

    static const uint32_t ADRV910X_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
	static const uint32_t ADRV910X_RXNB_TXNB  = 1; /*!< Rx2/Tx2 channels */

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_Tracking_Get_Validate, adrv910x, trackingCals);

    ADI_MUTEX_AQUIRE(adrv910x);

    payload[0] = 0; /* channel mask is ignored */
	payload[1] = OBJID_GS_TRACKCALS_ENABLE;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &payload[0], sizeof(payload));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        payload[1],
        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_INTERVAL_US);

    /* Read tracking calmask from ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               adrv910x,
               (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
               &payloadMailbox[0],
               sizeof(payloadMailbox),
               ADRV910X_ARM_MEM_READ_AUTOINCR,
               ADI_PS1);

    trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1] = ((uint32_t)(payloadMailbox[0]) << 0)  |
                                                          ((uint32_t)(payloadMailbox[1]) << 8)  |
                                                          ((uint32_t)(payloadMailbox[2]) << 16) |
                                                          ((uint32_t)(payloadMailbox[3]) << 24);

	trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB] = ((uint32_t)(payloadMailbox[4]) << 0)  |
                                                          ((uint32_t)(payloadMailbox[5]) << 8)  |
                                                          ((uint32_t)(payloadMailbox[6]) << 16) |
                                                          ((uint32_t)(payloadMailbox[7]) << 24);

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_InitCalsLoGenParams_Configure_Validate(adi_adrv910x_Device_t *device,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        adi_adrv910x_LoGenCalSettings_t *loGenCalSettings)
{

    ADI_EXPECT(adi_adrv910x_Port_Validate, device, port);
    ADI_EXPECT(adi_adrv910x_Channel_Validate, device, channel);

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, loGenCalSettings);
    ADI_RANGE_CHECK(device, loGenCalSettings->termRes, 0, 15);
	ADI_RANGE_CHECK(device, loGenCalSettings->s2dMode, ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL, ADI_ADRV910X_CAL_LOGEN_DIFFERENTIAL_INVERTED);


    ADI_API_RETURN(device);

}

int32_t adi_adrv910x_cals_InitCalsLoGenParams_Configure(adi_adrv910x_Device_t *device,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        adi_adrv910x_LoGenCalSettings_t *loGenCalSettings)
{
    uint8_t armData[3] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_InitCalsLoGenParams_Configure_Validate, device, port, channel, loGenCalSettings);

	armData[0] = loGenCalSettings->termRes;
	armData[1] = loGenCalSettings->vcmBiasEnable;
	armData[2] = loGenCalSettings->s2dMode;

    ADI_MUTEX_AQUIRE(device);

    /* Write Ext LO VCO cal settings to ARM mailbox */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    
    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(port, channel);
    extData[1] = OBJID_GS_EXTLO_VCO_PARAMS;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_SET_OPCODE,
        extData[1],
		(uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);
    
    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);

}

int32_t adi_adrv910x_cals_InitCalsLoGenParams_Inspect_Validate(adi_adrv910x_Device_t *device,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        adi_adrv910x_LoGenCalSettings_t *loGenCalSettings)
{

    
    ADI_EXPECT(adi_adrv910x_Port_Validate, device, port);
    ADI_EXPECT(adi_adrv910x_Channel_Validate, device, channel);

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, loGenCalSettings);
    
    ADI_API_RETURN(device);

}

int32_t adi_adrv910x_cals_InitCalsLoGenParams_Inspect(adi_adrv910x_Device_t *device,
                                                        adi_common_Port_e port,
                                                        adi_common_ChannelNumber_e channel,
                                                        adi_adrv910x_LoGenCalSettings_t *loGenCalSettings)
{
    uint8_t armReadBack[4] = { 0 };	
    uint8_t extData[2] = { 0 };
    
    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_InitCalsLoGenParams_Inspect_Validate, device, port, channel, loGenCalSettings);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(port, channel);
    extData[1] = OBJID_GS_EXTLO_VCO_PARAMS;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        extData[1],
		(uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);

	/* read the ARM memory to get loGen Cal settings */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armReadBack[0],
		sizeof(armReadBack),
		ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);

	loGenCalSettings->termRes = armReadBack[0];
	loGenCalSettings->vcmBiasEnable = (bool)armReadBack[1];
	loGenCalSettings->s2dMode = (adi_adrv910x_LoGenDifferentialMode_e)armReadBack[2];


    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);

}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_cals_ExternalPathDelay_Run_Validate(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *initCalsError)
{
    uint8_t chan_index = 0;
    adi_adrv910x_RadioState_t currentState = { 0 };

    ADI_NULL_PTR_RETURN(&adrv910x->common, initCalsError);

    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    if (currentState.channelStates[ADI_TX][chan_index] != ADI_ADRV910X_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in CALIBRATED");
        ADI_API_RETURN(adrv910x)
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_ExternalPathDelay_Run(adi_adrv910x_Device_t *adrv910x,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t timeout_ms,
                                                uint8_t *initCalsError)
{
    initCals_t initCals = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_ExternalPathDelay_Run_Validate, adrv910x, channel, initCalsError);

    initCals.sysInitCalMask = 0;
    initCals.chanInitCalMask[0] = 0;
    initCals.chanInitCalMask[1] = 0;

    /* Only ADI_ADRV910X_INIT_CAL_MODE_ELB_ONLY is allowed to run */
    initCals.calMode = INIT_CAL_MODE_ELB_ONLY;

    if (ADI_CHANNEL_1 == channel)
    {
        initCals.chanInitCalMask[0] = INIT_CAL_TX_LB_PD;
    }
    else /* ADI_CHANNEL_2 */
    {
        initCals.chanInitCalMask[1] =INIT_CAL_TX_LB_PD;
    }

    ADI_EXPECT(adi_adrv910x_cals_InitCals_Run, adrv910x, &initCals, timeout_ms, initCalsError);

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_cals_ExternalMinusInternalPathDelay_Measure_Validate(adi_adrv910x_Device_t *adrv910x,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t *externalPathDelay_ps)
{
    uint8_t chan_index = 0;
    adi_adrv910x_RadioState_t currentState = { 0 };

    ADI_NULL_PTR_RETURN(&adrv910x->common, externalPathDelay_ps);

    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    if (currentState.channelStates[ADI_TX][chan_index] != ADI_ADRV910X_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in CALIBRATED");
        ADI_API_RETURN(adrv910x)
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_ExternalMinusInternalPathDelay_Measure(adi_adrv910x_Device_t *adrv910x,
                                                                 adi_common_ChannelNumber_e channel,
                                                                 uint32_t *externalPathDelay_ps)
{
    uint8_t armReadBack[2] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_ExternalMinusInternalPathDelay_Measure_Validate, adrv910x, channel, externalPathDelay_ps);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GO_ILB_ELB_DIFF_MEASUREMENT;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
                                        (uint8_t)ADRV910X_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_INTERVAL_US);

	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
			   adrv910x,
			   (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
			   &armReadBack[0],
			   sizeof(armReadBack),
			   false, ADI_PS1);

    /* The value measured in ADRV9001 device is in tenths of a nanosecond. Multiplying by 100 to convert to picoseconds */
    *externalPathDelay_ps = (((uint32_t)(armReadBack[0]) << 0) |
                            ((uint32_t)(armReadBack[1]) << 8)) * 100;

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_cals_ExternalPathDelay_Calibrate_Validate(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_ChannelNumber_e channel,
                                             uint8_t *initCalsError,
                                             uint32_t *externalPathDelay_ps)
{
    uint8_t chan_index = 0;
    adi_adrv910x_RadioState_t currentState = { 0 };

    ADI_NULL_PTR_RETURN(&adrv910x->common, initCalsError);
    ADI_NULL_PTR_RETURN(&adrv910x->common, externalPathDelay_ps);

    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    if (currentState.channelStates[ADI_TX][chan_index] != ADI_ADRV910X_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in CALIBRATED");
        ADI_API_RETURN(adrv910x)
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_ExternalPathDelay_Calibrate(adi_adrv910x_Device_t *adrv910x,
                                                      adi_common_ChannelNumber_e channel,
                                                      uint32_t timeout_ms,
                                                      uint8_t *initCalsError,
                                                      uint32_t *externalPathDelay_ps)
{
    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_ExternalPathDelay_Calibrate_Validate,
                           adrv910x,
                           channel,
                           initCalsError,
                           externalPathDelay_ps);

    ADI_EXPECT(adi_adrv910x_cals_ExternalPathDelay_Run, adrv910x, channel, timeout_ms, initCalsError);

    ADI_EXPECT(adi_adrv910x_cals_ExternalMinusInternalPathDelay_Measure, adrv910x, channel, externalPathDelay_ps);

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_cals_ExternalPathDelay_Set_Validate(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_ChannelNumber_e channel,
                                           uint32_t externalPathDelay_ps)
{
    uint8_t chan_index = 0;
    adi_adrv910x_RadioState_t currentState = { 0 };
    static const uint32_t MAX_DELAY_ps = 6553500;

    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(adrv910x, externalPathDelay_ps, 0, MAX_DELAY_ps);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv910x_Radio_State_Get, adrv910x, &currentState);
    if (!(currentState.channelStates[ADI_TX][chan_index] == ADI_ADRV910X_CHANNEL_STANDBY) &&
        !(currentState.channelStates[ADI_TX][chan_index] == ADI_ADRV910X_CHANNEL_CALIBRATED))
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in either STANDBY or CALIBRATED");
        ADI_API_RETURN(adrv910x)
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_ExternalPathDelay_Set(adi_adrv910x_Device_t *adrv910x,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t externalPathDelay_ps)
{
    uint8_t armData[2] = { 0 };
    uint8_t extData[2] = { 0 };
    uint32_t externalPathDelay = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_ExternalPathDelay_Set_Validate, adrv910x, channel, externalPathDelay_ps);

    /* Convert picosecond to tenths of a nanosecond*/
    externalPathDelay = externalPathDelay_ps / 100;

    adrv910x_LoadTwoBytes(&offset, armData, externalPathDelay);

    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, adrv910x, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_EXT_PATH_DELAY;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
                                        (uint8_t)ADRV910X_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_INTERVAL_US);

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_cals_ExternalPathDelay_Get_Validate(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_ChannelNumber_e channel,
                                           uint32_t *externalPathDelay_ps)
{
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, externalPathDelay_ps);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_ExternalPathDelay_Get(adi_adrv910x_Device_t *adrv910x,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t *externalPathDelay_ps)
{
    uint8_t armReadBack[2] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_ExternalPathDelay_Get_Validate, adrv910x, channel, externalPathDelay_ps);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = OBJID_GS_EXT_PATH_DELAY;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
                                        (uint8_t)ADRV910X_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_INTERVAL_US);

	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
			   adrv910x,
			   (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
			   &armReadBack[0],
			   sizeof(armReadBack),
			   false, ADI_PS1);

    /* The value measured in ADRV9001 device is in tenths of a nanosecond. Multiplying by 100 to convert to picoseconds */
    *externalPathDelay_ps = (((uint32_t)(armReadBack[0]) << 0) |
                             ((uint32_t)(armReadBack[1]) << 8)) * 100;

    ADI_API_RETURN(adrv910x);
}
int32_t adi_adrv910x_cals_InternalPathDelay_Get_Validate(adi_adrv910x_Device_t *adrv910x,
                                                         adi_common_Port_e port,
                                                         adi_common_ChannelNumber_e channel,
                                                         uint32_t internalPathDelays_ns[],
                                                         uint32_t length)
{
    static uint8_t MAX_NUM_PROFILE = 6;
    adi_adrv910x_ChannelState_e state = ADI_ADRV910X_CHANNEL_STANDBY;
    
    ADI_RANGE_CHECK(adrv910x, port, ADI_RX, ADI_TX);
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv910x->common, internalPathDelays_ns);
    ADI_RANGE_CHECK(adrv910x, length, 1, MAX_NUM_PROFILE);
    ADI_EXPECT(adi_adrv910x_Radio_Channel_State_Get, adrv910x, port, channel, &state);
    if (ADI_ADRV910X_CHANNEL_STANDBY == state)
    {
        ADI_ERROR_REPORT(&adrv910x->common, 
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            state,
            "Invalid channel state. Channel state must be one of CALIBRATED, PRIMED, RF_ENABLED");
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_InternalPathDelay_Get(adi_adrv910x_Device_t *adrv910x,
                                                adi_common_Port_e port,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t internalPathDelays_ns[],
                                                uint32_t length)
{
    uint32_t offset = 0;
    uint32_t pathDelay = 0;
    uint8_t armReadBack[24] = { 0 };
    uint8_t extData[2] = { 0 };
    uint8_t i = 0;

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_InternalPathDelay_Get_Validate, adrv910x, port, channel, internalPathDelays_ns, length);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(port, channel);
    if (ADI_RX == port)
    {
        extData[1] = OBJID_GO_RX_PATH_DELAY_READ;
    }
    else /* ADI_TX */
    {
        extData[1] = OBJID_GO_TX_PATH_DELAY_READ;
    }

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_ENTRACKINGCALS_INTERVAL_US);

	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
			   adrv910x,
			   (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
			   &armReadBack[0],
			   sizeof(armReadBack),
			   ADRV910X_ARM_MEM_READ_AUTOINCR, ADI_PS1);

    for (i = 0; i < length; i++)
    {
        adrv910x_ParseFourBytes(&offset, armReadBack, &pathDelay);

        /* pathDelay[0] contains the value for the main profile
         * pathDelay[1:5] will return as 0x0 until Dynamic Profile Switching is supported */
        internalPathDelays_ns[i] = pathDelay;
    }

    ADI_API_RETURN(adrv910x);
}
