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
	if (((initCals->chanInitCalMask[ADRV910X_RX1_TX1] & ~adrv910x->devStateInfo.txAllowedInitCalMask[ADRV910X_RX1_TX1]) != 0) ||
        ((initCals->chanInitCalMask[ADRV910X_RX1_TX1] & ~adrv910x->devStateInfo.rxAllowedInitCalMask[ADRV910X_RX1_TX1]) != 0) ||
        ((initCals->chanInitCalMask[ADRV910X_RXNB_TXNB] & ~adrv910x->devStateInfo.txAllowedInitCalMask[ADRV910X_RXNB_TXNB]) != 0) ||
        ((initCals->chanInitCalMask[ADRV910X_RXNB_TXNB] & ~adrv910x->devStateInfo.rxAllowedInitCalMask[ADRV910X_RXNB_TXNB]) != 0))
    {
	    ADRV910X_WARNING_INFO("The specified initialization calibrations do not match the profile's allowed calibrations. Disallowed calibrations will be skipped by the firmware.");
    }

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
	
	ADI_MUTEX_ACQUIRE(adrv910x);
    
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

    /* Accumulate: INIT_CAL_MODE_SYSTEM_AND_RX / _LOOPBACK_AND_TX are run as separate calls */
    adrv910x->devStateInfo.chanEnabledInitCalMask[ADRV910X_RX1_TX1] |= (uint32_t)initCals->chanInitCalMask[ADRV910X_RX1_TX1];
    adrv910x->devStateInfo.chanEnabledInitCalMask[ADRV910X_RXNB_TXNB] |= (uint32_t)initCals->chanInitCalMask[ADRV910X_RXNB_TXNB];

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

int32_t adi_adrv910x_cals_InitCalsMask_Get(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_initCalMask_t *initCalsMask)
{
	uint8_t payloadMailbox[48] = { 0 };
	uint8_t payload[2] = { 0 };

	static const uint32_t ADRV910X_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
	static const uint32_t ADRV910X_RXNB_TXNB  = 1; /*!< Rx2/Tx2 channels */
	static const uint32_t LAST_RUN_CAL_MASK_START_INDEX = 24; /*!< The index in the ARM mailbox payload where the last run cal mask info starts */

	ADI_ENTRY_PTR_EXPECT(adrv910x, initCalsMask);
	ADI_NULL_PTR_RETURN(&adrv910x->common, initCalsMask);

	ADI_MUTEX_ACQUIRE(adrv910x);

	payload[0] = 0; /* channel mask is ignored */
	payload[1] = OBJID_GO_RUN_INIT_STATUS;

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &payload[0], sizeof(payload));

	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
		(uint8_t)ADRV910X_ARM_GET_OPCODE,
		payload[1],
		(uint32_t)ADI_ADRV910X_ENTRACKINGCALS_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_ENTRACKINGCALS_INTERVAL_US);

	/* Read init calmask from ARM mailbox */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		adrv910x,
		(uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
		&payloadMailbox[0],
		sizeof(payloadMailbox),
		ADRV910X_ARM_MEM_READ_AUTOINCR,
		ADI_PS1);

	/* initCalMask_t for the lastRunCalMask is available at 24th index of the payload onwards. 
	 * The payload is of the format initCalStatus_t of which this API intends to extract only lastRunCalMask*/
	initCalsMask->sysInitCalMask = ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX]) << 0)  |
	                             ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+1]) << 8)  |
	                             ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+2]) << 16) |
	                             ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+3]) << 24);
	initCalsMask->chanInitCalMask[ADRV910X_RX1_TX1] = ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+4]) << 0)  |
	                                                      ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+5]) << 8)  |
	                                                      ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+6]) << 16) |
	                                                      ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+7]) << 24);

	initCalsMask->chanInitCalMask[ADRV910X_RXNB_TXNB] = ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+8]) << 0)  |
                                                          ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+9]) << 8)  |
                                                          ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+10]) << 16) |
                                                          ((uint32_t)(payloadMailbox[LAST_RUN_CAL_MASK_START_INDEX+11]) << 24);

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

static __maybe_unused int32_t adi_adrv910x_cals_Tracking_Set_Validate(adi_adrv910x_Device_t *adrv910x,
                                      trackingCals_t *trackingCals)
{
    static const uint32_t TRACING_CAL_MASK_MAX = 0xF8133F;
    static const uint32_t ADRV910X_RX1_TX1  = 0; /*!< Rx1/Tx1 channels */
    static const uint32_t ADRV910X_RXNB_TXNB  = 1; /*!< Rx2/Tx2 channels */

    ADI_ENTRY_PTR_EXPECT(adrv910x, trackingCals);

    ADI_RANGE_CHECK(adrv910x, trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1], 0, TRACING_CAL_MASK_MAX);
	ADI_RANGE_CHECK(adrv910x, trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB], 0, TRACING_CAL_MASK_MAX);

    adi_adrv910x_TrackingCals_Channel_State_Validate(adrv910x, trackingCals);
	
	if (((trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1] & ~adrv910x->devStateInfo.txAllowedTrackingCalMask[ADRV910X_RX1_TX1]) != 0) ||
        ((trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1] & ~adrv910x->devStateInfo.rxAllowedTrackingCalMask[ADRV910X_RX1_TX1]) != 0) ||
        ((trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB] & ~adrv910x->devStateInfo.txAllowedTrackingCalMask[ADRV910X_RXNB_TXNB]) != 0) ||
        ((trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB] & ~adrv910x->devStateInfo.rxAllowedTrackingCalMask[ADRV910X_RXNB_TXNB]) != 0))
    {
        ADRV910X_WARNING_INFO("The specified tracking calibrations do not match the profile's allowed calibrations. Disallowed calibrations will be skipped by the firmware.");
    }

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
	
	ADI_MUTEX_ACQUIRE(adrv910x);
    
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

    adrv910x->devStateInfo.chanEnabledTrackingCalMask[ADRV910X_RX1_TX1] = (uint32_t)trackingCals->chanTrackingCalMask[ADRV910X_RX1_TX1];
    adrv910x->devStateInfo.chanEnabledTrackingCalMask[ADRV910X_RXNB_TXNB] = (uint32_t)trackingCals->chanTrackingCalMask[ADRV910X_RXNB_TXNB];

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

    ADI_MUTEX_ACQUIRE(adrv910x);

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

    ADI_MUTEX_ACQUIRE(device);

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

    ADI_MUTEX_ACQUIRE(device);

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

