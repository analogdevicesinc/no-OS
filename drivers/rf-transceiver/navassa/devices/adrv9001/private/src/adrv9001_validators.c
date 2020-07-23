/**
 * \file
 * \brief Contains ADRV9001 parameters validation related private function implementations
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* Header file corresponding to the C file */
#include "adrv9001_validators.h"

/* ADI specific header files */
#include "adi_adrv9001_error.h"

/* Header files related to libraries */

/* System header files */

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

/*********************************************************************************************************/
int32_t adi_adrv9001_Channel_Validate(adi_adrv9001_Device_t *adrv9001,
                                      adi_common_ChannelNumber_e channel)
{
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Port_Validate(adi_adrv9001_Device_t *adrv9001,
                                   adi_common_Port_e port)
{
    switch (port)
    {
    case ADI_RX:    /* Falls through */
    case ADI_TX:
        break;
    default:
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         port,
                         "Invalid parameter value. port must be either ADI_RX or ADI_TX.");
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Channel_State_GenericValidate(adi_adrv9001_Device_t *adrv9001,
                                                   adi_common_Port_e ports[],
                                                   adi_common_ChannelNumber_e channels[],
                                                   uint8_t length)
{
    uint8_t i = 0;

    /*Check that ARM and Stream processors have been loaded before enabling*/
    if (adrv9001->devStateInfo.devState < ADI_ADRV9001_STATE_ARM_LOADED)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         adrv9001->devStateInfo.devState,
                         "Channel Enable/Disable is valid only after ARM and stream processors have been loaded");
        ADI_ERROR_RETURN(adrv9001->common.error.newAction);
    }
    
    ADI_RANGE_CHECK(adrv9001, length, 1, ADI_ADRV9001_MAX_NUM_CHANNELS);

    /* Check for valid channel and port */
    for (i = 0; i < length; i++)
    {
        ADI_PERFORM_VALIDATION(adi_adrv9001_Port_Validate, adrv9001, ports[i]);
        ADI_PERFORM_VALIDATION(adi_adrv9001_Channel_Validate, adrv9001, channels[i]);
    }

    ADI_API_RETURN(adrv9001);
}
