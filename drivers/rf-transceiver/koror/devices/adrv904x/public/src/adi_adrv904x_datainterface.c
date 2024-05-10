/**
* Copyright 2015 - 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_datainterface.c
* \brief Contains function implementation defined in adi_adrv904x_datainterface.h
*
* ADRV904X API Version: 2.10.0.4
*/

#include "adi_adrv904x_datainterface.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_cals.h"
#include "adi_adrv904x_version.h"

#include "../../private/include/adrv904x_datainterface.h"

#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_cpu_cmd_ser_reset.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/include/adrv904x_struct_endian.h"


#include "../../private/bf/adrv904x_bf_tx_dig.h"
#include "../../private/bf/adrv904x_bf_orx_dig.h"


#define ADI_FILE ADI_ADRV904X_FILE_PUBLIC_DATAINTERFACE
#define NELEMS(x)   (sizeof(x) / sizeof((x)[0]))

static adi_adrv904x_ErrAction_e adrv904x_IlasChksum(adi_adrv904x_Device_t* const    device,
                                                    adi_adrv904x_DfrmCompareData_v2_t* dfrmData);


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxOrxDataCaptureStart( adi_adrv904x_Device_t* const                  device,
                                                                     const adi_adrv904x_RxChannels_e               channelSelect,
                                                                     const adi_adrv904x_RxOrxDataCaptureLocation_e captureLocation,
                                                                     uint32_t                                      captureData[],
                                                                     const uint32_t                                captureLength,
                                                                     const uint8_t                                 trigger,
                                                                     const uint32_t                                timeout_us)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t config = 0U;
    uint32_t cptCfgAddr = 0U;
    adrv904x_CpuCmd_CaptureRamType_e captureRamType = ADRV904X_CPU_CMD_CAP_RAM_TYPE_DPD;
    uint8_t success = 0U;
    adi_adrv904x_Channels_e fwChannel = ADI_ADRV904X_CH0;
    uint8_t bCptBusy = 1U;
    uint32_t bStreamDbgFlag = 1U;
    const uint32_t writeMask = 0xFFFFFFFFU;
    uint32_t waitInterval_us = 0U;
    uint32_t numEventChecks = 1U;
    uint32_t eventCheck = 0U;
    uint8_t curRadClkDivValue = 0U;
    uint8_t newRadClkDivValue = 0U;
    uint8_t streamingEnabled  = 0U;

    /* Null pointer checks */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, captureData, cleanup);

    /* range check the capture data length parameter */
    if ((channelSelect == ADI_ADRV904X_ORX0 ||
        channelSelect == ADI_ADRV904X_ORX1) &&
        (captureLength == ADI_ADRV904X_CAPTURE_SIZE_16K ||
        captureLength == ADI_ADRV904X_CAPTURE_SIZE_32K))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, captureLength, "Error in captureLength size parameter. The captureLength for the ORx capture should be 12k or less");
        goto cleanup;
    }
    if (captureLength != ADI_ADRV904X_CAPTURE_SIZE_16K &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_32K &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_12K &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_8K &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_4K &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_2K &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_1K &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_512 &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_256 &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_128 &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_64 &&
        captureLength != ADI_ADRV904X_CAPTURE_SIZE_32)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, captureLength, "Error in captureLength size parameter.");
        goto cleanup;
    }

    /* range check the channel selection parameter */
    if (channelSelect != ADI_ADRV904X_RX0 &&
        channelSelect != ADI_ADRV904X_RX1 &&
        channelSelect != ADI_ADRV904X_RX2 &&
        channelSelect != ADI_ADRV904X_RX3 &&
        channelSelect != ADI_ADRV904X_RX4 &&
        channelSelect != ADI_ADRV904X_RX5 &&
        channelSelect != ADI_ADRV904X_RX6 &&
        channelSelect != ADI_ADRV904X_RX7 &&
        channelSelect != ADI_ADRV904X_ORX0 &&
        channelSelect != ADI_ADRV904X_ORX1
        )
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, channelSelect, "Error in the channel selection parameter. Invalid channel selected for capture or readback");
        goto cleanup;
    }

    /* range check the capture location parameter */
    if (captureLocation != ADI_ADRV904X_CAPTURE_LOC_DDC0 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_DATAPATH &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_DPD  &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_DPD_PRE &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_DPD_POST &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_FSC &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX0 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX1 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX2 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX3 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX4 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX5 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX6 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_ORX_TX7 &&
        captureLocation != ADI_ADRV904X_CAPTURE_LOC_DDC1)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, captureLocation, "Error in the data capture location parameter. Invalid location selected for capture");
        goto cleanup;
    }

     /* Determine the number of reads equivalent to the requested timeout */
    waitInterval_us = (1000U > timeout_us) ? timeout_us : 1000U;
    numEventChecks = (waitInterval_us == 0U) ? 1U : (timeout_us / waitInterval_us);
    numEventChecks = (numEventChecks == 1) ? 2U : numEventChecks; /* Iterate at least twice through the for loops below so we wait at least once */

    /* resolve the channel and ram type for the firmware */
    if (channelSelect == ADI_ADRV904X_ORX0)
    {
        captureRamType = ADRV904X_CPU_CMD_CAP_RAM_TYPE_ORX;
        fwChannel = ADI_ADRV904X_CH0;
    }
    else if (channelSelect == ADI_ADRV904X_ORX1)
    {
        captureRamType = ADRV904X_CPU_CMD_CAP_RAM_TYPE_ORX;
        fwChannel = ADI_ADRV904X_CH1;
    }
    else
    {
        fwChannel = (adi_adrv904x_Channels_e) channelSelect;
    }

    /* Set the configuration */
    recoveryAction = adrv904x_RxOrxDataCaptureConfigSet(device, captureLocation, captureLength, &config);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, captureLocation, "Error setting the data capture configuration");
        goto cleanup;
    }

    /* figure out what address the config register is in */
    recoveryAction = adrv904x_RxOrxDataCaptureConfigAddressGet(device, channelSelect, &cptCfgAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error determining the data capture configuration register base address");
        goto cleanup;
    }

    /* Write the configuration to the capture control register */
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, cptCfgAddr, config, writeMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the data capture configuration register");
        goto cleanup;
    }

    /* Retrieve the current divider n value so it can be reset after the capture completes */
    if (channelSelect == ADI_ADRV904X_ORX0 ||
        channelSelect == ADI_ADRV904X_ORX1)
    {
        recoveryAction = adrv904x_OrxDig_ObsCapMemClkDivideRatio_BfGet(device,
                                                                       NULL,
                                                                       (adrv904x_BfOrxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                       &curRadClkDivValue);
    }
    else
    {
        recoveryAction = adrv904x_TxDig_RadClkDivideRatio_BfGet(device,
                                                                NULL,
                                                                (adrv904x_BfTxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                &curRadClkDivValue);
    }
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from the capture memory clock divider bit field");
        goto cleanup;
    }

    /* Calculate what the divider n value needs to be to match the capture clock rate to the sample rate */
    recoveryAction = adrv904x_RadClkDividerValueCalculate(device,
                                                          channelSelect,
                                                          captureLocation,
                                                          &newRadClkDivValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error calculating the capture memory clock divider value");
        goto cleanup;
    }

    /* Set the capture clock divider n value */
    if (channelSelect == ADI_ADRV904X_ORX0 ||
        channelSelect == ADI_ADRV904X_ORX1)
    {
        recoveryAction = adrv904x_OrxDig_ObsCapMemClkDivideRatio_BfSet(device,
                                                                       NULL,
                                                                       (adrv904x_BfOrxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                       newRadClkDivValue);
    }
    else
    {
        recoveryAction = adrv904x_TxDig_RadClkDivideRatio_BfSet(device,
                                                                NULL,
                                                                (adrv904x_BfTxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                newRadClkDivValue);
    }

    /* Send firmware command to request access to the RAM */
    recoveryAction = adrv904x_CpuRamAccessStart(device, captureRamType, fwChannel, &success);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error during firmware interaction");
        goto cleanup;
    }

    /* Check that RAM access has been granted */
    if (success == 1)
    {
        /* RAM is available for capture and read back
         * if trigger is 0 immediately trigger the capture
         * otherwise capture is triggered by the stream processor
         */

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the capture memory clock divider bit field");
            goto cleanup;
        }

        if (!trigger)
        {
            /* API sets the capture enable bit ; offset is accounted for in bf function so cptCfgAddr is masked to get the slice base address */
            if (channelSelect == ADI_ADRV904X_ORX0 ||
                channelSelect == ADI_ADRV904X_ORX1)
            {
                recoveryAction = adrv904x_OrxDig_CptTrigger_BfSet(device,
                                                                  NULL,
                                                                  (adrv904x_BfOrxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                  (uint8_t) 1U);
            }
            else
            {
                recoveryAction = adrv904x_TxDig_TxCptTrigger_BfSet(device,
                                                                   NULL,
                                                                   (adrv904x_BfTxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                   (uint8_t) 1U);
            }
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the data capture configuration register");
                goto cleanup;
            }
        }
    }
    else
    {
        /* RAM is not available for capture or read back */
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error gaining access to the capture memory resource");
        goto cleanup;
    }

    if (trigger)
    {
        /* poll the stream debug register scratch 1 */
        for (eventCheck = 0U; eventCheck <= numEventChecks; eventCheck++)
        {
            recoveryAction = adrv904x_RxOrxDataCaptureStreamDebugPoll(device, channelSelect, &bStreamDbgFlag);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from stream debug register");
                goto cleanup;
            }

            if (bStreamDbgFlag == 0)
            {
                break;
            }

            if (eventCheck < numEventChecks) /* Don't wait on last iteration */
            {
                /* Wait the specified wait interval, then check again for status. */
                recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
                    goto cleanup;
                }
            }

        }

        if (bStreamDbgFlag > 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Timed out waiting for the data capture to start");
            goto cleanup;
        }
    }

    /* poll the capture status register until capture busy goes low. */
    for (eventCheck = 0U; eventCheck <= numEventChecks; eventCheck++)
    {
        recoveryAction = adrv904x_StatusRegisterPoll(device, channelSelect, &bCptBusy);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from capture memory status register");
            goto cleanup;
        }

        if (bCptBusy == 0)
        {
            break;
        }

        if (eventCheck < numEventChecks) /* Don't wait on last iteration */
        {
            /* Wait the specified wait interval, then check again for status. */
            recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, waitInterval_us);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
                goto cleanup;
            }
        }
    }

    if (bCptBusy > 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Timed out waiting for Internal RAM capture to complete");
        goto cleanup;
    }

    /* API clears the capture enable bit ; offset is accounted for in bf function so cptCfgAddr is masked to get the slice base address */
    if (channelSelect == ADI_ADRV904X_ORX0 ||
        channelSelect == ADI_ADRV904X_ORX1)
    {
        recoveryAction = adrv904x_OrxDig_CptTrigger_BfSet( device,
                                                           NULL,
                                                           (adrv904x_BfOrxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                           (uint8_t) 0U);
    }
    else
    {
        recoveryAction = adrv904x_TxDig_TxCptTrigger_BfSet( device,
                                                            NULL,
                                                            (adrv904x_BfTxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                            (uint8_t) 0U);
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the data capture configuration register");
        goto cleanup;
    }

    /* Put the part into streaming mode to help facilitate the large, contiguous SPI reads in RAM */
    recoveryAction = adrv904x_SpiStreamingEntry(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with entering Spi Streaming mode");
        goto cleanup;
    }
    streamingEnabled = 1U;
    
    /* Read back data from capture memory */
    recoveryAction = adrv904x_RxOrxDataCaptureRead(device, channelSelect, captureData, captureLength, captureLocation);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading from capture memory bank 0");
        goto cleanup;
    }

    /* Send firmware command to stop accessing the ram */
    if (success == 1U)
    {
        /* TODO: This breaks ORx readback */
        /* clear success */
        success = 0U;
        recoveryAction = adrv904x_CpuRamAccessStop(device, captureRamType, fwChannel, &success);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error during firmware interaction");
            goto cleanup;
        }
    }
    /* Check that RAM access has been revoked */
    if (success != 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to revoke access to the capture memory resource");
        goto cleanup;
    }

    /* Set the capture clock divider n value back to what it was before the capture */
    if (channelSelect == ADI_ADRV904X_ORX0 ||
        channelSelect == ADI_ADRV904X_ORX1)
    {
        recoveryAction = adrv904x_OrxDig_ObsCapMemClkDivideRatio_BfSet(device,
                                                                        NULL,
                                                                        (adrv904x_BfOrxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                        curRadClkDivValue);
    }
    else
    {
        recoveryAction = adrv904x_TxDig_RadClkDivideRatio_BfSet(device,
                                                                NULL,
                                                                (adrv904x_BfTxDigChanAddr_e) (cptCfgAddr & ADI_ADRV904X_CAPTURE_LOC_SLICE_MASK),
                                                                curRadClkDivValue);
    }
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error writing to the capture memory clock divider bit field");
        goto cleanup;
    }