static __maybe_unused int32_t adi_adrv910x_cals_ExternalPathDelay_Run_Validate(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_ChannelNumber_e channel,
                                           uint32_t timeout_ms,
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

    ADI_PERFORM_VALIDATION(adi_adrv910x_cals_ExternalPathDelay_Run_Validate, adrv910x, channel, timeout_ms, initCalsError);

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

static __maybe_unused int32_t adi_adrv910x_cals_ExternalMinusInternalPathDelay_Measure_Validate(adi_adrv910x_Device_t *adrv910x,
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

static __maybe_unused int32_t adi_adrv910x_cals_ExternalPathDelay_Calibrate_Validate(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_ChannelNumber_e channel,
                                             uint32_t timeout_ms,
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
                           timeout_ms,
                           initCalsError,
                           externalPathDelay_ps);

    ADI_EXPECT(adi_adrv910x_cals_ExternalPathDelay_Run, adrv910x, channel, timeout_ms, initCalsError);

    ADI_EXPECT(adi_adrv910x_cals_ExternalMinusInternalPathDelay_Measure, adrv910x, channel, externalPathDelay_ps);

    ADI_API_RETURN(adrv910x);
}

static __maybe_unused int32_t adi_adrv910x_cals_ExternalPathDelay_Set_Validate(adi_adrv910x_Device_t *adrv910x,
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

static __maybe_unused int32_t adi_adrv910x_cals_ExternalPathDelay_Get_Validate(adi_adrv910x_Device_t *adrv910x,
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


/*============================================================================
 * Calibration Status Get APIs
 *===========================================================================*/

/* The firmware cal-status handlers decode chanMask as a receive profile selector
 * (RX 0x01, ORX 0x02, ILB 0x04, ELB 0x08), not as an adi_adrv910x_ChannelMask value. */
#define ADRV910X_CAL_STATUS_PROFILE_RX          (0x01u)

/* Size of the calStatusGet_t descriptor written to the mailbox: { uint32_t len, void *pCalStatus } */
#define ADRV910X_CAL_STATUS_DESCRIPTOR_SIZE     (8u)

/* ARM memory reads must be word aligned */
#define ADRV910X_CAL_STATUS_ALIGN4(n)           (((n) + 3u) & ~3u)

/* One rfdcChCorrWords_t: rxCorrWords[64] + orxElbCorrWords[7] + lpbkCorrWords[3], 8 bytes each */
#define ADRV910X_RFDC_CH_CORR_WORDS_BYTES       ((ADI_ADRV910X_RFDC_NUM_RX_GAINS +      \
                                                  ADI_ADRV910X_RFDC_NUM_ORX_FE_GAINS + \
                                                  ADI_ADRV910X_RFDC_NUM_LPBK_GAINS) * 8u)

/* Maps a public selector onto the firmware object ID and the ARM-padded size of one
 * channel's status struct. Sizes must stay in sync with the firmware headers. */
typedef struct adrv910x_CalStatusMap
{
    uint8_t objId;      /* Firmware cal object ID */
    uint32_t size;      /* ARM-padded sizeof() of one instance */
    bool perChannel;    /* Firmware returns an array indexed by channel */
} adrv910x_CalStatusMap_t;

static const adrv910x_CalStatusMap_t adrv910x_CalStatusMap[ADI_ADRV910X_CAL_STATUS_NUM_SEL] =
{
    [ADI_ADRV910X_CAL_STATUS_TX_QEC_INIT]            = { (uint8_t)OBJID_IC_TX_QEC,          36u,   true  }, /* TxqecInit_Status_t     */
    [ADI_ADRV910X_CAL_STATUS_TX_LOL_INIT]            = { (uint8_t)OBJID_IC_TX_LOL,          72u,   true  }, /* txLol_Status_t         */
    [ADI_ADRV910X_CAL_STATUS_TX_BBAF_INIT]           = { (uint8_t)OBJID_IC_TX_BBAF,         28u,   true  }, /* txBbf_Status_t         */
    [ADI_ADRV910X_CAL_STATUS_TX_BBAF_GD_INIT]        = { (uint8_t)OBJID_IC_TX_BBAF_GD,      76u,   true  }, /* txBbf_gd_status_t      */
    [ADI_ADRV910X_CAL_STATUS_RX_HPADC_RC_INIT]       = { (uint8_t)OBJID_IC_RX_HPADC_RC,     28u,   false }, /* AdcTunerStatus_t       */
    [ADI_ADRV910X_CAL_STATUS_RX_HPADC_FLASH_INIT]    = { (uint8_t)OBJID_IC_RX_HPADC_FLASH,  120u,  false }, /* FlashCal_Status_t      */
    [ADI_ADRV910X_CAL_STATUS_RX_TIA_CUTOFF_INIT]     = { (uint8_t)OBJID_IC_RX_TIA_CUTOFF,   12u + (12u * ADI_ADRV910X_TIA_CUTOFF_NUM_LPF_VALS), true }, /* tiaCutoffCal_Status_t */
    [ADI_ADRV910X_CAL_STATUS_RX_TCAL_INIT]           = { (uint8_t)OBJID_IC_RX_TCAL,         352u,  true  }, /* tcalProfileStatus_t    */
    [ADI_ADRV910X_CAL_STATUS_RX_RFDC_OFFSET_INIT]    = { (uint8_t)OBJID_IC_RX_RFDC_OFFSET,  4744u, false }, /* rfdcCalStatus_t        */
    [ADI_ADRV910X_CAL_STATUS_RX_DMR_PATH_DELAY_INIT] = { (uint8_t)OBJID_IC_RX_DMR_PD,       4u,    false }, /* MonitorMode_DmrPdStatus */
    [ADI_ADRV910X_CAL_STATUS_TX_DPD_TRACKING]        = { (uint8_t)OBJID_TC_TX_DPD,          16u,   true  }, /* dpdChannelStatus_t     */
    [ADI_ADRV910X_CAL_STATUS_RX_HD2_TRACKING]        = { (uint8_t)OBJID_TC_RX_HD2,          20u,   true  }, /* hd2_ch_status_t        */
    [ADI_ADRV910X_CAL_STATUS_RX_WB_POLY_TRACKING]    = { (uint8_t)OBJID_TC_RX_WBPOLY,       156u,  true  }, /* poly_sm_status_t       */
    [ADI_ADRV910X_CAL_STATUS_TX_QEC_TRACKING]       = { (uint8_t)OBJID_TC_TX_QEC,          24u,   true  }, /* TxqecTrack_Status_t    */
};

/* Both initCalibrations_e and trackingCalibrations_e are laid out so that a cal's
 * mask bit is 1 << its object ID item number. Deriving the bit avoids a per-cal
 * lookup table that would go stale as object IDs are added. */
#define ADRV910X_CAL_MASK_BIT_FROM_OBJID(calObjId)  ((uint32_t)1u << GET_ITEMID_FROM_OBJID(calObjId))

/* Bits that correspond to a real per-channel cal. Object IDs landing outside these
 * (reserved IDs, system cals such as PLL/AUXPLL) are not gated by a channel mask. */
#define ADRV910X_IC_CHANNEL_CAL_BITS  ((uint32_t)INIT_CAL_RX_TX_ALL | (uint32_t)INIT_CAL_RX_DMR_PATH_DELAY)
#define ADRV910X_TC_CHANNEL_CAL_BITS  (0x00F8131Fu)

/* The requested calibration must both be allowed by the loaded profile and have
 * actually been run/enabled by the user; otherwise the status ARM returns is stale
 * or undefined. */
static int32_t adi_adrv910x_cals_CalStatus_CalEnabled_Validate(
    adi_adrv910x_Device_t *adrv910x,
    adi_common_ChannelNumber_e channel,
    uint8_t calObjId)
{
    uint32_t calMaskBit = ADRV910X_CAL_MASK_BIT_FROM_OBJID(calObjId);
    uint32_t allowedMask = 0u;
    uint32_t enabledMask = 0u;
    uint8_t chanIndex = 0u;

    adi_common_channel_to_index(channel, &chanIndex);

    if (GET_GROUPID_FROM_OBJID(calObjId) == OBJ_GROUP_ID_IC)
    {
        if ((calMaskBit & ADRV910X_IC_CHANNEL_CAL_BITS) == 0u)
        {
            ADI_API_RETURN(adrv910x);
        }
        /* The Tx and Rx allowed masks are disjoint, so OR'ing them avoids classifying the cal */
        allowedMask = adrv910x->devStateInfo.txAllowedInitCalMask[chanIndex] |
                      adrv910x->devStateInfo.rxAllowedInitCalMask[chanIndex];
        enabledMask = adrv910x->devStateInfo.chanEnabledInitCalMask[chanIndex];
    }
    else
    {
        if ((calMaskBit & ADRV910X_TC_CHANNEL_CAL_BITS) == 0u)
        {
            ADI_API_RETURN(adrv910x);
        }
        allowedMask = adrv910x->devStateInfo.txAllowedTrackingCalMask[chanIndex] |
                      adrv910x->devStateInfo.rxAllowedTrackingCalMask[chanIndex];
        enabledMask = adrv910x->devStateInfo.chanEnabledTrackingCalMask[chanIndex];
    }

    /* The firmware silently skips cals the profile disallows, so only the intersection produces status */
    if ((calMaskBit & allowedMask) == 0u)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         calObjId,
                         "The requested calibration is not allowed by the loaded profile for the specified channel");
        ADI_API_RETURN(adrv910x);
    }

    if ((calMaskBit & enabledMask) == 0u)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         calObjId,
                         "The requested calibration was not enabled for the specified channel, so no status is available");
        ADI_API_RETURN(adrv910x);
    }

    ADI_API_RETURN(adrv910x);
}

