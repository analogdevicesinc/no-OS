/**
* \file
* \brief Contains Calibration features related function implementation defined in
* adi_adrv9001_cals.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"

#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_cals.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_mcs.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"

int32_t adi_adrv9001_cals_InitCals_Run(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_InitCals_t *initCals,
                                       uint32_t timeout_ms,
                                       uint8_t *errorFlag)
{
    uint8_t payloadMailbox[12] = { 0 };
    uint8_t payload[2] = { 0 };
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t cmdStatusByte = 0;
    uint8_t errFlag = 0;

    static const uint16_t TIMEOUT_MS_FACTOR = 1000;
    static const uint32_t ADRV9001_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
    static const uint32_t ADRV9001_RX2_TX2  = 1; /*!< Rx2/Tx2 channels */

    ADI_ENTRY_PTR_EXPECT(adrv9001, initCals);
    ADI_NULL_PTR_RETURN(&adrv9001->common, errorFlag);

    /* Bit mask info for non-channel related Init calibrations */
    payloadMailbox[0] = (uint8_t)(initCals->sysInitCalMask);
    payloadMailbox[1] = (uint8_t)(initCals->sysInitCalMask >> 8);
    payloadMailbox[2] = (uint8_t)(initCals->sysInitCalMask >> 16);
    payloadMailbox[3] = (uint8_t)(initCals->sysInitCalMask >> 24);

    /* Bit mask info for Rx1 and Tx1 channels */
    payloadMailbox[4] = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX1_TX1]);
    payloadMailbox[5] = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX1_TX1] >> 8);
    payloadMailbox[6] = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX1_TX1] >> 16);
    payloadMailbox[7] = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX1_TX1] >> 24);

    /* Bit mask info for Rx2 and Tx2 channels */
    payloadMailbox[8]  = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX2_TX2]);
    payloadMailbox[9]  = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX2_TX2] >> 8);
    payloadMailbox[10] = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX2_TX2] >> 16);
    payloadMailbox[11] = (uint8_t)(initCals->chanInitCalMask[ADRV9001_RX2_TX2] >> 24);

    ADI_MSG_EXPECT("Failed to write ARM mem",
                   adi_adrv9001_arm_Memory_Write,
                   adrv9001,
                   ADRV9001_ADDR_ARM_MAILBOX_RUN_INIT,
                   &payloadMailbox[0],
                   ADI_ARRAY_LEN(payloadMailbox),
                   ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    /* channelMask is ignored for RUN_INIT */
    payload[0] = 0;

    /* Mode to select the Init calibration algorithms to run */
    payload[1] = (uint8_t)(initCals->calMode);

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, ADRV9001_ARM_RUNINIT_OPCODE, &payload[0], ADI_ARRAY_LEN(payload));

    recoveryAction = adi_adrv9001_arm_CmdStatus_Wait(adrv9001,
                                                     ADRV9001_ARM_RUNINIT_OPCODE,
                                                     &cmdStatusByte,
                                                     (timeout_ms * TIMEOUT_MS_FACTOR),
                                                     ADI_ADRV9001_INITCALSWAIT_INTERVAL_US);

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
        ADI_EXPECT(adrv9001_ArmCmdErrorHandler,
                   adrv9001,
                   ADRV9001_ARMCMD_ERRCODE(ADRV9001_ARM_RUNINIT_OPCODE, 0, cmdStatusByte));
    }

    adrv9001->devStateInfo.devState = (adi_adrv9001_ApiStates_e)(adrv9001->devStateInfo.devState | ADI_ADRV9001_STATE_INITCALS_RUN);

    /* TODO: Uncomment when FW requires this */
#if 0
    /* TODO: Add "MCS required" flag to profile? */
    /* After some predefined delay, perform multi-chip synchronization */
    recoveryAction = adi_common_hal_Wait_us(&adrv9001->common, ADI_ADRV9001_READY_FOR_MCS_DELAY_US);
    ADI_ERROR_REPORT(&adrv9001->common,
                     ADI_COMMON_ERRSRC_ADI_HAL,
                     recoveryAction,
                     ADI_COMMON_ACT_ERR_CHECK_TIMER,
                     device,
                     "Timer not working");
    ADI_ERROR_RETURN(adrv9001->common.error.newAction);

    ADI_EXPECT(adi_adrv9001_Mcs_Execute, adrv9001, ADI_ADRV9001_MCS_EXT_ANALOG_ALL, true);