cleanup:
    if (streamingEnabled)
    {
        if (ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }
    
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AdcSampleXbarSet(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_FramerSel_e  framerSel,
                                                               const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t                      adcIdx   = 0U;
    uint8_t                      validAdc = ADI_FALSE;
    uint8_t                      mValue   = 0U;
    uint32_t                     i = 0U;
    adi_adrv904x_AdcSampleXbarSel_e validAdcXbar[] =
    {
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX0_BAND_1_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX1_BAND_1_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX2_BAND_1_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX3_BAND_1_DATA_Q,

        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX4_BAND_1_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX5_BAND_1_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX6_BAND_1_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_0_DATA_Q,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_I,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_RX7_BAND_1_DATA_Q,

        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_0,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_1,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_2,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_3,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_4,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_5,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_6,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_I_7,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_0,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_1,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_2,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_3,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_4,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_5,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_6,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX0_DATA_Q_7,

        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_0,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_1,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_2,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_3,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_4,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_5,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_6,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_I_7,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_0,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_1,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_2,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_3,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_4,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_5,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_6,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_ORX1_DATA_Q_7,
        ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID
    };

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, adcXbar, cleanup);

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSel, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Get the number of converters (M) */
    recoveryAction = adrv904x_JtxLink_JtxMCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &mValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters for the selected framer");
        goto cleanup;
    }
    mValue += 1;

    if (mValue > ADI_ADRV904X_NUM_FRAMERS_SAMPLE_XBAR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid M value read from the selected framer");
        goto cleanup;
    }

    /* ADC Xbar values validation */
    for (adcIdx = 0; adcIdx < mValue; adcIdx++)
    {
        validAdc = ADI_FALSE;
        for (i = 0; i < (sizeof(validAdcXbar) / sizeof(*validAdcXbar)); i++)
        {
            if (adcXbar->AdcSampleXbar[adcIdx] != validAdcXbar[i])
            {
                continue;
            }
            else
            {
                validAdc = ADI_TRUE;
                break;
            }
        }

        /* Expect validAdc to be true (1U) */
        if (validAdc == ADI_FALSE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid ADC Sample Crossbar selection");
            goto cleanup;
        }
    }


    {
        adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
        adi_adrv904x_CddcMode_e cddcMode = ADI_ADRV904X_CDDC_MODE;
        uint32_t  numberOfConverters = ADI_ADRV904X_NUM_FRAMERS_SAMPLE_XBAR;

        /* Framer 2 is not supported in this API */
        if (framerSel == ADI_ADRV904X_FRAMER_2)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection.");
            goto cleanup;
        }
        else if (framerSel == ADI_ADRV904X_FRAMER_1)
        {
            /* framer 1 only has half of framer-0 */
            numberOfConverters /= 2;
        }

        /* Only supported in bypass mode */
        recoveryAction =  adi_adrv904x_CddcModeGet(device, framerSel, &cddcMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Mode for the selected framer");
            goto cleanup;
        }

        if (cddcMode != ADI_ADRV904X_CDDC_BYPASS_MODE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CDDC Mode. It must be in CDDC Bypass mode.");
            goto cleanup;
        }

        /* program the converters */
        for (adcIdx = 0; adcIdx < numberOfConverters; adcIdx++)
        {
            if (adcXbar->AdcSampleXbar[adcIdx] == ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID)
            {
                /* Disable the converters */
                if (framerSel == ADI_ADRV904X_FRAMER_0)
                {
                    recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet(device,
                                                                                       NULL,
                                                                                       commonBaseAddr,
                                                                                       adcIdx,
                                                                                       1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting sample crossbar for the selected framer");
                        goto cleanup;
                    }
                }
                else if (framerSel == ADI_ADRV904X_FRAMER_1)
                {
                    recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet(device,
                                                                                       NULL,
                                                                                       commonBaseAddr,
                                                                                       adcIdx,
                                                                                       1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting sample crossbar for the selected framer");
                        goto cleanup;
                    }
                }
            }
            else
            {
                /* Enabling the converters and writing Sample Xbar value */
                if (framerSel == ADI_ADRV904X_FRAMER_0)
                {
                    /* Enabling converters */
                    recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet(device,
                                                                                       NULL,
                                                                                       commonBaseAddr,
                                                                                       adcIdx,
                                                                                       0U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling converter for the selected framer");
                        goto cleanup;
                    }

                    /* Writing Sample Xbar value */
                    recoveryAction = adrv904x_JesdCommon_RxSampleSelLink0_BfSet(device,
                                                                                NULL,
                                                                                commonBaseAddr,
                                                                                adcIdx,
                                                                                (uint8_t) adcXbar->AdcSampleXbar[adcIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting sample crossbar value for the selected framer");
                        goto cleanup;
                    }
                }
                else if (framerSel == ADI_ADRV904X_FRAMER_1)
                {
                    /* Enabling converters */
                    recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet(device,
                                                                                       NULL,
                                                                                       commonBaseAddr,
                                                                                       adcIdx,
                                                                                       0U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling converter for the selected framer");
                        goto cleanup;
                    }

                    /* Writing Sample Xbar value */
                    recoveryAction = adrv904x_JesdCommon_RxSampleSelLink1_BfSet(device,
                                                                                NULL,
                                                                                commonBaseAddr,
                                                                                adcIdx,
                                                                                (uint8_t) adcXbar->AdcSampleXbar[adcIdx]);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting sample crossbar value for the selected framer");
                        goto cleanup;
                    }
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AdcSampleXbarGet(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_FramerSel_e  framerSel,
                                                               adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                      xbarIdx     = 0U;
    uint8_t                      bfValue     = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, adcXbar, cleanup);


    {
        adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
        adi_adrv904x_CddcMode_e cddcMode = ADI_ADRV904X_CDDC_MODE;
        uint32_t  numberOfConverters = ADI_ADRV904X_NUM_FRAMERS_SAMPLE_XBAR;

        /* Framer 2 is not supported in this API */
        if (framerSel == ADI_ADRV904X_FRAMER_2)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection.");
            goto cleanup;
        }
        else if (framerSel == ADI_ADRV904X_FRAMER_1)
        {
            /* framer 1 only has half of framer-0 */
            numberOfConverters /= 2;
        }

        /* Only supported in bypass mode */
        recoveryAction =  adi_adrv904x_CddcModeGet(device, framerSel, &cddcMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Mode for the selected framer");
            goto cleanup;
        }

        if (cddcMode != ADI_ADRV904X_CDDC_BYPASS_MODE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CDDC Mode. It must be in CDDC Bypass mode.");
            goto cleanup;
        }

        for (xbarIdx = 0U; xbarIdx < numberOfConverters; xbarIdx++)
        {
            if (framerSel == ADI_ADRV904X_FRAMER_0)
            {
                /* Get this status of converters */
                recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet(device,
                                                                                   NULL,
                                                                                   commonBaseAddr,
                                                                                   xbarIdx,
                                                                                   &bfValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting converter status for the selected framer");
                    goto cleanup;
                }

                if (bfValue == 1U) /* Converter is in disable */
                {
                    adcXbar->AdcSampleXbar[xbarIdx] = ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID;
                }
                else /* Converter is in enable */
                {
                    /* Getting Sample Xbar value */
                    recoveryAction = adrv904x_JesdCommon_RxSampleSelLink0_BfGet(device,
                                                                                NULL,
                                                                                commonBaseAddr,
                                                                                xbarIdx,
                                                                                &bfValue);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting sample crossbar value for the selected framer");
                        goto cleanup;
                    }
                    adcXbar->AdcSampleXbar[xbarIdx] = (adi_adrv904x_AdcSampleXbarSel_e) bfValue;
                }
            }
            else if (framerSel == ADI_ADRV904X_FRAMER_1)
            {
                /* Get this status of converters */
                recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet(device,
                                                                                   NULL,
                                                                                   commonBaseAddr,
                                                                                   xbarIdx,
                                                                                   &bfValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting converter status for the selected framer");
                    goto cleanup;
                }

                if (bfValue == 1U) /* Converter is in disable */
                {
                    adcXbar->AdcSampleXbar[xbarIdx] = ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID;
                }
                else /* Converter is in enable */
                {
                    /* Getting Sample Xbar value */
                    recoveryAction = adrv904x_JesdCommon_RxSampleSelLink1_BfGet(device,
                                                                                NULL,
                                                                                commonBaseAddr,
                                                                                xbarIdx,
                                                                                &bfValue);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting sample crossbar value for the selected framer");
                        goto cleanup;
                    }
                    adcXbar->AdcSampleXbar[xbarIdx] = (adi_adrv904x_AdcSampleXbarSel_e) bfValue;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DacSampleXbarSet(adi_adrv904x_Device_t* const device,
                                                               const adi_adrv904x_DeframerSel_e  deframerSel,
                                                               const adi_adrv904x_DacSampleXbarCfg_t* const dacXbar)
{
    adi_adrv904x_ErrAction_e     recoveryAction   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t                      dacIdx        = 0U;
    uint8_t                      mValue        = 0U;
    uint8_t                      disabledCount = 0U;
    uint8_t                      validDac      = ADI_FALSE;
    uint32_t                     i             = 0U;

    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    adi_adrv904x_CducMode_e cducMode = ADI_ADRV904X_CDUC_MODE;
    adi_adrv904x_DacSampleXbarSel_e validDacXbar[] =
    {
        ADI_ADRV904X_DEFRAMER_OUT0,
        ADI_ADRV904X_DEFRAMER_OUT1,
        ADI_ADRV904X_DEFRAMER_OUT2,
        ADI_ADRV904X_DEFRAMER_OUT3,
        ADI_ADRV904X_DEFRAMER_OUT4,
        ADI_ADRV904X_DEFRAMER_OUT5,
        ADI_ADRV904X_DEFRAMER_OUT6,
        ADI_ADRV904X_DEFRAMER_OUT7,
        ADI_ADRV904X_DEFRAMER_OUT8,
        ADI_ADRV904X_DEFRAMER_OUT9,
        ADI_ADRV904X_DEFRAMER_OUT10,
        ADI_ADRV904X_DEFRAMER_OUT11,
        ADI_ADRV904X_DEFRAMER_OUT12,
        ADI_ADRV904X_DEFRAMER_OUT13,
        ADI_ADRV904X_DEFRAMER_OUT14,
        ADI_ADRV904X_DEFRAMER_OUT15,

        ADI_ADRV904X_DEFRAMER_OUT16,
        ADI_ADRV904X_DEFRAMER_OUT17,
        ADI_ADRV904X_DEFRAMER_OUT18,
        ADI_ADRV904X_DEFRAMER_OUT19,
        ADI_ADRV904X_DEFRAMER_OUT20,
        ADI_ADRV904X_DEFRAMER_OUT21,
        ADI_ADRV904X_DEFRAMER_OUT22,
        ADI_ADRV904X_DEFRAMER_OUT23,
        ADI_ADRV904X_DEFRAMER_OUT24,
        ADI_ADRV904X_DEFRAMER_OUT25,
        ADI_ADRV904X_DEFRAMER_OUT26,
        ADI_ADRV904X_DEFRAMER_OUT27,
        ADI_ADRV904X_DEFRAMER_OUT28,
        ADI_ADRV904X_DEFRAMER_OUT29,
        ADI_ADRV904X_DEFRAMER_OUT30,
        ADI_ADRV904X_DEFRAMER_OUT31,

        ADI_ADRV904X_DEFRAMER_OUT_INVALID
    };

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dacXbar, cleanup);

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }


        /* Only supported in bypass mode */
    recoveryAction =  adi_adrv904x_CducModeGet(device, deframerSel, &cducMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Mode for the selected deframer");
        goto cleanup;
    }

    if (cducMode != ADI_ADRV904X_CDUC_BYPASS_MODE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CDUC Mode. It must be in CDUC Bypass mode.");
        goto cleanup;
    }

    /* DAC Xbar values validation */
    for (dacIdx = 0; dacIdx < ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR; dacIdx++)
    {
        /* validate each I and Q of a channel */
        validDac = ADI_FALSE;
        for (i = 0; i < (sizeof(validDacXbar) / sizeof(*validDacXbar)); i++)
        {
            if (dacXbar->txDacChan[dacIdx] != validDacXbar[i])
            {
                continue;
            }
            else
            {
                validDac = ADI_TRUE;
                break;
            }
        }

        /* Expect validDac to be true (1U) */
        if (validDac == ADI_FALSE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid DAC Sample Crossbar selection");
            goto cleanup;
        }
    }

    /* Counting the disable sample xbar */
    for (dacIdx = 0; dacIdx < ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR; dacIdx++)
    {
        /* Counting the disable ones */
        if (dacXbar->txDacChan[dacIdx] == ADI_ADRV904X_DEFRAMER_OUT_INVALID)

        {
            disabledCount++;
        }
    }

    /* get M - Number of converters per device */
    recoveryAction = adrv904x_JrxLink_JrxCoreMCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &mValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters per device");
        goto cleanup;
    }

    mValue += 1;

    /* Verify number of disabled selections, this should be equal to 'ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR -mValue ' */
    if (disabledCount != (ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR - mValue))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Number of valid converter selections doesn't match with deframer M value");
        goto cleanup;
    }


    for (dacIdx = 0; dacIdx < ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR; dacIdx++)
    {
        if (dacXbar->txDacChan[dacIdx] == ADI_ADRV904X_DEFRAMER_OUT_INVALID)
        {
            /* Disable the DAC converters */
            if (deframerSel == ADI_ADRV904X_DEFRAMER_0)
            {
                recoveryAction = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet(device,
                                                                                   NULL,
                                                                                   commonBaseAddr,
                                                                                   dacIdx,
                                                                                   1U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling DAC sample crossbar for the selected deframer");
                    goto cleanup;
                }
            }
            else if (deframerSel == ADI_ADRV904X_DEFRAMER_1)
            {
                recoveryAction = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet(device,
                                                                                   NULL,
                                                                                   commonBaseAddr,
                                                                                   dacIdx,
                                                                                   1U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling DAC sample crossbar for the selected deframer");
                    goto cleanup;
                }
            }
        }
        else /* Valid DAC sample Xbar value */
        {
            /* Enabling the converters and writing Sample Xbar value */
            if (deframerSel == ADI_ADRV904X_DEFRAMER_0)
            {
                /* Enabling converters */
                recoveryAction = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet(device,
                                                                                   NULL,
                                                                                   commonBaseAddr,
                                                                                   dacIdx,
                                                                                   0U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling converter for the selected deframer");
                    goto cleanup;
                }

                /* Writing Sample Xbar value */
                recoveryAction = adrv904x_JesdCommon_TxSampleSelLink0_BfSet(device,
                                                                            NULL,
                                                                            commonBaseAddr,
                                                                            dacIdx,
                                                                            (uint8_t) dacXbar->txDacChan[dacIdx]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting sample crossbar value for the selected deframer");
                    goto cleanup;
                }
            }
            else if (deframerSel == ADI_ADRV904X_DEFRAMER_1)
            {
                /* Enabling converters */
                recoveryAction = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet(device,
                                                                                   NULL,
                                                                                   commonBaseAddr,
                                                                                   dacIdx,
                                                                                   0U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling converter for the selected deframer");
                    goto cleanup;
                }

                /* Writing DAC Sample Xbar value */
                recoveryAction = adrv904x_JesdCommon_TxSampleSelLink1_BfSet(device,
                                                                            NULL,
                                                                            commonBaseAddr,
                                                                            dacIdx,
                                                                            (uint8_t) dacXbar->txDacChan[dacIdx]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting sample crossbar value for the selected framer");
                    goto cleanup;
                }
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DacSampleXbarGet(adi_adrv904x_Device_t* const           device,
                                                               const adi_adrv904x_DeframerSel_e       deframerSel,
                                                               adi_adrv904x_DacSampleXbarCfg_t* const dacXbar)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr         = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t                      bfValue = 0U;
    uint8_t                      dacIdx = 0U;


    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    adi_adrv904x_CducMode_e cducMode = ADI_ADRV904X_CDUC_MODE;
    uint8_t                      dacValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dacXbar, cleanup);


        /* Only supported in bypass mode */
    recoveryAction =  adi_adrv904x_CducModeGet(device, deframerSel, &cducMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Mode for the selected deframer");
        goto cleanup;
    }

    if (cducMode != ADI_ADRV904X_CDUC_BYPASS_MODE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CDUC Mode. It must be in CDUC Bypass mode.");
        goto cleanup;
    }

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    for (dacIdx = 0U; dacIdx < ADI_ADRV904X_NUM_DAC_SAMPLE_XBAR; dacIdx++)
    {
        if (deframerSel == ADI_ADRV904X_DEFRAMER_0)
        {
            /* Get this status of converters */
            recoveryAction = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet(device,
                                                                               NULL,
                                                                               commonBaseAddr,
                                                                               dacIdx,
                                                                               &bfValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting converter status for the selected deframer");
                goto cleanup;
            }

            if (bfValue == 1U) /* Converter is in disable */
            {
                dacXbar->txDacChan[dacIdx] = ADI_ADRV904X_DEFRAMER_OUT_INVALID;
            }
            else /* Converter is in enable */
            {
                /* Getting Sample Xbar value */
                recoveryAction = adrv904x_JesdCommon_TxSampleSelLink0_BfGet(device,
                                                                            NULL,
                                                                            commonBaseAddr,
                                                                            dacIdx,
                                                                            &dacValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting sample crossbar value for the selected framer");
                    goto cleanup;
                }
                dacXbar->txDacChan[dacIdx] = (adi_adrv904x_DacSampleXbarSel_e) dacValue;
            }
        }
        else if (deframerSel == ADI_ADRV904X_DEFRAMER_1)
        {
            /* Get this status of converters */
            recoveryAction = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet(device,
                                                                               NULL,
                                                                               commonBaseAddr,
                                                                               dacIdx,
                                                                               &bfValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting converter status for the selected deframer");
                goto cleanup;
            }

            if (bfValue == 1U) /* Converter is in disable */
            {
                dacXbar->txDacChan[dacIdx] = ADI_ADRV904X_DEFRAMER_OUT_INVALID;
            }
            else /* Converter is in enable */
            {
                /* Getting Sample Xbar value */
                recoveryAction = adrv904x_JesdCommon_TxSampleSelLink1_BfGet(device,
                                                                            NULL,
                                                                            commonBaseAddr,
                                                                            dacIdx,
                                                                            &dacValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting sample crossbar value for the selected deframer");
                    goto cleanup;
                }
                dacXbar->txDacChan[dacIdx] = (adi_adrv904x_DacSampleXbarSel_e) dacValue;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerCfgGet(adi_adrv904x_Device_t*    const  device,
                                                           const adi_adrv904x_FramerSel_e   framerSel,
                                                           adi_adrv904x_FramerCfg_t* const  framerCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction    = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t                      dataByte          = 0U;
    uint8_t                      converter         = 0U;
    uint8_t                      enabledFramers    = 0U;
    uint8_t                      framerIdx         = 0U;
    uint8_t                      tmpByte           = 0U;
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, framerCfg, cleanup);
    ADI_LIBRARY_MEMSET(framerCfg, 0, sizeof(adi_adrv904x_FramerCfg_t));

    /* Get all framers link state */
    recoveryAction =adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                        &enabledFramers);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get framer link state failed");
        goto cleanup;
    }

    /* currently not used */
    framerCfg->overSample = 0;

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSel, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Get the framer link type */
    recoveryAction = adrv904x_JtxLink_JtxLinkType_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &framerCfg->enableJesd204C);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected framer");
        goto cleanup;
    }

    /* Get the framer band ID */
    recoveryAction = adrv904x_JtxLink_JtxBidCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->bankId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get band ID for the selected framer");
        goto cleanup;
    }

    /* Get the framer device link ID */
    recoveryAction = adrv904x_JtxLink_JtxDidCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->deviceId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get device link ID for the selected framer");
        goto cleanup;
    }

    /* Get the framer lane ID */
    recoveryAction = adrv904x_JtxLink_JtxLidCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->lane0Id);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane ID for the selected framer");
        goto cleanup;
    }

    /* Get the number of converters (M) */
    recoveryAction = adrv904x_JtxLink_JtxMCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &framerCfg->jesd204M);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters for the selected framer");
        goto cleanup;
    }
    ++framerCfg->jesd204M;

    /* Get the number of frames in a multi-frame/block (K) */
    recoveryAction = adrv904x_JtxLink_JtxKCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in a multi-frame/block for the selected framer");
        goto cleanup;
    }

    framerCfg->jesd204K = tmpByte + 1;

    /* Get the number of octets per lane (F) */
    recoveryAction = adrv904x_JtxLink_JtxFCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &framerCfg->jesd204F);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of octets per frame for the selected framer");
        goto cleanup;
    }
    ++framerCfg->jesd204F;

    /* Get the total number of bits per sample */
    recoveryAction = adrv904x_JtxLink_JtxNpCfg_BfGet(device,
                                                     NULL,
                                                     framerBaseAddr,
                                                     &framerCfg->jesd204Np);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get total number of bits per sample for the selected framer");
        goto cleanup;
    }
    ++framerCfg->jesd204Np;

    /* Get the number of multiblocks in extended multiblocks (E) */
    framerCfg->jesd204E = 0;
    if (framerCfg->enableJesd204C)
    {
        recoveryAction = adrv904x_JtxLink_JtxECfg_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &framerCfg->jesd204E);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of multiblocks in extended multiblocks for the selected framer");
            goto cleanup;
        }
    }

    /* Get the scrambler enable */
    recoveryAction = adrv904x_JtxLink_JtxScrCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->scramble);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get alternative scrambler enable for the selected framer");
        goto cleanup;
    }

    /* Get the lane crossbar selection and physical lane crossbar */
    recoveryAction = adrv904x_FramerLaneEnableGet(device, framerSel, framerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection and physical lane");
        goto cleanup;
    }

    /* Get the LMFC phase adjustment */
    recoveryAction = adrv904x_JtxLink_JtxTplPhaseAdjust_BfGet(device,
                                                              NULL,
                                                              framerBaseAddr,
                                                              &framerCfg->lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the LMFC phase adjustment for the selected framer");
        goto cleanup;
    }

    /* Get the Sysref for Relink */
    recoveryAction = adrv904x_JtxLink_JtxSysrefForRelink_BfGet(device,
                                                               NULL,
                                                               framerBaseAddr,
                                                               &framerCfg->newSysrefOnRelink);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sysref Relink for the selected framer");
        goto cleanup;
    }

    /* Get the Sync-N */
    recoveryAction = adrv904x_JtxLink_JtxSyncNSel_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &framerCfg->syncbInSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sync-N for the selected framer");
        goto cleanup;
    }

    /* Get the Sync In LVDS Mode Enable */
    recoveryAction = adrv904x_Core_SyncInCmosSchmittTrigEnable_BfGet(device,
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                     &dataByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sync In LVDS Mode Enable");
        goto cleanup;
    }
    if (framerCfg->syncbInSelect == 0U)
    {
        framerCfg->syncbInLvdsMode = dataByte & 0x01;
    }
    else if (framerCfg->syncbInSelect == 1U)
    {
        framerCfg->syncbInLvdsMode = (dataByte >> 1U) & 0x01;
    }
    else if (framerCfg->syncbInSelect == 2U)
    {
        framerCfg->syncbInLvdsMode = (dataByte >> 2U) & 0x01;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error syncbInSelect is invalid");
        goto cleanup;
    }

    /* Get the Sync In LVDS Pn Invert */
    recoveryAction = adrv904x_Core_SyncInLvdsPnInvert_BfGet(device,
                                                            NULL,
                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                            &framerCfg->syncbInLvdsMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sync In LVDS Pn Invert");
        goto cleanup;
    }


    if (framerSel == ADI_ADRV904X_FRAMER_2)
    {
        /* Get the ORX ADC Sample Xbar for framer 2 */
        recoveryAction = adi_adrv904x_OrxAdcSampleXbarGet(device,
                                                          framerSel,
                                                          &framerCfg->orxAdcCrossbar);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sample crossbar converter.");
            goto cleanup;
        }
    }
    else
    {
        /* Get the ADC Sample Xbar */
        recoveryAction = adi_adrv904x_AdcSampleXbarGet(device,
                                                       framerSel,
                                                       &framerCfg->adcCrossbar);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sample crossbar converter.");
            goto cleanup;
        }
    }

    /* Check jesd204M for number of ADCs, mark the unused ones as ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID */
    for (converter = framerCfg->jesd204M; converter < ADI_ADRV904X_MAX_CONVERTER; converter++)
    {
        framerCfg->adcCrossbar.AdcSampleXbar[converter] = ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID;
    }

    /* Get the sysref for start up */
    recoveryAction = adrv904x_JtxLink_JtxSysrefForStartup_BfGet(device,
                                                                NULL,
                                                                framerBaseAddr,
                                                                &framerCfg->sysrefForStartup);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref for start up");
        goto cleanup;
    }

    /* Get the sysref-N shot enable */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefNShotEnable_BfGet(device,
                                                                    NULL,
                                                                    framerBaseAddr,
                                                                    &framerCfg->sysrefNShotEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref-N shot enable");
        goto cleanup;
    }

    /* Get the sysref-N shot count */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefNShotCount_BfGet(device,
                                                                   NULL,
                                                                   framerBaseAddr,
                                                                   &framerCfg->sysrefNShotCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref-N shot count");
        goto cleanup;
    }

    /* Get the sysref ignore when linked */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefIgnoreWhenLinked_BfGet(device,
                                                                         NULL,
                                                                         framerBaseAddr,
                                                                         &framerCfg->sysrefIgnoreWhenLinked);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref ignore when linked");
        goto cleanup;
    }

    /* Get the Framer IQ Rate and Lane Rate */
    switch ((int32_t) framerSel)
    {
        case ADI_ADRV904X_FRAMER_0:
            framerIdx = 0;
            break;

        case ADI_ADRV904X_FRAMER_1:
            framerIdx = 1;
            break;

        case ADI_ADRV904X_FRAMER_2:
            framerIdx = 2;
            break;
    }
    framerCfg->iqRate_kHz = device->initExtract.jesdSetting.framerSetting[framerIdx].iqRate_kHz;
    framerCfg->laneRate_kHz = device->initExtract.jesdSetting.framerSetting[framerIdx].laneRate_kHz;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerCfgGet(adi_adrv904x_Device_t* const        device,
                                                             adi_adrv904x_DeframerSel_e          deframerSel,
                                                             adi_adrv904x_DeframerCfg_t* const   deframerCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr                            = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t                      enabledDeframers                            = 0U;
    uint8_t                      laneIdx                                     = 0U;
    uint8_t                      deframerIdx                                 = 0U;
    uint8_t                      tmpByte                                     = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, deframerCfg, cleanup);
    ADI_LIBRARY_MEMSET(deframerCfg, 0, sizeof(adi_adrv904x_DeframerCfg_t));

    /* Get all deframers link state */
    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &enabledDeframers);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get deframer link state failed");
        goto cleanup;
    }

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Get the deframer link type */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerCfg->enableJesd204C);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    /* Bank ID for Jrx is not available, assign it to zero */
    deframerCfg->bankId = 0;

    /* Get the deframer device ID */
    recoveryAction = adrv904x_JrxLink_JrxCoreDidCfg_BfGet(device,
                                                          NULL,
                                                          deframerBaseAddr,
                                                          &deframerCfg->deviceId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get device ID for the selected deframer");
        goto cleanup;
    }

    /* Get the deframer lane ID */
    for (laneIdx = 0U; laneIdx < ADI_ADRV904X_MAX_DESERIALIZER_LANES; laneIdx++)
    {
        recoveryAction = adrv904x_JrxLink_JrxCoreLidCfg_BfGet(device,
                                                              NULL,
                                                              deframerBaseAddr,
                                                              laneIdx,
                                                              &deframerCfg->laneId[laneIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane ID for the selected deframer");
            goto cleanup;
        }
    }

    /* get M - Number of converters per device */
    recoveryAction = adrv904x_JrxLink_JrxCoreMCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerCfg->jesd204M);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters per device");
        goto cleanup;
    }
    ++deframerCfg->jesd204M;

    /* get K - Number of frames in extended multiblocks */
    recoveryAction = adrv904x_JrxLink_JrxCoreKCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in extended multiblocks");
        goto cleanup;
    }

    deframerCfg->jesd204K = tmpByte + 1;

    /* get Np - Total number of bits per sample */
    recoveryAction = adrv904x_JrxLink_JrxCoreNpCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &deframerCfg->jesd204Np);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get total number of bits per sample");
        goto cleanup;
    }
    ++deframerCfg->jesd204Np;

    /* get F - Number of octets per frame per lane*/
    recoveryAction = adrv904x_JrxLink_JrxCoreFCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerCfg->jesd204F);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of octets per frame per lane");
        goto cleanup;
    }
    ++deframerCfg->jesd204F;

    /* get E - Number of multiblocks in extended multiblocks */
    /* This parameter is only valid for Jesd204C */
    deframerCfg->jesd204E = 0;
    if (deframerCfg->enableJesd204C == 1)
    {
        recoveryAction = adrv904x_JrxLink_JrxCoreECfg_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &deframerCfg->jesd204E);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of multiblocks in extended multiblocks");
            goto cleanup;
        }
    }

    /* get descrambling enabled */
    recoveryAction = adrv904x_JrxLink_JrxCoreDscrCfg_BfGet(device,
                                                           NULL,
                                                           deframerBaseAddr,
                                                           &deframerCfg->decrambling);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get descrambling enabled");
        goto cleanup;
    }

    /* Get the lane crossbar selection and physical lane crossbar */
    recoveryAction = adrv904x_DeframerLaneEnableGet(device, deframerSel, deframerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection and physical lane");
        goto cleanup;
    }

    /* get global LMFC phase adjustment */
    recoveryAction = adrv904x_JrxLink_JrxCorePhaseAdjust_BfGet(device,
                                                               NULL,
                                                               deframerBaseAddr,
                                                               &deframerCfg->lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get global LMFC phase adjustment");
        goto cleanup;
    }

    /* get sync Out Cmos Drive strength for both SyncOut0  and SyncOut1 */
    recoveryAction = adrv904x_Core_SyncOut0CmosTxDriveStrength_BfGet(device,
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                     &deframerCfg->syncbOut0CmosDriveStrength);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut0CmosDriveStrength");
        goto cleanup;
    }

    recoveryAction =adrv904x_Core_SyncOut1CmosTxDriveStrength_BfGet(device,
                                                                    NULL,
                                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                    &deframerCfg->syncbOut1CmosDriveStrength);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut1CmosDriveStrength");
        goto cleanup;
    }

    /* get sync out select */
    recoveryAction = adrv904x_JrxLink_JrxCoreSyncNSel_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &deframerCfg->syncbOutSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sync out select");
        goto cleanup;
    }

    /* Should be 0, 1, 2, or 3 */
    if (deframerCfg->syncbOutSelect > 3U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid syncbOutSelect value.");
        goto cleanup;
    }

    /* Re-encode syncbOutSelect to 0: pin-0, 1: pin-1, 2: both pins, 3: no pin selected */
    if (deframerCfg->syncbOutSelect == 0U)
    {
        deframerCfg->syncbOutSelect = 3U;
    }
    else
    {
        deframerCfg->syncbOutSelect -= 1U;
    }

    /* If any pin selected */
    if (deframerCfg->syncbOutSelect != 3U)
    {
        /* get syncbOut Lvds mode */
        recoveryAction = adrv904x_Core_SyncOutLvdsAndCmosEnable_BfGet(device,
                                                                      NULL,
                                                                      (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                      &deframerCfg->syncbOutLvdsMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut Lvds mode");
            goto cleanup;
        }

        /* get lvds pin invert */
        recoveryAction = adrv904x_Core_SyncOutLvdsPnInvert_BfGet(device,
                                                                 NULL,
                                                                 (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                 &deframerCfg->syncbOutLvdsPnInvert);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lvds pin invert");
            goto cleanup;
        }

        /* Case link-0 or both link-0 and link-1 selected */
        if ((deframerCfg->syncbOutSelect == 0U) || (deframerCfg->syncbOutSelect == 2U))
        {
            deframerCfg->syncbOutLvdsMode = deframerCfg->syncbOutLvdsMode & 0x01U;
            deframerCfg->syncbOutLvdsPnInvert = deframerCfg->syncbOutLvdsPnInvert & 0x01U;
        }
        else if (deframerCfg->syncbOutSelect == 1U) /* Case only link-1 selected */
        {
            deframerCfg->syncbOutLvdsMode = (deframerCfg->syncbOutLvdsMode >> 1U) & 0x01U;
            deframerCfg->syncbOutLvdsPnInvert = (deframerCfg->syncbOutLvdsPnInvert >> 1U) & 0x01U;
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid 'syncbOutSelect', should be 0-2");
            goto cleanup;
        }
    }

    /* get Deframer output to DAC mapping */
    recoveryAction = adi_adrv904x_DacSampleXbarGet(device,
                                                   deframerSel,
                                                   &deframerCfg->dacCrossbar);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get dacCrossbar");
        goto cleanup;
    }

    /* get sysref on relink enable */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefForRelink_BfGet(device,
                                                                   NULL,
                                                                   deframerBaseAddr,
                                                                   &deframerCfg->newSysrefOnRelink);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysref on relink enable");
        goto cleanup;
    }

    /* get sysrefForStartup */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefForStartup_BfGet(device,
                                                                    NULL,
                                                                    deframerBaseAddr,
                                                                    &deframerCfg->sysrefForStartup);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefForStartup");
        goto cleanup;
    }

    /* get sysrefNhot Enable*/
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefNShotEnable_BfGet(device,
                                                                     NULL,
                                                                     deframerBaseAddr,
                                                                     &deframerCfg->sysrefNShotEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefNhot enable");
        goto cleanup;
    }

    /* get sysrefNshotCount */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefNShotCount_BfGet(device,
                                                                    NULL,
                                                                    deframerBaseAddr,
                                                                    &deframerCfg->sysrefNShotCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefNshotCount");
        goto cleanup;
    }

    /* get sysrefIgnoreWhenLinked*/
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefIgnoreWhenLinked_BfGet(device,
                                                                          NULL,
                                                                          deframerBaseAddr,
                                                                          &deframerCfg->sysrefIgnoreWhenLinked);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefIgnoreWhenLinked");
        goto cleanup;
    }

    /* Get the Deframer IQ Rate and Lane Rate */
    switch ((int32_t) deframerSel)
    {
        case ADI_ADRV904X_DEFRAMER_0:
            deframerIdx = 0;
            break;

        case ADI_ADRV904X_DEFRAMER_1:
            deframerIdx = 1;
            break;
    }
    deframerCfg->iqRate_kHz = device->initExtract.jesdSetting.deframerSetting[deframerIdx].iqRate_kHz;
    deframerCfg->laneRate_kHz = device->initExtract.jesdSetting.deframerSetting[deframerIdx].laneRate_kHz;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerCfgGet_v2(adi_adrv904x_Device_t*    const  device,
                                                              const adi_adrv904x_FramerSel_e   framerSel,
                                                              const adi_adrv904x_RxChannels_e  chanSel,
                                                              adi_adrv904x_FramerCfg_t* const  framerCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction    = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t                      dataByte          = 0U;
    uint8_t                      enabledFramers    = 0U;
    uint8_t                      framerIdx         = 0U;
    uint8_t                      tmpByte           = 0U;
    uint8_t                      converter         = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, framerCfg, cleanup);
    ADI_LIBRARY_MEMSET(framerCfg, 0, sizeof(adi_adrv904x_FramerCfg_t));

    /* Get all framers link state */
    recoveryAction =adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                        &enabledFramers);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get framer link state failed");
        goto cleanup;
    }

    /* currently not used */
    framerCfg->overSample = 0;

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSel, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Get the framer link type */
    recoveryAction = adrv904x_JtxLink_JtxLinkType_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &framerCfg->enableJesd204C);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected framer");
        goto cleanup;
    }

    /* Get the framer band ID */
    recoveryAction = adrv904x_JtxLink_JtxBidCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->bankId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get band ID for the selected framer");
        goto cleanup;
    }

    /* Get the framer device link ID */
    recoveryAction = adrv904x_JtxLink_JtxDidCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->deviceId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get device link ID for the selected framer");
        goto cleanup;
    }

    /* Get the framer lane ID */
    recoveryAction = adrv904x_JtxLink_JtxLidCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->lane0Id);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane ID for the selected framer");
        goto cleanup;
    }

    /* Get the number of converters (M) */
    recoveryAction = adrv904x_JtxLink_JtxMCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &framerCfg->jesd204M);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters for the selected framer");
        goto cleanup;
    }
    ++framerCfg->jesd204M;

    /* Get the number of frames in a multi-frame/block (K) */
    recoveryAction = adrv904x_JtxLink_JtxKCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in a multi-frame/block for the selected framer");
        goto cleanup;
    }

    framerCfg->jesd204K = tmpByte + 1;

    /* Get the number of octets per lane (F) */
    recoveryAction = adrv904x_JtxLink_JtxFCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &framerCfg->jesd204F);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of octets per frame for the selected framer");
        goto cleanup;
    }
    ++framerCfg->jesd204F;

    /* Get the total number of bits per sample */
    recoveryAction = adrv904x_JtxLink_JtxNpCfg_BfGet(device,
                                                     NULL,
                                                     framerBaseAddr,
                                                     &framerCfg->jesd204Np);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get total number of bits per sample for the selected framer");
        goto cleanup;
    }
    ++framerCfg->jesd204Np;

    /* Get the number of multiblocks in extended multiblocks (E) */
    framerCfg->jesd204E = 0;
    if (framerCfg->enableJesd204C)
    {
        recoveryAction = adrv904x_JtxLink_JtxECfg_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &framerCfg->jesd204E);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of multiblocks in extended multiblocks for the selected framer");
            goto cleanup;
        }
    }

    /* Get the scrambler enable */
    recoveryAction = adrv904x_JtxLink_JtxScrCfg_BfGet(device,
                                                      NULL,
                                                      framerBaseAddr,
                                                      &framerCfg->scramble);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get alternative scrambler enable for the selected framer");
        goto cleanup;
    }

    /* Get the lane crossbar selection and physical lane crossbar */
    recoveryAction = adrv904x_FramerLaneEnableGet(device, framerSel, framerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection and physical lane");
        goto cleanup;
    }

    /* Get the LMFC phase adjustment */
    recoveryAction = adrv904x_JtxLink_JtxTplPhaseAdjust_BfGet(device,
                                                              NULL,
                                                              framerBaseAddr,
                                                              &framerCfg->lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the LMFC phase adjustment for the selected framer");
        goto cleanup;
    }

    /* Get the Sysref for Relink */
    recoveryAction = adrv904x_JtxLink_JtxSysrefForRelink_BfGet(device,
                                                               NULL,
                                                               framerBaseAddr,
                                                               &framerCfg->newSysrefOnRelink);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sysref Relink for the selected framer");
        goto cleanup;
    }

    /* Get the Sync-N */
    recoveryAction = adrv904x_JtxLink_JtxSyncNSel_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &framerCfg->syncbInSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sync-N for the selected framer");
        goto cleanup;
    }

    /* Get the Sync In LVDS Mode Enable */
    recoveryAction = adrv904x_Core_SyncInCmosSchmittTrigEnable_BfGet(device,
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                     &dataByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sync In LVDS Mode Enable");
        goto cleanup;
    }
    if (framerCfg->syncbInSelect == 0U)
    {
        framerCfg->syncbInLvdsMode = dataByte & 0x01;
    }
    else if (framerCfg->syncbInSelect == 1U)
    {
        framerCfg->syncbInLvdsMode = (dataByte >> 1U) & 0x01;
    }
    else if (framerCfg->syncbInSelect == 2U)
    {
        framerCfg->syncbInLvdsMode = (dataByte >> 2U) & 0x01;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error syncbInSelect is invalid");
        goto cleanup;
    }

    /* Get the Sync In LVDS Pn Invert */
    recoveryAction = adrv904x_Core_SyncInLvdsPnInvert_BfGet(device,
                                                            NULL,
                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                            &framerCfg->syncbInLvdsMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the Sync In LVDS Pn Invert");
        goto cleanup;
    }

    if (framerSel == ADI_ADRV904X_FRAMER_2)
    {
        /* Get the ORX ADC Sample Xbar for framer 2 */
        recoveryAction = adi_adrv904x_OrxAdcSampleXbarGet(device,
                                                          framerSel,
                                                          &framerCfg->orxAdcCrossbar);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sample crossbar converter.");
            goto cleanup;
        }
    }
    else
    {
        /* Get the ADC Sample Xbar */
        recoveryAction = adi_adrv904x_CddcSampleXBarGet(device,
                                                        framerSel,
                                                        chanSel,
                                                        &framerCfg->cddcCrossbar);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sample crossbar converter.");
            goto cleanup;
        }
    }


    /* Check jesd204M for number of ADCs, mark the unused ones as ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID */
    for (converter = framerCfg->jesd204M; converter < ADI_ADRV904X_MAX_CDDC_CONVERTER; converter++)
    {
        framerCfg->cddcCrossbar.cddcSampleXbar[converter] = ADI_ADRV904X_JESD_FRM_SPLXBAR_INVALID;
    }

    /* Get the sysref for start up */
    recoveryAction = adrv904x_JtxLink_JtxSysrefForStartup_BfGet(device,
                                                                NULL,
                                                                framerBaseAddr,
                                                                &framerCfg->sysrefForStartup);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref for start up");
        goto cleanup;
    }

    /* Get the sysref-N shot enable */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefNShotEnable_BfGet(device,
                                                                    NULL,
                                                                    framerBaseAddr,
                                                                    &framerCfg->sysrefNShotEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref-N shot enable");
        goto cleanup;
    }

    /* Get the sysref-N shot count */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefNShotCount_BfGet(device,
                                                                   NULL,
                                                                   framerBaseAddr,
                                                                   &framerCfg->sysrefNShotCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref-N shot count");
        goto cleanup;
    }

    /* Get the sysref ignore when linked */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefIgnoreWhenLinked_BfGet(device,
                                                                         NULL,
                                                                         framerBaseAddr,
                                                                         &framerCfg->sysrefIgnoreWhenLinked);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get the sysref ignore when linked");
        goto cleanup;
    }

    /* Get the Framer IQ Rate and Lane Rate */
    switch ((int32_t) framerSel)
    {
        case ADI_ADRV904X_FRAMER_0:
            framerIdx = 0;
            break;

        case ADI_ADRV904X_FRAMER_1:
            framerIdx = 1;
            break;

        case ADI_ADRV904X_FRAMER_2:
            framerIdx = 2;
            break;
    }
    framerCfg->iqRate_kHz = device->initExtract.jesdSetting.framerSetting[framerIdx].iqRate_kHz;
    framerCfg->laneRate_kHz = device->initExtract.jesdSetting.framerSetting[framerIdx].laneRate_kHz;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerCfgGet_v2(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_DeframerSel_e    deframerSel,
                                                                const adi_adrv904x_TxChannels_e     chanSel,
                                                                adi_adrv904x_DeframerCfg_t* const   deframerCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr                            = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t                      enabledDeframers                            = 0U;
    uint8_t                      laneIdx                                     = 0U;
    uint8_t                      deframerIdx                                 = 0U;
    uint8_t                      tmpByte                                     = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, deframerCfg, cleanup);
    ADI_LIBRARY_MEMSET(deframerCfg, 0, sizeof(adi_adrv904x_DeframerCfg_t));

    /* Get all deframers link state */
    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &enabledDeframers);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get deframer link state failed");
        goto cleanup;
    }

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Get the deframer link type */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerCfg->enableJesd204C);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    /* Bank ID for Jrx is not available, assign it to zero */
    deframerCfg->bankId = 0;

    /* Get the deframer device ID */
    recoveryAction = adrv904x_JrxLink_JrxCoreDidCfg_BfGet(device,
                                                          NULL,
                                                          deframerBaseAddr,
                                                          &deframerCfg->deviceId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get device ID for the selected deframer");
        goto cleanup;
    }

    /* Get the deframer lane ID */
    for (laneIdx = 0U; laneIdx < ADI_ADRV904X_MAX_DESERIALIZER_LANES; laneIdx++)
    {
        recoveryAction = adrv904x_JrxLink_JrxCoreLidCfg_BfGet(device,
                                                              NULL,
                                                              deframerBaseAddr,
                                                              laneIdx,
                                                              &deframerCfg->laneId[laneIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane ID for the selected deframer");
            goto cleanup;
        }
    }

    /* get M - Number of converters per device */
    recoveryAction = adrv904x_JrxLink_JrxCoreMCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerCfg->jesd204M);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of converters per device");
        goto cleanup;
    }
    ++deframerCfg->jesd204M;

    /* get K - Number of frames in extended multiblocks */
    recoveryAction = adrv904x_JrxLink_JrxCoreKCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in extended multiblocks");
        goto cleanup;
    }

    deframerCfg->jesd204K = tmpByte + 1;

    /* get Np - Total number of bits per sample */
    recoveryAction = adrv904x_JrxLink_JrxCoreNpCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &deframerCfg->jesd204Np);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get total number of bits per sample");
        goto cleanup;
    }
    ++deframerCfg->jesd204Np;

    /* get F - Number of octets per frame per lane*/
    recoveryAction = adrv904x_JrxLink_JrxCoreFCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerCfg->jesd204F);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of octets per frame per lane");
        goto cleanup;
    }
    ++deframerCfg->jesd204F;

    /* get E - Number of multiblocks in extended multiblocks */
    /* This parameter is only valid for Jesd204C */
    deframerCfg->jesd204E = 0;
    if (deframerCfg->enableJesd204C == 1)
    {
        recoveryAction = adrv904x_JrxLink_JrxCoreECfg_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &deframerCfg->jesd204E);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of multiblocks in extended multiblocks");
            goto cleanup;
        }
    }

    /* get descrambling enabled */
    recoveryAction = adrv904x_JrxLink_JrxCoreDscrCfg_BfGet(device,
                                                           NULL,
                                                           deframerBaseAddr,
                                                           &deframerCfg->decrambling);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get descrambling enabled");
        goto cleanup;
    }

    /* Get the lane crossbar selection and physical lane crossbar */
    recoveryAction = adrv904x_DeframerLaneEnableGet(device, deframerSel, deframerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane crossbar selection and physical lane");
        goto cleanup;
    }

    /* get global LMFC phase adjustment */
    recoveryAction = adrv904x_JrxLink_JrxCorePhaseAdjust_BfGet(device,
                                                               NULL,
                                                               deframerBaseAddr,
                                                               &deframerCfg->lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get global LMFC phase adjustment");
        goto cleanup;
    }

    /* get sync Out Cmos Drive strength for both SyncOut0  and SyncOut1 */
    recoveryAction = adrv904x_Core_SyncOut0CmosTxDriveStrength_BfGet(device,
                                                                     NULL,
                                                                     (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                     &deframerCfg->syncbOut0CmosDriveStrength);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut0CmosDriveStrength");
        goto cleanup;
    }

    recoveryAction =adrv904x_Core_SyncOut1CmosTxDriveStrength_BfGet(device,
                                                                    NULL,
                                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                    &deframerCfg->syncbOut1CmosDriveStrength);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut1CmosDriveStrength");
        goto cleanup;
    }

    /* get sync out select */
    recoveryAction = adrv904x_JrxLink_JrxCoreSyncNSel_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &deframerCfg->syncbOutSelect);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sync out select");
        goto cleanup;
    }

    /* Should be 0, 1, 2, or 3 */
    if (deframerCfg->syncbOutSelect > 3U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid syncbOutSelect value.");
        goto cleanup;
    }

    /* Re-encode syncbOutSelect to 0: pin-0, 1: pin-1, 2: both pins, 3: no pin selected */
    if (deframerCfg->syncbOutSelect == 0U)
    {
        deframerCfg->syncbOutSelect = 3U;
    }
    else
    {
        deframerCfg->syncbOutSelect -= 1U;
    }

    /* If any pin selected */
    if (deframerCfg->syncbOutSelect != 3U)
    {
        /* get syncbOut Lvds mode */
        recoveryAction = adrv904x_Core_SyncOutLvdsAndCmosEnable_BfGet(device,
                                                                      NULL,
                                                                      (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                      &deframerCfg->syncbOutLvdsMode);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get syncbOut Lvds mode");
            goto cleanup;
        }

        /* get lvds pin invert */
        recoveryAction = adrv904x_Core_SyncOutLvdsPnInvert_BfGet(device,
                                                                 NULL,
                                                                 (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                 &deframerCfg->syncbOutLvdsPnInvert);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lvds pin invert");
            goto cleanup;
        }

        /* Case link-0 or both link-0 and link-1 selected */
        if ((deframerCfg->syncbOutSelect == 0U) || (deframerCfg->syncbOutSelect == 2U))
        {
            deframerCfg->syncbOutLvdsMode = deframerCfg->syncbOutLvdsMode & 0x01U;
            deframerCfg->syncbOutLvdsPnInvert = deframerCfg->syncbOutLvdsPnInvert & 0x01U;
        }
        else if (deframerCfg->syncbOutSelect == 1U) /* Case only link-1 selected */
        {
            deframerCfg->syncbOutLvdsMode = (deframerCfg->syncbOutLvdsMode >> 1U) & 0x01U;
            deframerCfg->syncbOutLvdsPnInvert = (deframerCfg->syncbOutLvdsPnInvert >> 1U) & 0x01U;
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid 'syncbOutSelect', should be 0-2");
            goto cleanup;
        }
    }

    /* get Deframer output to DAC mapping */
    recoveryAction = adi_adrv904x_CducSampleXBarGet(device,
                                                   deframerSel,
                                                   chanSel,
                                                   &deframerCfg->cducCrossbar);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get dacCrossbar");
        goto cleanup;
    }

    /* get sysref on relink enable */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefForRelink_BfGet(device,
                                                                   NULL,
                                                                   deframerBaseAddr,
                                                                   &deframerCfg->newSysrefOnRelink);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysref on relink enable");
        goto cleanup;
    }

    /* get sysrefForStartup */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefForStartup_BfGet(device,
                                                                    NULL,
                                                                    deframerBaseAddr,
                                                                    &deframerCfg->sysrefForStartup);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefForStartup");
        goto cleanup;
    }

    /* get sysrefNhot Enable*/
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefNShotEnable_BfGet(device,
                                                                     NULL,
                                                                     deframerBaseAddr,
                                                                     &deframerCfg->sysrefNShotEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefNhot enable");
        goto cleanup;
    }

    /* get sysrefNshotCount */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefNShotCount_BfGet(device,
                                                                    NULL,
                                                                    deframerBaseAddr,
                                                                    &deframerCfg->sysrefNShotCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefNshotCount");
        goto cleanup;
    }

    /* get sysrefIgnoreWhenLinked*/
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefIgnoreWhenLinked_BfGet(device,
                                                                          NULL,
                                                                          deframerBaseAddr,
                                                                          &deframerCfg->sysrefIgnoreWhenLinked);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get sysrefIgnoreWhenLinked");
        goto cleanup;
    }

    /* Get the Deframer IQ Rate and Lane Rate */
    switch ((int32_t) deframerSel)
    {
        case ADI_ADRV904X_DEFRAMER_0:
            deframerIdx = 0;
            break;

        case ADI_ADRV904X_DEFRAMER_1:
            deframerIdx = 1;
            break;
    }
    deframerCfg->iqRate_kHz = device->initExtract.jesdSetting.deframerSetting[deframerIdx].iqRate_kHz;
    deframerCfg->laneRate_kHz = device->initExtract.jesdSetting.deframerSetting[deframerIdx].laneRate_kHz;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLinkStateSet(adi_adrv904x_Device_t*  device,
                                                                 const uint8_t  framerSelMask,
                                                                 uint8_t const  enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t framerLinkSet = 0U;
    uint8_t framerIdx = 0U;
    uint8_t framerSel = 0U;


    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Validate parameters */
    if (((framerSelMask & ~ADI_ADRV904X_ALL_FRAMERS) > 0U) || (framerSelMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framerSelMask.");
        goto cleanup;
    }

    if ((enable & ~(1U)) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid enable parameter.");
        goto cleanup;
    }

    /* Read the current framer link state */
    recoveryAction =   adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           &framerLinkSet);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get framer link state");
        goto cleanup;
    }

    for (framerIdx = 0; framerIdx < ADI_ADRV904X_MAX_FRAMERS; framerIdx++)
    {
        framerSel = 1U << framerIdx;
        /* Skip unselected framer */
        if ((framerSel & framerSelMask) == 0)
        {
            continue;
        }

        if (enable)
        {
            framerLinkSet |= framerSel;
        }
        else /* disable */
        {
            framerLinkSet &= ~framerSel;
        }
    }

    recoveryAction =   adrv904x_JesdCommon_JtxLinkEn_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           framerLinkSet);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set framer link state");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLinkStateGet(adi_adrv904x_Device_t*  const device,
                                                                 uint8_t* const   framerLinkState)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, framerLinkState, cleanup);

    /* Read the current framer link state */
    recoveryAction = adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         framerLinkState);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get framer link state");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLinkStateSet(adi_adrv904x_Device_t*  device,
                                                                   const uint8_t  deframerSelMask,
                                                                   uint8_t const  enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t deframerLinkSet = 0U;
    uint8_t deframerIdx = 0U;
    uint8_t deframerSel = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Validate parameters */
    if (((deframerSelMask & ~ADI_ADRV904X_ALL_DEFRAMER) > 0U) || (deframerSelMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframerSelMask.");
        goto cleanup;
    }

    if ((enable & ~(1U)) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid enable parameter.");
        goto cleanup;
    }

    /* Get all deframers link state */
    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &deframerLinkSet);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get deframer link state");
        goto cleanup;
    }

    /* Prepare for deframers enabling and disabling */
    for (deframerIdx = 0; deframerIdx < ADI_ADRV904X_MAX_DEFRAMERS; deframerIdx++)
    {
        deframerSel = 1U << deframerIdx;
        if ((deframerSel & deframerSelMask) > 0)
        {
            if (enable)
            {
                deframerLinkSet |= deframerSel;
            }
            else /* disable */
            {
                deframerLinkSet &= ~deframerSel;
            }
        }
    }

    recoveryAction =   adrv904x_JesdCommon_JrxLinkEn_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           deframerLinkSet);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set deframer link state");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLinkStateGet(adi_adrv904x_Device_t*  device,
                                                                   uint8_t* const   deframerLinkState)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, deframerLinkState, cleanup);

    /* Get all deframers link state */
    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         deframerLinkState);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get deframer link state");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsCountReset(adi_adrv904x_Device_t*  const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_JesdCommon_JrxTestLaneClearErrors_BfSet(device,
                                                                      NULL,
                                                                      ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                      1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to clear test lane JRX error");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxTestLaneClearErrors_BfSet(device,
                                                                      NULL,
                                                                      ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                      0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to perform PRBS Count Reset");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsCheckerStateSet(adi_adrv904x_Device_t*  const device,
                                                                      const adi_adrv904x_DfrmPrbsCfg_t * const dfrmPrbsCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfrmPrbsCfg, cleanup);

    /* Validate parameters */
    if (dfrmPrbsCfg->polyOrder > ADI_ADRV904X_USERDATA)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid parameter: test mode");
        goto cleanup;
    }

    /* Clear error counter for all lanes */
    recoveryAction = adi_adrv904x_DfrmPrbsCountReset(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to reset PRBS Count error");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxTestSource_BfSet(device,
                                                             NULL,
                                                             ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                             dfrmPrbsCfg->checkerLocation);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set Checker Location.");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxTestMode_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           dfrmPrbsCfg->polyOrder);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set Test Mode.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsCheckerStateGet(adi_adrv904x_Device_t*  const device,
                                                                      adi_adrv904x_DfrmPrbsCfg_t * const dfrmPrbsCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfrmPrbsCfg, cleanup);

    recoveryAction = adrv904x_JesdCommon_JrxTestSource_BfGet(device,
                                                             NULL,
                                                             ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                             &regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Checker Location.");
        goto cleanup;
    }

    dfrmPrbsCfg->checkerLocation = (adi_adrv904x_DeframerPrbsCheckLoc_e) regValue;

    recoveryAction = adrv904x_JesdCommon_JrxTestMode_BfGet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           &regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Test Mode.");
        goto cleanup;
    }

    dfrmPrbsCfg->polyOrder = (adi_adrv904x_DeframerPrbsOrder_e) regValue;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSysrefCtrlSet(adi_adrv904x_Device_t*  const device,
                                                                  const uint8_t  framerSelMask,
                                                                  uint8_t const  enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t maskSysref = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Validate parameters */
    if (((framerSelMask & ~ADI_ADRV904X_ALL_FRAMERS) > 0U) || (framerSelMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framerSelMask.");
        goto cleanup;
    }

    if ((enable & ~(1U)) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid enable parameter.");
        goto cleanup;
    }

    /* Converting enable value to sysref mask value.
     * Note that the sysref mask when high, mask any incoming SYSREF to 0
     */
    maskSysref = (enable == 0U) ? 1U : 0U;

    if (framerSelMask & ADI_ADRV904X_FRAMER_0)
    {
        recoveryAction = adrv904x_JtxLink_JtxTplSysrefMask_BfSet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_,
                                                                 maskSysref);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set SYSREF CRTL for framer 0.");
            goto cleanup;
        }
    }

    if (framerSelMask & ADI_ADRV904X_FRAMER_1)
    {
        recoveryAction = adrv904x_JtxLink_JtxTplSysrefMask_BfSet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_1_,
                                                                 maskSysref);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set SYSREF CRTL for framer 1.");
            goto cleanup;
        }
    }

    if (framerSelMask & ADI_ADRV904X_FRAMER_2)
    {
        recoveryAction = adrv904x_JtxLink_JtxTplSysrefMask_BfSet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_2_,
                                                                 maskSysref);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set SYSREF CRTL for framer 2.");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSysrefCtrlGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSel,
                                                                  uint8_t * const enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t maskSysref = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, enable, cleanup);

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSel, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Read the current framer SYSREF CTRL */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefMask_BfGet(device,
                                                             NULL,
                                                             framerBaseAddr,
                                                             &maskSysref);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get framer SYSREF CTRL");
        goto cleanup;
    }

    *enable = ((~maskSysref) & 0x01);

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSysrefCtrlSet(adi_adrv904x_Device_t*  const device,
                                                                    const uint8_t  deframerSelMask,
                                                                    uint8_t const  enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t maskSysref = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Validate parameters */
    if (((deframerSelMask & ~ADI_ADRV904X_ALL_DEFRAMER) > 0U) || (deframerSelMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframerSelMask.");
        goto cleanup;
    }

    if ((enable & ~(1U)) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid enable parameter.");
        goto cleanup;
    }

    /* Converting enable value to sysref mask value.
     * Note that the sysref mask when low, mask any incoming SYSREF to 0
     */
    maskSysref = enable;

    if (deframerSelMask & ADI_ADRV904X_DEFRAMER_0)
    {
        recoveryAction = adrv904x_JrxLink_JrxCoreSysrefEnable_BfSet(device,
                                                                    NULL,
                                                                    ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_,
                                                                    maskSysref);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set SYSREF CRTL for deframer 0.");
            goto cleanup;
        }
    }

    if (deframerSelMask & ADI_ADRV904X_DEFRAMER_1)
    {
        recoveryAction = adrv904x_JrxLink_JrxCoreSysrefEnable_BfSet(device,
                                                                    NULL,
                                                                    ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_1_,
                                                                    maskSysref);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set SYSREF CRTL for deframer 1.");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSysrefCtrlGet(adi_adrv904x_Device_t*  const device,
                                                                    const adi_adrv904x_DeframerSel_e  deframerSel,
                                                                    uint8_t * const enable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t regValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, enable, cleanup);

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* Read the current framer SYSREF CTRL */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefEnable_BfGet(device,
                                                                NULL,
                                                                deframerBaseAddr,
                                                                &regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get deframer SYSREF CTRL");
        goto cleanup;
    }

    *enable = regValue;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerTestDataSet(adi_adrv904x_Device_t*  const device,
                                                                adi_adrv904x_FrmTestDataCfg_t * const frmTestDataCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t framerLinkState;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, frmTestDataCfg, cleanup);

    /* Validate parameters */
    if (((frmTestDataCfg->framerSelMask & ~ADI_ADRV904X_ALL_FRAMERS) > 0U) ||
        (frmTestDataCfg->framerSelMask == 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framerSelMask.");
        goto cleanup;
    }

    if (frmTestDataCfg->framerSelMask & ADI_ADRV904X_FRAMER_0)
    {
        recoveryAction = adrv904x_JtxLink_JtxTestGenMode_BfSet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_,
                                                               (uint8_t) frmTestDataCfg->testDataSource);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set PRBS Test Gen Mode for framer 0.");
            goto cleanup;
        }

        recoveryAction = adrv904x_JtxLink_JtxTestGenSel_BfSet(device,
                                                              NULL,
                                                              ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_,
                                                              (uint8_t) frmTestDataCfg->injectPoint);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set PRBS Test Inject Point for framer 0.");
            goto cleanup;
        }
    }

    if (frmTestDataCfg->framerSelMask & ADI_ADRV904X_FRAMER_1)
    {
        recoveryAction = adrv904x_JtxLink_JtxTestGenMode_BfSet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_1_,
                                                               (uint8_t) frmTestDataCfg->testDataSource);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set PRBS Test Gen Mode for framer 1.");
            goto cleanup;
        }

        recoveryAction = adrv904x_JtxLink_JtxTestGenSel_BfSet(device,
                                                              NULL,
                                                              ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_1_,
                                                              (uint8_t) frmTestDataCfg->injectPoint);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set PRBS Test Inject Point for framer 1.");
            goto cleanup;
        }
    }

    if (frmTestDataCfg->framerSelMask & ADI_ADRV904X_FRAMER_2)
    {
        recoveryAction = adrv904x_JtxLink_JtxTestGenMode_BfSet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_2_,
                                                               (uint8_t) frmTestDataCfg->testDataSource);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set PRBS Test Gen Mode for framer 2.");
            goto cleanup;
        }

        recoveryAction = adrv904x_JtxLink_JtxTestGenSel_BfSet(device,
                                                              NULL,
                                                              ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_2_,
                                                              (uint8_t) frmTestDataCfg->injectPoint);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set PRBS Test Inject Point for framer 2.");
            goto cleanup;
        }
    }

    /* Toggle framer link enable changing to PRBS otherwise PRBS might get stuck */
    /* Read the current framer link state */
    recoveryAction =   adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           &framerLinkState);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get framer link state");
        goto cleanup;
    }

    recoveryAction =   adrv904x_JesdCommon_JtxLinkEn_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           0u);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set framer link state");
        goto cleanup;
    }

    recoveryAction =   adrv904x_JesdCommon_JtxLinkEn_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           framerLinkState);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set framer link state");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerTestDataGet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_FramerSel_e  framerSel,
                                                                adi_adrv904x_FrmTestDataCfg_t * const frmTestDataCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction    = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t                      tmpByte           = 0U;
  
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, frmTestDataCfg, cleanup);

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSel, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Read the current framer PRBS Gen Mode */
    recoveryAction = adrv904x_JtxLink_JtxTestGenMode_BfGet(device,
                                                           NULL,
                                                           framerBaseAddr,
                                                           &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get framer PRBS Gen Mode");
        goto cleanup;
    }

    frmTestDataCfg->testDataSource = (adi_adrv904x_FramerDataSource_e)tmpByte;

    /* Read the current framer PRBS Inject Point */
    recoveryAction = adrv904x_JtxLink_JtxTestGenSel_BfGet(device,
                                                          NULL,
                                                          framerBaseAddr,
                                                          &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get framer PRBS Inject Point");
        goto cleanup;
    }

    frmTestDataCfg->injectPoint = (adi_adrv904x_FramerDataInjectPoint_e)tmpByte;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPrbsErrCountGet(adi_adrv904x_Device_t*  const device,
                                                                  adi_adrv904x_DfrmPrbsErrCounters_t * const counters)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regData = 0U;
    uint8_t laneIdx = 0U;
    uint8_t tmpByte = 0U;

    adi_adrv904x_DfrmPrbsCfg_t dfrmPrbsCfg;

    ADI_LIBRARY_MEMSET(&dfrmPrbsCfg, 0, sizeof(adi_adrv904x_DfrmPrbsCfg_t));
  
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, counters, cleanup);

    recoveryAction = adi_adrv904x_DfrmPrbsCheckerStateGet(device, &dfrmPrbsCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get the PRBS mode");
        goto cleanup;
    }

    counters->sampleSource = dfrmPrbsCfg.checkerLocation;
    if ( (dfrmPrbsCfg.checkerLocation == ADI_ADRV904X_PRBSCHECK_SAMPLEDATA) &&  (dfrmPrbsCfg.polyOrder  != ADI_ADRV904X_USERDATA) )
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: The HW isn't configured as expected for sample data mode. Call adi_adrv904x_DfrmPrbsCheckerStateSet() with the appropriate config");
        goto cleanup;
    }

    if (counters->sampleSource == ADI_ADRV904X_PRBSCHECK_SAMPLEDATA)
    {
        /* Trigger update of the sample error counters */
        recoveryAction = adrv904x_JesdCommon_JrxTestSampleUpdateErrorCount_BfSet(device,
                                                                                 NULL,
                                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                                 1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to update test sample error count.");
            goto cleanup;
        }

        /* Now read the sample counter value. We use the zeroth index only in sample mode. */
        recoveryAction = adrv904x_JesdCommon_JrxTestSampleErrorCount_BfGet(device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                           &tmpByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get test sample error count.");
            goto cleanup;
        }

        counters->laneErrors[0] = tmpByte;

        recoveryAction = adrv904x_JesdCommon_JrxTestSampleErrorFlag_BfGet(device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                           &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get test sample error flag.");
            goto cleanup;
        }
        counters->errorStatus[0] |= ((regData & 1U) << 1);

        recoveryAction = adrv904x_JesdCommon_JrxTestSampleClearErrors_BfGet(device,
                                                                          NULL,
                                                                          ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                          &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get test sample error flag.");
            goto cleanup;
        }
        counters->errorStatus[0] |= ((regData & 1U) << 2);
    }

    if (counters->sampleSource == ADI_ADRV904X_PRBSCHECK_LANEDATA)
    {
        /* Trigger update of lane error counters */
        recoveryAction = adrv904x_JesdCommon_JrxTestLaneUpdateErrorCount_BfSet(device,
                                                                               NULL,
                                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                               1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to update test lane error count.");
            goto cleanup;
        }

        for (laneIdx = 0; laneIdx < ADI_ADRV904X_MAX_DESERIALIZER_LANES; laneIdx++)
        {
            recoveryAction = adrv904x_JesdCommon_JrxTestLaneErrorCount_BfGet(device,
                                                                             NULL,
                                                                             ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                             laneIdx,
                                                                             &counters->laneErrors[laneIdx]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get test lane error count.");
                goto cleanup;
            }

            recoveryAction = adrv904x_JesdCommon_JrxTestLaneInv_BfGet(device,
                                                                      NULL,
                                                                      ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                      laneIdx,
                                                                      &regData);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get test lane error count.");
                goto cleanup;
            }

            counters->errorStatus[laneIdx] = regData;

            recoveryAction = adrv904x_JesdCommon_JrxTestLaneInvalidDataFlag_BfGet(device,
                                                                                  NULL,
                                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                                  laneIdx,
                                                                                  &regData);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get test lane invalid data flag.");
                goto cleanup;
            }

            counters->errorStatus[laneIdx] |= ((regData & 1U) << 1U);

            recoveryAction = adrv904x_JesdCommon_JrxTestLaneErrorFlag_BfGet(device,
                                                                            NULL,
                                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                            laneIdx,
                                                                            &regData);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get test lane error flag.");
                goto cleanup;
            }

            counters->errorStatus[laneIdx] |= ((regData & 1U) << 2U);
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerializerReset(adi_adrv904x_Device_t*  const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t framerLinkType = 0U;
    uint8_t phyLanePd = 0U;
    uint8_t phyLanePdMask = 0U;
    uint8_t phyLaneIdx = 0U;
    adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e laneSerdesPhyBitfieldAddr = ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_;
    adrv904x_CpuCmd_JtxLanePower_t jtxPwrCmd;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
        
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_LIBRARY_MEMSET(&jtxPwrCmd, 0, sizeof(jtxPwrCmd));

    /* Get the framer link type: 204B or 204C - Check link0 type only */
    recoveryAction = adrv904x_JtxLink_JtxLinkType_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_,
                                                        &framerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected framer");
        goto cleanup;
    }

    /* The reset sequence differs for 204c/204b mode */
    if (framerLinkType == 0U)
    {
        /* 204B case: 
         * 1. Set the clock offset to 4 for powered-up lanes
         * 2. Then send CPU_CMD_ID_JESD_SER_RESET */
        
        /* Iterate over each jtx lane. If lane is powered-up set the clock offset to 4. */
        for(phyLaneIdx = 0U ; phyLaneIdx < ADI_ADRV904X_MAX_SERDES_LANES ; phyLaneIdx++)
        {
            recoveryAction = (adi_adrv904x_ErrAction_e) adrv904x_FramerLaneSerdesPhyBitfieldAddressGet(device,
                                                                                                    phyLaneIdx,
                                                                                                    &laneSerdesPhyBitfieldAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane serdes PHY address.");
                goto cleanup;
            }

            recoveryAction = adrv904x_GetJtxLanePoweredDown(device,
                                                            laneSerdesPhyBitfieldAddr,
                                                            &phyLanePd);
            
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Jtx lane power-up status");
                goto cleanup;
            }

            if (phyLanePd == 0U)
            {
                /* Jtx lane is powered-up; set Clockoffset to 4 */
                recoveryAction = adrv904x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet( device,
                                                                                            NULL,
                                                                                            laneSerdesPhyBitfieldAddr,
                                                                                            4U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing to serializer clock offset");
                    goto cleanup;
                }
            }
        } /* End for loop to set clock offset on powered lanes */

        adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
        adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

        adrv904x_CpuCmd_SerReset_t serReset;
        adrv904x_CpuCmd_SerResetResp_t serResetResp;
        ADI_LIBRARY_MEMSET(&serReset, 0, sizeof(adrv904x_CpuCmd_SerReset_t));
        ADI_LIBRARY_MEMSET(&serResetResp, 0, sizeof(adrv904x_CpuCmd_SerResetResp_t));

        /* Use default reset parameter */
        serReset.serResetParm = ADRV904X_HTOCL(1u);
        /* Send serializer reset command */
        recoveryAction = adrv904x_CpuCmdSend( device,
                                              ADI_ADRV904X_CPU_TYPE_0,
                                              ADRV904X_LINK_ID_0,
                                              ADRV904X_CPU_CMD_ID_JESD_SER_RESET,
                                              (void*)&serReset,
                                              sizeof(serReset),
                                              (void*)&serResetResp,
                                              sizeof(serResetResp),
                                              &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(serResetResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    } /* End 204b case */
    else
    {
        /* 204C case:
         * 1. Power down currenty powered up lanes and set their FIFO addrs and clk offsets.
         * 2. Power up ALL lanes.
         * 3. Power down the originally powered down lanes. */
        for (phyLaneIdx = 0U; phyLaneIdx < ADI_ADRV904X_MAX_SERDES_LANES; phyLaneIdx++)
        {
            recoveryAction = (adi_adrv904x_ErrAction_e) adrv904x_FramerLaneSerdesPhyBitfieldAddressGet(device,
                                                                                                    phyLaneIdx,
                                                                                                    &laneSerdesPhyBitfieldAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane serdes PHY address.");
                goto cleanup;
            }

            recoveryAction = adrv904x_GetJtxLanePoweredDown(device,
                                                            laneSerdesPhyBitfieldAddr,
                                                            &phyLanePd);
            
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Jtx lane power-up status");
                goto cleanup;
            }

            /* Keep track of powered-down lanes */
            phyLanePdMask |= phyLanePd << phyLaneIdx;

            if (phyLanePd == 0U)
            {
                /* Lane is powered up; power it down */
                /* Prepare the command payload to power-down the lane. uint8_t fields don't require HTOC conversion. */
                jtxPwrCmd.jtxLaneMask = 1U << phyLaneIdx;
                jtxPwrCmd.jtxLanePower = 0x00;
    
                /* Send command. There is no cmd-specific response expected.
                 * This command is always sent to CPU0 regardless of lane-CPU assignment. */
                recoveryAction = adrv904x_CpuCmdSend(device,
                                                     ADI_ADRV904X_CPU_TYPE_0,
                                                     ADRV904X_LINK_ID_0,
                                                     ADRV904X_CPU_CMD_ID_JESD_TX_LANE_POWER,
                                                     (void*)&jtxPwrCmd,
                                                     sizeof(jtxPwrCmd),
                                                     NULL,
                                                     0U,
                                                     &cmdStatus);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to call CPU cmd ADRV904X_CPU_CMD_ID_JESD_TX_LANE_POWER");
                    goto cleanup;
                }
                
                /* set FIFO Start Addr to 1 */
                recoveryAction = adrv904x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet( device,
                                                                                            NULL,
                                                                                            laneSerdesPhyBitfieldAddr,
                                                                                            1U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing Fifo Start Address");
                    goto cleanup;
                }

                /* set Clockoffset to 1 */
                recoveryAction = adrv904x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet( device,
                                                                                            NULL,
                                                                                            laneSerdesPhyBitfieldAddr,
                                                                                            1U);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing digital logic clock shift");
                    goto cleanup;
                }
            }
        } /* end for loop per lane to set clk offset and fifo */
        
        /* Power up all lanes */
        /* uint8_t fields don't require HTOC conversion. */        
        jtxPwrCmd.jtxLaneMask = 0xFF;
        jtxPwrCmd.jtxLanePower = 0xFF;
    
        /* Send command. There is no cmd-specific response expected.
         * This command is always sent to CPU0 regardless of lane-CPU assignment. */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_JESD_TX_LANE_POWER,
                                                (void*)&jtxPwrCmd,
                                                sizeof(jtxPwrCmd),
                                                NULL,
                                                0U,
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to call CPU cmd ADRV904X_CPU_CMD_ID_JESD_TX_LANE_POWER");
            goto cleanup;
        }
        
        /* Now put the lanes that were powered down before this fn was called back into power-down. */
        jtxPwrCmd.jtxLaneMask = phyLanePdMask;
        jtxPwrCmd.jtxLanePower = 0x00;
    
        /* Send command. There is no cmd-specific response expected.
         * This command is always sent to CPU0 regardless of lane-CPU assignment. */
        recoveryAction = adrv904x_CpuCmdSend(device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_JESD_TX_LANE_POWER,
                                                (void*)&jtxPwrCmd,
                                                sizeof(jtxPwrCmd),
                                                NULL,
                                                0U,
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to re-powerdown disabled lanes.");
            goto cleanup;
        }
        
    } /* End 204c if */

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLmfcOffsetSet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSelect,
                                                                  const uint16_t lmfcOffset)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint16_t kValue  = 0U;
    uint8_t  sValue  = 0U;
    uint8_t  tmpByte = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSelect, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* get K - Number of frames in extended multiblocks */
    recoveryAction = adrv904x_JtxLink_JtxKCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &tmpByte);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in extended multiblocks");
        goto cleanup;
    }

    kValue = tmpByte + 1;

    /* get S - Samples per converter per frame */
    recoveryAction = adrv904x_JtxLink_JtxSCfg_BfGet(device,
                                                    NULL,
                                                    framerBaseAddr,
                                                    &sValue);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get samples per frame");
        goto cleanup;
    }
    ++sValue;

    if (lmfcOffset > ((kValue * sValue) - 1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lmfcOffset, "lmfcOffet should be less than or equal to K*S-1");
        goto cleanup;
    }

    recoveryAction = adrv904x_JtxLink_JtxTplPhaseAdjust_BfSet(device,
                                                              NULL,
                                                              framerBaseAddr,
                                                              lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing phase adjust value for the selected framer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLmfcOffsetGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSelect,
                                                                  uint16_t * const lmfcOffset)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, lmfcOffset, cleanup);

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSelect, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JtxLink_JtxTplPhaseAdjust_BfGet(device,
                                                               NULL,
                                                               framerBaseAddr,
                                                               lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading phase adjust value for selected framer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmLmfcOffsetSet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_DeframerSel_e  deframerSelect,
                                                                const uint16_t lmfcOffset)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint16_t kValue  = 0U;
    uint8_t  sValue  = 0U;
    uint8_t  tmpByte = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSelect, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* get K - Number of frames in extended multiblocks */
    recoveryAction = adrv904x_JrxLink_JrxCoreKCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in extended multiblocks");
        goto cleanup;
    }

    kValue = tmpByte + 1;

    /* get S - Samples per converter per frame */
    recoveryAction = adrv904x_JrxLink_JrxCoreSCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &sValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get samples per frame");
        goto cleanup;
    }
    ++sValue;

    if (lmfcOffset > ((kValue * sValue) - 1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, lmfcOffset, "lmfcOffet should be less than or equal to K*S-1");
        goto cleanup;
    }

    recoveryAction = adrv904x_JrxLink_JrxCorePhaseAdjust_BfSet(device,
                                                               NULL,
                                                               deframerBaseAddr,
                                                               lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing phase adjust value");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmLmfcOffsetGet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_DeframerSel_e  deframerSelect,
                                                                uint16_t * const lmfcOffset)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, lmfcOffset, cleanup);

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSelect, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JrxLink_JrxCorePhaseAdjust_BfGet(device,
                                                               NULL,
                                                               deframerBaseAddr,
                                                               lmfcOffset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading phase adjust value");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmPhaseDiffGet(adi_adrv904x_Device_t*  const device,
                                                               const adi_adrv904x_DeframerSel_e  deframerSelect,
                                                               uint16_t * const phaseDiff)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr    = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint16_t kValue            = 0U;
    uint8_t  sValue            = 0U;
    uint16_t phaseDiffReadback = 0U;
    uint16_t phaseAdjReadback  = 0U;
    uint16_t kTimesS           = 0U;
    uint8_t  tmpByte           = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, phaseDiff, cleanup);

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSelect, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* get K - Number of frames in extended multiblocks */
    recoveryAction = adrv904x_JrxLink_JrxCoreKCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &tmpByte);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get number of frames in extended multiblocks");
        goto cleanup;
    }

    kValue = tmpByte + 1;

    /* get S - Samples per converter per frame */
    recoveryAction = adrv904x_JrxLink_JrxCoreSCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &sValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get samples per frame");
        goto cleanup;
    }
    ++sValue;

    /* get phase diff for selected deframer */
    recoveryAction = adrv904x_JrxLink_JrxCorePhaseDiff_BfGet(device,
                                                             NULL,
                                                             deframerBaseAddr,
                                                             &phaseDiffReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading phase diff value from device");
        goto cleanup;
    }

    recoveryAction = adrv904x_JrxLink_JrxCorePhaseAdjust_BfGet(device,
                                                               NULL,
                                                               deframerBaseAddr,
                                                               &phaseAdjReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading phase adjust value");
        goto cleanup;
    }

    kTimesS = (uint16_t)kValue * (uint16_t)sValue;

    /* Calculate the phase diff after the adjustment by using K*S circular buffer */
    *phaseDiff = (kTimesS + phaseDiffReadback - phaseAdjReadback) % kTimesS;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerStatusGet(adi_adrv904x_Device_t*  const device,
                                                              const adi_adrv904x_FramerSel_e framerSel,
                                                              adi_adrv904x_FramerStatus_t * const framerStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t framerLinkType = 0U;
    uint8_t regData = 0U;
    const uint8_t linkType204C = 0x80U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, framerStatus, cleanup);
    ADI_LIBRARY_MEMSET(framerStatus, 0, sizeof(adi_adrv904x_FramerStatus_t));

    framerStatus->status = 0U;

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSel, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    /* Get the framer link type: 204B or 204C */
    recoveryAction = adrv904x_JtxLink_JtxLinkType_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &framerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected framer");
        goto cleanup;
    }

    /* Get TPL CFG Invalid bit */
    recoveryAction = adrv904x_JtxLink_JtxTplCfgInvalid_BfGet(device,
                                                             NULL,
                                                             framerBaseAddr,
                                                             &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get TPL CFG invalid bit for the selected framer");
        goto cleanup;
    }
    framerStatus->status |= ((regData & 1U) << 0U); /* set bit 0 */

    /* Get TPL Sysref Received bit */
    if (framerLinkType != 0U) /* 204C case */
    {
        framerStatus->status |= linkType204C; /* set 204C bit */
        recoveryAction = adrv904x_JtxLink_JtxDl204cSysrefRcvd_BfGet(device,
                                                                 NULL,
                                                                 framerBaseAddr,
                                                                 &regData);
    }
    else /* 204B */
    {
        framerStatus->status &= ~linkType204C; /* clear 204C bit */

        recoveryAction = adrv904x_JtxLink_JtxTplSysrefRcvd_BfGet(device,
                                                                 NULL,
                                                                 framerBaseAddr,
                                                                 &regData);
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get TPL Sysref Received bit for the selected framer");
        goto cleanup;
    }
    framerStatus->status |= ((regData & 1U) << 1U); /* set bit 1 */

     /* Get TPL Sysref Phase Err bit */
    recoveryAction = adrv904x_JtxLink_JtxTplSysrefPhaseErr_BfGet(device,
                                                                 NULL,
                                                                 framerBaseAddr,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get TPL Sysref Phase Err bit for the selected framer");
        goto cleanup;
    }
    framerStatus->status |= ((regData & 1U) << 2U); /* set bit 2 */

    /* Get Pclk Slow Error bit */
    recoveryAction = adrv904x_JtxLink_JtxPclkSlowError_BfGet(device,
                                                             NULL,
                                                             framerBaseAddr,
                                                             &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Pclk Slow Error bit for the selected framer");
        goto cleanup;
    }
    framerStatus->status |= ((regData & 1U) << 3U); /* set bit 3 */

    /* Get Pclk Fast Error bit */
    recoveryAction = adrv904x_JtxLink_JtxPclkFastError_BfGet(device,
                                                             NULL,
                                                             framerBaseAddr,
                                                             &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Pclk Fast Error bit for the selected framer");
        goto cleanup;
    }
    framerStatus->status |= ((regData & 1U) << 4U); /* set bit 4 */

    if (framerLinkType != 0U) /* 204C case */
    {
        goto cleanup; /* Done for 204C case */
    }

    /* Get SyncN Sel */
    recoveryAction = adrv904x_JtxLink_JtxSyncNSel_BfGet(device,
                                                        NULL,
                                                        framerBaseAddr,
                                                        &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get SyncN Sel for the selected framer");
        goto cleanup;
    }

    /* Reset framer address to active framer depending to value of Sync Sel value */
    if (regData == 0)
    {
        framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    }
    else if (regData == 1)
    {
        framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_1_;
    }
    else if (regData == 2)
    {
        framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_2_;
    }
    else
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SyncN Sel is invalid for the selected framer");
        goto cleanup;
    }

    /* Get 204B SyncN */
    recoveryAction = adrv904x_JtxLink_JtxDl204bSyncN_BfGet(device,
                                                           NULL,
                                                           framerBaseAddr,
                                                           &framerStatus->syncNSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get SyncN for the selected framer");
        goto cleanup;
    }

    /* Get 204B SyncNe Count */
    recoveryAction = adrv904x_JtxLink_JtxDl204bSyncNeCount_BfGet(device,
                                                                 NULL,
                                                                 framerBaseAddr,
                                                                 &framerStatus->framerSyncNeCount);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get SyncNe Count for the selected framer");
        goto cleanup;
    }

    /* Get 204B QBF State */
    recoveryAction = adrv904x_JtxLink_JtxDl204bState_BfGet(device,
                                                                 NULL,
                                                                 framerBaseAddr,
                                                                 &framerStatus->qbfStateStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get QBF State for the selected framer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerStatusGet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_DeframerSel_e deframerSel,
                                                                adi_adrv904x_DeframerStatus_t * const deframerStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    adi_adrv904x_DeframerCfg_t deframerCfg; /* Initialized later on */
    uint8_t deframerLinkType = 0U;
    uint8_t laneId = 0U;
    uint8_t laneSel = 0U;
    uint8_t laneIdx = 0U;
    uint8_t regData = 0U;
    const uint8_t linkType204C = 0x01U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, deframerStatus, cleanup);

    ADI_LIBRARY_MEMSET(&deframerCfg, 0, sizeof(adi_adrv904x_DeframerCfg_t));

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    /* Get Deframer Lane Crossbar information */
    recoveryAction = adrv904x_DeframerLaneEnableGet(device, deframerSel, &deframerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane cross bar the selected deframer");
        goto cleanup;
    }

    for (laneId = 0U; laneId < ADI_ADRV904X_MAX_SERDES_LANES; ++laneId)
    {
        laneSel = 1U << laneId;
        if ((deframerCfg.deserializerLanesEnabled & laneSel) == laneSel)
        {
            /* Find first lane in use. */
            laneIdx = laneId;
            break;
        }
    }

    if (deframerLinkType != 0) /* 204C case */
    {
        deframerStatus->reserved |= linkType204C; /* Set 204C indication bit */

        recoveryAction = adrv904x_JesdCommon_JrxDl204cState_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  laneIdx,
                                                                  &deframerStatus->status);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get 204C status for the selected deframer");
            goto cleanup;
        }

        goto cleanup; /* Done for 204C case */
    }

    deframerStatus->reserved &= ~linkType204C; /* Clear 204C indication bit */

    /* Get 204B SyncN Sel */
    recoveryAction = adrv904x_JrxLink_JrxCoreSyncNSel_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get SyncN Sel for the selected deframer");
        goto cleanup;
    }
    deframerStatus->status |= ((regData & 1U) << 0U); /* set bit 0 */

    /* Get 204B Sysref Received */
    recoveryAction = adrv904x_JrxLink_JrxCoreSysrefRcvd_BfGet(device,
                                                              NULL,
                                                              deframerBaseAddr,
                                                              &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Sysref Received for the selected deframer");
        goto cleanup;
    }
    deframerStatus->status |= ((regData & 1U) << 1U); /* set bit 1 */

    /* Get 204B User Data */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bUserData_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get User Data for the selected deframer");
        goto cleanup;
    }
    deframerStatus->status |= ((regData & 1U) << 2U); /* set bit 2 */

    /* deframerStatus->status bit 3 is reserved */

    /* Get 204B FS Lost */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bFsLost_BfGet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                               laneIdx,
                                                               &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get FS Lost for the selected deframer");
        goto cleanup;
    }
    deframerStatus->status |= ((regData & 1U) << 4U); /* set bit 4 */

    /* Get 204B EOMF Event */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEomfEvent_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  laneIdx,
                                                                  &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get EOMF Event for the selected deframer");
        goto cleanup;
    }
    deframerStatus->status |= ((regData & 1U) << 5U); /* set bit 5 */

    /* Get 204B EOF Event */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEofEvent_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get EOF Event for the selected deframer");
        goto cleanup;
    }
    deframerStatus->status |= ((regData & 1U) << 6U); /* set bit 6 */

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerStatusGet_v2(adi_adrv904x_Device_t*  const device,
                                                                   const adi_adrv904x_DeframerSel_e deframerSel,
                                                                   adi_adrv904x_DeframerStatus_v2_t * const deframerStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    adi_adrv904x_DeframerCfg_t deframerCfg; /* Initialized later on */
    uint8_t deframerLinkType = 0U;
    uint8_t laneId = 0U;
    uint8_t phyLaneId = 0U;
    uint8_t regData = 0U;
    const uint8_t linkType204C = 0x01U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, deframerStatus, cleanup);

    ADI_LIBRARY_MEMSET(&deframerCfg, 0, sizeof(adi_adrv904x_DeframerCfg_t));
    ADI_LIBRARY_MEMSET(deframerStatus, 0, sizeof(adi_adrv904x_DeframerStatus_v2_t));

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* Get deframer link up/down status and set linkState bit 1 */
    recoveryAction = adrv904x_JrxLink_JrxCoreUsrDataRdy_BfGet(device,
                                                              NULL,
                                                              deframerBaseAddr,
                                                              &regData);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link condition for the selected deframer");
        goto cleanup;
    }

    if (regData == 1U)
    {
        deframerStatus->linkState |= 1U << 1U; /* Set linkState bit 1 */
    }
    else
    {
        deframerStatus->linkState &= ~(1U << 1U); /* Clear linkState bit 1 */
    }

    /* Get Deframer Lane Crossbar information and set phyLaneMask */
    recoveryAction = adrv904x_DeframerLaneEnableGet(device, deframerSel, &deframerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane cross bar the selected deframer");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    if (deframerLinkType != 0U)
    {
        /* 204C link type case */
        deframerStatus->linkState |= linkType204C; /* Set linkState bit 0 to indicate link type 204C */
    }
    else
    {
        /* 204B link type case */
        deframerStatus->linkState &= ~linkType204C; /* Clear linkState bit 0 to indicate link type 204B */
    }

    for (laneId = 0U; laneId < ADI_ADRV904X_MAX_SERDES_LANES; ++laneId)
    {
        phyLaneId = deframerCfg.deserializerLaneCrossbar.deframerInputLaneSel[laneId];

        /* verify that physical lane is enabled */
        if (phyLaneId < ADI_ADRV904X_MAX_SERDES_LANES)
        {
            deframerStatus->phyLaneMask |= (0x1U << phyLaneId);  /* enable phy lane in Mask */
            deframerStatus->laneStatus[phyLaneId] |= (1U << 3U); /* Set bit 3 to indicate this lane is enabled */
        }
        else
        {
            continue; /* out of range phyLaneId means not enabled */
        }

        /* 204C link type case */
        if (deframerLinkType != 0U)
        {
            recoveryAction = adrv904x_JesdCommon_JrxDl204cState_BfGet(device,
                                                                      NULL,
                                                                      ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                      phyLaneId,
                                                                      &regData);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get 204C status for the selected deframer");
                goto cleanup;
            }

            deframerStatus->laneStatus[phyLaneId] |= (regData & 0x07U); /* Get lane status bit 0, 1, and 2 */
            continue; /* Get next lane status */
        }

        /* Get 204B SyncN Sel */
        recoveryAction = adrv904x_JrxLink_JrxCoreSyncNSel_BfGet(device,
                                                                NULL,
                                                                deframerBaseAddr,
                                                                &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get SyncN Sel for the selected deframer");
            goto cleanup;
        }
        deframerStatus->laneStatus[phyLaneId] |= ((regData & 1U) << 0U); /* set bit 0 */

        /* Get 204B Sysref Received */
        recoveryAction = adrv904x_JrxLink_JrxCoreSysrefRcvd_BfGet(device,
                                                                  NULL,
                                                                  deframerBaseAddr,
                                                                  &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Sysref Received for the selected deframer");
            goto cleanup;
        }
        deframerStatus->laneStatus[phyLaneId] |= ((regData & 1U) << 1U); /* set bit 1 */

        /* Get 204B User Data */
        recoveryAction = adrv904x_JesdCommon_JrxDl204bUserData_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     phyLaneId,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get User Data for the selected deframer");
            goto cleanup;
        }
        deframerStatus->laneStatus[phyLaneId] |= ((regData & 1U) << 2U); /* set bit 2 */

        /* deframerStatus->status bit 3 is reserved */

        /* Get 204B FS Lost */
        recoveryAction = adrv904x_JesdCommon_JrxDl204bFsLost_BfGet(device,
                                                                   NULL,
                                                                   ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                   phyLaneId,
                                                                   &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get FS Lost for the selected deframer");
            goto cleanup;
        }
        deframerStatus->laneStatus[phyLaneId] |= ((regData & 1U) << 4U); /* set bit 4 */

        /* Get 204B EOMF Event */
        recoveryAction = adrv904x_JesdCommon_JrxDl204bEomfEvent_BfGet(device,
                                                                      NULL,
                                                                      ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                      phyLaneId,
                                                                      &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get EOMF Event for the selected deframer");
            goto cleanup;
        }
        deframerStatus->laneStatus[phyLaneId] |= ((regData & 1U) << 5U); /* set bit 5 */

        /* Get 204B EOF Event */
        recoveryAction = adrv904x_JesdCommon_JrxDl204bEofEvent_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     phyLaneId,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get EOF Event for the selected deframer");
            goto cleanup;
        }
        deframerStatus->laneStatus[phyLaneId] |= ((regData & 1U) << 6U); /* set bit 6 */
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCounterStatusGet(adi_adrv904x_Device_t*  const device,
                                                                      const adi_adrv904x_DeframerSel_e deframerSel,
                                                                      const uint8_t laneNumber,
                                                                      adi_adrv904x_DfrmErrCounterStatus_t * const errCounterStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t regData = 0U;
    uint8_t deframerLinkType = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, errCounterStatus, cleanup);

    if (laneNumber >= ADI_ADRV904X_MAX_SERDES_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid laneNumber for the selected deframer.");
        goto cleanup;
    }

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    if (deframerLinkType != 0U)  /* 204C */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid link type: only supports 204B link.");
        goto cleanup;
    }

    /* Get 204B BDE */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bBde_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                            laneNumber,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Bad Disparity errors for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 0U); /* set bit 0 */

    /* Get 204B CGS */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bCgs_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                            laneNumber,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Code Group Sync for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 1U); /* set bit 1 */

    /* Get 204B CKS */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bCks_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                            laneNumber,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Computed CheckSum status for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 2U); /* set bit 2 */

    /* Get 204B FS */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bFs_BfGet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                           laneNumber,
                                                           &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Frame Sync status for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 3U); /* set bit 3 */

    /* Get 204B ILD */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIld_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                            laneNumber,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Inter-Lane De-skew status for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 4U); /* set bit 4 */

    /* Get 204B ILS */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIls_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                            laneNumber,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Initial Lane Synchronization status for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 5U); /* set bit 5 */

    /* Get 204B NIT */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bNit_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                            laneNumber,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Not-In-Table errors status for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 6U); /* set bit 6 */

    /* Get 204B UEK */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bUek_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                            laneNumber,
                                                            &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Unexpected K-character errors status for the selected deframer");
        goto cleanup;
    }
    errCounterStatus->laneStatus |= ((regData & 1U) << 7U); /* set bit 7 */

    /* Get 204B BD-Cnt */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bBdCnt_BfGet(device,
                                                              NULL,
                                                              ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                              laneNumber,
                                                               &errCounterStatus->bdCntValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Bad Disparity 8-bit error counters for the selected deframer");
        goto cleanup;
    }

    /* Get 204B UEK-Cnt */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bUekCnt_BfGet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                               laneNumber,
                                                               &errCounterStatus->uekCntValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Unexpected K-character 8-bit error counters for the selected deframer");
        goto cleanup;
    }

    /* Get 204B NIT-Cnt */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bNitCnt_BfGet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                               laneNumber,
                                                               &errCounterStatus->nitCntValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get Not-In-Table 8-bit error counters for the selected deframer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCounterReset(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_DeframerSel_e deframerSel,
                                                                  const uint8_t laneNumber,
                                                                  uint32_t const errCounterMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t deframerLinkType = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (laneNumber >= ADI_ADRV904X_MAX_SERDES_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid laneNumber for the selected deframer.");
        goto cleanup;
    }

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    if (deframerLinkType != 0U)  /* 204C */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid link type: only supports 204B link.");
        goto cleanup;
    }

    if ((errCounterMask  & (uint32_t) ADI_ADRV904X_DFRM_BD_CLEAR) != 0U)
    {
        recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet(device,
                                                                    NULL,
                                                                    ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                    laneNumber,
                                                                    0x01); /* bit 0 for BD */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to clear Bad Disparity counter for the selected deframer");
            goto cleanup;
        }
    }

    if ((errCounterMask  & (uint32_t) ADI_ADRV904X_DFRM_INT_CLEAR) != 0U)
    {
        recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet(device,
                                                                    NULL,
                                                                    ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                    laneNumber,
                                                                    0x02); /* bit 1 for INT */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to clear Not-In-Table counter for the selected deframer");
            goto cleanup;
        }
    }

    if ((errCounterMask  & (uint32_t) ADI_ADRV904X_DFRM_UEK_CLEAR) != 0U)
    {
        recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet(device,
                                                                    NULL,
                                                                    ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                    laneNumber,
                                                                    0x04); /* bit 2 for INT */
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to clearUnexpected K-character counter for the selected deframer");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCounterThresholdSet(adi_adrv904x_Device_t* const device,
                                                                         const uint8_t                threshold)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_JesdCommon_JrxCrcErrCntThreshold_BfSet(device, NULL, ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON, threshold);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set Threshold of CRC errors");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Dfrm204cErrCounterStatusGet(adi_adrv904x_Device_t*  const device,
                                                                          const adi_adrv904x_DeframerSel_e deframerSel,
                                                                          const uint8_t laneNumber,
                                                                          adi_adrv904x_Dfrm204cErrCounterStatus_t * const errCounterStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t deframerLinkType = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, errCounterStatus, cleanup);

    if (laneNumber >= ADI_ADRV904X_MAX_SERDES_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid laneNumber for the selected deframer.");
        goto cleanup;
    }

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    if (deframerLinkType == 0U)  /* 204B */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid link type: only supports 204C link.");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxDl204cShErrCnt_BfGet(device,
                                                                NULL,
                                                                ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                laneNumber,
                                                                 &errCounterStatus->shCntValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get block alignment errors counter for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxDl204cEmbErrCnt_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  laneNumber,
                                                                  &errCounterStatus->embCntValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get extended multiblocks alignment errors counter for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxDl204cMbErrCnt_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneNumber,
                                                                 &errCounterStatus->mbCntValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get multiblocks alignment errors counter for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxDl204cCrcErrCnt_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  laneNumber,
                                                                  &errCounterStatus->crcCntValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CRC parity errors counter for the selected deframer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Dfrm204cErrCounterReset(adi_adrv904x_Device_t*  const device,
                                                                      const adi_adrv904x_DeframerSel_e deframerSel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    uint8_t deframerLinkType = 0U;

    uint8_t  writeGpIntPin1Bool   = 0U;
    uint8_t  writeGpIntPin0Bool   = 0U;
    uint8_t  gpintTmpByteData[2]  = { 0U };
    uint8_t  gpintPin1ByteData[2] = { 0U };
    uint8_t  gpintPin0ByteData[2] = { 0U };

    static const uint8_t GPINT_DFRM0_204C_LL_CRC_BYTE1_MASK = (1U << 2U);
    static const uint8_t GPINT_DFRM1_204C_LL_CRC_BYTE2_MASK = (1U << 0U);

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);


    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected deframer");
        goto cleanup;
    }

    if (deframerLinkType == 0U)  /* 204B */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid link type: only supports 204C link.");
        goto cleanup;
    }

    /* Read GPINT registers */
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN1_BYTE1, gpintPin1ByteData, NULL, 2U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RegistersByteRead GPINT_PIN1 issue");
        goto cleanup;
    }
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN0_BYTE1, gpintPin0ByteData, NULL, 2U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RegistersByteRead GPINT_PIN0 issue");
        goto cleanup;
    }

    /* Disable GPINT while clearing Error Counter */
    gpintTmpByteData[0] = gpintPin1ByteData[0] | GPINT_DFRM0_204C_LL_CRC_BYTE1_MASK;
    gpintTmpByteData[1] = gpintPin1ByteData[1] | GPINT_DFRM1_204C_LL_CRC_BYTE2_MASK;
    writeGpIntPin1Bool  = (gpintTmpByteData[0] != gpintPin1ByteData[0]) || (gpintTmpByteData[1] != gpintPin1ByteData[1]);
    if (writeGpIntPin1Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN1_BYTE1, gpintTmpByteData, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RegistersByteWrite Disable GPINT_PIN1 issue");
            goto cleanup;
        }
    }

    gpintTmpByteData[0] = gpintPin0ByteData[0] | GPINT_DFRM0_204C_LL_CRC_BYTE1_MASK;
    gpintTmpByteData[1] = gpintPin0ByteData[1] | GPINT_DFRM1_204C_LL_CRC_BYTE2_MASK;
    writeGpIntPin0Bool  = (gpintTmpByteData[0] != gpintPin0ByteData[0]) || (gpintTmpByteData[1] != gpintPin0ByteData[1]);
    if (writeGpIntPin0Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN0_BYTE1, gpintTmpByteData, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RegistersByteWrite Disable GPINT_PIN0 issue");
            goto cleanup;
        }
    }

    recoveryAction = adrv904x_JesdCommon_JrxDl204cClrErrCnt_BfSet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to clear error counters for the selected deframer");
        goto cleanup;
    }

    /* 204C Clear Err Counter bit is not self clearing bit so it must be set back to 0 */
    recoveryAction = adrv904x_JesdCommon_JrxDl204cClrErrCnt_BfSet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to clear error bit for the selected deframer");
        goto cleanup;
    }

    /* Clear GPINT status bits */
    gpintTmpByteData[0] = GPINT_DFRM0_204C_LL_CRC_BYTE1_MASK;
    gpintTmpByteData[1] = GPINT_DFRM1_204C_LL_CRC_BYTE2_MASK;
    recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_STATUS_BYTE1, gpintTmpByteData, 2U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error clearing GP Interrupt Status bits.");
        return recoveryAction;
    }

    /* Restore GPINT config */
    if (writeGpIntPin1Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN1_BYTE1, gpintPin1ByteData, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RegistersByteWrite Restore GPINT_PIN1 issue");
            goto cleanup;
        }
    }

    if (writeGpIntPin0Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN0_BYTE1, gpintPin0ByteData, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_RegistersByteWrite Restore GPINT_PIN0 issue");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmLinkConditionGet(adi_adrv904x_Device_t*  const device,
                                                                   const adi_adrv904x_DeframerSel_e deframerSel,
                                                                   uint8_t * const dfrmLinkCondition)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfrmLinkCondition, cleanup);

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JrxLink_JrxCoreUsrDataRdy_BfGet(device,
                                                              NULL,
                                                              deframerBaseAddr,
                                                              dfrmLinkCondition);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link condition for the selected deframer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmFifoDepthGet(adi_adrv904x_Device_t*  const device,
                                                               const adi_adrv904x_DeframerSel_e deframerSel,
                                                               const uint8_t laneNumber,
                                                               uint8_t * const fifoDepth)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, fifoDepth, cleanup);

    if (laneNumber >= ADI_ADRV904X_MAX_SERDES_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid laneNumber for the selected deframer.");
        goto cleanup;
    }

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JrxLink_JrxTplBufDepth_BfGet(device,
                                                           NULL,
                                                           deframerBaseAddr,
                                                           laneNumber,
                                                           fifoDepth);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get elastic FIFO depth for the selected deframer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmCoreBufDepthGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_DeframerSel_e deframerSel,
                                                                  uint8_t * const coreBufDepth)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, coreBufDepth, cleanup);

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JrxLink_JrxCoreBufDepth_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            coreBufDepth);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get core buffer depth for the selected deframer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLoopbackSet(adi_adrv904x_Device_t*  const device,
                                                                const adi_adrv904x_FramerSel_e framerSel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_AdcSampleXbarCfg_t  adcCrossbar; /* Initialized below */
    uint8_t                      framerEnabled    = 0;
    uint8_t                      deframerEnabled  = 0;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_LIBRARY_MEMSET(&adcCrossbar, 0, sizeof(adi_adrv904x_AdcSampleXbarCfg_t));

    /* Verify only one framer is selected */
    if ((framerSel == 0) || /* one framer must be selected */
        (framerSel & (framerSel - 1)) || /* ensure no more than one framer is selected */
        ((framerSel & ~ADI_ADRV904X_ALL_FRAMERS) != 0)) /* ensure non-framer bits are not set */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Get all framers link state */
    recoveryAction = adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &framerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get framer link state failed");
        goto cleanup;
    }

    /* Verify the select framer is enabled */
    if ((framerEnabled & framerSel) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: The selected framer must be enabled.");
        goto cleanup;
    }

    /* Verify at least one deframer is enabled */
    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &deframerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get deframer link state failed");
        goto cleanup;
    }

    /* Verify the select deframer is enabled */
    if ((deframerEnabled & (uint8_t) ADI_ADRV904X_ALL_DEFRAMER) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: at least one deframer must be enabled.");
        goto cleanup;
    }

    /* Configuring ADC Crossbar for loopback */
    adcCrossbar.AdcSampleXbar[ 0] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX0_DATA_I;
    adcCrossbar.AdcSampleXbar[ 1] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX0_DATA_Q;
    adcCrossbar.AdcSampleXbar[ 2] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX1_DATA_I;
    adcCrossbar.AdcSampleXbar[ 3] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX1_DATA_Q;
    adcCrossbar.AdcSampleXbar[ 4] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX2_DATA_I;
    adcCrossbar.AdcSampleXbar[ 5] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX2_DATA_Q;
    adcCrossbar.AdcSampleXbar[ 6] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX3_DATA_I;
    adcCrossbar.AdcSampleXbar[ 7] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX3_DATA_Q;
    adcCrossbar.AdcSampleXbar[ 8] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX4_DATA_I;
    adcCrossbar.AdcSampleXbar[ 9] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX4_DATA_Q;
    adcCrossbar.AdcSampleXbar[10] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX5_DATA_I;
    adcCrossbar.AdcSampleXbar[11] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX5_DATA_Q;
    adcCrossbar.AdcSampleXbar[12] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX6_DATA_I;
    adcCrossbar.AdcSampleXbar[13] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX6_DATA_Q;
    adcCrossbar.AdcSampleXbar[14] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX7_DATA_I;
    adcCrossbar.AdcSampleXbar[15] = (adi_adrv904x_AdcSampleXbarSel_e) ADI_ADRV904X_JESD_FRM_LB_XBAR_TX7_DATA_Q;

    recoveryAction = adi_adrv904x_AdcSampleXbarSet(device, framerSel, &adcCrossbar);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set ADC Xbar loopback for the selected framer.");
        goto cleanup;
    }

    /* Enable framer loopback */
    recoveryAction = adrv904x_JesdCommon_JtxSampleLoopback_BfSet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 framerSel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to enable loopback for the selected framer");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerLoopbackDisable(adi_adrv904x_Device_t* const         device,
                                                                    const adi_adrv904x_FramerSel_e        framerSel,
                                                                    const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar)
{
        adi_adrv904x_ErrAction_e recoveryAction        = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                  framerEnabled         = 0U;
    uint8_t                  framerLoopbackEnabled = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, adcXbar, cleanup);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Verify only one framer is selected */
    if ((framerSel == 0U) || /* one framer must be selected */
        (framerSel & (framerSel - 1)) || /* ensure no more than one framer is selected */
        ((framerSel & ~ADI_ADRV904X_ALL_FRAMERS) != 0U)) /* ensure non-framer bits are not set */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, framerSel, "Invalid framer selection");
        goto cleanup;
    }

    /* Get all framers link state */
    recoveryAction = adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                        &framerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get framer link state failed");
        goto cleanup;
    }

    /* Verify the select framer is enabled */
    if ((framerEnabled & framerSel) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, framerSel, "Error: The selected framer must be enabled.");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JtxSampleLoopback_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 &framerLoopbackEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to read which framers have enabled loopbacks");
        goto cleanup;
    }

    /* Verify the select framer has its loopback enabled */
    if ((framerLoopbackEnabled & framerSel) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, framerSel, "Error: The selected framer loopback has not been enabled.");
        goto cleanup;
    }

    /* Disable framer loopback for that particular framer*/
    recoveryAction = adrv904x_JesdCommon_JtxSampleLoopback_BfSet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 (framerLoopbackEnabled & ~framerSel));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to disable loopback for the selected framer");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_AdcSampleXbarSet(device, framerSel, adcXbar);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set ADC Xbar loopback for the selected framer.");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLoopbackSet(adi_adrv904x_Device_t*  const device)
{
        adi_adrv904x_ErrAction_e recoveryAction   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                  framerEnabled    = 0U;
    uint8_t                  deframerEnabled  = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Get all framers link state */
    recoveryAction = adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &framerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get framer link state failed");
        goto cleanup;
    }

    /* Verify at least one framer is enabled */
    if ((framerEnabled & (uint8_t) ADI_ADRV904X_ALL_FRAMERS) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: At least one framer must be enabled.");
        goto cleanup;
    }

    /* Verify at least one deframer is enabled */
    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &deframerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get deframer link state failed");
        goto cleanup;
    }

    if ((deframerEnabled & (uint8_t) ADI_ADRV904X_ALL_DEFRAMER) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: deframer-0 must be enabled.");
        goto cleanup;
    }

    /* If framer 0 not enabled, enable it to use it as loopback */
    if ((framerEnabled & (uint8_t) ADI_ADRV904X_FRAMER_0) == 0U)
    {
        framerEnabled = framerEnabled | ADI_ADRV904X_FRAMER_0;
        recoveryAction = adrv904x_JesdCommon_JtxLinkEn_BfSet(device,
                                                             NULL,
                                                             ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                             framerEnabled);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get framer link state failed");
            goto cleanup;
        }
    }

    /* Enable deframer loopback */
    recoveryAction = adrv904x_JesdCommon_JrxSampleLoopback_BfSet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 ADI_ENABLE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to enable deframer loopback");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLoopbackDisable(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction          = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Disable deframer loopback */
    recoveryAction = adrv904x_JesdCommon_JrxSampleLoopback_BfSet(device,
                                                                NULL,
                                                                ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                ADI_DISABLE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to disable deframer loopback");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLoopbackDisable_v2(adi_adrv904x_Device_t*                 const device,
                                                                         const adi_adrv904x_FramerSel_e               framerSel,
                                                                         const adi_adrv904x_AdcSampleXbarCfg_t* const adcXbar)
{
        adi_adrv904x_ErrAction_e recoveryAction          = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                  framerEnabled         = 0U;
    uint8_t                  deframerLoopbackEnabled = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, adcXbar, cleanup);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Verify only one deframer is selected */
    if ((framerSel == 0U) || /* one deframer must be selected */
        (framerSel & (framerSel - 1U)) || /* ensure no more than one deframer is selected */
        ((framerSel & ~ADI_ADRV904X_ALL_DEFRAMER) != 0U)) /* ensure non-deframer bits are not set */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, framerSel, "Invalid deframer selection");
        goto cleanup;
    }

    /* Get all framers link state */
    recoveryAction = adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &framerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get deframer link state failed");
        goto cleanup;
    }

    /* Verify the select deframer is enabled */
    if ((framerEnabled & framerSel) == 0U) //TODO: does this framer NEED to be framer 0??
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, framerSel, "Error: The selected framer must be enabled.");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxSampleLoopback_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 &deframerLoopbackEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to read which deframers have enabled loopbacks");
        goto cleanup;
    }

    /* Verify the deframer has its loopback enabled */
    if (deframerLoopbackEnabled == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, deframerLoopbackEnabled, "Error: The deframer loopback has not been enabled.");
        goto cleanup;
    }

    /* Disable deframer loopback */
    recoveryAction = adrv904x_JesdCommon_JrxSampleLoopback_BfSet(device,
                                                                NULL,
                                                                ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                ADI_DISABLE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to disable deframer loopback");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_AdcSampleXbarSet(device, framerSel, adcXbar);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set ADC Xbar loopback for the selected framer.");
        goto cleanup;
    }


cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLaneLoopbackSet(adi_adrv904x_Device_t*  const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                      framerEnabled    = 0;
    uint8_t                      deframerEnabled  = 0;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Get all framers link state */
    recoveryAction = adrv904x_JesdCommon_JtxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &framerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get framer link state failed");
        goto cleanup;
    }

    /* Verify at least one framer is enabled */
    if ((framerEnabled & (uint8_t) ADI_ADRV904X_ALL_FRAMERS) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: at least one framer must be enabled.");
        goto cleanup;
    }

    /* Verify at least one deframer is enabled */
    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &deframerEnabled);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get deframer link state failed");
        goto cleanup;
    }

    if ((deframerEnabled & (uint8_t) ADI_ADRV904X_ALL_DEFRAMER) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: at least one deframer must be enabled.");
        goto cleanup;
    }

    /* Enable deframer lane loopback */
    recoveryAction = adrv904x_JesdCommon_JrxLaneLoopback_BfSet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                               ADI_ENABLE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to enable deframer lane loopback");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerLaneLoopbackDisable(adi_adrv904x_Device_t*  const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Disable deframer lane loopback */
    recoveryAction = adrv904x_JesdCommon_JrxLaneLoopback_BfSet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                               ADI_DISABLE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to disable deframer lane loopback");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbModeSet(adi_adrv904x_Device_t* const  device,
                                                                 const uint8_t                 framerSelMask,
                                                                 const uint8_t                 syncbMode)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    adi_adrv904x_FramerSel_e     framerSel      = ADI_ADRV904X_FRAMER_0;
    uint32_t framerSelection = 0U;

    uint32_t i = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check framer id */
    if ((framerSelMask < (uint8_t)ADI_ADRV904X_FRAMER_0) ||
        (framerSelMask > (uint8_t)ADI_ADRV904X_ALL_FRAMERS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Check syncb mode */
    if ((syncbMode != 1U) &&
        (syncbMode != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer syncb mode value");
        goto cleanup;
    }

    /* Framer syncb mode set */
    for (i = 0U; i < ADI_ADRV904X_MAX_FRAMERS; i++)
    {
        framerSelection = 1U << i;
        framerSel = (adi_adrv904x_FramerSel_e)(framerSelection);

        if ((framerSelMask & (uint8_t)framerSelection) != 0U)
        {
            /* Get framer base address */
            recoveryAction = adrv904x_FramerBitfieldAddressGet(device,
                                                               framerSel,
                                                               &framerBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for framer");
                goto cleanup;
            }

            /* Set syncb mode */
            recoveryAction = adrv904x_JtxLink_JtxDl204bSyncNForceEn_BfSet(device,
                                                                          NULL,
                                                                          framerBaseAddr,
                                                                          syncbMode);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting syncb mode for framer");
                goto cleanup;
            }
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbModeGet(adi_adrv904x_Device_t* const  device,
                                                                 const uint8_t                 framerSelMask,
                                                                 uint8_t* const                syncbMode)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, syncbMode, cleanup);

    /* Get framer base address */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device,
                                                       (adi_adrv904x_FramerSel_e)framerSelMask,
                                                       &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for framer");
        goto cleanup;
    }

    /* Framer syncb mode get */
    recoveryAction = adrv904x_JtxLink_JtxDl204bSyncNForceEn_BfGet(device,
                                                                  NULL,
                                                                  framerBaseAddr,
                                                                  syncbMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting syncb mode for framer");
        goto cleanup;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbStatusSet(adi_adrv904x_Device_t* const  device,
                                                                   const uint8_t                 framerSelMask,
                                                                   const uint8_t                 syncbStatus)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    adi_adrv904x_FramerSel_e     framerSel      = ADI_ADRV904X_FRAMER_0;
    uint32_t framerSelection = 0U;
    uint32_t i = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check framer id */
    if ((framerSelMask < (uint8_t)ADI_ADRV904X_FRAMER_0) ||
        (framerSelMask > (uint8_t)ADI_ADRV904X_ALL_FRAMERS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Check syncb force enable value */
    if ((syncbStatus != 1U) &&
        (syncbStatus != 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer syncb status value");
        goto cleanup;
    }

    /* Framer syncb status set */
    for (i = 0U; i < ADI_ADRV904X_MAX_FRAMERS; i++)
    {
        framerSelection = 1U << i;
        framerSel = (adi_adrv904x_FramerSel_e)(framerSelection);

        if ((framerSelMask & (uint8_t)framerSelection) != 0U)
        {
            /* Get framer base address */
            recoveryAction = adrv904x_FramerBitfieldAddressGet(device,
                                                               framerSel,
                                                               &framerBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for framer");
                goto cleanup;
            }

            /* Set syncb status enable */
            recoveryAction = adrv904x_JtxLink_JtxDl204bSyncNForceVal_BfSet(device,
                                                                           NULL,
                                                                           framerBaseAddr,
                                                                           syncbStatus);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting syncb status for framer");
                goto cleanup;
            }
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbStatusGet(adi_adrv904x_Device_t* const  device,
                                                                   const uint8_t                 framerSelMask,
                                                                   uint8_t* const                syncbStatus)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, syncbStatus, cleanup);

    /* Get framer base address */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device,
                                                       (adi_adrv904x_FramerSel_e)framerSelMask,
                                                       &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for framer");
        goto cleanup;
    }

    /* Framer syncb status get */
    recoveryAction = adrv904x_JtxLink_JtxDl204bSyncN_BfGet(device,
                                                           NULL,
                                                           framerBaseAddr,
                                                           syncbStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting syncb status for framer");
        goto cleanup;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbErrCntGet(adi_adrv904x_Device_t* const  device,
                                                                   const uint8_t                 framerSelMask,
                                                                   uint8_t* const                syncbErrCnt)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, syncbErrCnt, cleanup);

    /* Get framer base address */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device,
                                                       (adi_adrv904x_FramerSel_e)framerSelMask,
                                                       &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for framer");
        goto cleanup;
    }

    /* Framer syncb error counter get */
    recoveryAction = adrv904x_JtxLink_JtxDl204bSyncNeCount_BfGet(device,
                                                                 NULL,
                                                                 framerBaseAddr,
                                                                 syncbErrCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting syncb error counter for framer");
        goto cleanup;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerSyncbErrCntReset(adi_adrv904x_Device_t* const  device,
                                                                     const uint8_t                 framerSelMask)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    adi_adrv904x_FramerSel_e     framerSel      = ADI_ADRV904X_FRAMER_0;
    uint32_t framerSelection = 0U;
    uint32_t      i                = 0U;
    const uint8_t syncbErrCntClear = 1U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check framer id */
    if ((framerSelMask < (uint8_t)ADI_ADRV904X_FRAMER_0) ||
        (framerSelMask > (uint8_t)ADI_ADRV904X_ALL_FRAMERS))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Framer syncb error counter clear */
    for (i = 0U; i < ADI_ADRV904X_MAX_FRAMERS; i++)
    {
        framerSelection = 1U << i;
        framerSel = (adi_adrv904x_FramerSel_e)(framerSelection);

        if ((framerSelMask & (uint8_t)framerSelection) != 0U)
        {
            /* Get framer base address */
            recoveryAction = adrv904x_FramerBitfieldAddressGet(device,
                                                               framerSel,
                                                               &framerBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for framer");
                goto cleanup;
            }

            /* Clear syncb error counter */
            recoveryAction = adrv904x_JtxLink_JtxDl204bClearSyncNeCount_BfSet(device,
                                                                              NULL,
                                                                              framerBaseAddr,
                                                                              syncbErrCntClear);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing syncb error counter for framer");
                goto cleanup;
            }
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSyncbErrCntGet(adi_adrv904x_Device_t* const  device,
                                                                     const uint8_t                 deframerSelMask,
                                                                     uint8_t* const                syncbErrCnt)
{
        adi_adrv904x_ErrAction_e     recoveryAction   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, syncbErrCnt, cleanup);

    /* Get deframer base address */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device,
                                                         (adi_adrv904x_DeframerSel_e)deframerSelMask,
                                                         &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for deframer");
        goto cleanup;
    }

    /* Deframer syncb error counter get */
    recoveryAction = adrv904x_JrxLink_JrxCoreSyncNeCount_BfGet(device,
                                                               NULL,
                                                               deframerBaseAddr,
                                                               syncbErrCnt);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting syncb error counter for deframer");
        goto cleanup;
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerSyncbErrCntReset(adi_adrv904x_Device_t* const  device,
                                                                       const uint8_t                 deframerSelMask)
{
        adi_adrv904x_ErrAction_e     recoveryAction   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    adi_adrv904x_DeframerSel_e   deframerSel      = ADI_ADRV904X_DEFRAMER_0;
    uint32_t deframerSelection = 0U;
    uint32_t      i                = 0U;
    const uint8_t syncbErrCntClear = 1U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check deframer id */
    if ((deframerSelMask < (uint8_t)ADI_ADRV904X_DEFRAMER_0) ||
        (deframerSelMask > (uint8_t)ADI_ADRV904X_ALL_DEFRAMER))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframer selection");
        goto cleanup;
    }

    /* Deframer syncb error counter clear */
    for (i = 0U; i < ADI_ADRV904X_MAX_DEFRAMERS; i++)
    {
        deframerSelection = 1U << i;
        deframerSel = (adi_adrv904x_DeframerSel_e)(deframerSelection);

        if ((deframerSelMask & (uint8_t)deframerSelection) != 0U)
        {
            /* Get deframer base address */
            recoveryAction = adrv904x_DeframerBitfieldAddressGet(device,
                                                                 deframerSel,
                                                                 &deframerBaseAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting base address for deframer");
                goto cleanup;
            }

            /* Clear syncb error counter */
            recoveryAction = adrv904x_JrxLink_JrxCoreClearSyncNeCount_BfSet(device,
                                                                            NULL,
                                                                            deframerBaseAddr,
                                                                            syncbErrCntClear);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while clearing syncb error counter for deframer");
                goto cleanup;
            }
            
            /* Reset syncb error counter after clear */
            recoveryAction = adrv904x_JrxLink_JrxCoreClearSyncNeCount_BfSet(device,
                                                                            NULL,
                                                                            deframerBaseAddr,
                                                                            0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while resetting syncb error counter for deframer");
                goto cleanup;
            }
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerErrorCtrl(adi_adrv904x_Device_t* const          device,
                                                              const adi_adrv904x_FramerSel_e        framerSel,
                                                              const adi_adrv904x_SerdesErrAction_e  action)
{
        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;

    uint32_t framerIdx  = 0U;
    uint8_t  errClr = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (((uint32_t)action & ADI_ADRV904X_SERDES_ALL_ERR_CLEAR) == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, action, "Invalid Serdes Error Action Selected");
        goto cleanup;
    }

    for (framerIdx = 0U; framerIdx < (uint32_t)ADI_ADRV904X_MAX_FRAMERS; framerIdx++)
    {
        if ((((1U << framerIdx) & (uint32_t)framerSel) == 0U) ||
            ((device->initExtract.jesdSetting.framerSetting[framerIdx].serialLaneEnabled == 0U) && (framerSel == ADI_ADRV904X_ALL_FRAMERS)))
        {
            /* skip if framer is not selected or if it is an unused framer and all framer is selected */
            continue;
        }

        /* Get the base address of the selected framer */
        recoveryAction = adrv904x_FramerBitfieldAddressGet(device, (adi_adrv904x_FramerSel_e)(1U << framerIdx), &framerBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
            goto cleanup;
        }

        /* Handle PCLK Errors */
        if ((action & ADI_ADRV904X_SERDES_PCLK_ERR_CLEAR) == ADI_ADRV904X_SERDES_PCLK_ERR_CLEAR)
        {
            /* Read PCLK error clear bitfield */
            recoveryAction = adrv904x_JtxLink_JtxPclkErrorClear_BfGet(device, NULL, framerBaseAddr, &errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, framerIdx, "Failed to Read Framer PCLK Error Clear status");
                goto cleanup;
            }
        }
        else
        {
            errClr = 0U;
        }

        if ((action & ADI_ADRV904X_SERDES_PCLK_ERR_DISABLE) == ADI_ADRV904X_SERDES_PCLK_ERR_DISABLE)
        {
            /* Set the bitfield to 1 disables the Error and clears it */
            recoveryAction = adrv904x_JtxLink_JtxPclkErrorClear_BfSet(device, NULL, framerBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, framerIdx, "Failed to Clear/Disable Framer PCLK Error");
                goto cleanup;
            }
        }

        if ((action & ADI_ADRV904X_SERDES_PCLK_ERR_ENABLE) == ADI_ADRV904X_SERDES_PCLK_ERR_ENABLE)
        {
            /* Set the bitfield to 0 enables the Error */
            recoveryAction = adrv904x_JtxLink_JtxPclkErrorClear_BfSet(device, NULL, framerBaseAddr, errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, framerIdx, "Failed to Enable Framer PCLK Error");
                goto cleanup;
            }
        }

        /* Handle SYSREF Phase Error */
        if ((action & ADI_ADRV904X_SERDES_SYSREF_ERR_CLEAR) == ADI_ADRV904X_SERDES_SYSREF_ERR_CLEAR)
        {
            /* Read Sysref phase error clear bitfield */
            recoveryAction = adrv904x_JtxLink_JtxTplSysrefClrPhaseErr_BfGet(device, NULL, framerBaseAddr, &errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, framerIdx, "Failed to Read Framer Sysref Phase Error Clear status");
                goto cleanup;
            }
        }
        else
        {
            errClr = 0U;
        }

        if ((action & ADI_ADRV904X_SERDES_SYSREF_ERR_DISABLE) == ADI_ADRV904X_SERDES_SYSREF_ERR_DISABLE)
        {
            /* Set the bitfield to 1 disables the Error and clears it */
            recoveryAction = adrv904x_JtxLink_JtxTplSysrefClrPhaseErr_BfSet(device, NULL, framerBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, framerIdx, "Failed to Clear/Disable Framer Sysref Phase Error");
                goto cleanup;
            }
        }

        if ((action & ADI_ADRV904X_SERDES_SYSREF_ERR_ENABLE) == ADI_ADRV904X_SERDES_SYSREF_ERR_ENABLE)
        {
            /* Set the bitfield to 0 enables the Error */
            recoveryAction = adrv904x_JtxLink_JtxTplSysrefClrPhaseErr_BfSet(device, NULL, framerBaseAddr, errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, framerIdx, "Failed to Enable Framer Sysref Phase Error");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeframerErrorCtrl(adi_adrv904x_Device_t* const          device,
                                                                const adi_adrv904x_DeframerSel_e      deframerSel,
                                                                const adi_adrv904x_SerdesErrAction_e  action)
{
        adi_adrv904x_ErrAction_e recoveryAction       = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;

    uint32_t deframerIdx = 0U;
    uint8_t  errClr = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (((uint32_t)action & ADI_ADRV904X_SERDES_ALL_ERR_CLEAR) == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, action, "Invalid Serdes Error Action Selected");
        goto cleanup;
    }

    for (deframerIdx = 0U; deframerIdx < (uint32_t)ADI_ADRV904X_MAX_DEFRAMERS; deframerIdx++)
    {
        if ((((1U << deframerIdx) & (uint32_t)deframerSel) == 0U) ||
            ((device->initExtract.jesdSetting.deframerSetting[deframerIdx].deserialLaneEnabled == 0U) && (deframerSel == ADI_ADRV904X_ALL_DEFRAMER)))
        {
            /* skip if deframer is not selected or if it is an unused deframer and all deframer is selected */
            continue;
        }

        /* Get the base address of the selected deframer */
        recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, (adi_adrv904x_DeframerSel_e)(1U << deframerIdx), &deframerBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
            goto cleanup;
        }

        /* Handle PCLK Errors */
        if ((action & ADI_ADRV904X_SERDES_PCLK_ERR_CLEAR) == ADI_ADRV904X_SERDES_PCLK_ERR_CLEAR)
        {
            /* Read PCLK error clear bitfield */
            recoveryAction = adrv904x_JrxLink_JrxCorePclkErrorClear_BfGet(device, NULL, deframerBaseAddr, &errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, deframerIdx, "Failed to Read Deframer PCLK Error Clear status");
                goto cleanup;
            }
        }
        else
        {
            errClr = 0U;
        }

        if ((action & ADI_ADRV904X_SERDES_PCLK_ERR_DISABLE) == ADI_ADRV904X_SERDES_PCLK_ERR_DISABLE)
        {
            /* Set the bitfield to 1 disables the Error and clears it */
            recoveryAction = adrv904x_JrxLink_JrxCorePclkErrorClear_BfSet(device, NULL, deframerBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, deframerIdx, "Failed to Clear/Disable Deframer PCLK Error");
                goto cleanup;
            }
        }

        if ((action & ADI_ADRV904X_SERDES_PCLK_ERR_ENABLE) == ADI_ADRV904X_SERDES_PCLK_ERR_ENABLE)
        {
            /* Set the bitfield to 0 enables the Error */
            recoveryAction = adrv904x_JrxLink_JrxCorePclkErrorClear_BfSet(device, NULL, deframerBaseAddr, errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, deframerIdx, "Failed to Enable Deframer PCLK Error");
                goto cleanup;
            }
        }

        /* Handle SYSREF Phase Error */
        if ((action & ADI_ADRV904X_SERDES_SYSREF_ERR_CLEAR) == ADI_ADRV904X_SERDES_SYSREF_ERR_CLEAR)
        {
            /* Read Sysref phase error clear bitfield */
            recoveryAction = adrv904x_JrxLink_JrxCoreSysrefClrPhaseErr_BfGet(device, NULL, deframerBaseAddr, &errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, deframerIdx, "Failed to Read Deframer Sysref Phase Error Clear status");
                goto cleanup;
            }
        }
        else
        {
            errClr = 0U;
        }

        if ((action & ADI_ADRV904X_SERDES_SYSREF_ERR_DISABLE) == ADI_ADRV904X_SERDES_SYSREF_ERR_DISABLE)
        {
            /* Set the bitfield to 1 disables the Error and clears it */
            recoveryAction = adrv904x_JrxLink_JrxCoreSysrefClrPhaseErr_BfSet(device, NULL, deframerBaseAddr, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, deframerIdx, "Failed to Clear/Disable Deframer Sysref Phase Error");
                goto cleanup;
            }
        }

        if ((action & ADI_ADRV904X_SERDES_SYSREF_ERR_ENABLE) == ADI_ADRV904X_SERDES_SYSREF_ERR_ENABLE)
        {
            /* Set the bitfield to 0 enables the Error */
            recoveryAction = adrv904x_JrxLink_JrxCoreSysrefClrPhaseErr_BfSet(device, NULL, deframerBaseAddr, errClr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ERROR_REPORT(&device->common, ADI_ADRV904X_ERRSRC_API, ADI_COMMON_ERRCODE_API, recoveryAction, deframerIdx, "Failed to Enable Deframer Sysref Phase Error");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIlasMismatchGet(adi_adrv904x_Device_t*  const device,
                                                                  const adi_adrv904x_DeframerSel_e deframerSel,
                                                                  adi_adrv904x_DfrmCompareData_t* const dfrmData)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    adi_adrv904x_DeframerCfg_t deframerCfg; /* Initialized later on */
    uint8_t  deframerLinkType = 0U;
    uint8_t  laneId = 0U;
    uint8_t  laneSel = 0U;
    uint8_t  laneIdx = 0xFF;
    uint8_t  regData = 0U;
    uint8_t  cfgData = 0U;
    uint32_t ilasIdx = 0U;
    uint32_t zeroData = 0U;
    /* uint16_t irqVec = 0U; */
    /* uint16_t  cfgMismatch = 0U; */

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfrmData, cleanup);
    ADI_LIBRARY_MEMSET(&deframerCfg, 0U, sizeof(adi_adrv904x_DeframerCfg_t));
    ADI_LIBRARY_MEMSET(dfrmData, 0U, sizeof(adi_adrv904x_DfrmCompareData_t));

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading deframer enabled status");
        goto cleanup;
    }

    /* Check if deframer requested is enabled, if not return error */
    if ((regData & deframerSel) == 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: Requested deframer not enabled");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading link type from device");
        goto cleanup;
    }

    if (deframerLinkType != 0U)  /* 204C is not supported */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid link type: only supports 204B link.");
        goto cleanup;
    }

    /* Get Deframer Lane Crossbar information to find the first lane in used */
    recoveryAction = adrv904x_DeframerLaneEnableGet(device, deframerSel, &deframerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane cross bar the selected deframer");
        goto cleanup;
    }

    for (laneId = 0U; laneId < ADI_ADRV904X_MAX_SERDES_LANES; ++laneId)
    {
        laneSel = 1U << laneId;
        if ((deframerCfg.deserializerLanesEnabled & laneSel) == laneSel)
        {
            /* Find first lane in use. */
            laneIdx = laneId;
            break;
        }
    }

    if (laneIdx == 0xFF) /* No active lane found - Still have initialized value */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to find active lane for the selected deframer");
        goto cleanup;
    }


    /* TODO: We can not use this field, because hw calculates checksum with BankID =0,
     * Add code to calculate checksum in SW */

    /* Checking IRQ Vector if HW reporting any CFG mismatch */
    /*
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqVec_BfGet(device,
                                                               NULL,
                                                               ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                               laneIdx,
                                                               &irqVec);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get IRQ Vector for the selected deframer");
        goto cleanup;
    }
    */
    /* cfgMismatch = irqVec >> 8U; */

    /*** Comparing DID ***/
    /* ILAS DID */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS DID for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmDID = regData;
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG DID */
    recoveryAction = adrv904x_JrxLink_JrxCoreDidCfg_BfGet(device,
                                                          NULL,
                                                          deframerBaseAddr,
                                                          &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG DID cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmDID = cfgData;

    /* Compare ILAS & CFG DID */
    if (dfrmData->dfrmIlasData.dfrmDID != dfrmData->dfrmCfgData.dfrmDID)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing BID ***/
    /* ILAS BID */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS BID for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmBID = (regData & 0x0F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG BID is not supported so deframer assign it to ILAS read */
    dfrmData->dfrmCfgData.dfrmBID = dfrmData->dfrmIlasData.dfrmBID;

    /* Compare ILAS & CFG BID */
    if (dfrmData->dfrmIlasData.dfrmBID != dfrmData->dfrmCfgData.dfrmBID)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing LID ***/
    /* ILAS LID */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS LID for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmLID0 = (regData & 0x1F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG LID */
    recoveryAction = adrv904x_JrxLink_JrxCoreLidCfg_BfGet(device,
                                                          NULL,
                                                          deframerBaseAddr,
                                                          laneIdx,
                                                          &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG LID cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmLID0 = cfgData;

    /* Compare ILAS & CFG LID */
    if (dfrmData->dfrmIlasData.dfrmLID0 != dfrmData->dfrmCfgData.dfrmLID0)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing L ***/
    /* ILAS L */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS L for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmL = (regData & 0x1F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG L */
    recoveryAction = adrv904x_JrxLink_JrxCoreLCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG L cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmL = cfgData;

    /* Compare ILAS & CFG L */
    if (dfrmData->dfrmIlasData.dfrmL != dfrmData->dfrmCfgData.dfrmL)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing DSCR ***/
    /* ILAS DSCR */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS DSCR for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmSCR = ((regData & 0x80) >> 7);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG DSCR */
    recoveryAction = adrv904x_JrxLink_JrxCoreDscrCfg_BfGet(device,
                                                           NULL,
                                                           deframerBaseAddr,
                                                           &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG DSCR cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmSCR = cfgData;

    /* Compare ILAS & CFG DSCR */
    if (dfrmData->dfrmIlasData.dfrmSCR != dfrmData->dfrmCfgData.dfrmSCR)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing F ***/
    /* ILAS F */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS F for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmF = regData;
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG F */
    recoveryAction = adrv904x_JrxLink_JrxCoreFCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG F cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmF = cfgData;

    /* Compare ILAS & CFG F */
    if (dfrmData->dfrmIlasData.dfrmF != dfrmData->dfrmCfgData.dfrmF)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing K ***/
    /* ILAS K */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS K for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmK = (regData & 0x1F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG K */
    recoveryAction = adrv904x_JrxLink_JrxCoreKCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG K cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmK = cfgData;

    /* Compare ILAS & CFG K */
    if (dfrmData->dfrmIlasData.dfrmK != dfrmData->dfrmCfgData.dfrmK)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing M ***/
    /* ILAS M */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS M for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmM = regData;
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG M */
    recoveryAction = adrv904x_JrxLink_JrxCoreMCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG M cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmM = cfgData;

    /* Compare ILAS & CFG M */
    if (dfrmData->dfrmIlasData.dfrmM != dfrmData->dfrmCfgData.dfrmM)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing N ***/
    /* ILAS N */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS N for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmN = (regData & 0x1F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG N */
    recoveryAction = adrv904x_JrxLink_JrxCoreNCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG N cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmN = cfgData;

    /* Compare ILAS & CFG N */
    if (dfrmData->dfrmIlasData.dfrmN != dfrmData->dfrmCfgData.dfrmN)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing CS ***/
    /* ILAS CS */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS CS for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmCS = ((regData & 0xC0) >> 6);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG CS */
    recoveryAction = adrv904x_JrxLink_JrxCoreCsCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG CS cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmCS = cfgData;

    /* Compare ILAS & CFG CS */
    if (dfrmData->dfrmIlasData.dfrmCS != dfrmData->dfrmCfgData.dfrmCS)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing NP ***/
    /* ILAS NP */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS NP for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmNP = (regData & 0X1F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG NP */
    recoveryAction = adrv904x_JrxLink_JrxCoreNpCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG NP cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmNP = cfgData;

    /* Compare ILAS & CFG NP */
    if (dfrmData->dfrmIlasData.dfrmNP != dfrmData->dfrmCfgData.dfrmNP)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing S ***/
    /* ILAS S */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet(device,
                                                                 NULL,
                                                                 ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                 laneIdx,
                                                                 &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS S for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmS = (regData & 0X1F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG S */
    recoveryAction = adrv904x_JrxLink_JrxCoreSCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG S cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmS = cfgData;

    /* Compare ILAS & CFG S */
    if (dfrmData->dfrmIlasData.dfrmS != dfrmData->dfrmCfgData.dfrmS)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing CF ***/
    /* ILAS CF */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  laneIdx,
                                                                  &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS CF for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmCF = (regData & 0X1F);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG CF */
    /* CF is not supported so deframer assign it to value read */
    dfrmData->dfrmCfgData.dfrmCF = dfrmData->dfrmIlasData.dfrmCF;

    /* Compare ILAS & CFG CF */
    if (dfrmData->dfrmIlasData.dfrmCF != dfrmData->dfrmCfgData.dfrmCF)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing HD ***/
    /* ILAS HD */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  laneIdx,
                                                                  &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS HD for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmHD = ((regData & 0x80) >> 7);
    zeroData = (regData == 0) ? zeroData : (zeroData | (1U << ilasIdx));

    /* CFG HD */
    recoveryAction = adrv904x_JrxLink_JrxCoreHdCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG HD cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmHD = cfgData;

    /* Compare ILAS & CFG HD */
    if (dfrmData->dfrmIlasData.dfrmHD != dfrmData->dfrmCfgData.dfrmHD)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    /*** Comparing CHKSUM ***/
    /* ILAS CHKSUM */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                  laneIdx,
                                                                  &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS CHKSUM for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmIlasData.dfrmFCHK0 = regData;

    /* CFG CHKSUM */
    recoveryAction = adrv904x_JrxLink_JrxCoreChksumCfg_BfGet(device,
                                                             NULL,
                                                             deframerBaseAddr,
                                                             laneIdx,
                                                             &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG CHKSUM cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmFCHK0 = cfgData;

    /* Compare ILAS & CFG CHKSUM */
    if (dfrmData->dfrmIlasData.dfrmFCHK0 != dfrmData->dfrmCfgData.dfrmFCHK0)
    {
        dfrmData->ilasMismatchDfrm |=  (1U << ilasIdx);
    }
    ilasIdx++;

    dfrmData->zeroCheckFlag = zeroData;

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

static adi_adrv904x_ErrAction_e adrv904x_IlasChksum(adi_adrv904x_Device_t* const    device,
                                                    adi_adrv904x_DfrmCompareData_v2_t* dfrmData)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t laneIdx = 0U;
    uint8_t chksum = 0U;
    uint8_t preChksum = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_PTR_RETURN(dfrmData);



    /* Calculate checksum for common fields */
    preChksum = dfrmData->dfrmCfgData.jesdV;      /* JESDV */
    preChksum += dfrmData->dfrmCfgData.subclassV; /* SUBCLASSV */
    preChksum += dfrmData->dfrmCfgData.dfrmDID;   /* DID */
    preChksum += dfrmData->dfrmCfgData.dfrmL;     /* L */
    preChksum += dfrmData->dfrmCfgData.dfrmF;     /* F */
    preChksum += dfrmData->dfrmCfgData.dfrmK;     /* K */
    preChksum += dfrmData->dfrmCfgData.dfrmM;     /* M */
    preChksum += dfrmData->dfrmCfgData.dfrmN;     /* N */
    preChksum += dfrmData->dfrmCfgData.dfrmCS;    /* CS */
    preChksum += dfrmData->dfrmCfgData.dfrmNP;    /* NP */
    preChksum += dfrmData->dfrmCfgData.dfrmS;     /* S */
    preChksum += dfrmData->dfrmCfgData.dfrmHD;    /* HD */
    preChksum += dfrmData->dfrmCfgData.dfrmSCR;   /* SCR */
    preChksum += dfrmData->dfrmCfgData.dfrmCF;    /* CF */

    /* Continue to calculate checksum to include lane dependency */
    for (laneIdx = 0; laneIdx < ADI_ADRV904X_MAX_SERDES_LANES; laneIdx++)
    {
        chksum = preChksum;
        chksum += dfrmData->dfrmIlasData[laneIdx].dfrmBID; /* use lane BID */
        chksum += dfrmData->dfrmIlasData[laneIdx].dfrmLID; /* use lane LID */

        dfrmData->cfgDataChksum[laneIdx] = chksum;

        if (dfrmData->cfgDataChksum[laneIdx] != dfrmData->dfrmIlasData[laneIdx].dfrmFCHK)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |= ADI_ADRV904X_ILAS_CKSM;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIlasMismatchGet_v2(adi_adrv904x_Device_t*  const device,
                                                                     const adi_adrv904x_DeframerSel_e deframerSel,
                                                                     adi_adrv904x_DfrmCompareData_v2_t* const dfrmData)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJrxLinkChanAddr_e deframerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JRX_LINK_0_;
    adi_adrv904x_DeframerCfg_t deframerCfg; /* Initialized later on */
    uint8_t  deframerLinkType = 0U;
    uint8_t  laneIdx = 0U;
    uint8_t  regData = 0U;
    uint8_t  cfgData = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dfrmData, cleanup);
    ADI_LIBRARY_MEMSET(&deframerCfg, 0U, sizeof(adi_adrv904x_DeframerCfg_t));
    ADI_LIBRARY_MEMSET(dfrmData, 0U, sizeof(adi_adrv904x_DfrmCompareData_v2_t));

    /* Get the base address of the selected deframer */
    recoveryAction = adrv904x_DeframerBitfieldAddressGet(device, deframerSel, &deframerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected deframer");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxLinkEn_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                         &regData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading deframer enabled status");
        goto cleanup;
    }

    /* Check if deframer requested is enabled, if not return error */
    if ((regData & deframerSel) == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error: Requested deframer not enabled");
        goto cleanup;
    }

    /* Get the deframer link type: 204B or 204C */
    recoveryAction = adrv904x_JrxLink_JrxLinkType_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &deframerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading link type from device");
        goto cleanup;
    }

    if (deframerLinkType != 0U)  /* 204C is not supported */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid link type: only supports 204B link.");
        goto cleanup;
    }

    /* Get Deframer Lane Crossbar information to find the first lane in used */
    recoveryAction = adrv904x_DeframerLaneEnableGet(device, deframerSel, &deframerCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane cross bar the selected deframer");
        goto cleanup;
    }
    dfrmData->phyLaneEnMask = deframerCfg.deserializerLanesEnabled;

    /***** Read all necessary configuration of the selected deframer for comparison *****/

    /* CFG DID */
    recoveryAction = adrv904x_JrxLink_JrxCoreDidCfg_BfGet(device,
                                                          NULL,
                                                          deframerBaseAddr,
                                                          &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG DID cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmDID = cfgData;

    /* CFG L */
    recoveryAction = adrv904x_JrxLink_JrxCoreLCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG L cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmL = cfgData;

    /* CFG DSCR */
    recoveryAction = adrv904x_JrxLink_JrxCoreDscrCfg_BfGet(device,
                                                           NULL,
                                                           deframerBaseAddr,
                                                           &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG DSCR cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmSCR = cfgData;

    /* CFG F */
    recoveryAction = adrv904x_JrxLink_JrxCoreFCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG F cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmF = cfgData;

    /* CFG K */
    recoveryAction = adrv904x_JrxLink_JrxCoreKCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG K cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmK = cfgData;

    /* CFG M */
    recoveryAction = adrv904x_JrxLink_JrxCoreMCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG M cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmM = cfgData;

    /* CFG N */
    recoveryAction = adrv904x_JrxLink_JrxCoreNCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG N cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmN = cfgData;

    /* CFG CS */
    recoveryAction = adrv904x_JrxLink_JrxCoreCsCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG CS cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmCS = cfgData;

    /* CFG NP */
    recoveryAction = adrv904x_JrxLink_JrxCoreNpCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG NP cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmNP = cfgData;

    /* CFG S */
    recoveryAction = adrv904x_JrxLink_JrxCoreSCfg_BfGet(device,
                                                        NULL,
                                                        deframerBaseAddr,
                                                        &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG S cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmS = cfgData;

    /* CFG HD */
    recoveryAction = adrv904x_JrxLink_JrxCoreHdCfg_BfGet(device,
                                                         NULL,
                                                         deframerBaseAddr,
                                                         &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG HD cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.dfrmHD = cfgData;

    /* CFG CF is always zero */
    dfrmData->dfrmCfgData.dfrmCF = 0U;

    /* CFG JESDV */
    recoveryAction = adrv904x_JrxLink_JrxCoreJesdvCfg_BfGet(device,
                                                            NULL,
                                                            deframerBaseAddr,
                                                            &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG JESDV cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.jesdV = cfgData;

    /* CFG SUBCLASSV */
    recoveryAction = adrv904x_JrxLink_JrxCoreSubclassvCfg_BfGet(device,
                                                                NULL,
                                                                deframerBaseAddr,
                                                                &cfgData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get CFG SUBCLASSV cfg for the selected deframer");
        goto cleanup;
    }
    dfrmData->dfrmCfgData.subclassV = cfgData;

    /***** Read configuration for each lane and compare with the selected deframer configuration *****/

    for (laneIdx = 0U; laneIdx < ADI_ADRV904X_MAX_SERDES_LANES; laneIdx++)
    {
        if ((dfrmData->phyLaneEnMask & (1U << laneIdx)) == 0U)
        {
            continue; /* skip lane that is not enabled */
        }

        /*** Read and comparing lane DID ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg0_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS DID for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmDID = regData;

        if (dfrmData->dfrmIlasData[laneIdx].dfrmDID != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_DID;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmDID != dfrmData->dfrmCfgData.dfrmDID)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_DID;
        }

        /*** Read and comparing lane BID ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg1_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS BID for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmBID = (regData & 0x0FU);

        if (dfrmData->dfrmIlasData[laneIdx].dfrmBID != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_BID;
        }

        /* CFG BID is not supported so deframer assign it to lane ILAS read */
        dfrmData->dfrmCfgData.dfrmBID = dfrmData->dfrmIlasData[laneIdx].dfrmBID;

        if (dfrmData->dfrmIlasData[laneIdx].dfrmBID != dfrmData->dfrmCfgData.dfrmBID)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_BID;
        }

        /*** Read and comparing lane LID ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg2_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS LID for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmLID = (regData & 0x1FU);

        if (dfrmData->dfrmIlasData[laneIdx].dfrmLID != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_LID;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmLID != dfrmData->dfrmCfgData.dfrmLID)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_LID;
        }

        /*** Read and comparing lane L and SCR ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg3_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS L and SCR for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmL = (regData & 0x1FU);
        dfrmData->dfrmIlasData[laneIdx].dfrmSCR = ((regData & 0x80U) >> 7U);

        if (dfrmData->dfrmIlasData[laneIdx].dfrmL != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_L;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmL != dfrmData->dfrmCfgData.dfrmL)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_L;
        }

        /*** Comparing lane DSCR ***/
        if (dfrmData->dfrmIlasData[laneIdx].dfrmSCR != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_SCR;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmSCR != dfrmData->dfrmCfgData.dfrmSCR)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_SCR;
        }

        /*** Read and comparing lane F ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg4_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS F for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmF = regData;

        if (dfrmData->dfrmIlasData[laneIdx].dfrmF != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_F;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmF != dfrmData->dfrmCfgData.dfrmF)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_F;
        }

        /*** Read and comparing lane K ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg5_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS K for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmK = (regData & 0x1FU);

        if (dfrmData->dfrmIlasData[laneIdx].dfrmK != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_K;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmK != dfrmData->dfrmCfgData.dfrmK)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_K;
        }

        /*** Read and comparing lane M ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg6_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS M for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmM = regData;

        if (dfrmData->dfrmIlasData[laneIdx].dfrmM != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_M;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmM != dfrmData->dfrmCfgData.dfrmM)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_M;
        }

        /*** Read and comparing lane N and CS ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg7_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS N and CS for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmN = (regData & 0x1FU);
        dfrmData->dfrmIlasData[laneIdx].dfrmCS = ((regData & 0xC0U) >> 6U);

        if (dfrmData->dfrmIlasData[laneIdx].dfrmN != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_N;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmN != dfrmData->dfrmCfgData.dfrmN)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_N;
        }

        /*** Comparing lane CS ***/
        if (dfrmData->dfrmIlasData[laneIdx].dfrmCS != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_CS;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmCS != dfrmData->dfrmCfgData.dfrmCS)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_CS;
        }

        /*** Read and comparing lane NP and JESDV ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg8_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS NP and JESDV for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmNP = (regData & 0X1FU);
        dfrmData->dfrmIlasData[laneIdx].jesdV = (regData & 0XE0U) >> 5U;

        if (dfrmData->dfrmIlasData[laneIdx].dfrmNP != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_NP;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmNP != dfrmData->dfrmCfgData.dfrmNP)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_NP;
        }

        /*** Comparing lane JESDV ***/
        if (dfrmData->dfrmIlasData[laneIdx].jesdV != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_JESDV;
        }

        if (dfrmData->dfrmIlasData[laneIdx].jesdV != dfrmData->dfrmCfgData.jesdV)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_JESDV;
        }

        /*** Read and comparing lane S ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg9_BfGet(device,
                                                                     NULL,
                                                                     ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                     laneIdx,
                                                                     &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS S for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmS = (regData & 0X1FU);
        dfrmData->dfrmIlasData[laneIdx].subclassV = (regData & 0XE0) >> 5U;

        if (dfrmData->dfrmIlasData[laneIdx].dfrmS != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_S;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmS != dfrmData->dfrmCfgData.dfrmS)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_S;
        }

        /*** Comparing lane SUBCLASSV ***/
        if (dfrmData->dfrmIlasData[laneIdx].subclassV != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_SUBCLASSV;
        }

        if (dfrmData->dfrmIlasData[laneIdx].subclassV != dfrmData->dfrmCfgData.subclassV)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_SUBCLASSV;
        }

        /*** Read and comparing lane CF ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg10_BfGet(device,
                                                                      NULL,
                                                                      ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                      laneIdx,
                                                                      &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS CF and HD for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmCF = (regData & 0X1FU);
        dfrmData->dfrmIlasData[laneIdx].dfrmHD = ((regData & 0x80U) >> 7U);

        if (dfrmData->dfrmIlasData[laneIdx].dfrmCF != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_CF;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmCF != dfrmData->dfrmCfgData.dfrmCF)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_CF;
        }

        /*** Comparing lane HD ***/
        if (dfrmData->dfrmIlasData[laneIdx].dfrmHD != 0U)
        {
            dfrmData->zeroCheckFlag[laneIdx] |= ADI_ADRV904X_ILAS_HD;
        }

        if (dfrmData->dfrmIlasData[laneIdx].dfrmHD != dfrmData->dfrmCfgData.dfrmHD)
        {
            dfrmData->ilasMismatchDfrm[laneIdx] |=  ADI_ADRV904X_ILAS_HD;
        }

        /*** Read lane CHKSUM ***/
        recoveryAction = adrv904x_JesdCommon_JrxDl204bL0Rxcfg13_BfGet(device,
                                                                      NULL,
                                                                      ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON,
                                                                      laneIdx,
                                                                      &regData);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get ILAS CHKSUM for the selected deframer");
            goto cleanup;
        }
        dfrmData->dfrmIlasData[laneIdx].dfrmFCHK = regData;

        /*** Set lane mismatch bit map */
        if (dfrmData->ilasMismatchDfrm[laneIdx] != 0U)
        {
            dfrmData->laneMismatchMask |= 1U << laneIdx;
        }
    }

    /*** Calculate and compare CHKSUM ***/
    recoveryAction = adrv904x_IlasChksum(device, dfrmData);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to compare ILAS CHKSUM for the selected deframer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqMaskGet(   adi_adrv904x_Device_t*  const   device,
                                                                uint16_t* const                 irqMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, irqMask, cleanup);
    *irqMask = 0U;

    /* Read back the Dfrm IRQ Mask Vector for deframers */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqClr_BfGet( device,
                                                                NULL,
                                                                commonBaseAddr,
                                                                irqMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading 204B Irq Mask for Deframers");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqMaskSet(   adi_adrv904x_Device_t*  const   device,
                                                                const uint16_t                  irqMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Set the Dfrm IRQ Mask Vector for deframers */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet( device,
                                                                NULL,
                                                                commonBaseAddr,
                                                                irqMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting 204B Irq Mask for Deframers");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqSourceReset(   adi_adrv904x_Device_t*  const   device)
{
        static const uint16_t irqMask_Clr = 0xFFFFU;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint16_t irqMask_Save = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Read back the Dfrm IRQ Vector, clear interrupts, restore original mask */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqClr_BfGet(  device,
                                                                 NULL,
                                                                 commonBaseAddr,
                                                                &irqMask_Save);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error saving initial 204B Irq Mask for Deframers");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet(  device,
                                                                 NULL,
                                                                 commonBaseAddr,
                                                                 irqMask_Clr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error clearing all 204B Irqs for Deframers");
        goto cleanup;
    }

    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet(  device,
                                                                 NULL,
                                                                 commonBaseAddr,
                                                                 irqMask_Save);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error restoring initial 204B Irq Mask for Deframers");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmIrqSourceGet( adi_adrv904x_Device_t*  const               device,
                                                                const adi_adrv904x_DeframerSel_e            deframerSelect,
                                                                adi_adrv904x_DeframerIrqVector_t* const     irqSourceVector)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    adi_adrv904x_DeframerCfg_t deframerCfg[ADI_ADRV904X_MAX_DEFRAMERS];
    uint8_t deframerIdx = 0U;
    uint8_t laneIdx = 0U;
    uint8_t laneSel = 0U;

    ADI_LIBRARY_MEMSET(&deframerCfg, 0, sizeof(deframerCfg));

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, irqSourceVector, cleanup);

    /* Range Check deframerSelect */
    if ((deframerSelect != ADI_ADRV904X_DEFRAMER_0) &&
        (deframerSelect != ADI_ADRV904X_DEFRAMER_1) &&
        (deframerSelect != ADI_ADRV904X_ALL_DEFRAMER))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, deframerSelect, "Invalid JESD Deframer Select Value. Must select Deframer 0, 1, or All.");
        goto cleanup;
    }

    /* Clear irqSourceVector */
    ADI_LIBRARY_MEMSET(irqSourceVector, 0, sizeof(adi_adrv904x_DeframerIrqVector_t));

    /* Determine which lane vectors are used by each deframer */
    for (deframerIdx = 0U; deframerIdx < ADI_ADRV904X_MAX_DEFRAMERS; deframerIdx++)
    {
        recoveryAction = adi_adrv904x_DeframerCfgGet(device, ADI_ADRV904X_DEFRAMER_0, &deframerCfg[deframerIdx]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving deframer configuration");
            goto cleanup;
        }
    }

    /* Read back Dfrm IRQ Vectors for all lanes enabled for any selected deframer */
    for (laneIdx = 0U; laneIdx < ADI_ADRV904X_MAX_DESERIALIZER_LANES; laneIdx++)
    {
        laneSel = 1U << laneIdx;

        // Read this lane if enabled for a selected deframer
        if ((deframerSelect == ADI_ADRV904X_DEFRAMER_0) ||
            (deframerSelect == ADI_ADRV904X_ALL_DEFRAMER))
        {
            if ((deframerCfg[0U].deserializerLanesEnabled & laneSel) != 0U)
            {
                recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqVec_BfGet(  device,
                                                                             NULL,
                                                                             commonBaseAddr,
                                                                             laneIdx,
                                                                            &irqSourceVector->lane[laneIdx]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Irq Source Vector for a deserialized lane enabled for Deframer0.");
                    goto cleanup;
                }

                irqSourceVector->deframer0 |= irqSourceVector->lane[laneIdx];
            }
        }

        if ((deframerSelect == ADI_ADRV904X_DEFRAMER_1) ||
            (deframerSelect == ADI_ADRV904X_ALL_DEFRAMER))
        {
            if ((deframerCfg[1U].deserializerLanesEnabled & laneSel) != 0U)
            {
                recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqVec_BfGet(  device,
                                                                             NULL,
                                                                             commonBaseAddr,
                                                                             laneIdx,
                                                                            &irqSourceVector->lane[laneIdx]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Irq Source Vector for a deserialized lane enabled for Deframer0.");
                    goto cleanup;
                }

                irqSourceVector->deframer1 |= irqSourceVector->lane[laneIdx];
            }
        }

    }


cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCntrCntrlSet(  adi_adrv904x_Device_t*  const               device,
                                                                    const adi_adrv904x_DfrmErrCounterIrqSel_e   interruptEnable,
                                                                    const uint8_t                               laneNumber,
                                                                    const uint8_t                               errCounterControl,
                                                                    const uint8_t                               errCounterHoldCfg)
{
        static const uint16_t IRQERRCOUNTERMASK   = 0x00E0U;
    static const uint8_t  ERRCOUNTERRSTMASK   = 0X70U;
    static const uint8_t  ERRCOUNTERCNTRLMASK = 0x07U;
    static const uint8_t  ERRCOUNTERHOLDMASK  = 0x07U;
    static const uint8_t  ERRCOUNTERRSTSHIFT  = 4U;
    static const uint8_t  CLEAR_RESET         = 0x00U;
    static const uint8_t  MAX_COUNTER         = 0xFFU;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint16_t irqMask = 0U;
    uint8_t errorCounterReset  = 0x00U;
    uint8_t errorCounterConfig = 0x00U;
    uint8_t lcl_errCounterHoldCfg = 0x00U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check interrupt enable is valid */
    if ((interruptEnable != ADI_ADRV904X_DFRM_ERR_COUNT_ENABLE_IRQ) &&
        (interruptEnable != ADI_ADRV904X_DFRM_ERR_COUNT_DISABLE_IRQ))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, interruptEnable, "Invalid JESD 204b Deframer Error Counter Interrupt Enable value. Must select Enable or Disable.");
        goto cleanup;
    }

    /* Check lane number */
    if (laneNumber >= ADI_ADRV904X_MAX_DESERIALIZER_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Invalid Lane Number value selected. Must Select lane 0-7.");
        goto cleanup;
    }

    /* Read current irq mask for read/modify/write */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqClr_BfGet(  device,
                                                                 NULL,
                                                                 commonBaseAddr,
                                                                &irqMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error saving initial 204B Irq Mask for Deframers");
        goto cleanup;
    }

    /* Mask or unmask the three 204B Error-Counter based IRQs (BD/NIT/UnexpectedK) */
    if (interruptEnable == ADI_ADRV904X_DFRM_ERR_COUNT_ENABLE_IRQ)
    {
        irqMask &= (~IRQERRCOUNTERMASK);
    }
    else
    {
        irqMask |= IRQERRCOUNTERMASK;
    }

    /* Write modified irqMask to device */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bIrqClr_BfSet(  device,
                                                                 NULL,
                                                                 commonBaseAddr,
                                                                 irqMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error setting new 204B Irq Mask for Deframers");
        goto cleanup;
    }

    /* Extract 3 reset bits from input argument */
    errorCounterReset = (errCounterControl & ERRCOUNTERRSTMASK) >> ERRCOUNTERRSTSHIFT;

    /* Mask 3 config bits from input argument */
    errorCounterConfig = errCounterControl & ERRCOUNTERCNTRLMASK;
    lcl_errCounterHoldCfg  = errCounterHoldCfg & ERRCOUNTERHOLDMASK;

    /* Reset the requested error counters */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet(    device,
                                                                    NULL,
                                                                    commonBaseAddr,
                                                                    laneNumber,
                                                                    errorCounterReset);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to reset error counters.");
        goto cleanup;
    }

    /* Clear reset bits */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntRst_BfSet(    device,
                                                                    NULL,
                                                                    commonBaseAddr,
                                                                    laneNumber,
                                                                    CLEAR_RESET);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear reset flag for error counters.");
        goto cleanup;
    }

    /* Set error counter threshold to 255 */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEth_BfSet(    device,
                                                                NULL,
                                                                commonBaseAddr,
                                                                MAX_COUNTER);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear set error threshold.");
        goto cleanup;
    }

    /* Write counter enable bits */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntEna_BfSet(    device,
                                                                    NULL,
                                                                    commonBaseAddr,
                                                                    laneNumber,
                                                                    errorCounterConfig);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write error counter enable.");
        goto cleanup;
    }

    /* Write the error counter hold configuration */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntTch_BfSet(    device,
                                                                    NULL,
                                                                    commonBaseAddr,
                                                                    laneNumber,
                                                                    lcl_errCounterHoldCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write error counter hold configuration.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfrmErrCntrCntrlGet(  adi_adrv904x_Device_t*  const       device,
                                                                    const uint8_t                       laneNumber,
                                                                    uint8_t* const                      errCounterControl,
                                                                    uint8_t* const                      errCounterHoldCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, errCounterControl, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, errCounterHoldCfg, cleanup);

    /* Check lane number */
    if (laneNumber >= ADI_ADRV904X_MAX_DESERIALIZER_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Invalid Lane Number value selected. Must Select lane 0-7.");
        goto cleanup;
    }

    /* Read out the contents of the error counter control */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntEna_BfGet(    device,
                                                                    NULL,
                                                                    commonBaseAddr,
                                                                    laneNumber,
                                                                    errCounterControl);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read error counter control.");
        goto cleanup;
    }

    /* Read out the contents of the error counter hold configuration */
    recoveryAction = adrv904x_JesdCommon_JrxDl204bEcntTch_BfGet(    device,
                                                                    NULL,
                                                                    commonBaseAddr,
                                                                    laneNumber,
                                                                    errCounterHoldCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read error counter hold configuration.");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunEyeSweep(adi_adrv904x_Device_t* const device,
                                                          const adi_adrv904x_CpuCmd_RunEyeSweep_t* const runEyeSweep,
                                                          adi_adrv904x_CpuCmd_RunEyeSweepResp_t* const runEyeSweepResp)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_RunEyeSweep_t runEyeSweepCmd;
    adrv904x_CpuCmd_RunEyeSweepResp_t runEyeSweepCmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    uint32_t chanSel = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runEyeSweep, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runEyeSweepResp, cleanup);
    ADI_LIBRARY_MEMSET(&runEyeSweepCmd, 0, sizeof(adrv904x_CpuCmd_RunEyeSweep_t));
    ADI_LIBRARY_MEMSET(&runEyeSweepCmdRsp, 0, sizeof(adrv904x_CpuCmd_RunEyeSweepResp_t));

    if (runEyeSweep->lane > 7U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->lane, "Invalid lane provided.");
        goto cleanup;
    }

    if (runEyeSweep->prbsPattern > 3U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->prbsPattern, "Invalid prbsPattern provided.");
        goto cleanup;
    }

    if (runEyeSweep->forceUsingOuter > 1U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->forceUsingOuter, "Invalid forceUsingOuter provided.");
        goto cleanup;
    }

    if (runEyeSweep->prbsCheckDuration_ms == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->prbsCheckDuration_ms, "Invalid prbsCheckDuration_ms provided.");
        goto cleanup;
    }

    /* Get the CPU that is responsible for the requested lane. */
    chanSel = 1U << (uint32_t)runEyeSweep->lane;
    recoveryAction = adrv904x_CpuChannelMappingGet(device, (adi_adrv904x_Channels_e)(chanSel), ADRV904X_CPU_OBJID_IC_SERDES, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->lane, "Invalid lane provided");
        goto cleanup;
    }

    /* Prepare the command payload */
    runEyeSweepCmd.lane = (uint8_t)runEyeSweep->lane;
    runEyeSweepCmd.prbsPattern = (uint8_t)runEyeSweep->prbsPattern;
    runEyeSweepCmd.forceUsingOuter = (uint8_t)runEyeSweep->forceUsingOuter;
    runEyeSweepCmd.prbsCheckDuration_ms = ADRV904X_HTOCL(runEyeSweep->prbsCheckDuration_ms);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            cpuType,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_RUN_SERDES_EYE_SWEEP,
                                            (void*)&runEyeSweepCmd,
                                            sizeof(runEyeSweepCmd),
                                            (void*)&runEyeSweepCmdRsp,
                                            sizeof(runEyeSweepCmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(runEyeSweepCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    runEyeSweepResp->spoLeft = runEyeSweepCmdRsp.spoLeft;
    runEyeSweepResp->spoRight = runEyeSweepCmdRsp.spoRight;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunEyeSweep_v2(adi_adrv904x_Device_t* const device,
                                                             const adi_adrv904x_CpuCmd_RunEyeSweep_t* const runEyeSweep,
                                                             adi_adrv904x_CpuCmd_RunEyeSweepResp_t* const runEyeSweepResp)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanSel = 0U;
    uint32_t lengthResp = 0U;
    uint32_t timeElapsedUs = 0U;
    uint32_t le32PrbsCheckDuration_ms = 0U;
    adi_adrv904x_CpuCmd_RunEyeSweepResp_t* runEyeSweepCmdRsp = NULL;
    uint8_t ctrlDataSet[ADRV904X_SERDES_HORIZ_EYE_SWEEP_CMD_SIZE_BYTES];
    uint8_t ctrlDataGet[ADRV904X_SERDES_HORIZ_EYE_SWEEP_CMD_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runEyeSweep, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runEyeSweepResp, cleanup);

    ADI_LIBRARY_MEMSET((void*)ctrlDataSet, 0, sizeof(ctrlDataSet));
    ADI_LIBRARY_MEMSET((void*)ctrlDataGet, 0, sizeof(ctrlDataGet));

    /* Verify Parameters*/
    if (runEyeSweep->lane > 7U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->lane, "Invalid lane provided.");
        goto cleanup;
    }

    if (runEyeSweep->prbsPattern > 3U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->prbsPattern, "Invalid prbsPattern provided.");
        goto cleanup;
    }

    if (runEyeSweep->prbsCheckDuration_ms == 0U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runEyeSweep->prbsCheckDuration_ms, "Invalid prbsCheckDuration_ms provided.");
        goto cleanup;
    }

    le32PrbsCheckDuration_ms = ADRV904X_HTOCL(runEyeSweep->prbsCheckDuration_ms);

    /* Get channel corresponded to the requested lane. */
    chanSel = 1U << (uint32_t)runEyeSweep->lane;

    /* Prepare the command payload */
    ctrlDataSet[0]  = (uint8_t)runEyeSweep->lane;
    ctrlDataSet[1]  = (uint8_t)ADRV904X_SERDES_TEST_CMD;
    ctrlDataSet[2]  = (uint8_t)ADRV904X_SERDES_TEST_HORIZ_EYE_SWEEP;
    ctrlDataSet[3]  = 0x01U;
    ctrlDataSet[4]  = (uint8_t)runEyeSweep->prbsPattern;
    ctrlDataSet[8]  = (uint8_t)(le32PrbsCheckDuration_ms & 0xFFU);
    ctrlDataSet[9]  = (uint8_t)((le32PrbsCheckDuration_ms >> 8U)  & 0xFFU);
    ctrlDataSet[10] = (uint8_t)((le32PrbsCheckDuration_ms >> 16U) & 0xFFU);
    ctrlDataSet[11] = (uint8_t)((le32PrbsCheckDuration_ms >> 24U) & 0xFFU);
    ctrlDataSet[12] = 0x01U;

    /* Exec command */
    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    ADRV904X_CPU_OBJID_IC_SERDES,
                                                    ADRV904X_SERDES_CTRL_SET_FSM_CMD,
                                                    (adi_adrv904x_Channels_e)(chanSel),
                                                    ctrlDataSet,
                                                    sizeof(ctrlDataSet),
                                                    &lengthResp,
                                                    ctrlDataGet,
                                                    sizeof(ctrlDataGet));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         chanSel,
                         "Failed to Run Horizontal Eye Sweep");
        goto cleanup;
    }

    runEyeSweepCmdRsp = (adi_adrv904x_CpuCmd_RunEyeSweepResp_t*)((uint8_t*)ctrlDataGet + ADRV904X_SERDES_CTRL_FSM_CMD_RSP_HDR_SIZE_BYTES);

    /* Poll Serdes Calibration Status for completion of the Eye Sweep */
    for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV904X_RUNEYESWEEP_TIMEOUT_US; timeElapsedUs += ADI_ADRV904X_RUNEYESWEEP_INTERVAL_US)
    {
        /* Get Serdes Calibration Status */
        recoveryAction = adi_adrv904x_CalSpecificStatusGet( device,
                                                            (adi_adrv904x_Channels_e)(chanSel),
                                                            ADRV904X_CPU_OBJID_TC_SERDES,
                                                            ctrlDataGet,
                                                            sizeof(ctrlDataGet));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get Serdes calibration status failed");
            goto cleanup;
        }

        /* Break out here if Eye sweep have been completed */
        if (ctrlDataGet[1] == (uint8_t)ADRV904X_SERDES_TEST_CMD_DONE)
        {
            break;
        }

        /* Eye Sweep is still in progress. Wait the specified wait interval, then check again for status. */
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV904X_RUNEYESWEEP_INTERVAL_US);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    /* Check for timeout */
    if (timeElapsedUs >= ADI_ADRV904X_RUNEYESWEEP_TIMEOUT_US)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_TIMEOUT,
                         recoveryAction,
                         timeElapsedUs,
                         "Run Horizontal Eye Sweep Timeout");
        goto cleanup;
    }

    /* Extract the command-specific response from the response payload */
    runEyeSweepResp->spoLeft = runEyeSweepCmdRsp->spoLeft;
    runEyeSweepResp->spoRight = runEyeSweepCmdRsp->spoRight;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunVerticalEyeSweep(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_CpuCmd_RunVertEyeSweep_t* const runVerticalEyeSweep,
                                                                  adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t* const runVerticalEyeSweepResp)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_RunVertEyeSweep_t runVerticalEyeSweepCmd;
    adrv904x_CpuCmd_RunVertEyeSweepResp_t runVerticalEyeSweepCmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    uint32_t chanSel = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runVerticalEyeSweep, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runVerticalEyeSweepResp, cleanup);

    ADI_LIBRARY_MEMSET(&runVerticalEyeSweepCmd, 0, sizeof(adrv904x_CpuCmd_RunVertEyeSweep_t));
    ADI_LIBRARY_MEMSET(&runVerticalEyeSweepCmdRsp, 0, sizeof(adrv904x_CpuCmd_RunVertEyeSweepResp_t));

    if (runVerticalEyeSweep->lane > 7U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runVerticalEyeSweep->lane, "Invalid lane provided.");
        goto cleanup;
    }

    /* Get the CPU that is responsible for the requested lane. */
    chanSel = (uint32_t)(1U << runVerticalEyeSweep->lane);
    recoveryAction = adrv904x_CpuChannelMappingGet(device, (adi_adrv904x_Channels_e)(chanSel), ADRV904X_CPU_OBJID_IC_SERDES, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runVerticalEyeSweep->lane, "Invalid lane provided");
        goto cleanup;
    }

    /* Prepare the command payload */
    runVerticalEyeSweepCmd.lane = (uint8_t)runVerticalEyeSweep->lane;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                         cpuType,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_RUN_SERDES_VERT_EYE_SWEEP,
                                         (void*)&runVerticalEyeSweepCmd,
                                         sizeof(runVerticalEyeSweepCmd),
                                         (void*)&runVerticalEyeSweepCmdRsp,
                                         sizeof(runVerticalEyeSweepCmdRsp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(runVerticalEyeSweepCmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    ADI_LIBRARY_MEMCPY(&runVerticalEyeSweepResp->eyeHeightsAtSpo, runVerticalEyeSweepCmdRsp.eyeHeightsAtSpo, sizeof(runVerticalEyeSweepCmdRsp.eyeHeightsAtSpo));

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RunVerticalEyeSweep_v2(adi_adrv904x_Device_t* const device,
                                                                     const adi_adrv904x_CpuCmd_RunVertEyeSweep_t* const runVerticalEyeSweep,
                                                                     adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t* const runVerticalEyeSweepResp)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t chanSel = 0U;
    uint32_t lengthResp = 0U;
    uint32_t timeElapsedUs = 0U;
    adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t* runEyeSweepCmdRsp = NULL;
    uint8_t ctrlDataSet[ADRV904X_SERDES_VERT_EYE_SWEEP_CMD_SIZE_BYTES];
    uint8_t ctrlDataGet[sizeof(adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t) + ADRV904X_SERDES_CTRL_FSM_CMD_RSP_HDR_SIZE_BYTES];

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runVerticalEyeSweep, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, runVerticalEyeSweepResp, cleanup);

    ADI_LIBRARY_MEMSET((void*)ctrlDataSet, 0, sizeof(ctrlDataSet));
    ADI_LIBRARY_MEMSET((void*)ctrlDataGet, 0, sizeof(ctrlDataGet));

    if (runVerticalEyeSweep->lane > 7U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, runVerticalEyeSweep->lane, "Invalid lane provided.");
        goto cleanup;
    }

    /* Get channel corresponded to the requested lane. */
    chanSel = 1U << (uint32_t)runVerticalEyeSweep->lane;

    /* Prepare the command payload */
    ctrlDataSet[0] = (uint8_t)runVerticalEyeSweep->lane;
    ctrlDataSet[1] = (uint8_t)ADRV904X_SERDES_TEST_CMD;
    ctrlDataSet[2] = (uint8_t)ADRV904X_SERDES_TEST_VERT_EYE_SWEEP;
    ctrlDataSet[3] = 0x01U;
    ctrlDataSet[4] = 0x10U;
    ctrlDataSet[5] = 0x01U;

    /* Exec command */
    recoveryAction = adi_adrv904x_CpuControlCmdExec(device,
                                                    ADRV904X_CPU_OBJID_IC_SERDES,
                                                    ADRV904X_SERDES_CTRL_SET_FSM_CMD,
                                                    (adi_adrv904x_Channels_e)(chanSel),
                                                    ctrlDataSet,
                                                    sizeof(ctrlDataSet),
                                                    &lengthResp,
                                                    ctrlDataGet,
                                                    sizeof(ctrlDataGet));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         chanSel,
                         "Failed to Run Vertical Eye Sweep");
        goto cleanup;
    }

    runEyeSweepCmdRsp = (adi_adrv904x_CpuCmd_RunVertEyeSweepResp_t*)((uint8_t*)ctrlDataGet + ADRV904X_SERDES_CTRL_FSM_CMD_RSP_HDR_SIZE_BYTES);

    /* Poll Serdes Calibration Status for completion of the Eye Sweep */
    for (timeElapsedUs = 0U; timeElapsedUs < ADI_ADRV904X_RUNEYESWEEP_TIMEOUT_US; timeElapsedUs += ADI_ADRV904X_RUNEYESWEEP_INTERVAL_US)
    {
        /* Get Serdes Calibration Status */
        recoveryAction = adi_adrv904x_CalSpecificStatusGet( device,
                                                            (adi_adrv904x_Channels_e)(chanSel),
                                                            ADRV904X_CPU_OBJID_TC_SERDES,
                                                            ctrlDataGet,
                                                            sizeof(ctrlDataGet));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Get Serdes calibration status failed");
            goto cleanup;
        }

        /* Break out here if Eye sweep have been completed */
        if (ctrlDataGet[1] == (uint8_t)ADRV904X_SERDES_TEST_CMD_DONE)
        {
            break;
        }

        /* Eye Sweep is still in progress. Wait the specified wait interval, then check again for status. */
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, ADI_ADRV904X_RUNEYESWEEP_INTERVAL_US);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait Issue");
            goto cleanup;
        }
    }

    /* Check for timeout */
    if (timeElapsedUs >= ADI_ADRV904X_RUNEYESWEEP_TIMEOUT_US)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_ERROR_REPORT(&device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_COMMON_ERRCODE_TIMEOUT,
                         recoveryAction,
                         timeElapsedUs,
                         "Run Vertical Eye Sweep Timeout");
        goto cleanup;
    }

    /* Extract the command-specific response from the response payload */
    ADI_LIBRARY_MEMCPY(&runVerticalEyeSweepResp->eyeHeightsAtSpo, runEyeSweepCmdRsp->eyeHeightsAtSpo, sizeof(runEyeSweepCmdRsp->eyeHeightsAtSpo));

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerializerReset_v2(adi_adrv904x_Device_t* const device,
                                                                 adi_adrv904x_CpuCmd_SerReset_t* const pSerResetParms,
                                                                 adi_adrv904x_CpuCmd_SerResetResp_t* const pSerResetResp)
{
        uint8_t framerLinkType = 0U;
    uint8_t phyLanePd = 0U;
    uint8_t phyLaneIdx = 0U;
    adrv904x_BfSerdesTxdigPhyRegmapCore1p2ChanAddr_e laneSerdesPhyBitfieldAddr = ADRV904X_BF_DIGITAL_CORE_JESD_SERIALIZER_SER_PHY_0_;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adrv904x_CpuCmd_SerReset_t serReset;
    adrv904x_CpuCmd_SerResetResp_t serResetResp;
    uint8_t presetClockOffsets = 0U;
    adrv904x_CpuCmd_JtxLanePower_t jtxPwrCmd;            

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pSerResetParms, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pSerResetResp, cleanup);

    ADI_LIBRARY_MEMSET(&serReset, 0, sizeof(adrv904x_CpuCmd_SerReset_t));
    ADI_LIBRARY_MEMSET(&serResetResp, 0, sizeof(adrv904x_CpuCmd_SerResetResp_t));
    ADI_LIBRARY_MEMSET(&jtxPwrCmd, 0, sizeof(jtxPwrCmd));

    /* copy parameter */
    serReset.serResetParm = ADRV904X_HTOCL(pSerResetParms->serResetParm);
    presetClockOffsets    = pSerResetParms->presetClockOffsets;

    /* Get the framer link type: 204B or 204C - Check link0 type only */
    recoveryAction = adrv904x_JtxLink_JtxLinkType_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_,
                                                        &framerLinkType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get link type for the selected framer");
        goto cleanup;
    }

    if (presetClockOffsets)
    {
        for (phyLaneIdx = 0U; phyLaneIdx < ADI_ADRV904X_MAX_SERDES_LANES; phyLaneIdx++)
        {
            recoveryAction = (adi_adrv904x_ErrAction_e) adrv904x_FramerLaneSerdesPhyBitfieldAddressGet(device,
                                                                                                       phyLaneIdx,
                                                                                                       &laneSerdesPhyBitfieldAddr);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane serdes PHY address.");
                goto cleanup;
            }

            recoveryAction = adrv904x_GetJtxLanePoweredDown(device,
                                                            laneSerdesPhyBitfieldAddr,
                                                            &phyLanePd);
            
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Jtx lane power-up status");
                goto cleanup;
            }
            
            if (phyLanePd == 0U)
            {
                /* Lane is powered up; Determine the reset sequence based on the 204c/204b mode */
                if (framerLinkType == 0U)
                {
                    /* 204b case set Clockoffset to 4 */
                    recoveryAction = adrv904x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet( device,
                                                                                                NULL,
                                                                                                laneSerdesPhyBitfieldAddr,
                                                                                                4U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing to serializer clock offset");
                        goto cleanup;
                    }

                }
                else
                {
                    /* 204C - set FIFO Start Addr to 1 */
                    recoveryAction = adrv904x_SerdesTxdigPhyRegmapCore1p2_FifoStartAddr_BfSet( device,
                                                                                                NULL,
                                                                                                laneSerdesPhyBitfieldAddr,
                                                                                                1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing Fifo Start Address");
                        goto cleanup;
                    }

                    /* set Clockoffset to 1 */
                    recoveryAction = adrv904x_SerdesTxdigPhyRegmapCore1p2_ClkoffsetSerRc_BfSet( device,
                                                                                                NULL,
                                                                                                laneSerdesPhyBitfieldAddr,
                                                                                                1U);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing digital logic clock shift");
                        goto cleanup;
                    }
                }
            }
        } /* End for loop per lane */
    }

    /* Send serializer reset command */
    recoveryAction = adrv904x_CpuCmdSend( device,
                                          ADI_ADRV904X_CPU_TYPE_0,
                                          ADRV904X_LINK_ID_0,
                                          ADRV904X_CPU_CMD_ID_JESD_SER_RESET,
                                          (void*)&serReset,
                                          sizeof(serReset),
                                          (void*)&serResetResp,
                                          sizeof(serResetResp),
                                          &cmdStatus);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(serResetResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    pSerResetResp->serResetResults = ADRV904X_CTOHL(serResetResp.serResetResults);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerLaneCfgGet(adi_adrv904x_Device_t* const device,
                                                            const uint8_t laneNumber,
                                                            adi_adrv904x_SerLaneCfg_t* const serLaneCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetJesdSerLaneCfg_t getJesdSerLaneCfg;
    adrv904x_CpuCmd_GetJesdSerLaneCfgResp_t getJesdSerLaneCfgResp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    uint32_t chanSel = 0U;

    ADI_LIBRARY_MEMSET(&getJesdSerLaneCfg, 0, sizeof(adrv904x_CpuCmd_GetJesdSerLaneCfg_t));
    ADI_LIBRARY_MEMSET(&getJesdSerLaneCfgResp, 0, sizeof(adrv904x_CpuCmd_GetJesdSerLaneCfgResp_t));

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, serLaneCfg, cleanup);

    if (laneNumber > 7U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Invalid lane provided.");
        goto cleanup;
    }

    ADI_LIBRARY_MEMSET(serLaneCfg, 0, sizeof(adi_adrv904x_SerLaneCfg_t));

    /* Get the CPU that is responsible for the requested lane. TODO: Using channel mapping for now */
    chanSel = 1U << (uint32_t)laneNumber;
    recoveryAction = adrv904x_CpuChannelMappingGet(device, (adi_adrv904x_Channels_e)(chanSel), ADRV904X_CPU_OBJID_SYSTEM_END, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Invalid lane provided");
        goto cleanup;
    }

    /* Prepare the command payload */
    getJesdSerLaneCfg.lane = (uint8_t)laneNumber;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            cpuType,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_JESD_SER_LANE_GET_CFG,
                                            (void*)&getJesdSerLaneCfg,
                                            sizeof(getJesdSerLaneCfg),
                                            (void*)&getJesdSerLaneCfgResp,
                                            sizeof(getJesdSerLaneCfgResp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(getJesdSerLaneCfgResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Extract the command-specific response from the response payload */
    serLaneCfg->outputDriveSwing = (adi_adrv904x_SerLaneCfgOutputDriveSwing_e)getJesdSerLaneCfgResp.outputDriveSwing;
    serLaneCfg->preEmphasis      = (adi_adrv904x_SerLaneCfgPreEmphasis_e)getJesdSerLaneCfgResp.preEmphasis;
    serLaneCfg->postEmphasis     = (adi_adrv904x_SerLaneCfgPostEmphasis_e)getJesdSerLaneCfgResp.postEmphasis;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerLaneCfgSet(adi_adrv904x_Device_t* const device,
                                                            const uint8_t laneNumber,
                                                            const adi_adrv904x_SerLaneCfg_t* const serLaneCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_SetJesdSerLaneCfg_t setJesdSerLaneCfg;
    adrv904x_CpuCmd_SetJesdSerLaneCfgResp_t setJesdSerLaneCfgResp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adi_adrv904x_CpuErrorCode_t cpuErrorCode = 0U;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    uint32_t chanSel = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, serLaneCfg, cleanup);
    ADI_LIBRARY_MEMSET(&setJesdSerLaneCfg, 0, sizeof(adrv904x_CpuCmd_SetJesdSerLaneCfg_t));
    ADI_LIBRARY_MEMSET(&setJesdSerLaneCfgResp, 0, sizeof(adrv904x_CpuCmd_SetJesdSerLaneCfgResp_t));

    if (laneNumber > 7U)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Invalid lane provided.");
        goto cleanup;
    }

    /* Get the CPU that is responsible for the requested lane. TODO: Using channel mapping for now */
    chanSel = 1U << (uint32_t)laneNumber;
    recoveryAction = adrv904x_CpuChannelMappingGet(device, (adi_adrv904x_Channels_e)(chanSel), ADRV904X_CPU_OBJID_SYSTEM_END, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Invalid lane provided");
        goto cleanup;
    }

    /* Prepare the command payload */
    setJesdSerLaneCfg.lane = (uint8_t)laneNumber;
    setJesdSerLaneCfg.outputDriveSwing = (uint8_t)serLaneCfg->outputDriveSwing;
    setJesdSerLaneCfg.preEmphasis = (uint8_t)serLaneCfg->preEmphasis;
    setJesdSerLaneCfg.postEmphasis = (uint8_t)serLaneCfg->postEmphasis;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            cpuType,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_JESD_SER_LANE_SET_CFG,
                                            (void*)&setJesdSerLaneCfg,
                                            sizeof(setJesdSerLaneCfg),
                                            (void*)&setJesdSerLaneCfgResp,
                                            sizeof(setJesdSerLaneCfgResp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(setJesdSerLaneCfgResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_FramerTestDataInjectError(adi_adrv904x_Device_t*  const  device,
                                                                        const adi_adrv904x_FramerSel_e framerSelect,
                                                                        const uint8_t  laneMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    adrv904x_BfJtxLinkChanAddr_e framerBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JTX_LINK_0_;
    uint8_t                      ballLane         = 0U;
    uint8_t                      invert           = 0U;
    uint8_t                      pnInvertBit      = 0U;
    uint8_t                      dieLaneId        = 0U;
    uint8_t                      framerOutForLane = 0U;

    static const uint8_t framerLaneLamTbl[ADI_ADRV904X_MAX_SERIALIZER_LANES] = { 1u, /* [0] SEROUT A */
                                                                                 3u, /* [1] SEROUT B */
                                                                                 5u, /* [2] SEROUT C */
                                                                                 7u, /* [3] SEROUT D */
                                                                                 0u, /* [4] SEROUT E */
                                                                                 2u, /* [5] SEROUT F */
                                                                                 4u, /* [6] SEROUT G */
                                                                                 6u  /* [7] SEROUT H */
                                                                                };

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Get the base address of the selected framer */
    recoveryAction = adrv904x_FramerBitfieldAddressGet(device, framerSelect, &framerBaseAddr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get base address for the selected framer");
        goto cleanup;
    }

    for (ballLane = 0U; ballLane < ADI_ADRV904X_MAX_SERIALIZER_LANES ; ballLane++)
    {
        invert = ((laneMask >> ballLane) & 0x01U);

        if (invert == 1)
        {
            dieLaneId = framerLaneLamTbl[ballLane];

            /* PN Invert occurs at framer output before lane crossbar */
            recoveryAction = adrv904x_JtxLink_JtxLaneSel_BfGet(device,
                                                               NULL,
                                                               framerBaseAddr,
                                                               dieLaneId,
                                                               &framerOutForLane);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get lane select for the selected framer");
                goto cleanup;
            }

            if (framerOutForLane < ADI_ADRV904X_MAX_SERIALIZER_LANES)
            {
                /* Read current PN invert setting and set it back at end*/
                recoveryAction = adrv904x_JtxLink_JtxLaneInv_BfGet(device,
                                                                   NULL,
                                                                   framerBaseAddr,
                                                                   framerOutForLane,
                                                                   &pnInvertBit);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to get pn invert bit for the selected framer");
                    goto cleanup;
                }

                recoveryAction = adrv904x_JtxLink_JtxLaneInv_BfSet(device,
                                                                   NULL,
                                                                   framerBaseAddr,
                                                                   framerOutForLane,
                                                                   ((~pnInvertBit) & 0x01U));
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set pn invert bit for the selected framer");
                    goto cleanup;
                }

                recoveryAction = adrv904x_JtxLink_JtxLaneInv_BfSet(device,
                                                                   NULL,
                                                                   framerBaseAddr,
                                                                   framerOutForLane,
                                                                   pnInvertBit);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while attempting to set pn invert bit for the selected framer");
                    goto cleanup;
                }

            }
            else
            {
                /* Lane disabled - do not invert */
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


/* Open 'filePath' for logging serdes cal status.
 * 
 * Create the file if required. Ensure the header information is written to start of the file.
 * 
 * On success return a FILE* to the opened file; On failure return NULL.
 */
static FILE* adrv904x_openSerdesCalStatusLogFile(const adi_adrv904x_GenericStrBuf_t* const filePath)
{
    FILE* filePtr = NULL;
    long filePos = 0;
        
#ifdef __GNUC__
    filePtr = ADI_LIBRARY_FOPEN((const char *)filePath->c_str, "a");
#else
    if (ADI_LIBRARY_FOPEN_S(&filePtr, (const char*)filePath->c_str, "a") != 0)
#endif
    
    if (filePtr == NULL)
    {
        return NULL;        
    }        
        
    filePos = ADI_LIBRARY_FTELL(filePtr);
                
    if (filePos == 0)
    {
        /* The file was created by fopen or was empty when opened; Write the header lines. */
        ADI_LIBRARY_FPRINTF(filePtr, "Per-lane Serdes initCal (ic) and trackingCal (tc) status\n");
        ADI_LIBRARY_FPRINTF(filePtr,
            "APIver: %d.%d.%d.%d\n",
            ADI_ADRV904X_CURRENT_MAJOR_VERSION,
            ADI_ADRV904X_CURRENT_MINOR_VERSION,
            ADI_ADRV904X_CURRENT_MAINTENANCE_VERSION,
            ADI_ADRV904X_CURRENT_BUILD_VERSION);
        ADI_LIBRARY_FPRINTF(filePtr,
            "laneId,\"ic\",msg,temperature,lpfIndex,ctleIndex,numEyes,bsum,bsum_dc,spoLeft,spoRight,eom,"
            "eomMax,pd,innerUp,innerDown,outerUp,outerDown,b[]\n");
        ADI_LIBRARY_FPRINTF(filePtr,
            "laneId,\"tc\",msg,temperature,pd[],dllPeakPd[],dllPeakPdDelta[],innerUp,innerDown,outerUp,outerDown,"
            "b[],ps[],yVector[]\n");
    }

    if (ferror(filePtr) != 0)
    {
        /* Open worked, write failed; Close file and return NULL. */
        ADI_LIBRARY_FCLOSE(filePtr);
        return NULL;
    }
    
    /* Success */
    return filePtr;    
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerdesInitCalStatusGet(adi_adrv904x_Device_t* const              device,
                                                                     const adi_adrv904x_GenericStrBuf_t* const filePath,
                                                                     const uint8_t                             laneNumber,
                                                                     const adi_adrv904x_GenericStrBuf_t* const msg,
                                                                     adi_adrv904x_SerdesInitCalStatus_t* const calStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    adrv904x_CpuCmd_SerdesCalStatusGet_t cpuCmd;
    adrv904x_SerdesInitCalStatusCmdResp_t cmdResp;
    FILE* filePtr = NULL;        
    uint32_t i = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calStatus, cleanup);
    /* filePath and msg may be NULL */
    
    ADI_LIBRARY_MEMSET(&cpuCmd, 0, sizeof(cpuCmd));
    ADI_LIBRARY_MEMSET(&cmdResp, 0, sizeof(cmdResp));
    
    if (laneNumber >= ADRV904X_JESD_MAX_DESERIALIZER_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, ADI_ADRV904X_ERR_ACT_CHECK_PARAM, laneNumber, "Invalid laneNumber value");
        goto cleanup;
    }
                
    /* Get the CPU that is responsible for the requested lane. */
    recoveryAction = adrv904x_CpuChannelMappingGet(device, (adi_adrv904x_Channels_e)(1U << laneNumber), ADRV904X_CPU_OBJID_IC_SERDES, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Lane to CPU lookup failed");
        goto cleanup;
    }

    /* Prepare the command payload */
    cpuCmd.lane = laneNumber;
    CTOH_STRUCT(adrv904x_CpuCmd_SerdesCalStatusGet, cpuCmd);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         cpuType,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_GET_SERDES_FG_METRICS,
                                         (void*)&cpuCmd,
                                         sizeof(cpuCmd),
                                         (void*)&cmdResp,
                                         sizeof(cmdResp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to call CPU cmd SERDES_TEST_GET_FG_METRICS");
        goto cleanup;
    }
    
    CTOH_STRUCT(cmdResp, adrv904x_SerdesInitCalStatusCmdResp);
    
    if (cmdStatus != ADRV904X_CPU_CMD_STATUS_NO_ERROR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;        
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdResp.cpuErrorCode,
                         "CPU cmd SERDES_TEST_GET_FG_METRICS failed");
        goto cleanup;
    }
        
    *calStatus = cmdResp.details;
    
    if (filePath == NULL || filePath->c_str[0] == '\0')
    {
        /* No log file has been supplied; We're done. */
        goto cleanup;
    }
    
    /* Log file supplied; Write data to it. */
    filePtr = adrv904x_openSerdesCalStatusLogFile(filePath);

    if (filePtr == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, filePath, "Unable to open serdes init cal status log file");
        goto cleanup;
    }
            
    /* File is open and header line, if required, has been written; Write data line.*/
    ADI_LIBRARY_FPRINTF(filePtr,
                        "%u,ic,%s,%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%d,%d,%d,%d,%d,[",
                        laneNumber,
                        msg ? (const char*) msg->c_str : "",
                        calStatus->temperature,
                        calStatus->lpfIndex,
                        calStatus->ctleIndex,
                        calStatus->numEyes,
                        calStatus->bsum,
                        calStatus->bsum_dc,
                        calStatus->spoLeft,
                        calStatus->spoRight,
                        calStatus->eom,
                        calStatus->eomMax,
                        calStatus->pd,
                        calStatus->innerUp,
                        calStatus->innerDown,
                        calStatus->outerUp,
                        calStatus->outerDown);
    
    for (i = 0U; i < NELEMS(calStatus->b); i++)
    {
        ADI_LIBRARY_FPRINTF(filePtr, "%d ", calStatus->b[i]);
    }
    ADI_LIBRARY_FPRINTF(filePtr, "]\n");
    ADI_LIBRARY_FFLUSH(filePtr);

    if (ADI_LIBRARY_FERROR(filePtr) != 0)
    { 
        /* Some part of write to file failed. */
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, filePath, "Failed to write to serdes init cal status log file");
        goto cleanup;
    }

cleanup:
    if (filePtr != NULL)
    {
        ADI_LIBRARY_FCLOSE(filePtr);
    }
    
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}                                                                   

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SerdesTrackingCalStatusGet(adi_adrv904x_Device_t* const                  device,
                                                                         const adi_adrv904x_GenericStrBuf_t* const     filePath,
                                                                         const uint8_t                                 laneNumber,
                                                                         const adi_adrv904x_GenericStrBuf_t* const     msg,
                                                                         adi_adrv904x_SerdesTrackingCalStatus_t* const calStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;
    adrv904x_CpuCmd_SerdesCalStatusGet_t cpuCmd;
    adrv904x_SerdesTrackingCalStatusCmdResp_t cmdResp;
    FILE* filePtr = NULL;        
    uint32_t i = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, calStatus, cleanup);
    /* msg and filePath may be NULL */
    
    ADI_LIBRARY_MEMSET(&cpuCmd, 0, sizeof(cpuCmd));
    ADI_LIBRARY_MEMSET(&cmdResp, 0, sizeof(cmdResp));

    if (laneNumber >= ADRV904X_JESD_MAX_DESERIALIZER_LANES)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, ADI_ADRV904X_ERR_ACT_CHECK_PARAM, laneNumber, "Invalid laneNumber value");
        goto cleanup;
    }
                
    /* Get the CPU that is responsible for the requested lane. */
    recoveryAction = adrv904x_CpuChannelMappingGet(device, (adi_adrv904x_Channels_e)(1U << laneNumber), ADRV904X_CPU_OBJID_IC_SERDES, &cpuType);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, laneNumber, "Lane to CPU lookup failed");
        goto cleanup;
    }

    /* Prepare the command payload */
    cpuCmd.lane = laneNumber;
    CTOH_STRUCT(adrv904x_CpuCmd_SerdesCalStatusGet, cpuCmd);

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(device,
                                         cpuType,
                                         ADRV904X_LINK_ID_0,
                                         ADRV904X_CPU_CMD_ID_GET_SERDES_BG_METRICS, //SERDES_TEST_GET_FG_METRICS && SERDES_TEST_GET_BG_METRICS
                                         (void*)&cpuCmd,
                                         sizeof(cpuCmd),
                                         (void*)&cmdResp,
                                         sizeof(cmdResp),
                                         &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to call CPU cmd SERDES_TEST_GET_BG_METRICS");        
        goto cleanup;
    }
    
    CTOH_STRUCT(cmdResp, adrv904x_SerdesTrackingCalStatusCmdResp);

    if (cmdStatus != ADRV904X_CPU_CMD_STATUS_NO_ERROR)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;        
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERRCODE_API,
                         recoveryAction,
                         cmdResp.cpuErrorCode,
                         "CPU cmd SERDES_TEST_GET_BG_METRICS failed");
        goto cleanup;
    }
    
    *calStatus = cmdResp.details;
    
    if (filePath == NULL || filePath->c_str[0] == '\0')
    {
        /* No log file has been supplied; We're done. */
        goto cleanup;
    }
    
    /* Log file supplied; Write data to it. */
    filePtr = adrv904x_openSerdesCalStatusLogFile(filePath);
    
    if (filePtr == NULL)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, filePath, "Unable to open serdes init cal status log file");
        goto cleanup;
    }
        
    /* File is open and header line, if required, has been written; Write data line.*/
    ADI_LIBRARY_FPRINTF(filePtr,
                        "%u,tc,%s,%d,[",
                        laneNumber,
                        msg ? (const char*) msg->c_str : "",
                        calStatus->temperature);
    
    for (i = 0U; i < NELEMS(calStatus->pd); i++)
    {
        ADI_LIBRARY_FPRINTF(filePtr, "%d ", calStatus->pd[i]);
    }            
    ADI_LIBRARY_FPRINTF(filePtr, "],[");
    
    for (i = 0U; i < NELEMS(calStatus->dllPeakPd); i++)
    {
        ADI_LIBRARY_FPRINTF(filePtr, "%d ", calStatus->dllPeakPd[i]);
    }                
    ADI_LIBRARY_FPRINTF(filePtr, "],[");
    
    for (i = 0U; i < NELEMS(calStatus->dllPeakPdDelta); i++)
    {
        ADI_LIBRARY_FPRINTF(filePtr, "%d ", calStatus->dllPeakPdDelta[i]);
    }                
    ADI_LIBRARY_FPRINTF(filePtr, "],%d,%d,%d,%d,[", calStatus->innerUp, calStatus->innerDown, calStatus->outerUp, calStatus->outerDown);
    
    for (i = 0U; i < NELEMS(calStatus->b); i++)
    {
        ADI_LIBRARY_FPRINTF(filePtr, "%d ", calStatus->b[i]);
    }
    ADI_LIBRARY_FPRINTF(filePtr, "],[");

    for (i = 0U; i < NELEMS(calStatus->ps); i++)
    {
        ADI_LIBRARY_FPRINTF(filePtr, "%f ", calStatus->ps[i]);
    }
    ADI_LIBRARY_FPRINTF(filePtr, "],[");

    for (i = 0U; i < NELEMS(calStatus->yVector); i++)
    {
        ADI_LIBRARY_FPRINTF(filePtr, "%f ", calStatus->yVector[i]);
    }    
    ADI_LIBRARY_FPRINTF(filePtr, "]\n");
    ADI_LIBRARY_FFLUSH(filePtr);

    if (ADI_LIBRARY_FERROR(filePtr) != 0)
    { 
        /* Write to file failed. */
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_FEATURE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, filePath, "Failed to write to tracking cal status log file");
        goto cleanup;
    }