/* Called unconditionally (not via ADI_PERFORM_VALIDATION): sel indexes
 * adrv910x_CalStatusMap[] and calStatus is unconditionally memset by the caller, so
 * these checks cannot be compiled out when ADI_VALIDATE_PARAMS is undefined. */
static int32_t adi_adrv910x_cals_CalStatus_Get_Validate(
    adi_adrv910x_Device_t *adrv910x,
    adi_common_ChannelNumber_e channel,
    adi_adrv910x_CalStatusSel_e sel,
    adi_adrv910x_CalStatus_t *calStatus)
{
    ADI_NULL_PTR_RETURN(&adrv910x->common, calStatus);

    /* Casting to unsigned folds the negative case into the upper-bound test and avoids a
     * "comparison always false" warning when the enum's underlying type is unsigned. */
    if ((uint32_t)sel >= (uint32_t)ADI_ADRV910X_CAL_STATUS_NUM_SEL)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sel,
                         "Calibration status selector is out of range");
        ADI_API_RETURN(adrv910x);
    }

    if ((channel != ADI_CHANNEL_1) && (channel != ADI_CHANNEL_2))
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Channel must be ADI_CHANNEL_1 or ADI_CHANNEL_2");
        ADI_API_RETURN(adrv910x);
    }

    ADI_EXPECT(adi_adrv910x_cals_CalStatus_CalEnabled_Validate, adrv910x, channel, adrv910x_CalStatusMap[sel].objId);

    ADI_API_RETURN(adrv910x);
}


/*----------------------------------------------------------------------------
 * Little-endian readers built on the shared adrv910x_Parse*Bytes() helpers.
 * Each field is looked up by its own explicit offset (rather than one shared
 * auto-incrementing cursor) because the firmware structs have padding gaps
 * and repeated/nested arrays that don't visit bytes in a single linear pass.
 *---------------------------------------------------------------------------*/

