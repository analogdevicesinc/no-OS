/**
 * \file
 * \brief Contains ADRV910X parameters validation related private function implementations
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adrv910x_validators.h"
#include "adi_adrv910x_error.h"

int32_t adi_adrv910x_Channel_Validate(adi_adrv910x_Device_t *adrv910x,
                                      adi_common_ChannelNumber_e channel)
{
    ADI_RANGE_CHECK(adrv910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Port_Validate(adi_adrv910x_Device_t *adrv910x,
                                   adi_common_Port_e port)
{
    switch (port)
    {
    case ADI_RX:    /* Falls through */
    case ADI_TX:
        break;
    default:
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         port,
                         "Invalid parameter value. port must be either ADI_RX or ADI_TX.");
    }

    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_Channel_State_GenericValidate(adi_adrv910x_Device_t *adrv910x,
                                                   adi_common_Port_e ports[],
                                                   adi_common_ChannelNumber_e channels[],
                                                   uint8_t length)
{
    uint8_t i = 0;
    static const uint8_t ADRV910X_MAX_NUM_CHANNELS = 4;

    /*Check that ARM and Stream processors have been loaded before enabling*/
    if (adrv910x->devStateInfo.devState < ADI_ADRV910X_STATE_ARM_LOADED)
    {
        ADI_ERROR_REPORT(&adrv910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         adrv910x->devStateInfo.devState,
                         "Channel Enable/Disable is valid only after ARM and stream processors have been loaded");
        ADI_ERROR_RETURN(adrv910x->common.error.newAction);
    }
    
    ADI_RANGE_CHECK(adrv910x, length, 1, ADRV910X_MAX_NUM_CHANNELS);

    /* Check for valid channel and port */
    for (i = 0; i < length; i++)
    {
        ADI_PERFORM_VALIDATION(adi_adrv910x_Port_Validate, adrv910x, ports[i]);
        ADI_PERFORM_VALIDATION(adi_adrv910x_Channel_Validate, adrv910x, channels[i]);
    }

    ADI_API_RETURN(adrv910x);
}