cleanup:
    if (filePtr != NULL)
    {
        ADI_LIBRARY_FCLOSE(filePtr);
    }
    
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcModeSet(adi_adrv904x_Device_t* const     device,
                                                          const adi_adrv904x_FramerSel_e   framerSel,
                                                          const adi_adrv904x_CddcMode_e    cddcMode)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint8_t regValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check framer selection: only Framer 0 and 1 supported */
    if ((framerSel != ADI_ADRV904X_FRAMER_0)  &&
        (framerSel != ADI_ADRV904X_FRAMER_1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Get framer CDDC mode */
    recoveryAction = adrv904x_JesdCommon_CddcBypassMode_BfGet(device,
                                                              NULL,
                                                              commonBaseAddr,
                                                              &regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC bypass mode for the selected framer");
        goto cleanup;
    }

    if (cddcMode == ADI_ADRV904X_CDDC_BYPASS_MODE)
    {
        /* Enabled CDDC Bypass Mode for the selected framer */
        regValue |= framerSel;
    }
    else
    {
        /* Disabled CDDC Bypass Mode for the selected framer */
        regValue &= ~framerSel;
    }

    /* Set CDDC mode */
    recoveryAction = adrv904x_JesdCommon_CddcBypassMode_BfSet(device,
                                                              NULL,
                                                              commonBaseAddr,
                                                              regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDDC bypass mode for the selected framer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcModeGet(adi_adrv904x_Device_t* const     device,
                                                          const adi_adrv904x_FramerSel_e   framerSel,
                                                          adi_adrv904x_CddcMode_e * const  cddcMode)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint8_t regValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cddcMode, cleanup);

    /* Check framer selection */
    if ((framerSel & ~ADI_ADRV904X_ALL_FRAMERS) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    if (framerSel == ADI_ADRV904X_FRAMER_2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        *cddcMode = ADI_ADRV904X_CDDC_BYPASS_MODE;
    }
    else
    {
        /* Get framer CDDC mode for all framers */
        recoveryAction = adrv904x_JesdCommon_CddcBypassMode_BfGet(device,
            NULL,
            commonBaseAddr,
            &regValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC mode for the selected framer");
            goto cleanup;
        }  

        if ((regValue & framerSel) != 0) 
        {
            *cddcMode = ADI_ADRV904X_CDDC_BYPASS_MODE;
        }
        else 
        {
            *cddcMode = ADI_ADRV904X_CDDC_MODE;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducModeSet(adi_adrv904x_Device_t* const       device,
                                                          const adi_adrv904x_DeframerSel_e   deframerSel,
                                                          const adi_adrv904x_CducMode_e      cducMode)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint8_t regValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check deframer selection */
    if ((deframerSel & ~ADI_ADRV904X_ALL_DEFRAMER) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframer selection");
        goto cleanup;
    }

    /* Get deframer CDUC mode */
    recoveryAction = adrv904x_JesdCommon_CducBypassMode_BfGet(device,
                                                              NULL,
                                                              commonBaseAddr,
                                                              &regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC bypass mode for the selected deframer");
        goto cleanup;
    }

    if (cducMode == ADI_ADRV904X_CDUC_BYPASS_MODE)
    {
        /* Enabled CDUC Bypass Mode for the selected deframer */
        regValue |= deframerSel;
    }
    else
    {
        /* Disabled CDUC Bypass Mode for the selected deframer */
        regValue &= ~deframerSel;
    }

    /* Set CDUC mode */
    recoveryAction = adrv904x_JesdCommon_CducBypassMode_BfSet(device,
                                                              NULL,
                                                              commonBaseAddr,
                                                              regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDUC bypass mode for the selected deframer");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducModeGet(adi_adrv904x_Device_t* const       device,
                                                          const adi_adrv904x_DeframerSel_e   deframerSel,
                                                          adi_adrv904x_CducMode_e * const    cducMode)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint8_t regValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cducMode, cleanup);

    /* Check deframer selection */
    if ((deframerSel & ~ADI_ADRV904X_ALL_DEFRAMER) != 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframer selection");
        goto cleanup;
    }

    /* Get framer CDUC mode for all deframers */
    recoveryAction = adrv904x_JesdCommon_CducBypassMode_BfGet(device,
                                                              NULL,
                                                              commonBaseAddr,
                                                              &regValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC mode for the selected deframer");
        goto cleanup;
    }

    if ((regValue & deframerSel) != 0)
    {
        *cducMode = ADI_ADRV904X_CDUC_BYPASS_MODE;
    }
    else
    {
        *cducMode = ADI_ADRV904X_CDUC_MODE;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcSampleXBarSet(adi_adrv904x_Device_t* const                   device,
                                                                const adi_adrv904x_FramerSel_e                 framerSel,
                                                                const adi_adrv904x_RxChannels_e                rxChanSel,
                                                                const adi_adrv904x_CddcSampleXbarCfg_t * const cddcSampleXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cddcXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  regValue = 0U;
    uint8_t  xbarEntryValue = 0U;
    adi_adrv904x_ErrAction_e(*CddcSampleXBarBfSet)(adi_adrv904x_Device_t*,
                                                   adi_adrv904x_SpiCache_t* const,
                                                   const adrv904x_BfJesdCommonChanAddr_e,
                                                   uint8_t,
                                                   const uint8_t) = NULL;
    adi_adrv904x_ErrAction_e(*CddcSampleXBarConverterBfSet)(adi_adrv904x_Device_t* const,
                                                            adi_adrv904x_SpiCache_t* const,
                                                            const adrv904x_BfJesdCommonChanAddr_e,
                                                            uint8_t,
                                                            const uint8_t) = NULL;
    adi_adrv904x_ErrAction_e(*CddcSampleXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                            adi_adrv904x_SpiCache_t* const,
                                                            const adrv904x_BfJesdCommonChanAddr_e,
                                                            uint8_t,
                                                            uint8_t* const) = NULL;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cddcSampleXbarCfg, cleanup);

    /* Verify only one valid RX channel is selected */
    if ((rxChanSel == 0)                               || /* No RXchannel selected */
        ((rxChanSel & ~ADI_ADRV904X_RX_MASK_ALL) != 0) || /* non RX bits is set */
        ((rxChanSel & (rxChanSel - 1)) != 0))             /* more than 1 bit is set */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid RX channel selection");
        goto cleanup;
    }

    /* Check framer selection: only Framer 0 and 1 supported and assign sample xbar limit per framer */
    if (framerSel == ADI_ADRV904X_FRAMER_0)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX;
        CddcSampleXBarConverterBfSet = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet;
        CddcSampleXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet;

        switch ((uint32_t) rxChanSel)
        {
            case ADI_ADRV904X_RX0:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_RX1:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_RX2:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_RX3:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_RX4:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_RX5:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_RX6:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_RX7:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfSet;
                break;
        }
    }
    else if (framerSel == ADI_ADRV904X_FRAMER_1)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX / 2U; /* framer 1 has half of framer 0 */
        CddcSampleXBarConverterBfSet = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet;
        CddcSampleXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet;

        switch ((int32_t) rxChanSel)
        {
            case ADI_ADRV904X_RX0:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_RX1:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_RX2:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_RX3:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_RX4:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_RX5:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_RX6:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_RX7:
                CddcSampleXBarBfSet = adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfSet;
                break;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Verifying and setting CDDC Sample XBar for all entries */
    for (index = 0U; index < cddcXbarLimit; index++)
    {
        /* Read Xbar converter entry status */
        recoveryAction = CddcSampleXBarConverterBfGet(device,
                                                      NULL,
                                                      commonBaseAddr,
                                                      index,
                                                      &xbarEntryValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Sample XBar Converter index.");
            goto cleanup;
        }

        /* Verify sample xbar value before write */
        if (cddcSampleXbarCfg->cddcSampleXbar[index] == ADI_ADRV904X_XBAR_ENTRY_DISABLED)
        {
            /* Set the XBar Converter as disabled */
            regValue = ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED;
            recoveryAction = CddcSampleXBarConverterBfSet(device,
                                                          NULL,
                                                          commonBaseAddr,
                                                          index,
                                                          regValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disable CDDC Sample XBar Converter index.");
                goto cleanup;
            }
        }
        else if (cddcSampleXbarCfg->cddcSampleXbar[index] < ADI_ADRV904X_MAX_SAMPLE_XBAR_INDEX_VALUE)
        {
            /* Enable the Sample Xbar converter index */
            if (xbarEntryValue == ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED)
            {
                regValue = ADI_ADRV904X_XBAR_CONTROL_INDEX_ENABLED;
                recoveryAction = CddcSampleXBarConverterBfSet(device,
                                                              NULL,
                                                              commonBaseAddr,
                                                              index,
                                                              regValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Sample XBar Converter index.");
                    goto cleanup;
                }
            }

            /* Writing CDDC Sample Xbar Index value */
            recoveryAction = CddcSampleXBarBfSet(device,
                                                 NULL,
                                                 commonBaseAddr,
                                                 index,
                                                 cddcSampleXbarCfg->cddcSampleXbar[index]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDDC Sample XBar selected framer and Rx channel.");
                goto cleanup;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CDDC Sample Xbar configuration value.");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcSampleXBarGet(adi_adrv904x_Device_t* const             device,
                                                                const adi_adrv904x_FramerSel_e           framerSel,
                                                                const adi_adrv904x_RxChannels_e          rxChanSel,
                                                                adi_adrv904x_CddcSampleXbarCfg_t * const cddcSampleXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cddcXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  regValue = 0U;
    adi_adrv904x_ErrAction_e(*CddcSampleXBarBfGet)(adi_adrv904x_Device_t*,
                                                   adi_adrv904x_SpiCache_t* const,
                                                   const adrv904x_BfJesdCommonChanAddr_e,
                                                   uint8_t,
                                                   uint8_t* const) = NULL;

    adi_adrv904x_ErrAction_e(*CddcSampleXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                            adi_adrv904x_SpiCache_t* const,
                                                            const adrv904x_BfJesdCommonChanAddr_e,
                                                            uint8_t,
                                                            uint8_t* const) = NULL;
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cddcSampleXbarCfg, cleanup);

    /* Verify only one valid RX channel is selected */
    if ((rxChanSel == 0)                               || /* No RXchannel selected */
        ((rxChanSel & ~ADI_ADRV904X_RX_MASK_ALL) != 0) || /* non RX bits is set */
        ((rxChanSel & (rxChanSel - 1)) != 0))             /* more than 1 bit is set */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid RX channel selection");
        goto cleanup;
    }

    /* Check framer selection: only Framer 0 and 1 supported and assign values */
    if (framerSel == ADI_ADRV904X_FRAMER_0)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX;
        CddcSampleXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet;

        switch ((uint32_t) rxChanSel)
        {
            case ADI_ADRV904X_RX0:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx0ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_RX1:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx1ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_RX2:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx2ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_RX3:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx3ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_RX4:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx4ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_RX5:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx5ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_RX6:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx6ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_RX7:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx7ChSampleSelLink0_BfGet;
                break;
        }
    }
    else if (framerSel == ADI_ADRV904X_FRAMER_1)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX / 2U; /* framer 1 has half of framer 0 */
        CddcSampleXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet;

        switch ((int32_t) rxChanSel)
        {
            case ADI_ADRV904X_RX0:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx0ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_RX1:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx1ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_RX2:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx2ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_RX3:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx3ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_RX4:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx4ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_RX5:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx5ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_RX6:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx6ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_RX7:
                CddcSampleXBarBfGet = adrv904x_JesdCommon_Rx7ChSampleSelLink1_BfGet;
                break;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Getting CDDC Sample XBar for all entries */
    for (index = 0U; index < cddcXbarLimit; index++)
    {
        /* Get the CDDC Sample XBar Converter status first */
        recoveryAction = CddcSampleXBarConverterBfGet(device,
                                                      NULL,
                                                      commonBaseAddr,
                                                      index,
                                                      &regValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Sample XBar Converter index.");
            goto cleanup;
        }

        if (regValue == ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED)
        {
            /* The CDDC Sample XBar Converter status is disabled so assign disable value to the index. */
            cddcSampleXbarCfg->cddcSampleXbar[index] = ADI_ADRV904X_XBAR_ENTRY_DISABLED;
        }
        else
        {
            /* The CDDC Sample XBar Converter status is enabled so read value from register */
            recoveryAction = CddcSampleXBarBfGet(device,
                                                 NULL,
                                                 commonBaseAddr,
                                                 index,
                                                 &regValue);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Sample XBar selected framer and Rx channel.");
                goto cleanup;
            }

            cddcSampleXbarCfg->cddcSampleXbar[index] = regValue;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducSampleXBarSet(adi_adrv904x_Device_t* const                   device,
                                                                const adi_adrv904x_DeframerSel_e               deframerSel,
                                                                const adi_adrv904x_TxChannels_e                txChanSel,
                                                                const adi_adrv904x_CducSampleXbarCfg_t * const cducSampleXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cducXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  regValue = 0U;
    uint8_t  xbarEntryValue = 0U;

    adi_adrv904x_ErrAction_e(*CducSampleXBarBfSet)(adi_adrv904x_Device_t*,
                                                   adi_adrv904x_SpiCache_t* const,
                                                   const adrv904x_BfJesdCommonChanAddr_e,
                                                   uint8_t,
                                                   const uint8_t) = NULL;
    adi_adrv904x_ErrAction_e(*CducSampleXBarConverterBfSet)(adi_adrv904x_Device_t* const,
                                                            adi_adrv904x_SpiCache_t* const,
                                                            const adrv904x_BfJesdCommonChanAddr_e,
                                                            uint8_t,
                                                            const uint8_t) = NULL;
    adi_adrv904x_ErrAction_e(*CducSampleXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                            adi_adrv904x_SpiCache_t* const,
                                                            const adrv904x_BfJesdCommonChanAddr_e,
                                                            uint8_t,
                                                            uint8_t* const) = NULL;
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cducSampleXbarCfg, cleanup);

    /* Verify only one valid TX channel is selected */
    if ((txChanSel == 0)                         ||        /* No TXchannel selected */
        ((txChanSel & ~ADI_ADRV904X_TXALL) != 0) ||        /* non TX bits is set */
        ((txChanSel & (txChanSel - 1)) != 0))              /* more than 1 bit is set */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid TX channel selection");
        goto cleanup;
    }

    /* Check deframer selection and assign sample xbar limit per deframer */
    if (deframerSel == ADI_ADRV904X_DEFRAMER_0)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX;
        CducSampleXBarConverterBfSet = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet;
        CducSampleXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet;

        switch ((uint32_t) txChanSel)
        {
            case ADI_ADRV904X_TX0:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_TX1:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_TX2:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_TX3:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_TX4:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_TX5:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_TX6:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfSet;
                break;
            case ADI_ADRV904X_TX7:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfSet;
                break;
        }
    }
    else if (deframerSel == ADI_ADRV904X_DEFRAMER_1)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX / 2U; /* deframer 1 has half of deframer 0 */
        CducSampleXBarConverterBfSet = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet;
        CducSampleXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet;

        switch ((int32_t) txChanSel)
        {
            case ADI_ADRV904X_TX0:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_TX1:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_TX2:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_TX3:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_TX4:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_TX5:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_TX6:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfSet;
                break;
            case ADI_ADRV904X_TX7:
                CducSampleXBarBfSet = adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfSet;
                break;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframer selection");
        goto cleanup;
    }

    /* Verifying and setting CDUC Sample XBar for all entries */
    for (index = 0U; index < cducXbarLimit; index++)
    {
        /* Read Xbar converter index status */
        recoveryAction = CducSampleXBarConverterBfGet(device,
                                                      NULL,
                                                      commonBaseAddr,
                                                      index,
                                                      &xbarEntryValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Sample XBar Converter index.");
            goto cleanup;
        }

        /* Verify sample xbar value before write */
        if (cducSampleXbarCfg->cducSampleXbar[index] == ADI_ADRV904X_XBAR_ENTRY_DISABLED)
        {
            /* Set the XBar Converter as disabled */
            regValue = ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED;
            recoveryAction = CducSampleXBarConverterBfSet(device,
                                                          NULL,
                                                          commonBaseAddr,
                                                          index,
                                                          regValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disable CDUC Sample XBar Converter index.");
                goto cleanup;
            }
        }
        else if (cducSampleXbarCfg->cducSampleXbar[index] < ADI_ADRV904X_MAX_SAMPLE_XBAR_INDEX_VALUE)
        {
            /* Enable the Sample Xbar converter index if it is disabled */
            if (xbarEntryValue == ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED)
            {
                regValue = ADI_ADRV904X_XBAR_CONTROL_INDEX_ENABLED;
                recoveryAction = CducSampleXBarConverterBfSet(device,
                                                              NULL,
                                                              commonBaseAddr,
                                                              index,
                                                              regValue);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Sample XBar Converter index.");
                    goto cleanup;
                }
            }

            /* Writing CDUC Sample Xbar Index value */
            recoveryAction = CducSampleXBarBfSet(device,
                                                 NULL,
                                                 commonBaseAddr,
                                                 index,
                                                 cducSampleXbarCfg->cducSampleXbar[index]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDUC Sample XBar selected deframer and Tx channel.");
                goto cleanup;
            }
        }
        else
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CDUC Sample Xbar configuration value.");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducSampleXBarGet(adi_adrv904x_Device_t* const             device,
                                                                const adi_adrv904x_DeframerSel_e         deframerSel,
                                                                const adi_adrv904x_TxChannels_e          txChanSel,
                                                                adi_adrv904x_CducSampleXbarCfg_t * const cducSampleXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cducXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  regValue = 0U;
    adi_adrv904x_ErrAction_e(*CducSampleXBarBfGet)(adi_adrv904x_Device_t*,
                                                   adi_adrv904x_SpiCache_t* const,
                                                   const adrv904x_BfJesdCommonChanAddr_e,
                                                   uint8_t,
                                                   uint8_t* const) = NULL;
    adi_adrv904x_ErrAction_e(*CducSampleXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                            adi_adrv904x_SpiCache_t* const,
                                                            const adrv904x_BfJesdCommonChanAddr_e,
                                                            uint8_t,
                                                            uint8_t* const) = NULL;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cducSampleXbarCfg, cleanup);

    /* Verify only one valid TX channel is selected */
    if ((txChanSel == 0)                         ||        /* No TXchannel selected */
        ((txChanSel & ~ADI_ADRV904X_TXALL) != 0) ||        /* non TX bits is set */
        ((txChanSel & (txChanSel - 1)) != 0))              /* more than 1 bit is set */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid TX channel selection");
        goto cleanup;
    }

    /* Check deframer selection and assign values */
    if (deframerSel == ADI_ADRV904X_DEFRAMER_0)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX;
        CducSampleXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet;

        switch ((uint32_t) txChanSel)
        {
            case ADI_ADRV904X_TX0:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx0ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_TX1:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx1ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_TX2:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx2ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_TX3:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx3ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_TX4:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx4ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_TX5:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx5ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_TX6:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx6ChSampleSelLink0_BfGet;
                break;
            case ADI_ADRV904X_TX7:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx7ChSampleSelLink0_BfGet;
                break;
        }
    }
    else if (deframerSel == ADI_ADRV904X_DEFRAMER_1)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX / 2U; /* deframer 1 has half of deframer 0 */
        CducSampleXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet;

        switch ((int32_t) txChanSel)
        {
            case ADI_ADRV904X_TX0:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx0ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_TX1:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx1ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_TX2:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx2ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_TX3:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx3ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_TX4:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx4ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_TX5:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx5ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_TX6:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx6ChSampleSelLink1_BfGet;
                break;
            case ADI_ADRV904X_TX7:
                CducSampleXBarBfGet = adrv904x_JesdCommon_Tx7ChSampleSelLink1_BfGet;
                break;
        }
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframer selection");
        goto cleanup;
    }

    /* Getting CDUC Sample XBar for all entries */
    for (index = 0U; index < cducXbarLimit; index++)
    {
        /* Get the CDUC Sample XBar Converter status first */
        recoveryAction = CducSampleXBarConverterBfGet(device,
                                                      NULL,
                                                      commonBaseAddr,
                                                      index,
                                                      &regValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Sample XBar Converter index.");
            goto cleanup;
        }

        if (regValue == ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED)
        {
            /* The CDUC Sample XBar Converter status is disabled so assign disable value to the index. */
            cducSampleXbarCfg->cducSampleXbar[index] = ADI_ADRV904X_XBAR_ENTRY_DISABLED;
        }
        else
        {
            /* The CDUC Sample XBar Converter status is enabled so read value from register */
            recoveryAction = CducSampleXBarBfGet(device,
                                                 NULL,
                                                 commonBaseAddr,
                                                 index,
                                                 &regValue);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Sample XBar selected deframer and Tx channel.");
                goto cleanup;
            }

            cducSampleXbarCfg->cducSampleXbar[index] = regValue;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcRxChanXBarSet(adi_adrv904x_Device_t* const                    device,
                                                                const adi_adrv904x_FramerSel_e                  framerSel,
                                                                const adi_adrv904x_CddcChannelXbarCfg_t * const cddcChannelXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cddcXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  channelXbarEntry = 0U;
    adi_adrv904x_ErrAction_e(*CddcJtxChannelXBarBfSet)(adi_adrv904x_Device_t*,
                                                       adi_adrv904x_SpiCache_t* const,
                                                       const adrv904x_BfJesdCommonChanAddr_e,
                                                       uint8_t,
                                                       const uint8_t) = NULL;

    adi_adrv904x_ErrAction_e(*CddcXBarConverterBfSet)(adi_adrv904x_Device_t* const,
                                                      adi_adrv904x_SpiCache_t* const,
                                                      const adrv904x_BfJesdCommonChanAddr_e,
                                                      uint8_t,
                                                      const uint8_t) = NULL;

    adi_adrv904x_ErrAction_e(*CddcXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                      adi_adrv904x_SpiCache_t* const,
                                                      const adrv904x_BfJesdCommonChanAddr_e,
                                                      uint8_t,
                                                      uint8_t* const) = NULL;
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cddcChannelXbarCfg, cleanup);

    /* Check framer selection: only Framer 0 and 1 supported and assign channel xbar limit per framer */
    if (framerSel == ADI_ADRV904X_FRAMER_0)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX;
        CddcJtxChannelXBarBfSet = adrv904x_JesdCommon_JtxChannelSelLink0_BfSet;
        CddcXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet;
        CddcXBarConverterBfSet = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfSet;
    }
    else if (framerSel == ADI_ADRV904X_FRAMER_1)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX / 2U; /* framer 1 has half of framer 0 */
        CddcJtxChannelXBarBfSet = adrv904x_JesdCommon_JtxChannelSelLink1_BfSet;
        CddcXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet;
        CddcXBarConverterBfSet = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfSet;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Verifying and setting CDDC RX Channel XBar for all entries */
    for (index = 0U; index < cddcXbarLimit; index++)
    {
        /* Read entry status */
        recoveryAction = CddcXBarConverterBfGet(device,
                                                NULL,
                                                commonBaseAddr,
                                                index,
                                                &channelXbarEntry);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC XBar Converter index.");
            goto cleanup;
        }

        /* Verify channel xbar value before write */
        if (cddcChannelXbarCfg->cddcChannelXbar[index] == ADI_ADRV904X_XBAR_ENTRY_DISABLED)
        {
            /* The value indicates the entry is to disable so disable the entry */
            recoveryAction = CddcXBarConverterBfSet(device,
                                                    NULL,
                                                    commonBaseAddr,
                                                    index,
                                                    ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC XBar Converter index.");
                goto cleanup;
            }
        }
        else if (cddcChannelXbarCfg->cddcChannelXbar[index] > ADI_ADRV904X_MAX_CHANNEL_XBAR_VALUE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid RX CDDC Channel Crossbar value.");
            goto cleanup;
        }
        else  /* Valid value */
        {
            /* Enable the entry if it is disable. */
            if (channelXbarEntry == ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED)
            {
                recoveryAction = CddcXBarConverterBfSet(device,
                                                        NULL,
                                                        commonBaseAddr,
                                                        index,
                                                        ADI_ADRV904X_XBAR_CONTROL_INDEX_ENABLED);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDDC XBar Converter index.");
                    goto cleanup;
                }
            }

            /* Write value */
            recoveryAction = CddcJtxChannelXBarBfSet(device,
                                                     NULL,
                                                     commonBaseAddr,
                                                     index,
                                                     cddcChannelXbarCfg->cddcChannelXbar[index]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDDC Channel XBar selected framer.");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CddcRxChanXBarGet(adi_adrv904x_Device_t* const             device,
                                                                const adi_adrv904x_FramerSel_e           framerSel,
                                                                adi_adrv904x_CddcChannelXbarCfg_t * const cddcChannelXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cddcXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  regValue = 0U;
    adi_adrv904x_ErrAction_e(*CddcJtxChannelXBarBfGet)(adi_adrv904x_Device_t*,
                                                       adi_adrv904x_SpiCache_t* const,
                                                       const adrv904x_BfJesdCommonChanAddr_e,
                                                       uint8_t,
                                                       uint8_t* const) = NULL;

    adi_adrv904x_ErrAction_e(*CddcXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                      adi_adrv904x_SpiCache_t* const,
                                                      const adrv904x_BfJesdCommonChanAddr_e,
                                                      uint8_t,
                                                      uint8_t* const) = NULL;
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cddcChannelXbarCfg, cleanup);

    /* Check framer selection: only Framer 0 and 1 supported and assign values */
    if (framerSel == ADI_ADRV904X_FRAMER_0)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX;
        CddcXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink0_BfGet;
        CddcJtxChannelXBarBfGet = adrv904x_JesdCommon_JtxChannelSelLink0_BfGet;
    }
    else if (framerSel == ADI_ADRV904X_FRAMER_1)
    {
        cddcXbarLimit = ADI_ADRV904X_MAX_FRM_CARRIER_XBAR_IDX / 2U; /* framer 1 has half of framer 0 */
        CddcXBarConverterBfGet = adrv904x_JesdCommon_JtxCoreConvDisableLink1_BfGet;
        CddcJtxChannelXBarBfGet = adrv904x_JesdCommon_JtxChannelSelLink1_BfGet;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection");
        goto cleanup;
    }

    /* Getting CDDC Sample XBar for all entries */
    for (index = 0U; index < cddcXbarLimit; index++)
    {
        /* Get the CDDC Sample XBar Converter status first */
        recoveryAction = CddcXBarConverterBfGet(device,
                                                NULL,
                                                commonBaseAddr,
                                                index,
                                                &regValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Crossbar Converter index.");
            goto cleanup;
        }

        if (regValue == ADI_ADRV904X_XBAR_CONTROL_INDEX_ENABLED)
        {
            /* The CDDC Sample XBar Converter status is enabled so read value from register */
            recoveryAction = CddcJtxChannelXBarBfGet(device,
                                                     NULL,
                                                     commonBaseAddr,
                                                     index,
                                                     &regValue);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDDC Sample XBar selected framer and Rx channel.");
                goto cleanup;
            }

            cddcChannelXbarCfg->cddcChannelXbar[index] = regValue;
        }
        else
        {
            cddcChannelXbarCfg->cddcChannelXbar[index] = ADI_ADRV904X_XBAR_ENTRY_DISABLED;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducTxChanXBarSet(adi_adrv904x_Device_t* const                   device,
                                                                const adi_adrv904x_DeframerSel_e               deframerSel,
                                                                const adi_adrv904x_CducChannelXbarCfg_t  * const cducChannelXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cducXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  channelXbarEntry = 0U;

    adi_adrv904x_ErrAction_e(*CducJrxXBarBfSet)(adi_adrv904x_Device_t*,
                                                adi_adrv904x_SpiCache_t* const,
                                                const adrv904x_BfJesdCommonChanAddr_e,
                                                uint8_t,
                                                const uint8_t) = NULL;

    adi_adrv904x_ErrAction_e(*CducXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                      adi_adrv904x_SpiCache_t* const,
                                                      const adrv904x_BfJesdCommonChanAddr_e,
                                                      uint8_t,
                                                      uint8_t* const) = NULL;

    adi_adrv904x_ErrAction_e(*CducXBarConverterBfSet)(adi_adrv904x_Device_t* const,
                                                      adi_adrv904x_SpiCache_t* const,
                                                      const adrv904x_BfJesdCommonChanAddr_e,
                                                      uint8_t,
                                                      const uint8_t) = NULL;
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cducChannelXbarCfg, cleanup);

    /* Check deframer selection and assign channel xbar limit per deframer */
    if (deframerSel == ADI_ADRV904X_DEFRAMER_0)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX;
        CducJrxXBarBfSet = adrv904x_JesdCommon_JrxChannelSelLink0_BfSet;
        CducXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet;
        CducXBarConverterBfSet = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfSet;
    }
    else if (deframerSel == ADI_ADRV904X_DEFRAMER_1)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX / 2U; /* deframer 1 has half of deframer 0 */
        CducJrxXBarBfSet = adrv904x_JesdCommon_JrxChannelSelLink1_BfSet;
        CducXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet;
        CducXBarConverterBfSet = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfSet;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframer selection");
        goto cleanup;
    }

    /* Verifying and setting CDUC Channel XBar for all entries */
    for (index = 0U; index < cducXbarLimit; index++)
    {
        /* Read the CDUC channel xbar entry */
        recoveryAction = CducXBarConverterBfGet(device,
                                                NULL,
                                                commonBaseAddr,
                                                index,
                                                &channelXbarEntry);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC XBar Converter index.");
            goto cleanup;
        }

        /* Verify Channel xbar value before write */
        if (cducChannelXbarCfg->cducChannelXbar[index] == ADI_ADRV904X_XBAR_ENTRY_DISABLED)
        {
            /* The value indicates the entry is to disable so disable the entry */
            recoveryAction = CducXBarConverterBfSet(device,
                                                    NULL,
                                                    commonBaseAddr,
                                                    index,
                                                    ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDUC XBar Converter index.");
                goto cleanup;
            }
        }
        else if (cducChannelXbarCfg->cducChannelXbar[index] > ADI_ADRV904X_MAX_CHANNEL_XBAR_VALUE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid CDUC Channel Xbar configuration value.");
            goto cleanup;
        }
        else /* valid value */
        {
            /* Enable xbar entry if it is disable */
            if (channelXbarEntry == ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED)
            {
                recoveryAction = CducXBarConverterBfSet(device,
                                                        NULL,
                                                        commonBaseAddr,
                                                        index,
                                                        ADI_ADRV904X_XBAR_CONTROL_INDEX_ENABLED);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDUC XBar Converter index.");
                    goto cleanup;
                }
            }

            /* Writing CDUC Sample Xbar Index value */
            recoveryAction = CducJrxXBarBfSet(device,
                                              NULL,
                                              commonBaseAddr,
                                              index,
                                              cducChannelXbarCfg->cducChannelXbar[index]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting CDUC Channel XBar selected deframer.");
                goto cleanup;
            }
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CducTxChanXBarGet(adi_adrv904x_Device_t* const             device,
                                                                const adi_adrv904x_DeframerSel_e         deframerSel,
                                                                adi_adrv904x_CducChannelXbarCfg_t  * const cducChannelXbarCfg)
{
        adi_adrv904x_ErrAction_e     recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    uint32_t cducXbarLimit = 0U;
    uint32_t index = 0U;
    uint8_t  regValue = 0U;
    adi_adrv904x_ErrAction_e(*CducJrxXBarBfGet)(adi_adrv904x_Device_t*,
                                                adi_adrv904x_SpiCache_t* const,
                                                const adrv904x_BfJesdCommonChanAddr_e,
                                                uint8_t,
                                                uint8_t* const) = NULL;

    adi_adrv904x_ErrAction_e(*CducXBarConverterBfGet)(adi_adrv904x_Device_t* const,
                                                      adi_adrv904x_SpiCache_t* const,
                                                      const adrv904x_BfJesdCommonChanAddr_e,
                                                      uint8_t,
                                                      uint8_t* const) = NULL;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, cducChannelXbarCfg, cleanup);

    /* Check deframer selection and assign values */
    if (deframerSel == ADI_ADRV904X_DEFRAMER_0)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX;
        CducJrxXBarBfGet = adrv904x_JesdCommon_JrxChannelSelLink0_BfGet;
        CducXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink0_BfGet;
    }
    else if (deframerSel == ADI_ADRV904X_DEFRAMER_1)
    {
        cducXbarLimit = ADI_ADRV904X_MAX_DFRM_CARRIER_XBAR_IDX / 2U; /* deframer 1 has half of deframer 0 */
        CducJrxXBarBfGet = adrv904x_JesdCommon_JrxChannelSelLink1_BfGet;
        CducXBarConverterBfGet = adrv904x_JesdCommon_JrxCoreConvDisableLink1_BfGet;
    }
    else
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid deframer selection");
        goto cleanup;
    }

    /* Getting CDUC Sample XBar for all entries */
    for (index = 0U; index < cducXbarLimit; index++)
    {
        /* Get the CDUC XBar Converter status first */
        recoveryAction = CducXBarConverterBfGet(device,
                                                NULL,
                                                commonBaseAddr,
                                                index,
                                                &regValue);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Channel XBar Converter index.");
            goto cleanup;
        }

        if (regValue == ADI_ADRV904X_XBAR_CONTROL_INDEX_DISABLED)
        {
            /* The CDUC Sample XBar Converter status is disabled so assign disable value to the index. */
            cducChannelXbarCfg->cducChannelXbar[index] = ADI_ADRV904X_XBAR_ENTRY_DISABLED;
        }
        else
        {
            /* The CDUC Channel XBar Converter status is enabled so read value from register */
            recoveryAction = CducJrxXBarBfGet(device,
                                              NULL,
                                              commonBaseAddr,
                                              index,
                                              &regValue);

            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting CDUC Channel XBar selected deframer.");
                goto cleanup;
            }

            cducChannelXbarCfg->cducChannelXbar[index] = regValue;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAdcSampleXbarSet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSel,
                                                                  const adi_adrv904x_OrxAdcSampleXbarCfg_t* const orxAdcXbar)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                      OrxAdcIdx = 0U;
    uint8_t                      validAdc = ADI_FALSE;
    uint32_t                     i = 0U;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;
    adi_adrv904x_OrxAdcSampleXbarSel_e validOrxAdcXbar[] =
    {
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_0,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_1,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_2,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_3,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_4,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_5,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_6,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_I_7,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_0,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_1,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_2,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_3,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_4,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_5,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_6,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX0_DATA_Q_7,

        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_0,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_1,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_2,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_3,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_4,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_5,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_6,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_I_7,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_0,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_1,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_2,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_3,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_4,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_5,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_6,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_ORX1_DATA_Q_7,
        ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_INVALID
    };

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orxAdcXbar, cleanup);

    /* Only Framer 2 is supported in this API */
    if (framerSel != ADI_ADRV904X_FRAMER_2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection.");
        goto cleanup;
    }

    /* ADC Xbar values validation */
    for (OrxAdcIdx = 0; OrxAdcIdx < ADI_ADRV904X_NUM_ORX_ADC_SAMPLE_XBAR; OrxAdcIdx++)
    {
        validAdc = ADI_FALSE;
        for (i = 0; i < (sizeof(validOrxAdcXbar) / sizeof(*validOrxAdcXbar)); i++)
        {
            if (orxAdcXbar->OrxAdcSampleXbar[OrxAdcIdx] != validOrxAdcXbar[i])
            {
                continue;
            }
            else
            {
                validAdc = ADI_TRUE;
                break;
            }
        }

        /* Expect validAdc to be true (1U) */
        if (validAdc == ADI_FALSE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid ORX ADC Sample Crossbar selection");
            goto cleanup;
        }
    }

    /* program the converters */
    for (OrxAdcIdx = 0; OrxAdcIdx < ADI_ADRV904X_NUM_ORX_ADC_SAMPLE_XBAR; OrxAdcIdx++)
    {
        if (orxAdcXbar->OrxAdcSampleXbar[OrxAdcIdx] == ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_INVALID)
        {
            /* Disable the converters */
            recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfSet(device,
                                                                               NULL,
                                                                               commonBaseAddr,
                                                                               OrxAdcIdx,
                                                                               1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting orx sample crossbar for the selected framer");
                goto cleanup;
            }
        }
        else
        {
            /* Enabling converters */
            recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfSet(device,
                                                                               NULL,
                                                                               commonBaseAddr,
                                                                               OrxAdcIdx,
                                                                               0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while enabling converter for the selected framer");
                goto cleanup;
            }

            /* Writing ORX Sample Xbar value  */
            recoveryAction = adrv904x_JesdCommon_OrxSampleSelLink2_BfSet(device,
                                                                         NULL,
                                                                         commonBaseAddr,
                                                                         OrxAdcIdx,
                                                                         (uint8_t) orxAdcXbar->OrxAdcSampleXbar[OrxAdcIdx]);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting orx sample crossbar value for the selected framer");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxAdcSampleXbarGet(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_FramerSel_e  framerSel,
                                                                  adi_adrv904x_OrxAdcSampleXbarCfg_t* const orxAdcXbar)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                      xbarIdx     = 0U;
    uint8_t                      bfValue     = 0U;
    uint8_t                      adcValue     = 0U;
    adrv904x_BfJesdCommonChanAddr_e commonBaseAddr = ADRV904X_BF_DIGITAL_CORE_JESD_JESD_COMMON;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orxAdcXbar, cleanup);

    /* Only Framer 2 is supported in this API */
    if (framerSel != ADI_ADRV904X_FRAMER_2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid framer selection.");
        goto cleanup;
    }

    for (xbarIdx = 0U; xbarIdx < ADI_ADRV904X_NUM_ORX_ADC_SAMPLE_XBAR; xbarIdx++)
    {
        /* Get this status of converters */
        recoveryAction = adrv904x_JesdCommon_JtxCoreConvDisableLink2_BfGet(device,
                                                                           NULL,
                                                                           commonBaseAddr,
                                                                           xbarIdx,
                                                                           &bfValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting converter status for the selected framer");
            goto cleanup;
        }

        if (bfValue == 1U) /* Converter is in disable */
        {
            orxAdcXbar->OrxAdcSampleXbar[xbarIdx] = ADI_ADRV904X_JESD_ADC_SAMPLE_XBAR_INVALID;
        }
        else /* Converter is in enable */
        {
            /* Getting Sample Xbar value */
            recoveryAction = adrv904x_JesdCommon_OrxSampleSelLink2_BfGet(device,
                                                                         NULL,
                                                                         commonBaseAddr,
                                                                         xbarIdx,
                                                                         &adcValue);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting sample crossbar value for the selected framer");
                goto cleanup;
            }
            orxAdcXbar->OrxAdcSampleXbar[xbarIdx] = (adi_adrv904x_OrxAdcSampleXbarSel_e) adcValue;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