static uint16_t adrv910x_CalStatus_Rd16(const uint8_t *raw, uint32_t offset)
{
    uint16_t value = 0u;

    adrv910x_ParseTwoBytes(&offset, (uint8_t *)raw, &value);

    return value;
}

static uint32_t adrv910x_CalStatus_Rd32(const uint8_t *raw, uint32_t offset)
{
    uint32_t value = 0u;

    adrv910x_ParseFourBytes(&offset, (uint8_t *)raw, &value);

    return value;
}

static uint64_t adrv910x_CalStatus_Rd64(const uint8_t *raw, uint32_t offset)
{
    uint64_t value = 0u;

    adrv910x_ParseEightBytes(&offset, (uint8_t *)raw, &value);

    return value;
}

static float adrv910x_CalStatus_RdFloat(const uint8_t *raw, uint32_t offset)
{
    uint32_t bits = adrv910x_CalStatus_Rd32(raw, offset);
    float value = 0.0f;

    (void)memcpy(&value, &bits, sizeof(value));

    return value;
}

/*----------------------------------------------------------------------------
 * Deserialisers. Offsets are the ARM-padded field offsets of the corresponding
 * firmware status structure.
 *---------------------------------------------------------------------------*/

static void adrv910x_CalStatus_ParseHdr(const uint8_t *raw, uint32_t offset, adi_adrv910x_CalStatusHdr_t *hdr)
{
    hdr->errorCode         = adrv910x_CalStatus_Rd32(raw, offset + 0u);
    hdr->percentComplete   = adrv910x_CalStatus_Rd32(raw, offset + 4u);
    hdr->performanceMetric = adrv910x_CalStatus_Rd32(raw, offset + 8u);
    hdr->iterCount         = adrv910x_CalStatus_Rd32(raw, offset + 12u);
    hdr->updateCount       = adrv910x_CalStatus_Rd32(raw, offset + 16u);
}

/* TxqecInit_Status_t (36): hdr(0) batchCnt(20) numEsts(22) estCnt[8](24) converged(32) pad(33) maxEstLoops(34) */
static void adrv910x_CalStatus_ParseTxQec(const uint8_t *raw, adi_adrv910x_TxQecInitStatus_t *out)
{
    uint32_t i = 0u;

    adrv910x_CalStatus_ParseHdr(raw, 0u, &out->hdr);
    out->batchCnt = adrv910x_CalStatus_Rd16(raw, 20u);
    out->numEsts  = adrv910x_CalStatus_Rd16(raw, 22u);

    for (i = 0u; i < ADI_ADRV910X_TXQEC_ATTEN_POINTS; i++)
    {
        out->estCnt[i] = raw[24u + i];
    }

    out->converged   = raw[32u];
    out->maxEstLoops = adrv910x_CalStatus_Rd16(raw, 34u);
}

static void adrv910x_CalStatus_ParseTxQecTracking(const uint8_t *raw, adi_adrv910x_TxQecTrackingStatus_t *out)
{
    adrv910x_CalStatus_ParseHdr(raw, 0u, &out->hdr);
    out->suppressedOverFreq = raw[20];   /* byte 20, immediately after the 20-byte header */
}

/* txLol_Status_t (72): hdr(0) saturationCount(20) gain(24) phase(28) mean(32) m2(36) count(40)
 * variance(44) perturbationScaler(48) batchSetCount(52) counters(56..69) updated(70) pad(71) */
static void adrv910x_CalStatus_ParseTxLol(const uint8_t *raw, adi_adrv910x_TxLolInitStatus_t *out)
{
    adrv910x_CalStatus_ParseHdr(raw, 0u, &out->hdr);
    out->saturationCount    = adrv910x_CalStatus_Rd32(raw, 20u);
    out->gain               = adrv910x_CalStatus_RdFloat(raw, 24u);
    out->phase              = adrv910x_CalStatus_RdFloat(raw, 28u);
    out->lolStatMean        = adrv910x_CalStatus_RdFloat(raw, 32u);
    out->lolStatM2          = adrv910x_CalStatus_RdFloat(raw, 36u);
    out->lolStatCount       = adrv910x_CalStatus_RdFloat(raw, 40u);
    out->lolStatVariance    = adrv910x_CalStatus_RdFloat(raw, 44u);
    out->perturbationScaler = (int32_t)adrv910x_CalStatus_Rd32(raw, 48u);
    out->batchSetCount      = adrv910x_CalStatus_Rd32(raw, 52u);
    out->startCnt           = adrv910x_CalStatus_Rd16(raw, 56u);
    out->stopCnt            = adrv910x_CalStatus_Rd16(raw, 58u);
    out->saveCnt            = adrv910x_CalStatus_Rd16(raw, 60u);
    out->completeCnt        = adrv910x_CalStatus_Rd16(raw, 62u);
    out->calExitCnt         = adrv910x_CalStatus_Rd16(raw, 64u);
    out->calEmptyCnt        = adrv910x_CalStatus_Rd16(raw, 66u);
    out->capPartialCnt      = adrv910x_CalStatus_Rd16(raw, 68u);
    out->updated            = (raw[70u] != 0u);
}

/* txBbf_Status_t (28): hdr(0) c1(20) trimQ(22) freqCalOffset(24) pad(25..27) */
static void adrv910x_CalStatus_ParseTxBbaf(const uint8_t *raw, adi_adrv910x_TxBbafInitStatus_t *out)
{
    adrv910x_CalStatus_ParseHdr(raw, 0u, &out->hdr);
    out->c1            = adrv910x_CalStatus_Rd16(raw, 20u);
    out->trimQ         = (int16_t)adrv910x_CalStatus_Rd16(raw, 22u);
    out->freqCalOffset = raw[24u];
}