#endif
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_InitCalsBuildDefault(adi_adrv9001_InitCals_t *initCals)
{
    int ii = 0;

    initCals->calMode = ADI_ADRV9001_INIT_CAL_MODE_ALL;
    initCals->sysInitCalMask = ADI_ADRV9001_INIT_CAL_SYSTEM_ALL;

    for (ii = 0; ii < ADI_ADRV9001_MAX_RX_ONLY; ii++)
    {
        initCals->chanInitCalMask[ii] = ADI_ADRV9001_INIT_CAL_RX_TX_ALL;
    }

    return ADI_COMMON_ACT_NO_ACTION;
}

static int32_t adi_adrv9001_TrackingCals_Channel_State_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                adi_adrv9001_TrackingCals_t *trackingCals)
{
    uint8_t i = 0;

    adi_adrv9001_RadioState_t currentState = { 0 };

    /* Validate current state */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    for (i = 0; i < ADI_ARRAY_LEN(trackingCals->chanTrackingCalMask); i++)
    {
        /* Check whether RX1/2 and TX1/2 are in CALIBRATED state to run tracking calibrations */
        if (trackingCals->chanTrackingCalMask[i] != 0)
        {
            if (!(currentState.channelStates[0][i] == ADI_ADRV9001_CHANNEL_STANDBY) &&
                !(currentState.channelStates[0][i] == ADI_ADRV9001_CHANNEL_CALIBRATED))
            {
                ADI_ERROR_REPORT(&adrv9001->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_API_FAIL,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    currentState.channelStates[0][i],
                    "Error while attempting to send tracking calmask mailbox command to ARM firmware. RX Channel must be in CALIBRATED state.");
                ADI_API_RETURN(adrv9001)
            }
            else if (!(currentState.channelStates[1][i] == ADI_ADRV9001_CHANNEL_STANDBY) &&
                     !(currentState.channelStates[1][i] == ADI_ADRV9001_CHANNEL_CALIBRATED))
            {
                ADI_ERROR_REPORT(&adrv9001->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_API_FAIL,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    currentState.channelStates[1][i],
                    "Error while attempting to send tracking calmask mailbox command to ARM firmware. TX Channel must be in CALIBRATED state.");
                ADI_API_RETURN(adrv9001)
            }
            else
            {
                /* Channel is in desired state */
            }
        }
    }

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_cals_Tracking_Set_Validate(adi_adrv9001_Device_t *adrv9001,
                                      adi_adrv9001_TrackingCals_t *trackingCals)
{
    static const uint32_t TRACING_CAL_MASK_MAX = 0xF8133F;
    static const uint32_t ADRV9001_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
    static const uint32_t ADRV9001_RX2_TX2  = 1; /*!< Rx2/Tx2 channels */

    ADI_ENTRY_PTR_EXPECT(adrv9001, trackingCals);

    ADI_RANGE_CHECK(adrv9001, trackingCals->chanTrackingCalMask[ADRV9001_RX1_TX1], 0, TRACING_CAL_MASK_MAX);
    ADI_RANGE_CHECK(adrv9001, trackingCals->chanTrackingCalMask[ADRV9001_RX2_TX2], 0, TRACING_CAL_MASK_MAX);

    adi_adrv9001_TrackingCals_Channel_State_Validate(adrv9001, trackingCals);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_Tracking_Set(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_TrackingCals_t *trackingCals)
{
    uint8_t payloadMailbox[8] = { 0 };
    uint8_t payload[2] = { 0 };

    static const uint32_t ADRV9001_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
    static const uint32_t ADRV9001_RX2_TX2  = 1; /*!< Rx2/Tx2 channels */

    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_Tracking_Set_Validate, adrv9001, trackingCals);

    /* Bit mask info for RX1-TX1 tracking calibrations */
    payloadMailbox[0] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX1_TX1] >> 0);
    payloadMailbox[1] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX1_TX1] >> 8);
    payloadMailbox[2] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX1_TX1] >> 16);
    payloadMailbox[3] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX1_TX1] >> 24);

    /* Bit mask info for RX2-TX2 tracking calibrations */
    payloadMailbox[4] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX2_TX2] >> 0);
    payloadMailbox[5] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX2_TX2] >> 8);
    payloadMailbox[6] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX2_TX2] >> 16);
    payloadMailbox[7] = (uint8_t)(trackingCals->chanTrackingCalMask[ADRV9001_RX2_TX2] >> 24);

    ADI_MSG_EXPECT("Failed to write ARM mem",
                   adi_adrv9001_arm_Memory_Write,
                   adrv9001,
                   ADRV9001_ADDR_ARM_MAILBOX_SET,
                   &payloadMailbox[0],
                   ADI_ARRAY_LEN(payloadMailbox),
                   ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    payload[0] = 0; /* channel mask is ignored */
    payload[1] = ADRV9001_ARM_OBJECTID_TRACKING_CALIBRATIONS_ENABLE;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, (uint8_t)ADRV9001_ARM_SET_OPCODE, &payload[0], sizeof(payload));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
        (uint8_t)ADRV9001_ARM_SET_OPCODE,
        payload[1],
        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_INTERVAL_US);

    ADI_API_RETURN(adrv9001);
}

