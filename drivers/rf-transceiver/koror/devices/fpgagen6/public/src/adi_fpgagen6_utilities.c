/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_utilities.c
* \brief Contains FPGAGEN6 API configuration and run-time type definitions for utility functions
*
* FPGA API Version: 2.9.0.4
*/
#include "adi_fpgagen6_utilities.h"
#include "adi_fpgagen6_hal.h"
#include "adi_fpgagen6_rx.h"
#include "adi_fpgagen6_tx.h"
#include "adi_fpgagen6_clk.h"
#include "adi_fpgagen6_data_interface.h"
#include "adi_fpgagen6_tdd.h"

#include "../../private/include/fpgagen6_data_interface.h"
#include "../../private/include/fpgagen6_drp.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_UTILITIES
/**
 * /brief  Configures MMCM and checks for lock
 */
static adi_fpgagen6_ErrAction_e fpgagen6_MmcmConfigureAndLockCheck( adi_fpgagen6_Device_t* const        device,
                                                                    const adi_fpgagen6_MmcmSel_e        mmcmSel,
                                                                    const adi_fpgagen6_MmcmCfg_t* const mmcmConfig);

/**
* /brief  Deserializer Config
*/
static adi_fpgagen6_ErrAction_e fpgagen6_DeserializerCfg(   adi_fpgagen6_Device_t* const    device,
                                                            adi_fpgagen6_Init_t* const      fpgaInit,
                                                            const uint8_t                   fpgaLane,
                                                            const uint8_t                   dVal,
                                                            const uint32_t                  refClk_kHz);

/**
* /brief  Serializer Config
*/
static adi_fpgagen6_ErrAction_e fpgagen6_SerializerCfg( adi_fpgagen6_Device_t* const    device,
                                                        adi_fpgagen6_Init_t* const      fpgaInit,
                                                        const uint8_t                   fpgaLane,
                                                        const uint8_t                   dVal,
                                                        const uint32_t                  refClk_kHz);

static adi_fpgagen6_ErrAction_e fpgagen6_MmcmConfigureAndLockCheck( adi_fpgagen6_Device_t* const        device,
                                                                    const adi_fpgagen6_MmcmSel_e        mmcmSel,
                                                                    const adi_fpgagen6_MmcmCfg_t* const mmcmConfig)
{
    adi_fpgagen6_ErrAction_e    recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     mmcmLockStatus      = 0U;
    uint32_t                    mmcmLockCheckIdx    = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, mmcmConfig);

    recoveryAction = adi_fpgagen6_MmcmResetSet(device, mmcmSel);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_MMCM_CONFIGURE_LOCK,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    recoveryAction = adi_fpgagen6_MmcmCfgSet(device, mmcmSel, mmcmConfig);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_MMCM_CONFIGURE_LOCK,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    mmcmLockStatus = 0U;

    for (mmcmLockCheckIdx = 0U; mmcmLockCheckIdx < ADI_FPGAGEN6_MMCM_LOCK_TIMEOUT; ++mmcmLockCheckIdx)
    {
        recoveryAction = adi_fpgagen6_MmcmLockGet(device, mmcmSel, &mmcmLockStatus);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_MMCM_CONFIGURE_LOCK,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }

        if (mmcmLockStatus != 0U)
        {
            break;
        }

        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_MMCM_CONFIGURE_LOCK,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction);
        }
    }

    if (mmcmLockStatus == 0U)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_MMCM_CONFIGURE_LOCK,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    return recoveryAction;
}

static adi_fpgagen6_ErrAction_e fpgagen6_DeserializerCfg(   adi_fpgagen6_Device_t* const    device,
                                                            adi_fpgagen6_Init_t* const      fpgaInit,
                                                            const uint8_t                   fpgaLane,
                                                            const uint8_t                   dVal,
                                                            const uint32_t                  refClk_kHz)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    //todo: put back in when DRP setup is put in
    uint32_t laneRate_kHz = (fpgaInit->clocks.qpll.qpllClockSettings.qpllVcoOut_kHz * ADI_FPGAGEN6_QPLL_DDC_MULTIPLIER) / dVal;
    (void)refClk_kHz;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, fpgaInit);

    fpgaInit->datapath.desCfg[fpgaLane].commonCfg.pllClkSel     = fpgaInit->clocks.qpll.qpllClockSettings.laneClkSel;
    fpgaInit->datapath.desCfg[fpgaLane].commonCfg.polarity      = fpgaInit->datapath.desLanePnInvert[fpgaLane];
    fpgaInit->datapath.desCfg[fpgaLane].commonCfg.powerDown     = 0U;
    fpgaInit->datapath.desCfg[fpgaLane].commonCfg.laneRate_kHz  = laneRate_kHz;
    fpgaInit->datapath.desCfg[fpgaLane].rxOutClkSel             = 0x5U;

    if (laneRate_kHz > ADI_FPGAGEN6_16_GHZ)
    {
        fpgaInit->datapath.desCfg[fpgaLane].rxDfeLpmEn = 0U;
    }
    else
    {
        fpgaInit->datapath.desCfg[fpgaLane].rxDfeLpmEn = 1U;
    }

    if(device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204B)
    {
        fpgaInit->datapath.desCfg[fpgaLane].commonCfg.encoder8b10bEnable = 1U;
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204C)
    {
        fpgaInit->datapath.desCfg[fpgaLane].commonCfg.encoder8b10bEnable = 0U;
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }

    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_DESERIALIZER_CFG,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    return recoveryAction;
}

