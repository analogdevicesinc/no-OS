/**
* \file
* \brief Contains Utility features related private function implementations
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adrv9001_utilities.h"

#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_cals.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_rx_gaincontrol.h"
#include "adi_adrv9001_ssi.h"
#include "adi_adrv9001_tx.h"
#include "adrv9001_powermanagement.h"
#include "adrv9001_reg_addr_macros.h"

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#endif

#ifdef ADI_DYNAMIC_PROFILE_LOAD
#ifdef __KERNEL__
int32_t adrv9001_SafeFileLoad(adi_adrv9001_Device_t *device, const char *filename, char **buffer, uint32_t *filelength)
{
    return ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED;
}
#else
int32_t adrv9001_SafeFileLoad(adi_adrv9001_Device_t *device, const char *filename, char **buffer, uint32_t *filelength)
{
    FILE * fp = NULL;

    /* Try to open the file. */
    fp = fopen(filename, "rb");

    /* Check that the file was opened. */
    if (fp == NULL)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            filename,
            "Invalid file name or path encountered.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check that we can move the file pointer from beginning to end. */
    if (fseek(fp, 0, SEEK_END) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            filename,
            "Unable to move file descriptor to the end of the file.");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fp);
    }

    *filelength = ftell(fp);

    /* Check that the file is non-empty. */
    if (*filelength <= 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            filename,
            "Empty file encountered.");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fp);
    }

    /* Rewind the file pointer to beginning of the file. */
    if (fseek(fp, 0, SEEK_SET) < 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            filename,
            "Unable to move file descriptor to the beginning of the file.");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fp);
    }

    /* Allocate space in the buffer. */
    *buffer = (char *)calloc(*filelength, sizeof(char*));

    /* Check that buffer memory allocation was successful. */
    if (NULL == *buffer)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_MEM_ALLOC_FAIL,
            ADI_COMMON_ACT_ERR_RESET_FULL,
            NULL,
            "Fatal error while reading file. Possible memory shortage.");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fp);
    }

    /* Read the file into the buffer. */
    if (fread(*buffer, 1, *filelength, fp) < *filelength)
    {
        free(*buffer);
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_RESET_FULL,
            NULL,
            "Fatal error while reading file. The file may be corrupt, or there may be a problem with memory.");
        ADI_ERROR_CLOSE_RETURN(device->common.error.newAction, fp);
    }

    fclose(fp);

    ADI_API_RETURN(device);
}
#endif
#endif

int32_t adrv9001_RadioCtrlInit(adi_adrv9001_Device_t *device, adi_adrv9001_RadioCtrlInit_t *radioCtrlInit, uint8_t channelMask, adi_adrv9001_SsiType_e ssiType)
{
    uint8_t i = 0;

    static const uint32_t RX_CHANNELS[] = { ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 };
    static const adi_common_ChannelNumber_e rxChannelArr[] = { ADI_CHANNEL_1, ADI_CHANNEL_2 };

    static const uint32_t TX_CHANNELS[] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
    static const adi_common_ChannelNumber_e txChannelArr[] = { ADI_CHANNEL_1, ADI_CHANNEL_2 };

    static adi_adrv9001_PllLoopFilterCfg_t defaultLoopFilterConfig = {
        .effectiveLoopBandwidth_kHz = 0,
        .loopBandwidth_kHz = 300,
        .phaseMargin_degrees = 60,
        .powerScale = 5
    };

    ADI_API_PRIV_ENTRY_PTR_EXPECT(device, radioCtrlInit);

    /* Set Loop filter configuration for PLL LO1 */
    ADI_EXPECT(adi_adrv9001_Radio_PllLoopFilter_Set, device, ADI_ADRV9001_PLL_LO1, &defaultLoopFilterConfig);

    /* Set Loop filter configuration for PLL LO2 */
    ADI_EXPECT(adi_adrv9001_Radio_PllLoopFilter_Set, device, ADI_ADRV9001_PLL_LO2, &defaultLoopFilterConfig);

    /* Set Loop filter configuration for AUX PLL */
    ADI_EXPECT(adi_adrv9001_Radio_PllLoopFilter_Set, device, ADI_ADRV9001_PLL_AUX, &defaultLoopFilterConfig);

    /* TODO Add Call to adi_adrv9001_MonitorModeDelay_Set when developed */
    /* Monitor mode RSSI configuration */
    ADI_EXPECT(adi_adrv9001_arm_MonitorMode_Rssi_Configure, device, &radioCtrlInit->monitorModeInitCfg.monitorModeRssiCfg);

    for (i = 0; i < ADI_ARRAY_LEN(rxChannelArr) ; i++)
    {
        if (ADRV9001_BF_EQUAL(channelMask, RX_CHANNELS[i]))
        {
            ADI_EXPECT(adi_adrv9001_Radio_Carrier_Configure,
                       device,
                       ADI_RX,
                       rxChannelArr[i],
                       &radioCtrlInit->rxCarriers[i]);

            ADI_EXPECT(adi_adrv9001_Radio_ChannelEnablementDelays_Configure,
                       device,
                       ADI_RX,
                       rxChannelArr[i],
                       &radioCtrlInit->rxEnableDelays[i]);

            ADI_EXPECT(adi_adrv9001_Rx_AdcSwitchEnable_Set,
                       device,
                       rxChannelArr[i],
                       radioCtrlInit->adcDynamicSwitchEnable[i]);

            if (ADI_ADRV9001_SSI_TYPE_LVDS == ssiType)
            {
                ADI_EXPECT(adi_adrv9001_Ssi_PowerDown_Set,
                           device,
                           ADI_RX,
                           rxChannelArr[i],
                           radioCtrlInit->rxSsiPowerDown[i]);
            }
        }
    }

    for (i = 0; i < ADI_ARRAY_LEN(txChannelArr); i++)
    {
        if (ADRV9001_BF_EQUAL(channelMask, TX_CHANNELS[i]))
        {
            ADI_EXPECT(adi_adrv9001_Radio_Carrier_Configure,
                       device,
                       ADI_TX,
                       txChannelArr[i],
                       &radioCtrlInit->txCarriers[i]);

            ADI_EXPECT(adi_adrv9001_Radio_ChannelEnablementDelays_Configure,
                       device,
                       ADI_TX,
                       txChannelArr[i],
                       &radioCtrlInit->txEnableDelays[i]);

            ADI_EXPECT(adi_adrv9001_cals_ExternalPathDelay_Set,
                       device,
                       txChannelArr[i],
                       radioCtrlInit->externalPathDelay_ps[i]);

            if (ADI_ADRV9001_SSI_TYPE_LVDS == ssiType)
            {
                ADI_EXPECT(adi_adrv9001_Ssi_PowerDown_Set,
                           device,
                           ADI_TX,
                           txChannelArr[i],
                           radioCtrlInit->txSsiPowerDown[i]);
            }

#if ADI_ADRV9001_SLEWRATE_CONFIG
            ADI_EXPECT(adi_adrv9001_Tx_SlewRateLimiter_Configure, device, txChannelArr[i], &radioCtrlInit->slewRateLimiterCfg);
#endif
        }
    }

    ADI_EXPECT(adrv9001_powermanagement_Configure, device, &radioCtrlInit->powerManagementSettings);

    ADI_API_RETURN(device);
}
