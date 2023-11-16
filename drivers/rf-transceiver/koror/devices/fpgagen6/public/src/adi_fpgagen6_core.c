/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_core.c
* \brief Contains FPGA features related function implementation defined in
* adi_fpgagen6_core.h
*
* FPGA API Version: 2.9.0.4
*/

/* Header file corresponding to the C file */
#include "adi_fpgagen6_core.h"
#include "adi_fpgagen6_hal.h"
#include "adi_fpgagen6_gpio.h"

#include "../../private/bf/fpgagen6_bf_axi_adrv904x_phy.h"
#include "../../private/bf/fpgagen6_bf_axi_memory_pager.h"
#include "../../private/bf/fpgagen6_bf_axi_ads10v1.h"
#include "../../private/include/fpgagen6_drp.h"
#include "../../private/include/fpgagen6_data_interface.h"


#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_CORE

#ifndef ADI_FPGAGEN6_LOGGING
#define ADI_FPGAGEN6_LOGGING    (uint32_t)  (   ADI_HAL_LOG_MSG     | \
                                                ADI_HAL_LOG_WARN    | \
                                                ADI_HAL_LOG_ERR     | \
                                                ADI_HAL_LOG_API)        /* LogLevel Set to API, Messages, Warnings, Error */
#endif /* !ADI_FPGAGEN6_LOGGING */
    
/**
* --brief  Used to populate the necessary channel and index values in the API functions below for the selected DRP interface
*
* - out param drpChannel will hold the correct value to be used as channelId in bitfield calls
* - out param drpFunctionIdx will hold the correct index to select the DRP functions assigned to the function pointers
* - out param drpDataRegister will hold the FPGA register address of the DRP Data & Data Ready register
*/
static adi_fpgagen6_ErrAction_e fpgagen6_DmaFunctionInfoGet(adi_fpgagen6_Device_t* const device,
                                                            const adi_fpgagen6_DrpInterface_e drp,
                                                            uint8_t* const drpChannel,
                                                            uint8_t* const drpFunctionIdx,
                                                            uint32_t* const drpDataRegister);

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Lock(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    /* Effectively we call adi_common_hal_ApiEnter and exit but don't release the lock on exit */
    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_TRUE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        return recoveryAction;
    }

    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        return recoveryAction;
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_Unlock(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    /* Effectively we call adi_common_hal_ApiEnter and exit but don't take the lock on enter */
    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        return recoveryAction;
    }

    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_TRUE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        return recoveryAction;
    }

    return recoveryAction;
}

/* TODO BOM - This impl needs to be completed. Currently it's doing the minimum in order
 * to call common_HwOpen and init the hal_mutex_t */
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_HwOpen(adi_fpgagen6_Device_t* const device, adi_fpgagen6_HwOpenCfg_t* const hwOpenCfg)
{
    adi_fpgagen6_ErrAction_e recoveryAction       = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_fpgagen6_ErrAction_e exitRecoveryAction   = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    /* ApiEnter ADI_FALSE => Do all the normal API enter jobs *except* take the mutex
     * This is because we don't have a mutex initialized until commonOpen completes. */
    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }

    if (!ADI_COMMON_DEVICE_STATE_IS_OPEN(device->common))
    {
        /* Perform Hardware Open Once */
        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_HwOpen(&device->common);
        if (recoveryAction != ADI_FPGAGEN6_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Common HAL HwOpen Issue");
            goto cleanup;
        }

        /* Indicate device is now open or else HwReset will fail */
        ADI_COMMON_DEVICE_STATE_OPEN_SET(device->common);

        /* Earliest Point for Logging */
        adi_common_LogLevelSet(&device->common, ADI_FPGAGEN6_LOGGING);

        ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API);
    }

    device->memoryPageSize = hwOpenCfg->memoryPageSize;

cleanup:
    /* ADI_FALSE to not release mutex as we did not take it at the top of the fn */
    exitRecoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != exitRecoveryAction)
    {
        recoveryAction = exitRecoveryAction;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_HwClose(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction       = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_fpgagen6_ErrAction_e exitRecoveryAction   = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);
    /* ApiEnter ADI_FALSE => Do all the normal API enter jobs *except* take the mutex
     * This is because we don't commonHwClose will be destroying the mutex. */
    recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }

    if (ADI_COMMON_DEVICE_STATE_IS_OPEN(device->common))
    {
        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_HwClose(&device->common);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Common HAL HwClose Issue");
            goto cleanup;
        }
        /* Indicate the Device is now closed */
        ADI_COMMON_DEVICE_STATE_OPEN_CLR(device->common);        
    }