static adi_fpgagen6_ErrAction_e fpgagen6_SerializerCfg( adi_fpgagen6_Device_t* const    device,
                                                        adi_fpgagen6_Init_t* const      fpgaInit,
                                                        const uint8_t                   fpgaLane,
                                                        const uint8_t                   dVal,
                                                        const uint32_t                  refClk_kHz)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t laneRate_kHz = (fpgaInit->clocks.qpll.qpllClockSettings.qpllVcoOut_kHz * ADI_FPGAGEN6_QPLL_DDC_MULTIPLIER) / dVal;
    (void)refClk_kHz;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, fpgaInit);

    fpgaInit->datapath.serCfg[fpgaLane].commonCfg.pllClkSel     = fpgaInit->clocks.qpll.qpllClockSettings.laneClkSel;
    fpgaInit->datapath.serCfg[fpgaLane].commonCfg.polarity      = fpgaInit->datapath.serLanePnInvert[fpgaLane];
    fpgaInit->datapath.serCfg[fpgaLane].commonCfg.powerDown     = 0U;
    fpgaInit->datapath.serCfg[fpgaLane].commonCfg.laneRate_kHz  = laneRate_kHz;
    fpgaInit->datapath.serCfg[fpgaLane].txOutClkSel             = 0x5U;
    if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204B)
    {
        if (laneRate_kHz >= 18000000U)
        {
            fpgaInit->datapath.serCfg[fpgaLane].precursor          = 0x03U;
            fpgaInit->datapath.serCfg[fpgaLane].diffCtrl           = 0x0CU;
            fpgaInit->datapath.serCfg[fpgaLane].postcursor         = 0x03U;
        }
        else
        {
            fpgaInit->datapath.serCfg[fpgaLane].precursor          = 0U;
            fpgaInit->datapath.serCfg[fpgaLane].diffCtrl           = 0x08U;
            fpgaInit->datapath.serCfg[fpgaLane].postcursor         = 0U;
        }
        fpgaInit->datapath.serCfg[fpgaLane].commonCfg.encoder8b10bEnable = 1U;
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    if (device->devStateInfo.interfaceType == ADI_FPGAGEN6_INTERFACE_JESD204C)
    {
        if (laneRate_kHz >= 25000000U)
        {
            fpgaInit->datapath.serCfg[fpgaLane].precursor  = 0x0CU;
            fpgaInit->datapath.serCfg[fpgaLane].diffCtrl   = 0x10U;
            fpgaInit->datapath.serCfg[fpgaLane].postcursor = 0x0CU;
        }
        else if (laneRate_kHz >= 20000000U)
        {
            fpgaInit->datapath.serCfg[fpgaLane].precursor  = 0x06U;
            fpgaInit->datapath.serCfg[fpgaLane].diffCtrl   = 0x10U;
            fpgaInit->datapath.serCfg[fpgaLane].postcursor = 0x06U;
        }
        else
        {
            fpgaInit->datapath.serCfg[fpgaLane].precursor  = 0U;
            fpgaInit->datapath.serCfg[fpgaLane].diffCtrl   = 0x08U;
            fpgaInit->datapath.serCfg[fpgaLane].postcursor = 0U;
        }
        fpgaInit->datapath.serCfg[fpgaLane].commonCfg.encoder8b10bEnable = 0U;
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }

    if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_RETURN(  ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_SERIALIZER_CFG,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction);
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmSettingsConfigure(adi_fpgagen6_Device_t* const    device,
                                                                    const uint32_t                  refClk_kHz,
                                                                    adi_fpgagen6_MmcmCfg_t* const   mmcmConfig)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t                    mmcmVcoFreq_kHz = 0U;
    uint32_t                    m               = 0U;
    uint32_t                    d               = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, mmcmConfig, cleanup);

    for (m = 0U; m < ADI_FPGAGEN6_NUM_MMCM_OUTPUT_CLOCKS; ++m)
    {
        if (mmcmConfig->outputClkFreq_kHz[m] == 0U)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mmcmConfig->outputClkFreq_kHz,
                                    "Cannot have a zero value for any output clock in MMCM");
            goto cleanup;
        }
    }

    /* find an m & d value that works for all output clocks */
    for (d = 1U; d < 256U; ++d)
    {
        for (m = 1U; m < 256U; ++m)
        {
            mmcmVcoFreq_kHz = (refClk_kHz * m) / d;

            if (mmcmVcoFreq_kHz > 1600000U || mmcmVcoFreq_kHz < 600000U)
            {
                continue;
            }

            /* with the current m & d, make sure the output clocks can be achieved within the channel divider limits */
            if (((mmcmVcoFreq_kHz % mmcmConfig->outputClkFreq_kHz[0]) == 0U)    &&
                ((mmcmVcoFreq_kHz % mmcmConfig->outputClkFreq_kHz[1]) == 0U)    &&
                ((mmcmVcoFreq_kHz % mmcmConfig->outputClkFreq_kHz[2]) == 0U)    &&
                ((mmcmVcoFreq_kHz % mmcmConfig->outputClkFreq_kHz[3]) == 0U)    &&
                ((mmcmVcoFreq_kHz % mmcmConfig->outputClkFreq_kHz[4]) == 0U)    &&
                ((mmcmVcoFreq_kHz % mmcmConfig->outputClkFreq_kHz[5]) == 0U)    &&
                ((mmcmVcoFreq_kHz % mmcmConfig->outputClkFreq_kHz[6]) == 0U)    &&
                ((mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[0]) < 256U)   &&
                ((mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[1]) < 256U)   &&
                ((mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[2]) < 256U)   &&
                ((mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[3]) < 256U)   &&
                ((mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[4]) < 256U)   &&
                ((mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[5]) < 256U)   &&
                ((mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[6]) < 256U))
            {
                mmcmConfig->mVal            = m;
                mmcmConfig->dVal            = d;
                mmcmConfig->outputClkDiv[0] = mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[0];
                mmcmConfig->outputClkDiv[1] = mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[1];
                mmcmConfig->outputClkDiv[2] = mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[2];
                mmcmConfig->outputClkDiv[3] = mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[3];
                mmcmConfig->outputClkDiv[4] = mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[4];
                mmcmConfig->outputClkDiv[5] = mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[5];
                mmcmConfig->outputClkDiv[6] = mmcmVcoFreq_kHz / mmcmConfig->outputClkFreq_kHz[6];

                /* Once we find a correct setting we should exit both loops.
                 * A goto is used here in place of a flag that would be required to 
                 * break out of both loops.
                 * */
                recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;

                goto loopexit;
            }
        }
    }

