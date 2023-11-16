/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file: adi_fpgagen6_clk.c
 *
 * \brief Contains top level fpgagen6 clock related functions
 *
 * FPGA API Version: 2.9.0.4
 */

#include "adi_fpgagen6_clk.h"
#include "adi_fpgagen6_hal.h"
#include "adi_fpgagen6_core.h"

#include "../../private/bf/fpgagen6_bf_xilinx_clock_wizard.h"
#include "../../private/bf/fpgagen6_bf_axi_adrv904x_phy.h"
#include "../../private/include/fpgagen6_drp.h"
#include "../../private/include/fpgagen6_data_interface.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_CLK

/**
* --brief  Used to encode and decode Qpll M value according the Xilinx UserGuide 578
*
* - inout param mEncoded-> when encode is True, the function will return the encoded value here; when False user should pass in the value to decode here
* - inout param mActual-> when encode is True, user should pass in the value to encode here; when False the function will return the decoded value here
* - in param encode-> True will encode mActual value to mEncoded; when False will decode mEncoded to mActual
*/
static adi_fpgagen6_ErrAction_e fpgagen6_QpllmTranslate(adi_fpgagen6_Device_t* const    device,
                                                        uint16_t* const                 mEncoded,
                                                        uint16_t* const                 mActual,
                                                        const uint8_t                   encode);


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmCfgSet(   adi_fpgagen6_Device_t* const        device,
                                                            const adi_fpgagen6_MmcmSel_e        mmcmSel,
                                                            const adi_fpgagen6_MmcmCfg_t* const mmcmCfgSet)
{
    adi_fpgagen6_ErrAction_e                recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfXilinxClockWizardChanAddr_e  baseAddr        = FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ;
    uint8_t                                 i               = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, mmcmCfgSet, cleanup);

    switch (mmcmSel)
    {
        case ADI_FPGAGEN6_CORE_MMCM:
            baseAddr = FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ;
            break;

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, mmcmSel, "Invalid mmcm");
            goto cleanup;
    }

    /* Reset the clock wizard */
    /* Workaround until bitfield functions are updated for word-sized access
    recoveryAction = fpgagen6_XilinxClockWizard_SoftwareReset_BfSet(device,
                                                                  baseAddr,
                                                                  0xAU);
    */
    recoveryAction = adi_fpgagen6_RegisterWrite(device, baseAddr, 0xAU);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Clear unused bitfields */
    recoveryAction = fpgagen6_XilinxClockWizard_ClkfboutFrac_BfSet( device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_XilinxClockWizard_ClkfboutPhase_BfSet( device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    
    recoveryAction = fpgagen6_XilinxClockWizard_Clkout0Frac_BfSet( device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Write the configuration registers & default the phase & duty */
    recoveryAction = fpgagen6_XilinxClockWizard_ClkfboutMult_BfSet( device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->mVal);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_DivclkDivide_BfSet( device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->dVal);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout0Divide_BfSet(device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->outputClkDiv[0U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout0Phase_BfSet(device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout0Duty_BfSet(device,
                                                                    baseAddr,
                                                                    0xC350U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout1Divide_BfSet(device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->outputClkDiv[1U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout1Phase_BfSet(device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout1Duty_BfSet(device,
                                                                    baseAddr,
                                                                    0xC350U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout2Divide_BfSet(device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->outputClkDiv[2U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout2Phase_BfSet(device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout2Duty_BfSet(device,
                                                                    baseAddr,
                                                                    0xC350U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout3Divide_BfSet(device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->outputClkDiv[3U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout3Phase_BfSet(device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout3Duty_BfSet(device,
                                                                    baseAddr,
                                                                    0xC350U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout4Divide_BfSet(device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->outputClkDiv[4U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout4Phase_BfSet(device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout4Duty_BfSet(device,
                                                                    baseAddr,
                                                                    0xC350U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout5Divide_BfSet(device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->outputClkDiv[5U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout5Phase_BfSet(device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout5Duty_BfSet(device,
                                                                    baseAddr,
                                                                    0xC350U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout6Divide_BfSet(device,
                                                                    baseAddr,
                                                                    mmcmCfgSet->outputClkDiv[6U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout6Phase_BfSet(device,
                                                                    baseAddr,
                                                                    0U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout6Duty_BfSet(device,
                                                                    baseAddr,
                                                                    0xC350U);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Set the saddr bitfield to indicate current configuration register values are for dynamic reconfiguration*/
    recoveryAction = fpgagen6_XilinxClockWizard_Saddr_BfSet(device, baseAddr, ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Set load bitfield to start the dynamic reconfiguration process*/
    recoveryAction = fpgagen6_XilinxClockWizard_Load_BfSet(device, baseAddr, ADI_ENABLE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Store Desired Channel Frequencies */
    for (i = 0U; i < ADI_FPGAGEN6_NUM_MMCM_OUTPUT_CLOCKS; ++i)
    {
        device->devStateInfo.outputClkFreq_kHz[i] = mmcmCfgSet->outputClkFreq_kHz[i];
    }

cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmCfgGet(   adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_MmcmSel_e    mmcmSel,
                                                            adi_fpgagen6_MmcmCfg_t* const   mmcmCfgGet)
{

    adi_fpgagen6_ErrAction_e                recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfXilinxClockWizardChanAddr_e  baseAddr        = FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ;
    uint8_t                                 i               = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, mmcmCfgGet, cleanup);

    (void)mmcmSel; /* Currently Unused */

    /* Read the configuration registers */
    recoveryAction = fpgagen6_XilinxClockWizard_ClkfboutMult_BfGet( device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->mVal);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_DivclkDivide_BfGet( device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->dVal);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout0Divide_BfGet(device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->outputClkDiv[0U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout1Divide_BfGet(device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->outputClkDiv[1U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout2Divide_BfGet(device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->outputClkDiv[2U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout3Divide_BfGet(device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->outputClkDiv[3U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout4Divide_BfGet(device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->outputClkDiv[4U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout5Divide_BfGet(device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->outputClkDiv[5U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_XilinxClockWizard_Clkout6Divide_BfGet(device,
                                                                    baseAddr,
                                                                    &mmcmCfgGet->outputClkDiv[6U]);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    /* Get Desired Channel Frequencies */
    for (i = 0U; i < ADI_FPGAGEN6_NUM_MMCM_OUTPUT_CLOCKS; ++i)
    {
        mmcmCfgGet->outputClkFreq_kHz[i] = device->devStateInfo.outputClkFreq_kHz[i];
    }

cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmLockGet(  adi_fpgagen6_Device_t* const    device,
                                                            const adi_fpgagen6_MmcmSel_e    mmcmSel,
                                                            uint8_t* const                  lockBit)
{
    adi_fpgagen6_ErrAction_e                recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfXilinxClockWizardChanAddr_e  baseAddr       = FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, lockBit, cleanup);

    switch (mmcmSel)
    {
        case ADI_FPGAGEN6_CORE_MMCM:
            baseAddr = FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ;
            break;

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mmcmSel,
                                    "Invalid mmcm");
            goto cleanup;
    }

    /* Load bit returns to 0 after reconfiguring and locked */
    recoveryAction = fpgagen6_XilinxClockWizard_Locked_BfGet( device, baseAddr, lockBit);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_LOCK_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_MmcmResetSet( adi_fpgagen6_Device_t* const device,
                                                            const adi_fpgagen6_MmcmSel_e mmcmSel)
{
    adi_fpgagen6_ErrAction_e                recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    fpgagen6_BfXilinxClockWizardChanAddr_e  baseAddr       = FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    switch (mmcmSel)
    {
        case ADI_FPGAGEN6_CORE_MMCM:
            baseAddr = FPGAGEN6_BF_ADS10_TOP_SYSTEM_TIMING_LINK_CLKWIZ;
            break;

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mmcmSel,
                                    "Invalid MMCM Selected");
            goto cleanup;
    }

    /* Reset the clock wizard */
    /* Workaround until bitfield functions are updated for word-sized access
    recoveryAction = fpgagen6_XilinxClockWizard_SoftwareReset_BfSet(device,
                                                                  baseAddr,
                                                                  0xA);
    */
    recoveryAction = adi_fpgagen6_RegisterWrite(device, baseAddr, 0xAU);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_MMCM_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllCfgSet(   adi_fpgagen6_Device_t*  const device,
                                                            const uint8_t                 quadMask,
                                                            const adi_fpgagen6_QpllCfg_t* const qPll)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t  powerGood = 0U;
    uint32_t quadIdx = 0U;
    uint8_t  fpgaQpllSetupIdx = 0U;
    uint8_t powerGoodExpected = 0U;
    
    /*  Function pointers allow the Qpll locking sequence for either Qpll0 or Qpll1 to use
     *  the same code flow and reduce duplicated code when the sequence is similar but with
     *  different bitfield function calls
     **/
    typedef struct
    {
        adi_fpgagen6_ErrAction_e(*QpllPdClear)( adi_fpgagen6_Device_t* const            device,
                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                uint8_t,
                                                const uint8_t);

        adi_fpgagen6_ErrAction_e(*QpllPdAssert)(adi_fpgagen6_Device_t* const            device,
                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                uint8_t,
                                                const uint8_t);

        adi_fpgagen6_ErrAction_e(*QpllResetSet)(adi_fpgagen6_Device_t* const            device,
                                                const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e  baseAddr,
                                                uint8_t,
                                                const uint8_t);
    } fpgaQpllSetup_t;
    
    /* Array of function pointers for each Qpll currently supported
     * fpgaQpllSetupFct[0] for setting up Qpll0
     * fpgaQpllSetupFct[1] for setting up Qpll1
     * Note: The selected Qpll will select the appropriate index. The other Qpll
     * pd bitfield is included here because we need to ensure that other qpll
     * is powered down.
     * */
    fpgaQpllSetup_t fpgaQpllSetupFct[2];
    ADI_LIBRARY_MEMSET(&fpgaQpllSetupFct, 0, sizeof(fpgaQpllSetupFct));

    fpgaQpllSetupFct[0].QpllPdClear = fpgagen6_AxiAdrvgen6Phy_QuadQpll0Pd_BfSet;
    fpgaQpllSetupFct[0].QpllPdAssert = fpgagen6_AxiAdrvgen6Phy_QuadQpll1Pd_BfSet;
    fpgaQpllSetupFct[0].QpllResetSet = fpgagen6_AxiAdrvgen6Phy_QuadQpll0Reset_BfSet;

    fpgaQpllSetupFct[1].QpllPdClear = fpgagen6_AxiAdrvgen6Phy_QuadQpll1Pd_BfSet;
    fpgaQpllSetupFct[1].QpllPdAssert = fpgagen6_AxiAdrvgen6Phy_QuadQpll0Pd_BfSet;
    fpgaQpllSetupFct[1].QpllResetSet = fpgagen6_AxiAdrvgen6Phy_QuadQpll1Reset_BfSet;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, qPll, cleanup);

    /* For the quadMask passed in we either program Qpll0 or Qpll1. The function
     * pointers above and the quadIdx loop allow for a single set of functions
     * to be used.
     * */
    for (quadIdx = 0U; quadIdx < ADI_FPGAGEN6_PHY_QUAD_COUNT; quadIdx++)
    {
        powerGoodExpected = (0xFU) << (4U * quadIdx);
        
        if (qPll->qpllClockSettings.laneClkSel == ADI_FPGAGEN6_QPLLCLK0)
        {
            fpgaQpllSetupIdx = 0U;
        }
        else if (qPll->qpllClockSettings.laneClkSel == ADI_FPGAGEN6_QPLLCLK1)
        {
            fpgaQpllSetupIdx = 1U;
        }
        else
        {
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                    ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        if (((quadMask >> quadIdx) & 0x1U) > 0U)
        {
            /* set ref clk for qpll*/
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_QuadQpll0RefClkSel_BfSet( device,
                                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                            quadIdx,
                                                                            qPll->refClkSelect[quadIdx]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                        ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            recoveryAction = fpgagen6_AxiAdrvgen6Phy_QuadQpll1RefClkSel_BfSet( device,
                                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                            quadIdx,
                                                                            qPll->refClkSelect[quadIdx]);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                        ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }
            
            /* Power up the selected Qpll */
            if (fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllPdClear != NULL)
            {
                recoveryAction = fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllPdClear(device,
                                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                                quadIdx,
                                                                                ADI_DISABLE);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                            ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
            
            /* Power down the other Qpll */
            if (fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllPdAssert != NULL)
            {
                recoveryAction = fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllPdAssert(   device,
                                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                                    quadIdx,
                                                                                    ADI_ENABLE);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                            ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }

            /* Write out the DRP registers */
            recoveryAction = adi_fpgagen6_DrpQpllCommonCfgSet(device, qPll->refClk_kHz, qPll->qpllClockSettings.laneRate_kHz);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                        ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            /* Ensure power good is set for all channels*/
            recoveryAction = adi_fpgagen6_LanePowerGoodGet(device, quadIdx, &powerGood);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                        ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
            }

            if ((powerGood & powerGoodExpected) != powerGoodExpected)
            {
                recoveryAction = ADI_FPGAGEN6_ERR_ACT_RESET_DEVICE;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        powerGood,
                                        "One of more quad channels reporting invalid power good");
                goto cleanup;
            }
            
            /* Bring the selected Qpll out of reset */
            if (fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllResetSet != NULL)
            {
                recoveryAction = fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllResetSet(   device,
                                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                                    quadIdx,
                                                                                    ADI_ENABLE);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                {
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                            ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,
                                                            ADI_NO_VARIABLE,
                                                            recoveryAction,
                                                            cleanup);
                }
            }
            
            /*  Clear the reset bit to the QPLL after setting it high */
            if (fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllResetSet != NULL)
            {
                recoveryAction = fpgaQpllSetupFct[fpgaQpllSetupIdx].QpllResetSet( device,
                                                                                 FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                                 quadIdx,
                                                                                 ADI_DISABLE);
                if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
                { 
                    ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK, 
                                                            ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET, 
                                                            ADI_NO_VARIABLE, 
                                                            recoveryAction, 
                                                            cleanup); 
                }
            }
        }
    }
    
cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllCfgGet(   adi_fpgagen6_Device_t* const    device,
                                                            const uint8_t                   quadSel,
                                                            adi_fpgagen6_QpllCfg_t* const   qPll)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_fpgagen6_DrpInterface_e drpSel = ADI_FPGAGEN6_DRP_INTERFACE_QUAD0;
    uint8_t qpll0PdReadback = 0U;
    uint8_t qpll1PdReadback = 0U;
    uint16_t qpllNAddr = 0U;
    uint16_t qpllMAddr = 0U;
    uint16_t clkoutRateAddr = 0U;
    uint16_t mEncodedVal = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, qPll, cleanup);

    switch (quadSel)
    {
        case 0:
            drpSel = ADI_FPGAGEN6_DRP_INTERFACE_QUAD0;
            break;

        case 1:
            drpSel = ADI_FPGAGEN6_DRP_INTERFACE_QUAD1;
            break;

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    quadSel,
                                    "Unsupported quad selection");
            goto cleanup;
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_QuadQpll0Pd_BfGet(device,
                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                            (uint8_t)quadSel,
                                                            &qpll0PdReadback);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_AxiAdrvgen6Phy_QuadQpll1Pd_BfGet(device,
                                                            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                            (uint8_t)quadSel,
                                                            &qpll1PdReadback);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    if ((qpll0PdReadback == 0U) && (qpll1PdReadback == 1U))
    {
        qpllNAddr = ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_N;
        qpllMAddr = ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_M;
        clkoutRateAddr = ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_CLKOUT_RATE;
        qPll->qpllClockSettings.laneClkSel = ADI_FPGAGEN6_QPLLCLK0;
    }
    else if ((qpll0PdReadback == 1U) && (qpll1PdReadback == 0U))
    {
        qpllNAddr = ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK1_N;
        qpllMAddr = ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK1_M;
        clkoutRateAddr = ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK1_CLKOUT_RATE;
        qPll->qpllClockSettings.laneClkSel = ADI_FPGAGEN6_QPLLCLK1;
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_RESET_DEVICE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                quadSel,
                                "Invalid configuration detected. Reset and re-program fpga");
        goto cleanup;
    }

    recoveryAction = adi_fpgagen6_DrpFieldRead( device,
                                                qpllNAddr,
                                                drpSel,
                                                &qPll->qpllClockSettings.nVal,
                                                ADI_FPGAGEN6_DRP_MASK_QPLL_N);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }
    qPll->qpllClockSettings.nVal += 2U;

    recoveryAction = adi_fpgagen6_DrpFieldRead( device,
                                                qpllMAddr,
                                                drpSel,
                                                &mEncodedVal,
                                                ADI_FPGAGEN6_DRP_MASK_QPLL_M);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET,
                                                mEncodedVal,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = fpgagen6_QpllmTranslate(device, &mEncodedVal, &qPll->qpllClockSettings.mVal, ADI_FALSE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET,
                                                qPll->qpllClockSettings.mVal,
                                                recoveryAction,
                                                cleanup);
    }

    recoveryAction = adi_fpgagen6_DrpFieldRead( device,
                                                clkoutRateAddr,
                                                drpSel,
                                                &qPll->qpllClockSettings.qpllClkOutRate,
                                                ADI_FPGAGEN6_DRP_MASK_QPLL_CLKOUT_RATE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET,
                                                qPll->qpllClockSettings.qpllClkOutRate,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_QpllLockGet(  adi_fpgagen6_Device_t* const    device,
                                                            const uint8_t                   quadSel,
                                                            const adi_fpgagen6_LaneClkSel_e laneClkSel,
                                                            uint8_t* const                  lockBit)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, lockBit, cleanup);

    if (quadSel >= ADI_FPGAGEN6_PHY_QUAD_COUNT)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                quadSel,
                                "Unsupported quad selection");
        goto cleanup;
    }
    
    if (laneClkSel == ADI_FPGAGEN6_QPLLCLK0)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Phy_QuadQpll0Lock_BfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    quadSel,
                                                                    lockBit);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                    ADI_FPGAGEN6_ERRCODE_CLK_QPLL_LOCK_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    else if (laneClkSel == ADI_FPGAGEN6_QPLLCLK1)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Phy_QuadQpll1Lock_BfGet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    quadSel,
                                                                    lockBit);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                    ADI_FPGAGEN6_ERRCODE_CLK_QPLL_LOCK_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                laneClkSel,
                                "Unsupported qpll selection");
        goto cleanup;
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_LanePowerGoodGet( adi_fpgagen6_Device_t* const    device,
                                                                const uint8_t                   quadSel,
                                                                uint8_t* const                  powerGood)
{
    (void)quadSel;
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t channelIdx = 0U;
    uint8_t channelGoodReadback = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, powerGood, cleanup);

    *powerGood = 0U;

    for (channelIdx = 0U; channelIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; ++channelIdx)
    {
        recoveryAction = fpgagen6_AxiAdrvgen6Phy_ChannelGtpowergood_BfGet( device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        channelIdx,
                                                                        &channelGoodReadback);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CLK,
                                                    ADI_FPGAGEN6_ERRCODE_CLK_LANE_PWR_GOOD_GET,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
        *powerGood |= (channelGoodReadback << channelIdx);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

static adi_fpgagen6_ErrAction_e fpgagen6_QpllmTranslate(adi_fpgagen6_Device_t* const    device,
                                                        uint16_t* const                 mEncoded,
                                                        uint16_t* const                 mActual,
                                                        const uint8_t                   encode)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, mActual);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, mEncoded);

    /* All values taken from page 415 of ug578 */
    if (encode == ADI_FALSE)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;

        switch (*mEncoded)
        {
            case 0U:
                *mActual = 2U;
                break;
            case 1U:
                *mActual = 3U;
                break;
            case 2U:
                *mActual = 4U;
                break;
            case 3U:
                *mActual = 5U;
                break;
            case 5U:
                *mActual = 6U;
                break;
            case 6U:
                *mActual = 8U;
                break;
            case 7U:
                *mActual = 10U;
                break;
            case 13U:
                *mActual = 12U;
                break;
            case 14U:
                *mActual = 16U;
                break;
            case 15U:
                *mActual = 20U;
                break;
            case 16U:
                *mActual = 1U;
                break;
            default:
                recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        mEncoded,
                                        "Invalid encoded M value");
                return recoveryAction;
                break;
        }
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;

        switch (*mActual)
        {
            case 2U:
                *mEncoded = 0U;
                break;
            case 3U:
                *mEncoded = 1U;
                break;
            case 4U:
                *mEncoded = 2U;
                break;
            case 5U:
                *mEncoded = 3U;
                break;
            case 6U:
                *mEncoded = 5U;
                break;
            case 8U:
                *mEncoded = 6U;
                break;
            case 10U:
                *mEncoded = 7U;
                break;
            case 12U:
                *mEncoded = 13U;
                break;
            case 16U:
                *mEncoded = 14U;
                break;
            case 20U:
                *mEncoded = 15U;
                break;
            case 1U:
                *mEncoded = 16U;
                break;
            default:
                recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT( &device->common,
                                        recoveryAction,
                                        mActual,
                                        "Invalid actual M value");
                return recoveryAction;
                break;
        }
    }

    return recoveryAction;
}
