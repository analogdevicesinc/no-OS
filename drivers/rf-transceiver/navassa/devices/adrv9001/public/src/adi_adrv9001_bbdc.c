/**
* \file
* \brief Contains baseband DC rejection algorithm related function implementation defined in
* adi_adrv9001_bbdc.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_bbdc.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_radio.h"
#include "adrv9001_arm_macros.h"

static int32_t __maybe_unused adi_adrv9001_bbdc_RejectionEnable_Set_Validate(adi_adrv9001_Device_t *device,
									     adi_common_Port_e port,
									     adi_common_ChannelNumber_e channel,
									     adi_adrv9001_BbdcRejectionStatus_e bbdcRejectionStatus)
{
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(device, bbdcRejectionStatus, ADI_ADRV9001_BBDC_REJECTION_DISABLED, ADI_ADRV9001_BBDC_REJECTION_PAUSED);

    if ((port != ADI_RX) &&
        (port != ADI_ORX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         port,
                         "Invalid parameter value. port must be ADI_RX or ADI_ORX");
        ADI_API_RETURN(device)
    }
    if (port == ADI_ORX)
    {
        port = ADI_RX;
    }
    ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, port, channel, &state);
    if (ADI_ADRV9001_CHANNEL_STANDBY == state)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         state,
                         "Invalid channel state. Must be one of CALIBRATED, PRIMED, RF_ENABLED");
    }

    ADI_API_RETURN(device);
}


int32_t adi_adrv9001_bbdc_RejectionEnable_Set(adi_adrv9001_Device_t *device,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_BbdcRejectionStatus_e bbdcRejectionStatus)
{
    uint8_t armData = 0;
    uint8_t armExtData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_bbdc_RejectionEnable_Set_Validate, device, port, channel, bbdcRejectionStatus);

    armData = (uint8_t)bbdcRejectionStatus;
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_MAILBOX_SET, &armData, sizeof(armData));

    armExtData[0] = adi_adrv9001_Radio_MailboxChannel_Get(port, channel);
    armExtData[1] = ADRV9001_ARM_OBJECTID_BBDC_ENABLE;

    /* send ARM SET opcode */
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_SET_OPCODE, armExtData, sizeof(armExtData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_SET_OPCODE,
                                        armExtData[1],
                                        ADI_ADRV9001_SETBBDCREJECTION_ENABLE_TIMEOUT_US,
                                        ADI_ADRV9001_SETBBDCREJECTION_ENABLE_INTERVAL_US);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adi_adrv9001_bbdc_RejectionEnable_Get_Validate(adi_adrv9001_Device_t *device,
									     adi_common_Port_e port,
									     adi_common_ChannelNumber_e channel,
									     adi_adrv9001_BbdcRejectionStatus_e *bbdcRejectionStatus)
{
    /* Check device pointer is not null */
    ADI_API_ENTRY_PTR_EXPECT(device, bbdcRejectionStatus);

    if ((port != ADI_RX) &&
        (port != ADI_ORX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         port,
                         "Invalid parameter value. port must be ADI_RX or ADI_ORX");
        ADI_API_RETURN(device)

    }

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_bbdc_RejectionEnable_Get(adi_adrv9001_Device_t *device,
                                              adi_common_Port_e port,
                                              adi_common_ChannelNumber_e channel,
                                              adi_adrv9001_BbdcRejectionStatus_e *bbdcRejectionStatus)
{
    uint8_t armData[2] = { 0 };
    uint8_t armExtData[2] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_bbdc_RejectionEnable_Get_Validate, device, port, channel, bbdcRejectionStatus);

    armExtData[0] = adi_adrv9001_Radio_MailboxChannel_Get(port, channel);
    armExtData[1] = ADRV9001_ARM_OBJECTID_BBDC_ENABLE;

    /* send ARM GET opcode */
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, armExtData, sizeof(armExtData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_GET_OPCODE,
                                        armExtData[1],
                                        ADI_ADRV9001_SETBBDCREJECTION_ENABLE_TIMEOUT_US,
                                        ADI_ADRV9001_SETBBDCREJECTION_ENABLE_INTERVAL_US);

    /* Read ORX BBDC status from ARM mailbox */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
                   device,
                   ADRV9001_ADDR_ARM_MAILBOX_GET,
                   armData,
                   sizeof(armData),
                   false);

    *bbdcRejectionStatus = (adi_adrv9001_BbdcRejectionStatus_e)armData[0];

    ADI_API_RETURN(device);
}
