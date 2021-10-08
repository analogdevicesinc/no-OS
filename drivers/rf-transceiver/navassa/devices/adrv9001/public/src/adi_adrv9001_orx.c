/**
* \file
* \brief Contains ORx features related function implementation defined in
* adi_adrv9001_orx.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* "adi_adrv9001_user.h" contains the #define that other header file use */
#include "adi_adrv9001_user.h"

/* Header file corresponding to the C file */
#include "adi_adrv9001_orx.h"

/* ADI specific header files */
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_radio.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_bf.h"

/* Header files related to libraries */


/* System header files */

/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

/*********************************************************************************************************/
static __maybe_unused int32_t __maybe_unused adi_adrv9001_ORx_Gain_Set_Validate(adi_adrv9001_Device_t *device,
								 adi_common_ChannelNumber_e channel,
								 uint8_t gainIndex)
{
    /* Check for valid channel */
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /*Check that ORx profile is valid*/
    if ((device->devStateInfo.profilesValid & ADI_ADRV9001_ORX_PROFILE_VALID) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            channel,
            "Gain index set requested for an ORx Channel but ORx profile is invalid in the device structure");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_RANGE_CHECK(device, gainIndex, ADI_ADRV9001_ORX_GAIN_INDEX_MIN, ADI_ADRV9001_ORX_GAIN_INDEX_MAX);
    
    ADI_API_RETURN(device)
}

int32_t adi_adrv9001_ORx_Gain_Set(adi_adrv9001_Device_t *device,
                                  adi_common_ChannelNumber_e channel,
                                  uint8_t gainIndex)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_ORx_Gain_Set_Validate, device, channel, gainIndex);

    /* Update manual gain index setting for the requested port and channel */
    if (channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainIndexOrx_Set, device, ADRV9001_BF_RXB1_CORE, gainIndex);
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainIndexOrxB_Set, device, ADRV9001_BF_RXB1_CORE, gainIndex);
    }
    else /* ORx2 */
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainIndexOrx_Set, device, ADRV9001_BF_RXB2_CORE, gainIndex);
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainIndexOrxB_Set, device, ADRV9001_BF_RXB2_CORE, gainIndex);
    }

    ADI_API_RETURN(device)
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_ORx_Gain_Get_Validate(adi_adrv9001_Device_t *device,
								 adi_common_ChannelNumber_e channel,
								 uint8_t *gainIndex)
{
    ADI_NULL_PTR_RETURN(&device->common, gainIndex);

    /*Check that the requested channel is valid*/
    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    /*Check that ORx profile is valid in current config*/
    if ((device->devStateInfo.profilesValid & ADI_ADRV9001_ORX_PROFILE_VALID) == 0)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         channel,
                         "Gain index read requested but ORx profile is invalid in device structure");
    }

    ADI_API_RETURN(device)
}

int32_t adi_adrv9001_ORx_Gain_Get(adi_adrv9001_Device_t *device,
                                 adi_common_ChannelNumber_e channel,
                                 uint8_t *gainIndex)
{
    ADI_PERFORM_VALIDATION(adi_adrv9001_ORx_Gain_Get_Validate, device, channel, gainIndex);

    /* Check the ORx channel for which the manual gain to be set */
    if (channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainIndexOrx_Get, device, ADRV9001_BF_RXB1_CORE, gainIndex);
    }
    else /* ADI_CHANNEL_2 */
    {
        ADI_EXPECT(adrv9001_NvsRegmapRxb_AgcManualGainIndexOrx_Get, device, ADRV9001_BF_RXB2_CORE, gainIndex);
    }

    ADI_API_RETURN(device);
}