cleanup:
    /* ADI_FALSE to not release mutex as we did not take it at the top of the fn */
    exitRecoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != exitRecoveryAction)
    {
        recoveryAction = exitRecoveryAction;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_HwReset(adi_fpgagen6_Device_t* const device)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t serdesIdx = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    /* Zero out all the currently stored device state */
    ADI_LIBRARY_MEMSET(&device->devStateInfo, 0, sizeof(adi_fpgagen6_Info_t));
    
    recoveryAction = fpgagen6_AxiAds10v1_GlobalReset_BfSet( device,
                                                            FPGAGEN6_BF_ADRV904X_ADS10_ADS10V1_TOP_AXI_ADS10V1,
                                                            ADI_TRUE);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                ADI_FPGAGEN6_ERRCODE_CORE_HW_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

    for (serdesIdx = 0U; serdesIdx < ADI_FPGAGEN6_LANES_PHYSICAL_NUM; serdesIdx++)
    {
        recoveryAction = adi_fpgagen6_DrpQpllChannelCfgSet(device, ADI_FPGAGEN6_DEFAULT_REF_CLK_KHZ, ADI_FPGAGEN6_DEFAULT_LANE_RATE_KHZ, ADI_FPGAGEN6_DEFAULT_LANE_RATE_KHZ, (adi_fpgagen6_DrpInterface_e)(serdesIdx));
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_HW_RESET,
                                                    serdesIdx,
                                                    recoveryAction,
                                                    cleanup);
        }
    }
    
    recoveryAction = adi_fpgagen6_DrpQpllCommonCfgSet(device, ADI_FPGAGEN6_DEFAULT_REF_CLK_KHZ, ADI_FPGAGEN6_DEFAULT_LANE_RATE_KHZ);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                ADI_FPGAGEN6_ERRCODE_CORE_HW_RESET,
                                                ADI_NO_VARIABLE,
                                                recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DrpFieldRead( adi_fpgagen6_Device_t* const        device,
                                                            const uint16_t addr,
                                                            const adi_fpgagen6_DrpInterface_e drp,
                                                            uint16_t* const                     rdWord,
                                                            const uint16_t rdMask)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t startBit = ADI_FPGAGEN6_DRP_REGISTER_WIDTH; /* Default to an invalid start bit */
    uint8_t idx = 0U;
    uint8_t drpChannel = 0U;
    uint8_t drpFunctionIdx = 0U;
    uint8_t drpReady = 0U;
    uint8_t drpError = 0U;
    uint32_t drpReadyDataRead = 0U;
    uint32_t drpDataReadAddress = 0U;
    
    /*  Function pointers allow the Quad and Channel DRP interfaces to use the same code flow allowing
     *  for less repeated code, since the sequence is the exact same with the only difference being
     *  the bitfield function names.
     **/
    typedef struct
    {
        adi_fpgagen6_ErrAction_e(*DrpAddressBfSet)( adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint16_t);

        adi_fpgagen6_ErrAction_e(*DrpWriteEnableBfSet)( adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint8_t);

        adi_fpgagen6_ErrAction_e(*DrpEnableBfSet)(  adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint8_t);

        adi_fpgagen6_ErrAction_e(*DrpReadyBfGet)(   adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            uint8_t* const);

        adi_fpgagen6_ErrAction_e(*DrpErrorBfGet)(   adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            uint8_t* const);

        adi_fpgagen6_ErrAction_e(*DrpReadDataBfGet)(adi_fpgagen6_Device_t* const device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            uint16_t* const);
    } fpgaDrpReadBf_t;
    
    fpgaDrpReadBf_t fpgaDrpReadFct[2];
    ADI_LIBRARY_MEMSET(&fpgaDrpReadFct, 0, sizeof(fpgaDrpReadFct));

    fpgaDrpReadFct[0].DrpAddressBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpAddress_BfSet;
    fpgaDrpReadFct[0].DrpWriteEnableBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteEnable_BfSet;
    fpgaDrpReadFct[0].DrpEnableBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpEnable_BfSet;
    fpgaDrpReadFct[0].DrpReadyBfGet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpReady_BfGet;
    fpgaDrpReadFct[0].DrpErrorBfGet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpError_BfGet;
    fpgaDrpReadFct[0].DrpReadDataBfGet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpReadData_BfGet;

    fpgaDrpReadFct[1].DrpAddressBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpAddress_BfSet;
    fpgaDrpReadFct[1].DrpWriteEnableBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteEnable_BfSet;
    fpgaDrpReadFct[1].DrpEnableBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpEnable_BfSet;
    fpgaDrpReadFct[1].DrpReadyBfGet = fpgagen6_AxiAdrvgen6Phy_QuadDrpReady_BfGet;
    fpgaDrpReadFct[1].DrpErrorBfGet = fpgagen6_AxiAdrvgen6Phy_QuadDrpError_BfGet;
    fpgaDrpReadFct[1].DrpReadDataBfGet = fpgagen6_AxiAdrvgen6Phy_QuadDrpReadData_BfGet;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, rdWord, cleanup);

    recoveryAction = fpgagen6_DmaFunctionInfoGet(   device,
        drp,
        &drpChannel,
        &drpFunctionIdx,
        &drpDataReadAddress);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid drp selected");
        goto cleanup;
    }

    for (idx = 0U; idx < ADI_FPGAGEN6_DRP_REGISTER_WIDTH; ++idx)
    {
        if (((rdMask >> idx) & 0x1U) > 0U)
        {
            startBit = idx;
            break;
        }
    }

    if (startBit == ADI_FPGAGEN6_DRP_REGISTER_WIDTH)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            rdMask,
            "A mask of 0U was detected, which is invalid");
        goto cleanup;
    }

    /* Write address */
    if (fpgaDrpReadFct[drpFunctionIdx].DrpAddressBfSet != NULL)
    {
        recoveryAction = fpgaDrpReadFct[drpFunctionIdx].DrpAddressBfSet(device,
                                                                        FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                        drpChannel,
                                                                        addr);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Write enable bit */
    if (fpgaDrpReadFct[drpFunctionIdx].DrpWriteEnableBfSet != NULL)
    {
        recoveryAction = fpgaDrpReadFct[drpFunctionIdx].DrpWriteEnableBfSet(device,
            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
            drpChannel,
            ADI_DISABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Set enable bit */
    if (fpgaDrpReadFct[drpFunctionIdx].DrpEnableBfSet != NULL)
    {
        recoveryAction = fpgaDrpReadFct[drpFunctionIdx].DrpEnableBfSet(   device,
            FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
            drpChannel,
            ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Looping and wait for write to complete */
    for (idx = 0U; idx < ADI_FPGAGEN6_DRP_TIMEOUT_MS; idx++)
    {
        recoveryAction = adi_fpgagen6_RegisterRead(device, drpDataReadAddress, &drpReadyDataRead);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        drpReady = (drpReadyDataRead >> ADI_FPGAGEN6_DRP_READY_START_BIT);
        drpError = (drpReadyDataRead >> ADI_FPGAGEN6_DRP_ERROR_START_BIT);

        if (drpReady || drpError)
        {
            /* Go ahead and set it here, we're checking for error first after this loop */
            *rdWord = drpReadyDataRead & ADI_FPGAGEN6_DRP_DATA_MASK;
            break;
        }
        else
        {
            recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                        ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ_SETUP,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
        }
    }
    }
    
    /* As a convenience, the API will shift the register value readback so 
    * that the bits of interest are in LSB position
    **/
    *rdWord = (*rdWord & rdMask) >> startBit;

    /* Important to check error first because drpReady will be zero in the case of error also. */
    if (drpError != 0U)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ,
                                                drpError,
            recoveryAction,
                                                cleanup);
    }

    if (drpReady != 1U)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ,
                                                drpReady,
            recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_DrpFieldWrite(adi_fpgagen6_Device_t* const device,
                                                            const uint16_t addr,
                                                            const adi_fpgagen6_DrpInterface_e drp,
                                                            uint16_t wrWord,
                                                            const uint16_t wrMask)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t startBit = ADI_FPGAGEN6_DRP_REGISTER_WIDTH; /* Default to an invalid start bit */
    uint8_t idx = 0U;
    uint8_t drpChannel = 0U;
    uint8_t drpFunctionIdx = 0U;
    uint8_t drpReady = 0U;
    uint8_t drpError = 0U;
    uint32_t drpReadyDataRead = 0U;
    uint32_t drpDataReadAddress = 0U;

    /*  Function pointers allow the Quad and Channel DRP interfaces to use the same code flow allowing
     *  for less repeated code, since the sequence is the exact same with the only difference being
     *  the bitfield function names.
     */
    typedef struct
    {
        adi_fpgagen6_ErrAction_e(*DrpWriteDataBfSet)(   adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint16_t);

        adi_fpgagen6_ErrAction_e(*DrpWriteDataMaskBfSet)(   adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint16_t);

        adi_fpgagen6_ErrAction_e(*DrpAddressBfSet)( adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint16_t);

        adi_fpgagen6_ErrAction_e(*DrpWriteEnableBfSet)( adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint8_t);

        adi_fpgagen6_ErrAction_e(*DrpEnableBfSet)(  adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            const uint8_t);

        adi_fpgagen6_ErrAction_e(*DrpReadyBfGet)(   adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            uint8_t* const);

        adi_fpgagen6_ErrAction_e(*DrpErrorBfGet)(   adi_fpgagen6_Device_t* const            device,
            const fpgagen6_BfAxiAdrvgen6PhyChanAddr_e baseAddr,
            uint8_t,
            uint8_t* const);
    } fpgaDrpWriteBf_t;

    fpgaDrpWriteBf_t fpgaDrpWriteFct[2];
    ADI_LIBRARY_MEMSET(&fpgaDrpWriteFct, 0, sizeof(fpgaDrpWriteFct));

    fpgaDrpWriteFct[0].DrpWriteDataBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteData_BfSet;
    fpgaDrpWriteFct[0].DrpWriteDataMaskBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteDataMask_BfSet;
    fpgaDrpWriteFct[0].DrpAddressBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpAddress_BfSet;
    fpgaDrpWriteFct[0].DrpWriteEnableBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpWriteEnable_BfSet;
    fpgaDrpWriteFct[0].DrpEnableBfSet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpEnable_BfSet;
    fpgaDrpWriteFct[0].DrpReadyBfGet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpReady_BfGet;
    fpgaDrpWriteFct[0].DrpErrorBfGet = fpgagen6_AxiAdrvgen6Phy_ChannelDrpError_BfGet;

    fpgaDrpWriteFct[1].DrpWriteDataBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteData_BfSet;
    fpgaDrpWriteFct[1].DrpWriteDataMaskBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteDataMask_BfSet;
    fpgaDrpWriteFct[1].DrpAddressBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpAddress_BfSet;
    fpgaDrpWriteFct[1].DrpWriteEnableBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpWriteEnable_BfSet;
    fpgaDrpWriteFct[1].DrpEnableBfSet = fpgagen6_AxiAdrvgen6Phy_QuadDrpEnable_BfSet;
    fpgaDrpWriteFct[1].DrpReadyBfGet = fpgagen6_AxiAdrvgen6Phy_QuadDrpReady_BfGet;
    fpgaDrpWriteFct[1].DrpErrorBfGet = fpgagen6_AxiAdrvgen6Phy_QuadDrpError_BfGet;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_API_ENTRY(&device->common);

    recoveryAction = fpgagen6_DmaFunctionInfoGet(   device,
                                                    drp,
                                                    &drpChannel,
                                                    &drpFunctionIdx,
                                                    &drpDataReadAddress);
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid Drp selected");
        goto cleanup;
    }
    
    for (idx = 0U; idx < ADI_FPGAGEN6_DRP_REGISTER_WIDTH; idx++)
    {
        if (((wrMask >> idx) & 0x1U) > 0U)
        {
            startBit = idx;
            break;
        }
    }

    if (startBit == ADI_FPGAGEN6_DRP_REGISTER_WIDTH)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            wrMask,
            "A mask of 0U was detected, which is invalid");
        goto cleanup;
    }

    /* 4.  The register value to write. The API will shift this value to the correct location
    *      based on the wrMask value.
    **/
    wrWord = wrWord << startBit;

    if ((wrWord & ~wrMask) > 0U)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            wrWord,
            "The value being written is out of range for the mask provided");
        goto cleanup;
    }

    /* Write data bitfields */
    if (fpgaDrpWriteFct[drpFunctionIdx].DrpWriteDataBfSet != NULL)
    {
        recoveryAction = fpgaDrpWriteFct[drpFunctionIdx].DrpWriteDataBfSet(device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                drpChannel,
                                                                wrWord);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Write mask bitfields */
    if (fpgaDrpWriteFct[drpFunctionIdx].DrpWriteDataMaskBfSet != NULL)
    {
        recoveryAction = fpgaDrpWriteFct[drpFunctionIdx].DrpWriteDataMaskBfSet(device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    drpChannel,
                                                                    wrMask);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Write address */
    if (fpgaDrpWriteFct[drpFunctionIdx].DrpAddressBfSet != NULL)
    {
        recoveryAction = fpgaDrpWriteFct[drpFunctionIdx].DrpAddressBfSet(  device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                drpChannel,
                                                                addr);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Write enable bit */
    if (fpgaDrpWriteFct[drpFunctionIdx].DrpWriteEnableBfSet != NULL)
    {
        recoveryAction = fpgaDrpWriteFct[drpFunctionIdx].DrpWriteEnableBfSet(  device,
                                                                    FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                    drpChannel,
                                                                    ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Set enable bit */
    if (fpgaDrpWriteFct[drpFunctionIdx].DrpEnableBfSet != NULL)
    {
        recoveryAction = fpgaDrpWriteFct[drpFunctionIdx].DrpEnableBfSet(   device,
                                                                FPGAGEN6_BF_ADS10_TOP_AXI_0_AXI_PHY,
                                                                drpChannel,
                                                                ADI_ENABLE);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }
    }

    /* Looping and wait for read to complete */
    for (idx = 0U; idx < ADI_FPGAGEN6_DRP_TIMEOUT_MS; ++idx)
    {
        recoveryAction = adi_fpgagen6_RegisterRead(device, drpDataReadAddress, &drpReadyDataRead);
        if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
        {
            ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                    ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,
                                                    ADI_NO_VARIABLE,
                                                    recoveryAction,
                                                    cleanup);
        }

        drpReady = (uint8_t)((drpReadyDataRead >> ADI_FPGAGEN6_DRP_READY_START_BIT) & 0x1U);
        drpError = (uint8_t)((drpReadyDataRead >> ADI_FPGAGEN6_DRP_ERROR_START_BIT) & 0x1U);

        if (drpReady || drpError)
        {
            break;
        }
        else
        {
            recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
            if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)
            {
                ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                        ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,
                                                        ADI_NO_VARIABLE,
                                                        recoveryAction,
                                                        cleanup);
        }
    }
    }

    /* Important to check error first because drpReady will be zero in the case of error also. */
    if (drpError != 0U)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE,
                                                drpError,
            recoveryAction,
                                                cleanup);
    }

    if (drpReady != 1U)
    {
        ADI_FPGAGEN6_ERROR_INFO_GET_REPORT_GOTO(ADI_FPGAGEN6_ERRSRC_CORE,
                                                ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE,
                                                drpReady,
            recoveryAction,
                                                cleanup);
    }