loopexit:

    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                mmcmConfig->outputClkFreq_kHz,
                                "Could not find a valid MMCM configuration based on the given inputs");
        goto cleanup;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllSettingsConfigure(adi_fpgagen6_Device_t* const    device,
                                                                    const uint32_t                  refClk_kHz,
                                                                    const uint32_t                  fastestLaneRate_kHz,
                                                                    adi_fpgagen6_QpllCfg_t* const   qplls)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    /* These START and END constants define the value values for the fraction-N divider
     * in the QPLL
     */
    static const uint8_t      ADI_FPGAGEN6_N_START                      = 16U;
    static const uint8_t      ADI_FPGAGEN6_N_END                        = 160U;
    uint8_t                   n                                         = 0U;
    uint8_t                   validMVals[]                              = { 1U, 2U, 3U, 4U };
    uint8_t                   m                                         = 0U;
    uint8_t                   validDVals[]                              = { 1U, 2U, 4U, 8U, 16U, 32U };
    uint8_t                   d                                         = 0U;
    uint32_t                  qpllVcoFreq_kHz                           = 0U;
    uint32_t                  qpllDivideRatio                           = 1U;
    uint32_t                  qpllVcoOutFreq_kHz                        = 0U;
    adi_fpgagen6_LaneClkSel_e laneClkSelLocal                           = ADI_FPGAGEN6_QPLLCLK0;
    adi_fpgagen6_QpllClkSel_e refClkSelect[ADI_FPGAGEN6_PHY_QUAD_COUNT] = { ADI_FPGAGEN6_QPLL_GTREFCLK0, ADI_FPGAGEN6_QPLL_GTREFCLK0 };
    uint8_t                     validConfigFound                            =   0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, qplls, cleanup);

    /* Assign these default values for refClkSelect since 0 isn't a valid option for this enum */
    qplls->refClkSelect[0]  = ADI_FPGAGEN6_QPLL_GTREFCLK0;
    qplls->refClkSelect[1]  = ADI_FPGAGEN6_QPLL_GTREFCLK0;
    
    if (fastestLaneRate_kHz < ADI_FPGAGEN6_QPLL0_MAX_KHZ)
    {
        qpllDivideRatio                         = 2U;
        qplls->qpllClockSettings.qpllClkOutRate = 0U;
    }
    else
    {
        qpllDivideRatio                         = 1U;
        qplls->qpllClockSettings.qpllClkOutRate = 1U;
    }

    for (m = 0U; (m < (sizeof(validMVals) / sizeof(validMVals[0]))) && (validConfigFound == 0U); ++m)
    {
        for (n = ADI_FPGAGEN6_N_END; (n >= ADI_FPGAGEN6_N_START)  && (validConfigFound == 0U); --n)
        {
            for (d = 0; (d < (sizeof(validDVals) / sizeof(validDVals[0])))  && (validConfigFound == 0U); ++d)
            {
                qpllVcoFreq_kHz    = refClk_kHz * n / validMVals[m];
                qpllVcoOutFreq_kHz = qpllVcoFreq_kHz / qpllDivideRatio;

                if ((qpllVcoFreq_kHz >= ADI_FPGAGEN6_QPLL0_MIN_KHZ) && (qpllVcoFreq_kHz <= ADI_FPGAGEN6_QPLL0_MAX_KHZ))
                {
                    laneClkSelLocal         = ADI_FPGAGEN6_QPLLCLK0;
                    qplls->refClkSelect[0]  = ADI_FPGAGEN6_QPLL_GTREFCLK0;
                    qplls->refClkSelect[1]  = ADI_FPGAGEN6_QPLL_GTREFCLK0;
                }
                else if ((qpllVcoFreq_kHz >= ADI_FPGAGEN6_QPLL1_MIN_KHZ) && (qpllVcoFreq_kHz <= ADI_FPGAGEN6_QPLL1_MAX_KHZ))
                {
                    laneClkSelLocal         = ADI_FPGAGEN6_QPLLCLK1;
                    qplls->refClkSelect[0]  = ADI_FPGAGEN6_QPLL_GTREFCLK1;
                    qplls->refClkSelect[1]  = ADI_FPGAGEN6_QPLL_GTREFCLK1;
                }
                else
                {
                    /* If we're out of range for both qplls continue on to the next divider + multiplier combination */
                    continue;
                }

                /* There is a final 2x multiplier that's constant in the qpll */
                if (qpllVcoOutFreq_kHz * 2U / validDVals[d] == fastestLaneRate_kHz)
                {
                    qplls->refClk_kHz                           = refClk_kHz;
                    qplls->qpllClockSettings.laneRate_kHz       = fastestLaneRate_kHz;
                    qplls->qpllClockSettings.mVal               = validMVals[m];
                    qplls->qpllClockSettings.nVal               = n;
                    qplls->qpllClockSettings.laneClkSel         = laneClkSelLocal;
                    qplls->qpllClockSettings.qpllVco_kHz        = qpllVcoFreq_kHz;
                    qplls->qpllClockSettings.qpllVcoOut_kHz     = qpllVcoOutFreq_kHz;
                    qplls->refClkSelect[0]                      = refClkSelect[0];
                    qplls->refClkSelect[1]                      = refClkSelect[1];

                    recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
                    
                    validConfigFound = 1U;
                }
            }
        }
    }

    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                fastestLaneRate_kHz,
                                "Could not find a valid Qpll configuration based on the given inputs");
        goto cleanup;
    }
    
    /* TODO: Once we get the generated values from Justin we will try to replace as much of this function
     * as possible using those value. This is the prototype that could possibly be used. 
     * recoveryAction = fpgagen6_QpllDrpCfg(device,
     *                                      refClk_kHz,
     *                                      qplls->qpllVco_kHz,
     *                                      (qplls->qpllClkOutRate == 0 ? 2 : 1));
    */

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_fpgagen6_QpllDivisorCalculate(adi_fpgagen6_Device_t* const    device,
                                                                            const uint32_t                  qpllVcoFreq_kHz,
                                                                            const uint32_t                  desiredLaneRate_kHz,
                                                                            uint8_t* const                  dVal)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t _D[] = { 1U, 2U, 4U, 8U, 16U };
    uint8_t _DLength       = 5U;
    uint8_t d              = 0U;
    uint8_t desiredDVal    = 1U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, dVal, cleanup);

    /* Prevent divide by 0 */
    if (desiredLaneRate_kHz == 0U)
    {
        *dVal = 1U; /* Return valid divide by 1 setting */
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
        goto cleanup;
    }
    else
    {
        desiredDVal = (uint8_t)(qpllVcoFreq_kHz / desiredLaneRate_kHz);
    }

    for (d = 0; d < _DLength; d++)
    {
        if (desiredDVal == _D[d])
        {
            *dVal = _D[d];
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;
        }
    }

    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                desiredLaneRate_kHz,
                                "Could not find a valid DIVIDER configuration based on the given inputs");
        goto cleanup;
    }
    
cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarConfigure(adi_fpgagen6_Device_t* const    device,
                                                                const uint8_t                   inputXBar[],
                                                                uint8_t                         outputXBar[],
                                                                const uint32_t                  laneCount)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t                    i               = 0U;
    uint32_t                    j               = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, inputXBar, cleanup);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, outputXBar, cleanup);

    for (i = 0U; i < laneCount; ++i)
    {
        /* Validate Device LaneXbar Information */
        if (inputXBar[i] < laneCount)
        {
            /* Translate PHY Lane to Index & Vice Versa Depending on dataLink Type Passed in */
            j               = inputXBar[i];
            outputXBar[j]   = i;
        }
    }

    if (i >= laneCount)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LaneXBarResetConfigure(   adi_fpgagen6_Device_t* const            device,
                                                                        const adi_fpgagen6_Init_t* const        init,
                                                                        adi_fpgagen6_LaneXBarResetMask_t* const laneResetMaskInfo)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     i               = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, init, cleanup);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, laneResetMaskInfo, cleanup);

    laneResetMaskInfo->deframer0 = 0U;
    laneResetMaskInfo->deframer1 = 0U;
    laneResetMaskInfo->deframer2 = 0U;
    laneResetMaskInfo->deframerSelect = 0U;
    laneResetMaskInfo->framer0 = 0U;
    laneResetMaskInfo->framer1 = 0U;
    laneResetMaskInfo->framerSelect = 0U;
    
    /* build up the link layer's lane xbar resets based on which phy lanes they're using */
    /* can't use dserializerChannelMask here because we need to reset based on which channels go to which
    *  link layer. the dserializerChannelMask shows us all the channels that are active, not how they map onto the link layers */
    for (i = 0U; i < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; ++i)
    {
        /* The reset bit location maps to the PHY lane # for tx & rx. */
        /* For rx, the value in the laneXBar array is used to 
            * determine the PHY lane #*/
        if (((init->datapath.deframerCfg[0U].lanesInUse >> i) & 0x01U) == 0x01U)
        {
            laneResetMaskInfo->deframerSelect |= (uint8_t) ADI_FPGAGEN6_DEFRAMER_0;
            laneResetMaskInfo->deframer0 |= (0x01U << init->datapath.deframerCfg[0U].laneXBar[i]);
        }

        if (((init->datapath.deframerCfg[1U].lanesInUse >> i) & 0x01U) == 0x01U)
        {
            laneResetMaskInfo->deframerSelect |= (uint8_t) ADI_FPGAGEN6_DEFRAMER_1;
            laneResetMaskInfo->deframer1 |= (0x01U << init->datapath.deframerCfg[1U].laneXBar[i]);
        }

        if (((init->datapath.deframerCfg[2U].lanesInUse >> i) & 0x01U) == 0x01U)
        {
            laneResetMaskInfo->deframerSelect |= (uint8_t) ADI_FPGAGEN6_DEFRAMER_2;
            laneResetMaskInfo->deframer2 |= (0x01U << init->datapath.deframerCfg[2U].laneXBar[i]);
        }

        /* For tx, the index of the laneXBar maps to the PHY lane #. 
            * If the laneXBar value is not the initial value (0x1F) that means this index corresponds to a PHY lane
            * connected to this framer */
        if (init->datapath.framerCfg[0U].laneXBar[i] != ADI_FPGAGEN6_LINK_LANE_UNUSED)
        {
            laneResetMaskInfo->framerSelect |= (uint8_t) ADI_FPGAGEN6_FRAMER_0;
            laneResetMaskInfo->framer0 |= (0x01U << i);
        }
        if (init->datapath.framerCfg[1U].laneXBar[i] != ADI_FPGAGEN6_LINK_LANE_UNUSED)
        {
            laneResetMaskInfo->framerSelect |= (uint8_t) ADI_FPGAGEN6_FRAMER_1;
            laneResetMaskInfo->framer1 |= (0x01U << i);
        }
    }

    if (i == ADI_FPGAGEN6_LANES_PHYSICAL_NUM)
    {
        laneResetMaskInfo->deframer0    = ~laneResetMaskInfo->deframer0;
        laneResetMaskInfo->deframer1    = ~laneResetMaskInfo->deframer1;
        laneResetMaskInfo->deframer2    = ~laneResetMaskInfo->deframer2;
        laneResetMaskInfo->framer0      = ~laneResetMaskInfo->framer0;
        laneResetMaskInfo->framer1      = ~laneResetMaskInfo->framer1;
        recoveryAction                  = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_LANEXBAR_RESET_CFG,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DeframerConfigure(adi_fpgagen6_Device_t* const                    device,
                                                                const adi_fpgagen6_DeframerDataLinkCfg_t* const deframerlink,
                                                                adi_fpgagen6_DeframerCfg_t* const               deframerCfg)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     defrmL          = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, deframerlink, cleanup);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, deframerCfg, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    defrmL = fpgagen6_LCalculate(deframerlink->dataLink.lanesEnabled);

    deframerCfg->sysRefDelay    = 0U;
    deframerCfg->subclassV      = 1U;
    deframerCfg->lanesInUse     =   (defrmL == 8U) ? 0xFFU :
                                    (defrmL == 4U) ? 0x0FU :
                                    (defrmL == 2U) ? 0x03U :
                                    (defrmL == 1U) ? 0x01U : 0U;
    deframerCfg->ctrlRxBufAdv   = 0U;

    recoveryAction = adi_fpgagen6_LaneXBarConfigure(device,
                                                    deframerlink->dataLink.deviceLaneXbar,
                                                    deframerCfg->laneXBar,
                                                    ADI_FPGAGEN6_LANES_DEFRAMER_MAX);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_DEFRAMER_CFG,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* FPGAGEN6_INTERFACE_IDX_SET Checks for Invalid Settings */
    if (interfaceIdx == ADI_FPGAGEN6_JESD204B_IDX)
    {
        /* JESD 204B */
        deframerCfg->scramblingEn           = deframerlink->dataLink.scramblingEn;
        deframerCfg->cfgF                   = (defrmL > 0U) ? deframerlink->dataLink.jesd204F : 0U;
        deframerCfg->cfgK                   = deframerlink->dataLink.jesd204K;
        deframerCfg->errReportingEnable     = 1U;
        deframerCfg->ilaSupportEn           = 1U;
        deframerCfg->sysRefAlways           = 0U;
        deframerCfg->sysRefRequiredOnResync = 0U;
        deframerCfg->ilaMultiFrames         = 4U;
        recoveryAction                      = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        /* JESD 204C */
        deframerCfg->enableCmdInterface     = 0U;
        deframerCfg->enableDataInterface    = 0U;
        deframerCfg->cfgE                   = deframerlink->dataLink.jesd204E;
        deframerCfg->ctrlMetaMode           = 0U;
        deframerCfg->ctrlRxMBlockTh         = 1U;
        deframerCfg->subclassV              = 1U;
        recoveryAction                      = ADI_FPGAGEN6_ERR_ACT_NONE;
        deframerCfg->laneBufferDelay        = ((deframerlink->dataLink.jesd204E - 1U) * 32U) + 16U;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_FramerConfigure(  adi_fpgagen6_Device_t* const                    device,
                                                                const adi_fpgagen6_FramerDataLinkCfg_t* const   framerlink,
                                                                adi_fpgagen6_FramerCfg_t* const                 framerCfg)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     interfaceIdx    = 0U;
    uint8_t                     frmL            = 0U;
    uint8_t                     laneIdx         = 0U;
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, framerlink, cleanup);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, framerCfg, cleanup);

    FPGAGEN6_INTERFACE_IDX_SET_GOTO(device->devStateInfo.interfaceType, recoveryAction, cleanup);

    frmL = fpgagen6_LCalculate(framerlink->dataLink.lanesEnabled);
    
    framerCfg->deviceId     = framerlink->dataLink.jesdDid;
    framerCfg->bankId       = framerlink->dataLink.jesdBid;
    for (laneIdx = 0; laneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; laneIdx++)
    {
        framerCfg->laneId[laneIdx]    = framerlink->dataLink.jesdLid[laneIdx];
    }
    
    framerCfg->sysRefDelay  = 0U;
    framerCfg->subclassV    = 1U;
    framerCfg->lanesInUse   =   (frmL == 8U) ? 0xFFU :
                                (frmL == 4U) ? 0x0FU :
                                (frmL == 2U) ? 0x03U :
                                (frmL == 1U) ? 0x01U : 0U;
    
    recoveryAction = adi_fpgagen6_LaneXBarConfigure(device,
                                                    framerlink->dataLink.deviceLaneXbar,
                                                    framerCfg->laneXBar,
                                                    ADI_FPGAGEN6_LANES_PHYSICAL_NUM);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_FRAMER_CFG,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    /* FPGAGEN6_INTERFACE_IDX_SET Checks for Invalid Settings */
    if (interfaceIdx == ADI_FPGAGEN6_JESD204B_IDX)
    {
        /* JESD 204B */
        framerCfg->scramblingEn             = framerlink->dataLink.scramblingEn;
        framerCfg->ilaMultiFrames           = 4U;
        framerCfg->cfgF                     = (frmL > 0U) ? framerlink->dataLink.jesd204F : 0U;
        framerCfg->cfgK                     = framerlink->dataLink.jesd204K;
        framerCfg->ilaM                     = framerlink->dataLink.jesd204M;
        framerCfg->ilaCs                    = framerlink->dataLink.jesd204Cs;
        framerCfg->ilaNp                    = framerlink->dataLink.jesd204Np;
        framerCfg->ilaN                     = framerlink->dataLink.jesd204N;
        framerCfg->ilaS                     = framerlink->dataLink.jesd204S;
        framerCfg->ilaCf                    = 0U;
        framerCfg->ilaSupportEn             = 1U;
        framerCfg->sysRefAlways             = 0U;
        framerCfg->sysRefRequiredOnResync   = 0U;
        recoveryAction                      = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        /* JESD 204C */
        framerCfg->enableCmdInterface   = 0U;
        framerCfg->enableDataInterface  = 0U;
        framerCfg->cfgE                 = framerlink->dataLink.jesd204E;
        framerCfg->ctrlMetaMode         = 0U;
        framerCfg->subclassV            = 1U;
        recoveryAction                  = ADI_FPGAGEN6_ERR_ACT_NONE;
    }

    cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_SyncXBarConfigure(adi_fpgagen6_Device_t* const        device,
                                                                const adi_fpgagen6_SyncModeSel_e    syncMode,
                                                                adi_fpgagen6_SyncXBar_t* const      syncXBarCfg)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
 
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, syncXBarCfg, cleanup);

    switch (syncMode)
    {
        case ADI_FPGAGEN6_SYNCMODE_DEFAULT:
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER0_IDX].select    = ADI_FPGAGEN6_SYNCOUT_DEFRAMER0;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER1_IDX].select    = ADI_FPGAGEN6_SYNCOUT_DEFRAMER1;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER2_IDX].select    = ADI_FPGAGEN6_SYNCOUT_DEFRAMER2;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER0_IDX].select        = ADI_FPGAGEN6_SYNCSRC_SYNCINB0;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER1_IDX].select        = ADI_FPGAGEN6_SYNCSRC_SYNCINB1;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER0_IDX].polarity  = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER1_IDX].polarity  = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER2_IDX].polarity  = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER0_IDX].polarity      = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER1_IDX].polarity      = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            recoveryAction                                              = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_SYNCMODE_LOOPBACK_NEAR:
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER0_IDX].select    = ADI_FPGAGEN6_SYNCOUT_DEFRAMER0;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER1_IDX].select    = ADI_FPGAGEN6_SYNCOUT_DEFRAMER1;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER2_IDX].select    = ADI_FPGAGEN6_SYNCOUT_DEFRAMER2;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER0_IDX].select        = ADI_FPGAGEN6_SYNCSRC_DEFRAMER0;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER1_IDX].select        = ADI_FPGAGEN6_SYNCSRC_DEFRAMER1;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER0_IDX].polarity  = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER1_IDX].polarity  = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->deframer[ADI_FPGAGEN6_DEFRAMER2_IDX].polarity  = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER0_IDX].polarity      = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            syncXBarCfg->framer[ADI_FPGAGEN6_FRAMER1_IDX].polarity      = ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT;
            recoveryAction                                              = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_SYNCMODE_UNKNOWN:
            /* Fall Through */

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, syncMode, recoveryAction, "Invalid SyncBar Mode Selection");
            goto cleanup;
            break;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_InitConfig(  adi_fpgagen6_Device_t* const            device,
                                                            const adi_fpgagen6_FpgaCfg_t* const     fpgaCfg,
                                                            adi_fpgagen6_Init_t* const              fpgaInit)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t fastestLaneRate_kHz = 0U;
    uint32_t refClk_kHz          = fpgaCfg->deviceClk_kHz;
    
    uint8_t defrmL[ADI_FPGAGEN6_MAX_DEFRAMERS] = { 0U };
    uint64_t defrmLaneRate_kHz[ADI_FPGAGEN6_MAX_DEFRAMERS] = { 0U };
    uint32_t defrmCoreClk_kHz[ADI_FPGAGEN6_MAX_DEFRAMERS] = { 0U };
    uint8_t fpgaPhyLanesUsed = 0U;
    
    uint8_t frmL[ADI_FPGAGEN6_MAX_FRAMERS] = { 0U };
    uint64_t frmLaneRate_kHz[ADI_FPGAGEN6_MAX_FRAMERS] = { 0U };
    uint32_t frmCoreClk_kHz[ADI_FPGAGEN6_MAX_FRAMERS] = { 0U };
    
    uint8_t linkIdx = 0U;
    uint8_t laneIdx = 0U;
    uint8_t dmaIdx = 0U;

    uint32_t mmcmUnusedClk_kHz = 0U;

    uint8_t dVal = 0U;
    
    uint32_t phyChannelIdx = 0U;
    
    uint32_t dmaDataWidth = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, fpgaCfg, cleanup);
    
    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, fpgaInit, cleanup);
    
    if (fpgaCfg->jesd204CEn == ADI_ENABLE)
    {
        device->devStateInfo.linkLayerDataWidth = 66U;
        device->devStateInfo.encodingHighValue = 66U;
        device->devStateInfo.encodingLowValue = 64U;
        device->devStateInfo.interfaceType = ADI_FPGAGEN6_INTERFACE_JESD204C;
        dmaDataWidth = 1U; /* 1 for 64 bits */
    }
    else
    {
        device->devStateInfo.linkLayerDataWidth = 40U;
        device->devStateInfo.encodingHighValue = 10U;
        device->devStateInfo.encodingLowValue = 8U;
        device->devStateInfo.interfaceType = ADI_FPGAGEN6_INTERFACE_JESD204B;
        dmaDataWidth = 0U; /* 0 for 32 bits */
    }
    
    if (fpgaCfg->deviceClk_kHz == 0U)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Calculate L and lane rates */
    {
        for (linkIdx = 0U; linkIdx < ADI_FPGAGEN6_MAX_DEFRAMERS; ++linkIdx)
        {
            defrmL[linkIdx] = fpgagen6_LCalculate(fpgaCfg->deframerlink[linkIdx].dataLink.lanesEnabled);

            if (defrmL[linkIdx] != 0U)
            {
                defrmLaneRate_kHz[linkIdx] = ((uint64_t)fpgaCfg->deframerlink[linkIdx].dataLink.iqRate_kHz
                    * fpgaCfg->deframerlink[linkIdx].dataLink.jesd204M
                    * fpgaCfg->deframerlink[linkIdx].dataLink.jesd204Np
                    * device->devStateInfo.encodingHighValue) / (device->devStateInfo.encodingLowValue * defrmL[linkIdx]);

                defrmCoreClk_kHz[linkIdx] = (uint32_t)(defrmLaneRate_kHz[linkIdx] / device->devStateInfo.linkLayerDataWidth);

                if ((mmcmUnusedClk_kHz == 0U) && (defrmCoreClk_kHz[linkIdx] != 0U))
                {
                    mmcmUnusedClk_kHz = defrmCoreClk_kHz[linkIdx];
                }
            }

            if (fastestLaneRate_kHz < defrmLaneRate_kHz[linkIdx])
            {
                fastestLaneRate_kHz = defrmLaneRate_kHz[linkIdx];
            }
        }

        for (linkIdx = 0U; linkIdx < ADI_FPGAGEN6_MAX_FRAMERS; linkIdx++)
        {
            frmL[linkIdx] = fpgagen6_LCalculate(fpgaCfg->framerlink[linkIdx].dataLink.lanesEnabled);

            if (frmL[linkIdx] != 0U)
            {
                frmLaneRate_kHz[linkIdx] = ((uint64_t)fpgaCfg->framerlink[linkIdx].dataLink.iqRate_kHz
                    * fpgaCfg->framerlink[linkIdx].dataLink.jesd204M
                    * fpgaCfg->framerlink[linkIdx].dataLink.jesd204Np
                    * device->devStateInfo.encodingHighValue) / (device->devStateInfo.encodingLowValue * frmL[linkIdx]);

                frmCoreClk_kHz[linkIdx] = (uint32_t)(frmLaneRate_kHz[linkIdx] / device->devStateInfo.linkLayerDataWidth);

                if ((mmcmUnusedClk_kHz == 0U) && (frmCoreClk_kHz[linkIdx] != 0U))
                {
                    mmcmUnusedClk_kHz = frmCoreClk_kHz[linkIdx];
                }
            }

            if (fastestLaneRate_kHz < frmLaneRate_kHz[linkIdx])
            {
                fastestLaneRate_kHz = frmLaneRate_kHz[linkIdx];
            }
        }

        if (fastestLaneRate_kHz == 0U)
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    
    /* Setup the clocks */
    {
        recoveryAction = adi_fpgagen6_QpllSettingsConfigure(device,
                                                            refClk_kHz,
                                                            fastestLaneRate_kHz,
                                                            &fpgaInit->clocks.qpll);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        /* Hardcoded assignment of MMCM output clocks */
        fpgaInit->clocks.coreMmcm.outputClkFreq_kHz[0] = (defrmCoreClk_kHz[0] == 0U) ? mmcmUnusedClk_kHz : defrmCoreClk_kHz[0];
        fpgaInit->clocks.coreMmcm.outputClkFreq_kHz[1] = (defrmCoreClk_kHz[1] == 0U) ? mmcmUnusedClk_kHz : defrmCoreClk_kHz[1];
        fpgaInit->clocks.coreMmcm.outputClkFreq_kHz[2] = (defrmCoreClk_kHz[2] == 0U) ? mmcmUnusedClk_kHz : defrmCoreClk_kHz[2];

        fpgaInit->clocks.coreMmcm.outputClkFreq_kHz[3] = (frmCoreClk_kHz[0] == 0U) ? mmcmUnusedClk_kHz : frmCoreClk_kHz[0];
        fpgaInit->clocks.coreMmcm.outputClkFreq_kHz[4] = (frmCoreClk_kHz[1] == 0U) ? mmcmUnusedClk_kHz : frmCoreClk_kHz[1];
        fpgaInit->clocks.coreMmcm.outputClkFreq_kHz[5] = fastestLaneRate_kHz / device->devStateInfo.linkLayerDataWidth; /* TDD FSM frequency */
        fpgaInit->clocks.coreMmcm.outputClkFreq_kHz[6] = refClk_kHz; /* SPI Master alternate clock */
    
        recoveryAction = adi_fpgagen6_MmcmSettingsConfigure(device,
                                                            refClk_kHz,
                                                            &fpgaInit->clocks.coreMmcm);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    
    /* Setup the Deframers & Deserializers */
    {
        /* Power down all deserializer lanes in FPGA */
        for (phyChannelIdx = 0U; phyChannelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; phyChannelIdx++)
        {
            fpgaInit->datapath.desCfg[phyChannelIdx].commonCfg.powerDown = 3U;
            fpgaInit->datapath.desCfg[phyChannelIdx].commonCfg.pllClkSel = fpgaInit->clocks.qpll.qpllClockSettings.laneClkSel; // ADI_FPGAGEN6_QPLLCLK0;
        }
    
        fpgaPhyLanesUsed = 0U;
        for (linkIdx = 0U; linkIdx < ADI_FPGAGEN6_MAX_DEFRAMERS; linkIdx++)
        {
            recoveryAction = adi_fpgagen6_fpgagen6_QpllDivisorCalculate(device,
                                                                        fpgaInit->clocks.qpll.qpllClockSettings.qpllVcoOut_kHz * ADI_FPGAGEN6_QPLL_DDC_MULTIPLIER,
                                                                        (uint32_t)defrmLaneRate_kHz[linkIdx],
                                                                        &dVal);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            recoveryAction = adi_fpgagen6_DeframerConfigure(device,
                                                            &fpgaCfg->deframerlink[linkIdx],
                                                            &fpgaInit->datapath.deframerCfg[linkIdx]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            /* Links must not share any PHY lanes */
            if ((fpgaPhyLanesUsed & fpgaCfg->deframerlink[linkIdx].dataLink.lanesEnabled) > 0U)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            fpgaPhyLanesUsed |= fpgaCfg->deframerlink[linkIdx].dataLink.lanesEnabled;

            for (laneIdx = 0U; laneIdx < defrmL[linkIdx]; laneIdx++)
            {
                recoveryAction = fpgagen6_DeserializerCfg(  device,
                                                            fpgaInit,
                                                            fpgaInit->datapath.deframerCfg[linkIdx].laneXBar[laneIdx],
                                                            dVal,
                                                            refClk_kHz);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
        }
    }

    /* Setup the Framers & Serializers */
    {
        /* Power down all FPGA serializer lanes */
        for (phyChannelIdx = 0U; phyChannelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; ++phyChannelIdx)
        {
            fpgaInit->datapath.serCfg[phyChannelIdx].commonCfg.powerDown = 3U;
            fpgaInit->datapath.serCfg[phyChannelIdx].commonCfg.pllClkSel = fpgaInit->clocks.qpll.qpllClockSettings.laneClkSel;
        }

        fpgaPhyLanesUsed = 0U;

        for (linkIdx = 0U; linkIdx < ADI_FPGAGEN6_MAX_FRAMERS; ++linkIdx)
        {
            recoveryAction = adi_fpgagen6_fpgagen6_QpllDivisorCalculate(device,
                                                                        fpgaInit->clocks.qpll.qpllClockSettings.qpllVcoOut_kHz * ADI_FPGAGEN6_QPLL_DDC_MULTIPLIER,
                                                                        (uint32_t)frmLaneRate_kHz[linkIdx],
                                                                        &dVal);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            recoveryAction = adi_fpgagen6_FramerConfigure(  device,
                                                            &fpgaCfg->framerlink[linkIdx],
                                                            &fpgaInit->datapath.framerCfg[linkIdx]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        
            /* Links must not share any PHY lanes */
            if ((fpgaPhyLanesUsed & fpgaCfg->framerlink[linkIdx].dataLink.lanesEnabled) > 0U)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            fpgaPhyLanesUsed |= fpgaCfg->framerlink[linkIdx].dataLink.lanesEnabled;

            for (laneIdx = 0U; laneIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; ++laneIdx)
            {
                if (((fpgaCfg->framerlink[linkIdx].dataLink.lanesEnabled >> laneIdx) & 1U) > 0U)
                {
                    recoveryAction = fpgagen6_SerializerCfg(device,
                                                            fpgaInit,
                                                            laneIdx,
                                                            dVal,
                                                            refClk_kHz);
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }
                }
                else
                {
                    /* Can't be sure what the input value is for unused lanes so overwrite with the unused value */
                    fpgaInit->datapath.framerCfg[linkIdx].laneXBar[laneIdx] = ADI_FPGAGEN6_LINK_LANE_UNUSED;
                }
            }
        }
    }

    /* Setup the DMAs */
    {

        for (dmaIdx = 0U; dmaIdx < ADI_FPGAGEN6_RX_DMA_COUNT; ++dmaIdx)
        {
            fpgaInit->datapath.rxDmaCfg[dmaIdx].dma.alignToSof              = 1U; /* If Transport layer is in FPGA, set to 0; otherwise 1*/
            fpgaInit->datapath.rxDmaCfg[dmaIdx].rxDmaCaptureCfg.validDataWidthStream = dmaDataWidth;
            fpgaInit->datapath.rxDmaCfg[dmaIdx].rxDmaCaptureCfg.triggerSelect = ADI_FPGAGEN6_RX_IMM_TRIG; /* Default to IMM TRIG since this enum doesn't start from 0 */
            fpgaInit->datapath.rxDmaCfg[dmaIdx].dma.arbitrationSelect       = 0U;
            fpgaInit->datapath.rxDmaCfg[dmaIdx].dma.dataMaskSelect          = 0U;
            fpgaInit->datapath.rxDmaCfg[dmaIdx].dma.qThreshold              = 7U;
            fpgaInit->datapath.rxDmaCfg[dmaIdx].dma.startingAddress         = ADI_FPGAGEN6_RX_DMA_RAM_START_ADDR + dmaIdx * ADI_FPGAGEN6_RX_DMA_MEM_BLOCK_SIZE;
            fpgaInit->datapath.rxDmaCfg[dmaIdx].dma.transactionSize         = 0x400000U; /* Default to 4MB. Doesn't much matter for this memory interface */
        }

        for (dmaIdx = 0U; dmaIdx < ADI_FPGAGEN6_TX_DMA_COUNT; ++dmaIdx)
        {
            fpgaInit->datapath.txDmaCfg[dmaIdx].dma.alignToSof                  = 1U; /* If Transport layer is in FPGA, set to 0; otherwise 1 */
            fpgaInit->datapath.txDmaCfg[dmaIdx].dma.arbitrationSelect           = 0U;
            fpgaInit->datapath.txDmaCfg[dmaIdx].dma.dataMaskSelect              = 0U;
            fpgaInit->datapath.txDmaCfg[dmaIdx].dma.qThreshold                  = 7U;
            fpgaInit->datapath.txDmaCfg[dmaIdx].dma.startingAddress             = ADI_FPGAGEN6_TX_DMA_RAM_START_ADDR + dmaIdx * ADI_FPGAGEN6_TX_DMA_MEM_BLOCK_SIZE;
            fpgaInit->datapath.txDmaCfg[dmaIdx].dma.transactionSize             = 0x400000U; /* Default to 4MB. Doesn't much matter for this memory interface */
            fpgaInit->datapath.txDmaCfg[dmaIdx].validDataWidthStream            = dmaDataWidth;
        }
    }

    /* Setup the Rx Transport */
    {
        /* 
         *  Transport 0 is Rx Link 0 default 
         *  Transport 1 is Rx Link 0 Link Sharing
         *  Transport 2 is Rx Link 1 default 
         *  Transport 3 is either Rx Link 1 Link Sharing -or-
         *                        Rx Link 2 default
         **/
        fpgaInit->datapath.rxTransport[0].cfgF              = fpgaCfg->deframerlink[0].dataLink.jesd204F;
        fpgaInit->datapath.rxTransport[0].cfgL              = fpgagen6_LCalculate(fpgaCfg->deframerlink[0].dataLink.lanesEnabled);
        fpgaInit->datapath.rxTransport[0].cfgLinkSharing    = ADI_FALSE;
        fpgaInit->datapath.rxTransport[0].cfgM              = fpgaCfg->deframerlink[0].dataLink.jesd204M;
        fpgaInit->datapath.rxTransport[0].cfgNp             = fpgaCfg->deframerlink[0].dataLink.jesd204Np;
        
        fpgaInit->datapath.rxTransport[1].cfgF              = fpgaCfg->deframerlink[0].dataLink.linkSharingF;
        fpgaInit->datapath.rxTransport[1].cfgL              = fpgagen6_LCalculate(fpgaCfg->deframerlink[0].dataLink.lanesEnabled);
        fpgaInit->datapath.rxTransport[1].cfgLinkSharing    = fpgaCfg->deframerlink[0].dataLink.linkSharingEn;
        fpgaInit->datapath.rxTransport[1].cfgM              = fpgaCfg->deframerlink[0].dataLink.linkSharingM;
        fpgaInit->datapath.rxTransport[1].cfgNp             = fpgaCfg->deframerlink[0].dataLink.linkSharingNp;
        
        fpgaInit->datapath.rxTransport[2].cfgF              = fpgaCfg->deframerlink[1].dataLink.jesd204F;
        fpgaInit->datapath.rxTransport[2].cfgL              = fpgagen6_LCalculate(fpgaCfg->deframerlink[1].dataLink.lanesEnabled);
        fpgaInit->datapath.rxTransport[2].cfgLinkSharing    = ADI_FALSE;
        fpgaInit->datapath.rxTransport[2].cfgM              = fpgaCfg->deframerlink[1].dataLink.jesd204M;
        fpgaInit->datapath.rxTransport[2].cfgNp             = fpgaCfg->deframerlink[1].dataLink.jesd204Np;
        
        if (fpgaCfg->deframerlink[1].dataLink.linkSharingEn == ADI_ENABLE)
        {
            fpgaInit->datapath.rxTransport[3].cfgF              = fpgaCfg->deframerlink[1].dataLink.linkSharingF;
            fpgaInit->datapath.rxTransport[3].cfgL              = fpgagen6_LCalculate(fpgaCfg->deframerlink[1].dataLink.lanesEnabled);
            fpgaInit->datapath.rxTransport[3].cfgLinkSharing    = fpgaCfg->deframerlink[1].dataLink.linkSharingEn;
            fpgaInit->datapath.rxTransport[3].cfgM              = fpgaCfg->deframerlink[1].dataLink.linkSharingM;
            fpgaInit->datapath.rxTransport[3].cfgNp             = fpgaCfg->deframerlink[1].dataLink.linkSharingNp;
        }
        else
        {
            fpgaInit->datapath.rxTransport[3].cfgF              = fpgaCfg->deframerlink[2].dataLink.jesd204F;
            fpgaInit->datapath.rxTransport[3].cfgL              = fpgagen6_LCalculate(fpgaCfg->deframerlink[2].dataLink.lanesEnabled);
            fpgaInit->datapath.rxTransport[3].cfgLinkSharing    = ADI_FALSE;
            fpgaInit->datapath.rxTransport[3].cfgM              = fpgaCfg->deframerlink[2].dataLink.jesd204M;
            fpgaInit->datapath.rxTransport[3].cfgNp             = fpgaCfg->deframerlink[2].dataLink.jesd204Np;
        }
    }

    /* Setup SYNCB crossbars in FPGA */
    recoveryAction = adi_fpgagen6_SyncXBarConfigure(device,
                                                    ADI_FPGAGEN6_SYNCMODE_DEFAULT,
                                                    &fpgaInit->datapath.syncXBarCfg);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_InitSet(  adi_fpgagen6_Device_t* const        device,
                                                        const adi_fpgagen6_Init_t* const    fpgaInit)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t dmaIdx = 0U;
    adi_fpgagen6_LaneXBarResetMask_t laneResetMaskInfo;
    uint8_t qpllLockStatus = 0U;
    uint32_t qpllLockCheckIdx = 0U;
    uint32_t qpllQuadIdx = 0U;
    uint32_t serdesIdx = 0U;
    uint32_t serdesResetDoneIdx = 0U;
    uint32_t serResetChMask = 0U;
    uint32_t desResetChMask = 0U;
    uint8_t serdesResetStatus = 0U;
    uint32_t txLaneRate_kHz = 0U;
    uint32_t rxLaneRate_kHz = 0U;
    uint32_t deframerIdx = 0U;
    uint32_t deframerSel = 0U;
    uint32_t framerIdx = 0U;
    uint32_t framerSel = 0U;
    uint32_t fpgaVersion = 0U;
    
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, fpgaInit, cleanup);
    
    ADI_LIBRARY_MEMSET(&laneResetMaskInfo, 0, sizeof(adi_fpgagen6_LaneXBarResetMask_t));

    /* Verify the correct FPGA is running */
    recoveryAction = adi_fpgagen6_RegisterRead(device, FPGA_VERSION_ADDR, &fpgaVersion);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    fpgaVersion = fpgaVersion & 0x01000000; /* bit 0 of device ID*/

    if (fpgaVersion == 0)
    {
        /* FPGA is not correct */ 
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
            ADI_FPGAGEN6_ERRCODE_UTILITIES_FPGA_VERSION_INVALID,
            ADI_NO_VARIABLE,
            recoveryAction,
            cleanup);
    }
    
    /* Put all LaneXbars into reset */    
    recoveryAction = adi_fpgagen6_LaneXBarFullResetSet(device);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /***************************************************************************************************************************************
    * Physical layer
    ***************************************************************************************************************************************/
    {
        /* Put all SERDES lanes in reset */
        {
            recoveryAction = adi_fpgagen6_SerializerResetSet(device, 0xFFU, ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            recoveryAction = adi_fpgagen6_DeserializerResetSet(device, 0xFFU, ADI_ENABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }

        /* Configure Qpll */
        {
            recoveryAction = adi_fpgagen6_QpllCfgSet(device, ADI_FPGAGEN6_QPLL_QUAD_ALL, &fpgaInit->clocks.qpll);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }

        /* Configure the enabled SERDES lanes and build up a channel mask */
        {
            for (serdesIdx = 0U; serdesIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; serdesIdx++)
            {
                
                recoveryAction = adi_fpgagen6_SerializerCfgSet(device, serdesIdx, &fpgaInit->datapath.serCfg[serdesIdx]);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
                if (fpgaInit->datapath.serCfg[serdesIdx].commonCfg.powerDown == ADI_FALSE)
                {
                    serResetChMask |= (1U << serdesIdx);
                    txLaneRate_kHz = fpgaInit->datapath.serCfg[serdesIdx].commonCfg.laneRate_kHz;
                }

                recoveryAction = adi_fpgagen6_DeserializerCfgSet(device, serdesIdx, &fpgaInit->datapath.desCfg[serdesIdx]);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
                if (fpgaInit->datapath.desCfg[serdesIdx].commonCfg.powerDown == ADI_FALSE)
                {
                    desResetChMask |= (1U << serdesIdx);
                    rxLaneRate_kHz = fpgaInit->datapath.desCfg[serdesIdx].commonCfg.laneRate_kHz;
                }

                if ((txLaneRate_kHz != 0U) || (rxLaneRate_kHz != 0U))
                {
                    /* If one of the lanes is powered down, use the same lane rate as the enabled lane. */
                    txLaneRate_kHz = (txLaneRate_kHz == 0U) ? rxLaneRate_kHz : txLaneRate_kHz;
                    rxLaneRate_kHz = (rxLaneRate_kHz == 0U) ? txLaneRate_kHz : rxLaneRate_kHz;

                    recoveryAction = adi_fpgagen6_DrpQpllChannelCfgSet(device, fpgaInit->clocks.qpll.refClk_kHz, txLaneRate_kHz, rxLaneRate_kHz, (adi_fpgagen6_DrpInterface_e)(serdesIdx));
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }
                }
                rxLaneRate_kHz = 0U;
                txLaneRate_kHz = 0U;
            }
        }

        /* Bring the enabled SERDES lanes out of reset */
        {
            recoveryAction = adi_fpgagen6_SerializerResetSet(device, serResetChMask, ADI_DISABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            recoveryAction = adi_fpgagen6_DeserializerResetSet(device, desResetChMask, ADI_DISABLE);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }

        /* Check the QPLLs in each quad for lock status */
        {
            for (qpllQuadIdx = 0U; qpllQuadIdx < ADI_FPGAGEN6_PHY_QUAD_COUNT; qpllQuadIdx++)
            {
                for (qpllLockCheckIdx = 0U; qpllLockCheckIdx < ADI_FPGAGEN6_QPLL_LOCK_TIMEOUT; qpllLockCheckIdx++)
                {
                    recoveryAction = adi_fpgagen6_QpllLockGet(device, qpllQuadIdx, fpgaInit->clocks.qpll.qpllClockSettings.laneClkSel, &qpllLockStatus);
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }

                    if (qpllLockStatus != 0U)
                    {
                        break;
                    }

                    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }
                }

                if (qpllLockStatus == 0U)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
        }
        /* Check the enabled serializer lanes came out of reset */
        {
            for (serdesResetDoneIdx = 0; serdesResetDoneIdx < ADI_FPGAGEN6_RESET_FIXED_LOOP; serdesResetDoneIdx++)
            {
                recoveryAction = adi_fpgagen6_SerializerResetDoneGet(device, &serdesResetStatus);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                if (serdesResetStatus == serResetChMask)
                {
                    break;
                }

                recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
            
            if (serdesResetStatus != serResetChMask)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }

        /* Check the enabled deserializer lanes came out of reset */
        {
            for (serdesResetDoneIdx = 0U; serdesResetDoneIdx < ADI_FPGAGEN6_RESET_FIXED_LOOP; ++serdesResetDoneIdx)
            {
                recoveryAction = adi_fpgagen6_DeserializerResetDoneGet(device, &serdesResetStatus);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }

                if (serdesResetStatus == desResetChMask)
                {
                    break;
                }

                recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }

            if (serdesResetStatus != desResetChMask)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                        ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
        }
    }

    /***************************************************************************************************************************************
    * MMCMs
    ***************************************************************************************************************************************/
    {
        recoveryAction = fpgagen6_MmcmConfigureAndLockCheck(device, ADI_FPGAGEN6_CORE_MMCM, &fpgaInit->clocks.coreMmcm);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }      
    }
    
    /***************************************************************************************************************************************
    * Lane Xbars
    ***************************************************************************************************************************************/
    {
        recoveryAction = adi_fpgagen6_LaneXBarResetConfigure(device, fpgaInit, &laneResetMaskInfo);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        recoveryAction = adi_fpgagen6_LaneXBarResetSet(device, &laneResetMaskInfo);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
        
    /***************************************************************************************************************************************
    * Link layer
    ***************************************************************************************************************************************/
    {
        /* Deframers- Configure only those with valid lane assignments but put all of them into reset to prepare for link bringup */
        {
            for (deframerIdx = 0U; deframerIdx < ADI_FPGAGEN6_MAX_DEFRAMERS; deframerIdx++)
            {
                deframerSel = 1U << deframerIdx;
                if (fpgaInit->datapath.deframerCfg[deframerIdx].lanesInUse != 0U)
                {
                    recoveryAction = adi_fpgagen6_DeframerCfgSet(device, (adi_fpgagen6_DeframerSel_e)(deframerSel), &fpgaInit->datapath.deframerCfg[deframerIdx]);
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }
                }

                recoveryAction = adi_fpgagen6_DeframerResetSet(device, (adi_fpgagen6_DeframerSel_e)(deframerSel));
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
        }
        
        /* Framers- Configure only those with valid lane assignments but put all of them into reset to prepare for link bringup */
        {
            for (framerIdx = 0U; framerIdx < ADI_FPGAGEN6_MAX_FRAMERS; ++framerIdx)
            {
                framerSel = 1U << framerIdx;
                if (fpgaInit->datapath.framerCfg[framerIdx].lanesInUse != 0U)
                {
                    recoveryAction = adi_fpgagen6_FramerCfgSet(device, (adi_fpgagen6_FramerSel_e)(framerSel), &fpgaInit->datapath.framerCfg[framerIdx]);
                    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                    {
                        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                                ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                                ADI_NO_VARIABLE,
                                                                recoveryAction,
                                                                cleanup);
                    }
                }
                
                recoveryAction = adi_fpgagen6_FramerResetSet(device, (adi_fpgagen6_FramerSel_e)(framerSel));
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
        }
    }

    /***************************************************************************************************************************************
    * Sync XBar
    ***************************************************************************************************************************************/
    {
        recoveryAction = adi_fpgagen6_SyncXBarCfgSet(device, &fpgaInit->datapath.syncXBarCfg);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /***************************************************************************************************************************************
    * Sysref
    ***************************************************************************************************************************************/
    {
        recoveryAction = adi_fpgagen6_SysRefControlSet(device, &fpgaInit->sysref);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    
    /***************************************************************************************************************************************
    * Datachain- Transport, DMA
    ***************************************************************************************************************************************/
    {
        /* Rx DMAs */
        {
            for (dmaIdx = 0U; dmaIdx < ADI_FPGAGEN6_RX_DMA_COUNT; ++dmaIdx)
            {
                /* Configure the DMAs */
                recoveryAction = adi_fpgagen6_RxDmaCfgSet(device, dmaIdx, &fpgaInit->datapath.rxDmaCfg[dmaIdx]);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
        }
        
        /* Tx DMAs */
        {
            for (dmaIdx = 0U; dmaIdx < ADI_FPGAGEN6_TX_DMA_COUNT; ++dmaIdx)
            {
                /* Configure the DMAs */
                recoveryAction = adi_fpgagen6_TxDmaCfgSet(device, dmaIdx, &fpgaInit->datapath.txDmaCfg[dmaIdx]);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                            ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
        }
        
        /* Reset Tx & Rx Datachains to get them in good state */
        recoveryAction = adi_fpgagen6_RxDataChainReset(device);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        
        recoveryAction = adi_fpgagen6_TxDataChainReset(device);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    
    /***************************************************************************************************************************************
    * TDD
    ***************************************************************************************************************************************/
    {
        recoveryAction = adi_fpgagen6_TddFsmDisable(device);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_UTILITIES,
                                                    ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    
    /***************************************************************************************************************************************
    * GPIO
    ***************************************************************************************************************************************/
    {
        //TODO
    }
    
cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}