/* txBbf_gd_status_t (76): hdr(0) gdNeg(20) gdPos(24) gdAvg(28) gdImageMeas[2][3](32)
 * gd(56) gdSelected(60) gdMeasCnt[3](64) gdCurveFitCnt(70) gdCurveFitUnusedCnt(72) pad(74) */
static void adrv910x_CalStatus_ParseTxBbafGd(const uint8_t *raw, adi_adrv910x_TxBbafGdInitStatus_t *out)
{
    uint32_t i = 0u;
    uint32_t j = 0u;

    adrv910x_CalStatus_ParseHdr(raw, 0u, &out->hdr);

    for (i = 0u; i < ADI_ADRV910X_TXBBAF_GD_ESTIMATES; i++)
    {
        out->gdNeg[i] = (int16_t)adrv910x_CalStatus_Rd16(raw, 20u + (i * 2u));
        out->gdPos[i] = (int16_t)adrv910x_CalStatus_Rd16(raw, 24u + (i * 2u));
        out->gdAvg[i] = (int16_t)adrv910x_CalStatus_Rd16(raw, 28u + (i * 2u));
        out->gd[i]    = (int16_t)adrv910x_CalStatus_Rd16(raw, 56u + (i * 2u));

        for (j = 0u; j < ADI_ADRV910X_TXBBAF_GD_MEAS_TYPES; j++)
        {
            out->gdImageMeas[i][j] = adrv910x_CalStatus_RdFloat(raw, 32u + (((i * ADI_ADRV910X_TXBBAF_GD_MEAS_TYPES) + j) * 4u));
        }
    }

    out->gdSelected = adrv910x_CalStatus_Rd32(raw, 60u);

    for (i = 0u; i < ADI_ADRV910X_TXBBAF_GD_MEAS_TYPES; i++)
    {
        out->gdMeasCnt[i] = adrv910x_CalStatus_Rd16(raw, 64u + (i * 2u));
    }

    out->gdCurveFitCnt       = adrv910x_CalStatus_Rd16(raw, 70u);
    out->gdCurveFitUnusedCnt = adrv910x_CalStatus_Rd16(raw, 72u);
}

/* AdcTunerStatus_t (28): hdr(0) sc(20) sg(22) rCal(24) cCal(25) adcProfileScaling(26) pad(27) */
static void adrv910x_CalStatus_ParseHpAdcRc(const uint8_t *raw, adi_adrv910x_RxHpAdcRcInitStatus_t *out)
{
    adrv910x_CalStatus_ParseHdr(raw, 0u, &out->hdr);
    out->sc                = adrv910x_CalStatus_Rd16(raw, 20u);
    out->sg                = adrv910x_CalStatus_Rd16(raw, 22u);
    out->rCalPerformed     = (raw[24u] != 0u);
    out->cCalPerformed     = (raw[25u] != 0u);
    out->adcProfileScaling = raw[26u];
}

/* FlashCal_Status_t (120): hdr(0) minimumRSSI[4](20) successCount[4](36) pad(52)
 * minimumCalCodes[4](56) unNerfMinimumCalCodes[4](88) */
static void adrv910x_CalStatus_ParseHpAdcFlash(const uint8_t *raw, adi_adrv910x_RxHpAdcFlashInitStatus_t *out)
{
    uint32_t i = 0u;

    adrv910x_CalStatus_ParseHdr(raw, 0u, &out->hdr);

    for (i = 0u; i < ADI_ADRV910X_FLASHCAL_MAX_NUM_ADC; i++)
    {
        out->minimumRssi[i]           = adrv910x_CalStatus_Rd32(raw, 20u + (i * 4u));
        out->successCount[i]          = adrv910x_CalStatus_Rd32(raw, 36u + (i * 4u));
        out->minimumCalCodes[i]       = adrv910x_CalStatus_Rd64(raw, 56u + (i * 8u));
        out->unNerfMinimumCalCodes[i] = adrv910x_CalStatus_Rd64(raw, 88u + (i * 8u));
    }
}

/* tiaCutoffCal_Status_t: errorCode(0) corner3dB(4) calCompleted(8) calConverged(9) pad(10)
 * beforeCalC1[N](12) afterCalC1[N](12+2N) beforeCalPowerDiffC1[N] afterCalPowerDiffC1[N] */
static void adrv910x_CalStatus_ParseTiaCutoff(const uint8_t *raw, adi_adrv910x_RxTiaCutoffInitStatus_t *out)
{
    const uint32_t n = ADI_ADRV910X_TIA_CUTOFF_NUM_LPF_VALS;
    const uint32_t beforeC1Offset   = 12u;
    const uint32_t afterC1Offset    = beforeC1Offset + (2u * n);
    const uint32_t beforePwrOffset  = afterC1Offset + (2u * n);
    const uint32_t afterPwrOffset   = beforePwrOffset + (4u * n);
    uint32_t i = 0u;

    out->errorCode    = adrv910x_CalStatus_Rd32(raw, 0u);
    out->corner3dB    = adrv910x_CalStatus_Rd32(raw, 4u);
    out->calCompleted = (raw[8u] != 0u);
    out->calConverged = (raw[9u] != 0u);

    for (i = 0u; i < n; i++)
    {
        out->beforeCalC1[i]          = adrv910x_CalStatus_Rd16(raw, beforeC1Offset + (i * 2u));
        out->afterCalC1[i]           = adrv910x_CalStatus_Rd16(raw, afterC1Offset + (i * 2u));
        out->beforeCalPowerDiffC1[i] = adrv910x_CalStatus_RdFloat(raw, beforePwrOffset + (i * 4u));
        out->afterCalPowerDiffC1[i]  = adrv910x_CalStatus_RdFloat(raw, afterPwrOffset + (i * 4u));
    }
}

/* tcalProfileStatus_t (352): errorCode(0) calCompleted(4) loDelayCalCompleted[3](5) busyFlag(8)
 * tonePen(12) toneNen(16) rxGainIndex(20) loc(24) hd[50](28) cfirAllPass(228)
 * cfirCoeffRe[13](232) cfirCoeffIm[13](284) loDelayCalIQ[2][3][1][2](336) freqId(348) pad(349) */