static uint32_t __maybe_unused adi_adrv9001_cals_Tracking_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_TrackingCals_t *trackingCals)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, trackingCals);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_Tracking_Get(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_TrackingCals_t *trackingCals)
{
    uint8_t payloadMailbox[8] = { 0 };
    uint8_t payload[2] = { 0 };

    static const uint32_t ADRV9001_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
    static const uint32_t ADRV9001_RX2_TX2  = 1; /*!< Rx2/Tx2 channels */

    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_Tracking_Get_Validate, adrv9001, trackingCals);

    payload[0] = 0; /* channel mask is ignored */
    payload[1] = ADRV9001_ARM_OBJECTID_TRACKING_CALIBRATIONS_ENABLE;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, (uint8_t)ADRV9001_ARM_GET_OPCODE, &payload[0], sizeof(payload));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
        (uint8_t)ADRV9001_ARM_GET_OPCODE,
        payload[1],
        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_INTERVAL_US);

    /* Read tracking calmask from ARM mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               adrv9001,
               (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_GET,
               &payloadMailbox[0],
               sizeof(payloadMailbox),
               ADRV9001_ARM_MEM_READ_AUTOINCR);

    trackingCals->chanTrackingCalMask[ADRV9001_RX1_TX1] = ((uint32_t)(payloadMailbox[0]) << 0)  |
                                                          ((uint32_t)(payloadMailbox[1]) << 8)  |
                                                          ((uint32_t)(payloadMailbox[2]) << 16) |
                                                          ((uint32_t)(payloadMailbox[3]) << 24);

    trackingCals->chanTrackingCalMask[ADRV9001_RX2_TX2] = ((uint32_t)(payloadMailbox[4]) << 0)  |
                                                          ((uint32_t)(payloadMailbox[5]) << 8)  |
                                                          ((uint32_t)(payloadMailbox[6]) << 16) |
                                                          ((uint32_t)(payloadMailbox[7]) << 24);

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_cals_ExternalPathDelay_Run_Validate(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_ChannelNumber_e channel,
                                           uint8_t *initCalsError)
{
    uint8_t chan_index = 0;
    adi_adrv9001_RadioState_t currentState = { 0 };

    ADI_NULL_PTR_RETURN(&adrv9001->common, initCalsError);

    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    if (currentState.channelStates[ADI_TX][chan_index] != ADI_ADRV9001_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in CALIBRATED");
        ADI_API_RETURN(adrv9001)
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_ExternalPathDelay_Run(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t timeout_ms,
                                                uint8_t *initCalsError)
{
    adi_adrv9001_InitCals_t initCals = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_ExternalPathDelay_Run_Validate, adrv9001, channel, initCalsError);

    initCals.sysInitCalMask = 0;
    initCals.chanInitCalMask[0] = 0;
    initCals.chanInitCalMask[1] = 0;

    /* Only ADI_ADRV9001_INIT_CAL_MODE_ELB_ONLY is allowed to run */
    initCals.calMode = ADI_ADRV9001_INIT_CAL_MODE_ELB_ONLY;

    if (ADI_CHANNEL_1 == channel)
    {
        initCals.chanInitCalMask[0] = ADI_ADRV9001_INIT_CAL_TX_LB_PD;
    }
    else /* ADI_CHANNEL_2 */
    {
        initCals.chanInitCalMask[1] = ADI_ADRV9001_INIT_CAL_TX_LB_PD;
    }

    ADI_EXPECT(adi_adrv9001_cals_InitCals_Run, adrv9001, &initCals, timeout_ms, initCalsError);

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_cals_ExternalMinusInternalPathDelay_Measure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t *externalPathDelay_ps)
{
    uint8_t chan_index = 0;
    adi_adrv9001_RadioState_t currentState = { 0 };

    ADI_NULL_PTR_RETURN(&adrv9001->common, externalPathDelay_ps);

    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    if (currentState.channelStates[ADI_TX][chan_index] != ADI_ADRV9001_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in CALIBRATED");
        ADI_API_RETURN(adrv9001)
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_ExternalMinusInternalPathDelay_Measure(adi_adrv9001_Device_t *adrv9001,
                                                                 adi_common_ChannelNumber_e channel,
                                                                 uint32_t *externalPathDelay_ps)
{
    uint8_t armReadBack[2] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_ExternalMinusInternalPathDelay_Measure_Validate, adrv9001, channel, externalPathDelay_ps);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_ILB_ELB_PATH_DELAY_DIFF;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_INTERVAL_US);

    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               adrv9001,
               (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false);

    /* The value measured in ADRV9001 device is in tenths of a nanosecond. Multiplying by 100 to convert to picoseconds */
    *externalPathDelay_ps = (((uint32_t)(armReadBack[0]) << 0) |
                            ((uint32_t)(armReadBack[1]) << 8)) * 100;

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_cals_ExternalPathDelay_Calibrate_Validate(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_ChannelNumber_e channel,
                                             uint8_t *initCalsError,
                                             uint32_t *externalPathDelay_ps)
{
    uint8_t chan_index = 0;
    adi_adrv9001_RadioState_t currentState = { 0 };

    ADI_NULL_PTR_RETURN(&adrv9001->common, initCalsError);
    ADI_NULL_PTR_RETURN(&adrv9001->common, externalPathDelay_ps);

    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    if (currentState.channelStates[ADI_TX][chan_index] != ADI_ADRV9001_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in CALIBRATED");
        ADI_API_RETURN(adrv9001)
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_ExternalPathDelay_Calibrate(adi_adrv9001_Device_t *adrv9001,
                                                      adi_common_ChannelNumber_e channel,
                                                      uint32_t timeout_ms,
                                                      uint8_t *initCalsError,
                                                      uint32_t *externalPathDelay_ps)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_ExternalPathDelay_Calibrate_Validate,
                           adrv9001,
                           channel,
                           initCalsError,
                           externalPathDelay_ps);

    ADI_EXPECT(adi_adrv9001_cals_ExternalPathDelay_Run, adrv9001, channel, timeout_ms, initCalsError);

    ADI_EXPECT(adi_adrv9001_cals_ExternalMinusInternalPathDelay_Measure, adrv9001, channel, externalPathDelay_ps);

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_cals_ExternalPathDelay_Set_Validate(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_ChannelNumber_e channel,
                                           uint32_t externalPathDelay_ps)
{
    uint8_t chan_index = 0;
    adi_adrv9001_RadioState_t currentState = { 0 };
    static const uint32_t MAX_DELAY_ps = 6553500;

    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(adrv9001, externalPathDelay_ps, 0, MAX_DELAY_ps);

    adi_common_channel_to_index(channel, &chan_index);

    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    if (!(currentState.channelStates[ADI_TX][chan_index] == ADI_ADRV9001_CHANNEL_STANDBY) &&
        !(currentState.channelStates[ADI_TX][chan_index] == ADI_ADRV9001_CHANNEL_CALIBRATED))
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_TX][chan_index],
                         "Specified channel must be in either STANDBY or CALIBRATED");
        ADI_API_RETURN(adrv9001)
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_ExternalPathDelay_Set(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t externalPathDelay_ps)
{
    uint8_t armData[2] = { 0 };
    uint8_t extData[2] = { 0 };
    uint32_t externalPathDelay = 0;
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_ExternalPathDelay_Set_Validate, adrv9001, channel, externalPathDelay_ps);

    /* Convert picosecond to tenths of a nanosecond*/
    externalPathDelay = externalPathDelay_ps / 100;

    adrv9001_LoadTwoBytes(&offset, armData, externalPathDelay);

    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_EXTERNAL_PATH_DELAY;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, (uint8_t)ADRV9001_ARM_SET_OPCODE, &extData[0], sizeof(extData));

    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        (uint8_t)ADRV9001_ARM_SET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_INTERVAL_US);

    ADI_API_RETURN(adrv9001);
}

