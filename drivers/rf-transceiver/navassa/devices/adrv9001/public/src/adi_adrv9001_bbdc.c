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
#include "object_ids.h"

static __maybe_unused int32_t __maybe_unused adi_adrv9001_bbdc_RejectionEnable_Set_Validate(adi_adrv9001_Device_t *device,
                                         adi_common_Port_e port,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv9001_BbdcRejectionStatus_e bbdcRejectionStatus)
{
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
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_MAILBOX_SET, &armData, sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    armExtData[0] = adi_adrv9001_Radio_MailboxChannel_Get(port, channel);
    armExtData[1] = OBJID_GS_BBDC_ENABLE;

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

static __maybe_unused int32_t __maybe_unused adi_adrv9001_bbdc_RejectionEnable_Get_Validate(adi_adrv9001_Device_t *device,
                                         adi_common_Port_e port,
                                         adi_common_ChannelNumber_e channel,
                                         adi_adrv9001_BbdcRejectionStatus_e *bbdcRejectionStatus)
{
    /* Check device pointer is not null */
    ADI_ENTRY_PTR_EXPECT(device, bbdcRejectionStatus);

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
    armExtData[1] = OBJID_GS_BBDC_ENABLE;

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

static __maybe_unused int32_t __maybe_unused adi_adrv9010_bbdc_LoopGain_Set_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                      adi_common_ChannelNumber_e channel,
                                                                      uint32_t loopGain)
{
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    if (0 == loopGain)
    {
        ADI_ERROR_REPORT(adrv9001, 
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         loopGain,
                         "Invalid parameter value. loopGain was 0, but must be greater than 0");
    }
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9010_bbdc_LoopGain_Set(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_ChannelNumber_e channel,
                                       uint32_t loopGain)
{
    uint8_t armData[8] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    
    ADI_PERFORM_VALIDATION(adi_adrv9010_bbdc_LoopGain_Set_Validate, adrv9001, channel, loopGain);
    
    adrv9001_LoadFourBytes(&offset, armData, sizeof(armData) - sizeof(uint32_t));
    adrv9001_LoadFourBytes(&offset, armData, loopGain);
    
    extData[0] = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_BBDC;
    
    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))
    
    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9010_bbdc_LoopGain_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                      adi_common_ChannelNumber_e channel,
                                                                      uint32_t *loopGain)
{
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(adrv9001, loopGain);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9010_bbdc_LoopGain_Get(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_ChannelNumber_e channel,
                                       uint32_t *loopGain)
{
    uint8_t armReadBack[4] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;
        
    ADI_PERFORM_VALIDATION(adi_adrv9010_bbdc_LoopGain_Get_Validate, adrv9001, channel, loopGain);
    
    channelMask = adi_adrv9001_Radio_MailboxChannel_Get(ADI_RX, channel);
    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_BBDC, channelMask, offset, armReadBack, sizeof(armReadBack));
    
    adrv9001_ParseFourBytes(&offset, armReadBack, loopGain);
    
    ADI_API_RETURN(adrv9001);
}