cleanup:
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction); 
}


static adi_fpgagen6_ErrAction_e fpgagen6_DmaFunctionInfoGet(adi_fpgagen6_Device_t* const device,
                                                            const adi_fpgagen6_DrpInterface_e drp,
                                                            uint8_t* const drpChannel,
                                                            uint8_t* const drpFunctionIdx,
                                                            uint32_t* const drpDataRegister)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, drpChannel);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, drpFunctionIdx);

    ADI_FPGAGEN6_NULL_PTR_REPORT_RETURN(&device->common, drpDataRegister);

    *drpDataRegister = ADI_FPGAGEN6_DRP_READY_DATA_ADDRESS_START + (uint32_t)drp * ADI_FPGAGEN6_DRP_READY_DATA_ADDRESS_STRIDE;

    switch (drp)
    {
        case ADI_FPGAGEN6_DRP_INTERFACE_CH0:
            *drpChannel = 0U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_CH1:
            *drpChannel = 1U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_CH2:
            *drpChannel = 2U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_CH3:
            *drpChannel = 3U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_CH4:
            *drpChannel = 4U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_CH5:
            *drpChannel = 5U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_CH6:
            *drpChannel = 6U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_CH7:
            *drpChannel = 7U;
            *drpFunctionIdx = 0U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_QUAD0:
            *drpChannel = 0U;
            *drpFunctionIdx = 1U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        case ADI_FPGAGEN6_DRP_INTERFACE_QUAD1:
            *drpChannel = 1U;
            *drpFunctionIdx = 1U;
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;

        default:
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                recoveryAction,
                drp,
                "Invalid drp interface selected");
            return recoveryAction;
    }

    return recoveryAction;
}