static int32_t __maybe_unused adi_adrv9001_cals_ExternalPathDelay_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_ChannelNumber_e channel,
                                           uint32_t *externalPathDelay_ps)
{
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, externalPathDelay_ps);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_ExternalPathDelay_Get(adi_adrv9001_Device_t *adrv9001,
                                                adi_common_ChannelNumber_e channel,
                                                uint32_t *externalPathDelay_ps)
{
    uint8_t armReadBack[2] = { 0 };
    uint8_t extData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_ExternalPathDelay_Get_Validate, adrv9001, channel, externalPathDelay_ps);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_TX, channel);
    extData[1] = ADRV9001_ARM_OBJECTID_EXTERNAL_PATH_DELAY;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_INTERVAL_US);

    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               adrv9001,
               (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false);

    /* The value measured in ADRV9001 device is in tenths of a nanosecond. Multiplying by 100 to convert to picoseconds */
    *externalPathDelay_ps = (((uint32_t)(armReadBack[0]) << 0) |
                             ((uint32_t)(armReadBack[1]) << 8)) * 100;

    ADI_API_RETURN(adrv9001);
}
int32_t adi_adrv9001_cals_InternalPathDelay_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                                         adi_common_Port_e port,
                                                         adi_common_ChannelNumber_e channel,
                                                         uint32_t internalPathDelays_ns[],
                                                         uint32_t length)
{
    static uint8_t MAX_NUM_PROFILE = 6;
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    
    ADI_RANGE_CHECK(adrv9001, port, ADI_RX, ADI_TX);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&adrv9001->common, internalPathDelays_ns);
    ADI_RANGE_CHECK(adrv9001, length, 1, MAX_NUM_PROFILE);
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, adrv9001, port, channel, &state);
    if (ADI_ADRV9001_CHANNEL_STANDBY == state)
    {
        ADI_ERROR_REPORT(&adrv9001->common, 
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            state,
            "Invalid channel state. Channel state must be one of CALIBRATED, PRIMED, RF_ENABLED");
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_cals_InternalPathDelay_Get(adi_adrv9001_Device_t *adrv9001,
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

    ADI_PERFORM_VALIDATION(adi_adrv9001_cals_InternalPathDelay_Get_Validate, adrv9001, port, channel, internalPathDelays_ns, length);

    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(port, channel);
    if (ADI_RX == port)
    {
        extData[1] = ADRV9001_ARM_OBJECTID_RX_PATH_DELAY_READ;
    }
    else /* ADI_TX */
    {
        extData[1] = ADRV9001_ARM_OBJECTID_TX_PATH_DELAY_READ;
    }

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, adrv9001, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(adrv9001,
        (uint8_t)ADRV9001_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_TIMEOUT_US,
        (uint32_t)ADI_ADRV9001_ENTRACKINGCALS_INTERVAL_US);

    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
        adrv9001,
        (uint32_t)ADRV9001_ADDR_ARM_MAILBOX_GET,
        &armReadBack[0],
        sizeof(armReadBack),
        ADRV9001_ARM_MEM_READ_AUTOINCR);

    for (i = 0; i < length; i++)
    {
        adrv9001_ParseFourBytes(&offset, armReadBack, &pathDelay);

        /* pathDelay[0] contains the value for the main profile
         * pathDelay[1:5] will return as 0x0 until Dynamic Profile Switching is supported */
        internalPathDelays_ns[i] = pathDelay;
    }

    ADI_API_RETURN(adrv9001);
}