static void adrv910x_CalStatus_ParseTcal(const uint8_t *raw, adi_adrv910x_RxTcalInitStatus_t *out)
{
    uint32_t i = 0u;
    uint32_t lo = 0u;
    uint32_t pll = 0u;
    uint32_t gain = 0u;
    uint32_t iq = 0u;
    uint32_t offset = 336u;

    out->errorCode    = adrv910x_CalStatus_Rd32(raw, 0u);
    out->calCompleted = (raw[4u] != 0u);

    for (i = 0u; i < ADI_ADRV910X_RX_LODELAY_NUM_PLL; i++)
    {
        out->loDelayCalCompleted[i] = (raw[5u + i] != 0u);
    }

    out->busyFlag    = adrv910x_CalStatus_Rd32(raw, 8u);
    out->tonePen     = adrv910x_CalStatus_Rd32(raw, 12u);
    out->toneNen     = adrv910x_CalStatus_Rd32(raw, 16u);
    out->rxGainIndex = adrv910x_CalStatus_Rd32(raw, 20u);
    out->loc         = (int32_t)adrv910x_CalStatus_Rd32(raw, 24u);

    for (i = 0u; i < ADI_ADRV910X_TCAL_HD_LEN; i++)
    {
        out->hd[i] = (int32_t)adrv910x_CalStatus_Rd32(raw, 28u + (i * 4u));
    }

    out->cfirAllPass = (int32_t)adrv910x_CalStatus_Rd32(raw, 228u);

    for (i = 0u; i < ADI_ADRV910X_RXQEC_CFIR_NTAPS; i++)
    {
        out->cfirCoeffRe[i] = adrv910x_CalStatus_RdFloat(raw, 232u + (i * 4u));
        out->cfirCoeffIm[i] = adrv910x_CalStatus_RdFloat(raw, 284u + (i * 4u));
    }

    for (lo = 0u; lo < ADI_ADRV910X_RX_LODELAY_LO_TABLE_SIZE; lo++)
    {
        for (pll = 0u; pll < ADI_ADRV910X_RX_LODELAY_NUM_PLL; pll++)
        {
            for (gain = 0u; gain < ADI_ADRV910X_RX_LODELAY_GAIN_TABLE_SIZE; gain++)
            {
                for (iq = 0u; iq < ADI_ADRV910X_LO_DELAY_NUM; iq++)
                {
                    out->loDelayCalIq[lo][pll][gain][iq] = raw[offset];
                    offset++;
                }
            }
        }
    }

    out->freqId = raw[348u];
}

/* rfdcChCorrWords_t (592): rxCorrWords[64](0) orxElbCorrWords[7](512) lpbkCorrWords[3](568) */
static void adrv910x_CalStatus_ParseRfdcCorrWords(const uint8_t *raw,
                                                  uint32_t offset,
                                                  adi_adrv910x_RfdcCorrWords_t *out,
                                                  uint32_t count)
{
    uint32_t i = 0u;

    for (i = 0u; i < count; i++)
    {
        out[i].dcCorrWordI = adrv910x_CalStatus_Rd32(raw, offset + (i * 8u));
        out[i].dcCorrWordQ = adrv910x_CalStatus_Rd32(raw, offset + (i * 8u) + 4u);
    }
}

/* rfdcCalStatus_t (4744): corrWords[2][2][2](0) rxInitDone(4736) orxInitDone(4738)
 * elbInitDone(4740) ilbInitDone(4742) */
static void adrv910x_CalStatus_ParseRfdc(const uint8_t *raw, adi_adrv910x_RxRfdcOffsetInitStatus_t *out)
{
    const uint32_t initDoneOffset = ADRV910X_RFDC_CH_CORR_WORDS_BYTES *
                                    ADI_ADRV910X_RFDC_NUM_FREQ_REGIONS *
                                    ADI_ADRV910X_RFDC_NUM_LOS *
                                    ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS;
    uint32_t ch = 0u;
    uint32_t fr = 0u;
    uint32_t lo = 0u;
    uint32_t offset = 0u;

    for (ch = 0u; ch < ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS; ch++)
    {
        for (fr = 0u; fr < ADI_ADRV910X_RFDC_NUM_FREQ_REGIONS; fr++)
        {
            for (lo = 0u; lo < ADI_ADRV910X_RFDC_NUM_LOS; lo++)
            {
                adi_adrv910x_RfdcChCorrWords_t *chWords = &out->corrWords[ch][fr][lo];

                adrv910x_CalStatus_ParseRfdcCorrWords(raw, offset,
                                                      &chWords->rxCorrWords[0],
                                                      ADI_ADRV910X_RFDC_NUM_RX_GAINS);
                adrv910x_CalStatus_ParseRfdcCorrWords(raw, offset + (ADI_ADRV910X_RFDC_NUM_RX_GAINS * 8u),
                                                      &chWords->orxElbCorrWords[0],
                                                      ADI_ADRV910X_RFDC_NUM_ORX_FE_GAINS);
                adrv910x_CalStatus_ParseRfdcCorrWords(raw, offset + ((ADI_ADRV910X_RFDC_NUM_RX_GAINS + ADI_ADRV910X_RFDC_NUM_ORX_FE_GAINS) * 8u),
                                                      &chWords->lpbkCorrWords[0],
                                                      ADI_ADRV910X_RFDC_NUM_LPBK_GAINS);
                offset += ADRV910X_RFDC_CH_CORR_WORDS_BYTES;
            }
        }
    }

    for (ch = 0u; ch < ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS; ch++)
    {
        out->rxInitDone[ch]  = raw[initDoneOffset + ch];
        out->orxInitDone[ch] = raw[initDoneOffset + ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS + ch];
        out->elbInitDone[ch] = raw[initDoneOffset + (2u * ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS) + ch];
        out->ilbInitDone[ch] = raw[initDoneOffset + (3u * ADI_ADRV910X_CAL_STATUS_NUM_CHANNELS) + ch];
    }
}

/* adi_MonitorMode_DmrPdStatus_t (4): pathDelay(0) syncIndex(2) reserved(3) */
static void adrv910x_CalStatus_ParseDmrPathDelay(const uint8_t *raw, adi_adrv910x_RxDmrPathDelayInitStatus_t *out)
{
    out->pathDelay = adrv910x_CalStatus_Rd16(raw, 0u);
    out->syncIndex = raw[2u];
}

/* dpdChannelStatus_t (16): successfulIterations(0) captures(4) normalRate(8) ilbInt(9)
 * ilbFrac(10) elbInt(11) elbFrac(12) pad(13..15) */
static void adrv910x_CalStatus_ParseDpd(const uint8_t *raw, adi_adrv910x_TxDpdTrackingStatus_t *out)
{
    out->numberOfSuccessfulIterations = adrv910x_CalStatus_Rd32(raw, 0u);
    out->numberOfCaptures             = adrv910x_CalStatus_Rd32(raw, 4u);
    out->normalRate                   = raw[8u];
    out->ilbIntegerDelay              = raw[9u];
    out->ilbFractionalDelay           = raw[10u];
    out->elbIntegerDelay              = raw[11u];
    out->elbFractionalDelay           = raw[12u];
}

/* poly_sm_status_cell_t (24): iterCount(0) errorFlags(4) errorCount(8)
 * selfcheckErrorCount(12) selfcheckIrrDbMin(16) selfcheckIrrDbMax(20) */
static void adrv910x_CalStatus_ParseWbPolyCell(const uint8_t *raw, uint32_t offset, adi_adrv910x_WbPolyStatusCell_t *out)
{
    out->iterCount           = adrv910x_CalStatus_Rd32(raw, offset + 0u);
    out->errorFlags          = adrv910x_CalStatus_Rd32(raw, offset + 4u);
    out->errorCount          = adrv910x_CalStatus_Rd32(raw, offset + 8u);
    out->selfcheckErrorCount = adrv910x_CalStatus_Rd32(raw, offset + 12u);
    out->selfcheckIrrDbMin   = adrv910x_CalStatus_Rd32(raw, offset + 16u);
    out->selfcheckIrrDbMax   = adrv910x_CalStatus_Rd32(raw, offset + 20u);
}

/* poly_sm_status_t (156): hdr(0..19) curr(20) cycle(44) radioOn(68) all(92) algrStatus(116) */
static void adrv910x_CalStatus_ParseWbPoly(const uint8_t *raw, adi_adrv910x_RxWbPolyTrackingStatus_t *out)
{
    const uint32_t cellSize = 24u;
    const uint32_t cellBase = 20u;
    uint32_t algoOffset = cellBase + (4u * cellSize);
    uint32_t i = 0u;
    uint32_t *algoFields[] =
    {
        &out->algoStatus.powerProfile, &out->algoStatus.sigBins,     &out->algoStatus.posSigBins,
        &out->algoStatus.negSigBins,   &out->algoStatus.equSigBins,  &out->algoStatus.peakPowBins,
        &out->algoStatus.peakPosBins,  &out->algoStatus.peakNegBins, &out->algoStatus.algo,
        &out->algoStatus.loEstMode
    };

    out->errorCode       = adrv910x_CalStatus_Rd32(raw, 0u);
    out->percentComplete = adrv910x_CalStatus_Rd32(raw, 4u);
    out->selfcheckIrrDb  = adrv910x_CalStatus_Rd32(raw, 8u);
    out->iterCount       = adrv910x_CalStatus_Rd32(raw, 12u);
    out->updateCount     = adrv910x_CalStatus_Rd32(raw, 16u);

    adrv910x_CalStatus_ParseWbPolyCell(raw, cellBase,                  &out->curr);
    adrv910x_CalStatus_ParseWbPolyCell(raw, cellBase + cellSize,       &out->cycle);
    adrv910x_CalStatus_ParseWbPolyCell(raw, cellBase + (2u * cellSize), &out->radioOn);
    adrv910x_CalStatus_ParseWbPolyCell(raw, cellBase + (3u * cellSize), &out->all);

    for (i = 0u; i < (sizeof(algoFields) / sizeof(algoFields[0])); i++)
    {
        *algoFields[i] = adrv910x_CalStatus_Rd32(raw, algoOffset + (i * 4u));
    }
}

/*----------------------------------------------------------------------------
 * Raw fetch
 *
 * The firmware writes a calStatusGet_t descriptor { uint32_t len, void *pCalStatus }
 * into the mailbox GET buffer; the status bytes themselves live at pCalStatus in
 * ARM memory. Per-channel cals are plain arrays there, so the requested channel's
 * slice is addressed directly rather than reading every channel.
 *---------------------------------------------------------------------------*/
static int32_t adrv910x_cals_CalStatus_RawGet(
    adi_adrv910x_Device_t *adrv910x,
    adi_common_ChannelNumber_e channel,
    const adrv910x_CalStatusMap_t *map,
    uint8_t *rawBuf,
    uint32_t rawBufLen)
{
    uint8_t payload[5] = { 0 };
    uint8_t descriptorBuf[ADRV910X_CAL_STATUS_DESCRIPTOR_SIZE] = { 0 };
    uint32_t statusLen = 0u;
    uint32_t statusAddr = 0u;
    uint32_t readLen = ADRV910X_CAL_STATUS_ALIGN4(map->size);
    uint8_t channelIndex = 0u;

    /* [0] receive profile mask, [1] GET framework object ID, [2] cal object ID, [3:4] byte offset */
    payload[0] = (uint8_t)ADRV910X_CAL_STATUS_PROFILE_RX;
    payload[1] = (uint8_t)OBJID_GO_CAL_STATUS;
    payload[2] = map->objId;
    payload[3] = 0u;
    payload[4] = 0u;

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write,
               adrv910x,
               (uint8_t)ADRV910X_ARM_GET_OPCODE,
               &payload[0],
               sizeof(payload));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        payload[1],
        (uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);

    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               adrv910x,
               (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_GET,
               &descriptorBuf[0],
               sizeof(descriptorBuf),
               ADRV910X_ARM_MEM_READ_AUTOINCR,
               ADI_PS1);

    statusLen  = adrv910x_CalStatus_Rd32(&descriptorBuf[0], 0u);
    statusAddr = adrv910x_CalStatus_Rd32(&descriptorBuf[0], 4u);

    /* A zero length or a null pointer means the firmware has no status for this cal object */
    if ((statusLen == 0u) || (statusAddr == 0u))
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         map->objId,
                         "Firmware returned an empty cal status descriptor for the requested calibration");
        ADI_API_RETURN(adrv910x);
    }

    /* Deserialising a short buffer would silently produce mis-aligned field values.
     * Only one element is validated: the Rx TCAL handler points at an array base but
     * advertises the length of a single tcalProfileStatus_t. */
    if (statusLen < map->size)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         statusLen,
                         "Firmware cal status is smaller than the layout this API version expects");
        ADI_API_RETURN(adrv910x);
    }

    if (readLen > rawBufLen)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         map->size,
                         "Cal status layout is larger than the API scratch buffer");
        ADI_API_RETURN(adrv910x);
    }

    if (map->perChannel)
    {
        adi_common_channel_to_index(channel, &channelIndex);
    }

    /* Every per-channel size is a multiple of 4, so skipping whole slices keeps the read
     * word aligned. adi_adrv910x_arm_Memory_Read() writes straight into rawBuf, so a
     * large byteCount costs no extra memory. */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               adrv910x,
               statusAddr + ((uint32_t)channelIndex * map->size),
               rawBuf,
               readLen,
               ADRV910X_ARM_MEM_READ_AUTOINCR,
               ADI_PS1);

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_cals_CalStatus_Get(
    adi_adrv910x_Device_t *adrv910x,
    adi_common_ChannelNumber_e channel,
    adi_adrv910x_CalStatusSel_e sel,
    adi_adrv910x_CalStatus_t *calStatus)
{
    /* rfdcCalStatus_t alone is 4744 bytes, far past this module's stack budget, so the
     * scratch lives in the per-device state */
    uint8_t *rawBuf = NULL;
    const adrv910x_CalStatusMap_t *map = NULL;

    /* adrv910x must be checked before the ADI_EXPECT below, which dereferences it while
     * reporting the validator's result; every other input is validated inside
     * adi_adrv910x_cals_CalStatus_Get_Validate(), called unconditionally, not gated behind
     * ADI_VALIDATE_PARAMS like the rest of this file's ADI_PERFORM_VALIDATION calls. */
    ADI_NULL_DEVICE_PTR_RETURN(adrv910x);
    ADI_EXPECT(adi_adrv910x_cals_CalStatus_Get_Validate, adrv910x, channel, sel, calStatus);

    rawBuf = &adrv910x->devStateInfo.calStatusRawBuf[0];

    ADI_MUTEX_ACQUIRE(adrv910x);

    (void)memset(calStatus, 0, sizeof(*calStatus));
    calStatus->sel = sel;
    map = &adrv910x_CalStatusMap[sel];

    ADI_EXPECT(adrv910x_cals_CalStatus_RawGet, adrv910x, channel, map, &rawBuf[0], (uint32_t)sizeof(adrv910x->devStateInfo.calStatusRawBuf));

    switch (sel)
    {
    case ADI_ADRV910X_CAL_STATUS_TX_QEC_INIT:
        adrv910x_CalStatus_ParseTxQec(&rawBuf[0], &calStatus->txQecInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_TX_LOL_INIT:
        adrv910x_CalStatus_ParseTxLol(&rawBuf[0], &calStatus->txLolInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_TX_BBAF_INIT:
        adrv910x_CalStatus_ParseTxBbaf(&rawBuf[0], &calStatus->txBbafInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_TX_BBAF_GD_INIT:
        adrv910x_CalStatus_ParseTxBbafGd(&rawBuf[0], &calStatus->txBbafGdInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_HPADC_RC_INIT:
        adrv910x_CalStatus_ParseHpAdcRc(&rawBuf[0], &calStatus->rxHpAdcRcInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_HPADC_FLASH_INIT:
        adrv910x_CalStatus_ParseHpAdcFlash(&rawBuf[0], &calStatus->rxHpAdcFlashInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_TIA_CUTOFF_INIT:
        adrv910x_CalStatus_ParseTiaCutoff(&rawBuf[0], &calStatus->rxTiaCutoffInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_TCAL_INIT:
        adrv910x_CalStatus_ParseTcal(&rawBuf[0], &calStatus->rxTcalInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_RFDC_OFFSET_INIT:
        adrv910x_CalStatus_ParseRfdc(&rawBuf[0], &calStatus->rxRfdcOffsetInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_DMR_PATH_DELAY_INIT:
        adrv910x_CalStatus_ParseDmrPathDelay(&rawBuf[0], &calStatus->rxDmrPathDelayInit);
        break;
    case ADI_ADRV910X_CAL_STATUS_TX_DPD_TRACKING:
        adrv910x_CalStatus_ParseDpd(&rawBuf[0], &calStatus->txDpdTracking);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_HD2_TRACKING:
        adrv910x_CalStatus_ParseHdr(&rawBuf[0], 0u, &calStatus->rxHd2Tracking.hdr);
        break;
    case ADI_ADRV910X_CAL_STATUS_RX_WB_POLY_TRACKING:
        adrv910x_CalStatus_ParseWbPoly(&rawBuf[0], &calStatus->rxWbPolyTracking);
        break;
    case ADI_ADRV910X_CAL_STATUS_TX_QEC_TRACKING:
        adrv910x_CalStatus_ParseTxQecTracking(&rawBuf[0], &calStatus->txQecTracking);
        break;
    default:
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         sel,
                         "Unsupported calibration status selector");
        break;
    }

    ADI_MUTEX_RELEASE(adrv910x);
    ADI_API_RETURN(adrv910x);
}
