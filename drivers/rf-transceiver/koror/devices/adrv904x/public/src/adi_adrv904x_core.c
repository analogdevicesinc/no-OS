/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_core.c
* \brief Contains ADRV904X features related function implementation defined in
* adi_adrv904x_core.h
*
* ADRV904X API Version: 2.9.0.4
*/

#include "adi_adrv904x_core.h"
#include "adi_adrv904x_gpio.h"
#include "adi_adrv904x_error.h"

#include "adi_adrv904x_version.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_shared_resource_manager.h"
#include "../../private/bf/adrv904x_bf_core.h"
#include "../../private/include/adrv904x_reg_addr_macros.h"
#include "../../private/bf/adrv904x_bf_pll_mem_map.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_CORE

#define ADRV904X_SPI_VERIFY_SCRATCHPAD_ADDR 0x60800000U
#define ADRV904X_SPI_VERIFY_BUFFER_SIZE     50U
#define ADRV904X_SPI_VERIFY_MASK            0xFFFFFFFFU

static adi_adrv904x_ErrAction_e adrv904x_SingleRegisterSpiVerify(adi_adrv904x_Device_t* const device);
static adi_adrv904x_ErrAction_e adrv904x_MultipleRegisterSpiVerify(adi_adrv904x_Device_t* const device);

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Lock(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    /* Effectively we call adi_common_hal_ApiEnter and exit but don't release the lock on exit */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_TRUE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Unlock(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    /* Effectively we call adi_common_hal_ApiEnter and exit but don't take the lock on enter */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_TRUE);

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HwOpen(adi_adrv904x_Device_t* const            device,
                                                     const adi_adrv904x_SpiConfigSettings_t* const spiSettings)
{
        adi_adrv904x_ErrAction_e recoveryAction       = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ErrAction_e exitRecoveryAction   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, spiSettings, cleanup);
    
    /* ApiEnter ADI_FALSE => Do all the normal API enter jobs *except* take the mutex
     * This is because we don't have a mutex initialized until commonOpen completes. */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }

    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_PlatformFunctionCheck(&device->common);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_ERROR_REPORT(   &device->common,
                         ADI_ADRV904X_ERRSRC_API,
                         ADI_HAL_ERR_NOT_IMPLEMENTED,
                         recoveryAction,
                         ADI_NO_VARIABLE,
                         "HAL Function Pointers Not Configured Correctly");
        goto cleanup;
    }

    /* Store SPI config into device structure */
    if ((spiSettings->cmosPadDrvStrength != ADI_ADRV904X_CMOSPAD_DRV_WEAK) &&
        (spiSettings->cmosPadDrvStrength != ADI_ADRV904X_CMOSPAD_DRV_STRONG) &&
        (spiSettings->cmosPadDrvStrength != ADI_ADRV904X_CMOSPAD_DRV_STRENGTH_1) &&
        (spiSettings->cmosPadDrvStrength != ADI_ADRV904X_CMOSPAD_DRV_STRENGTH_2))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, spiSettings->cmosPadDrvStrength, "Invalid CMOS PAD driver strength.");
        goto cleanup;
    }

    device->spiSettings.cmosPadDrvStrength = spiSettings->cmosPadDrvStrength;
    device->spiSettings.fourWireMode = spiSettings->fourWireMode;
    device->spiSettings.msbFirst     = spiSettings->msbFirst;

    if (!ADI_COMMON_DEVICE_STATE_IS_OPEN(device->common))
    {
        /* Perform Hardware Open Once */
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_HwOpen(&device->common);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Common HwOpen Issue");
            goto cleanup;
        }

        /* Earliest Point for Logging */
        adi_common_LogLevelSet(&device->common, ADI_ADRV904X_LOGGING);

        ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API);

        /* Indicate device is now open or else HwReset will fail */
        ADI_COMMON_DEVICE_STATE_OPEN_SET(device->common);
        
        if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_POWERONRESET) != ADI_ADRV904X_STATE_POWERONRESET)
        {
            /* Only HwReset on first call to HwOpen, end user must explicitly call HwReset
             * to reboot the part for future reset */
            /* Toggle RESETB pin */
            recoveryAction = adi_adrv904x_HwReset(device);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HwReset Failed");
                goto cleanup;
            }
        }
    }
    else
    {
        /* In case HW is already opened then reapply the SPI config setting. */
        recoveryAction = adi_adrv904x_SpiCfgSet(device, &device->spiSettings);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Config Issue");
            goto cleanup;
        }
    }
    
cleanup:
    /* ApiExit ADI_FALSE => don't release mutex as we did not take it at the top of the fn */
    exitRecoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);
    if (ADI_ADRV904X_ERR_ACT_NONE != exitRecoveryAction)
    {
        recoveryAction = exitRecoveryAction;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HwClose(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction       = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_ErrAction_e exitRecoveryAction   = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    /* ApiEnter ADI_FALSE => Do all the normal API enter jobs *except* take the mutex
     * This is because common_HwClose will be destroying the mutex. */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiEnter(&device->common, __func__, ADI_FALSE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Entry Issue");
        return recoveryAction;
    }

    if (ADI_COMMON_DEVICE_STATE_IS_OPEN(device->common))
    {
        recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_HwClose(&device->common);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Common HAL HwClose Failed");
            goto cleanup;
        }
        /* Indicate the Device is now closed */
        ADI_COMMON_DEVICE_STATE_OPEN_CLR(device->common);
    }

cleanup:
    /* ADI_FALSE to not release mutex as we did not take it at the top of the fn */
    exitRecoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_ApiExit(&device->common, __func__, ADI_FALSE);
    if (ADI_ADRV904X_ERR_ACT_NONE != exitRecoveryAction)
    {
        recoveryAction = exitRecoveryAction;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Exit Issue");
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_HwReset(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e    recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint8_t        WAIT_10_US          = 10U;
    static const uint8_t        RESETB_LEVEL_LOW    = 0U;
    static const uint8_t        RESETB_LEVEL_HIGH   = 1U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* HwReset is Positive Edge Triggered */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_HwReset(&device->common, RESETB_LEVEL_LOW);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with adi_common_hal_HwReset() setting Level Low");
        goto cleanup;
    }

    {
    static const uint8_t RESETB_WAIT_MS = 50U; /* move here to fix compiler warning */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_ms(&device->common, RESETB_WAIT_MS);
}
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait(ms) Failed");
        goto cleanup;
    }

    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_HwReset(&device->common, RESETB_LEVEL_HIGH);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue with adi_common_hal_HwReset() setting Level High");
        goto cleanup;
    }

    /* Give SPI state machine time to come out of reset */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_us(&device->common, WAIT_10_US);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait(us) Failed");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, ADRV904X_ADDR_SPI_INTERFACE_CONFIG_B, ADRV904X_CONFIG_B_SINGLE_INSTRUCTION, 0xFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Config Setup Issue");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_SpiCfgSet(device, &device->spiSettings);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Config Issue");
        goto cleanup;
    }

    device->devStateInfo.devState = ADI_ADRV904X_STATE_POWERONRESET;
    
    /* Reset Shared Resource Manager when HwReset occurs */
    recoveryAction = adrv904x_SharedResourceMgrReset(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Shared Resource Manager Reset Issue");
        goto cleanup;
    }
    
    /* Reset vswr waveform state in case of HW reset */
    (void)ADI_LIBRARY_MEMSET(&device->devStateInfo.vswrWaveformLoaded, 0U, sizeof(device->devStateInfo.vswrWaveformLoaded));

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Initialize(adi_adrv904x_Device_t* const        device,
                                                         const adi_adrv904x_Init_t* const    init)
{
        adi_adrv904x_Info_t devStateInfoClear;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t idx = 0U;
    uint8_t TX_SPIMODE_ENABLE_ALLCH = 0xFFU;
    uint8_t RX_SPIMODE_ENABLE_ALLCH = 0xFFU;
    uint8_t ORX_SPIMODE_ENABLE_ALLCH = 0x03U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, init, cleanup);

    ADI_LIBRARY_MEMSET(&devStateInfoClear, 0, sizeof(adi_adrv904x_Info_t));
    
    /* There is no HW Reset occurring right here, so we cannot throw away current Shared Resource States */
    /* Restore Shared Resource Manager from current device structure.*/
    ADI_LIBRARY_MEMCPY( &devStateInfoClear.sharedResourcePool, &device->devStateInfo.sharedResourcePool, sizeof(device->devStateInfo.sharedResourcePool) );

    /* Restore silicon revision from current device structure */
    devStateInfoClear.deviceSiRev = device->devStateInfo.deviceSiRev;
    devStateInfoClear.swTest = device->devStateInfo.swTest;
    devStateInfoClear.dfeSwTest = device->devStateInfo.dfeSwTest;
    (void)ADI_LIBRARY_MEMCPY(&devStateInfoClear.vswrWaveformLoaded, &device->devStateInfo.vswrWaveformLoaded, sizeof(device->devStateInfo.vswrWaveformLoaded));
    devStateInfoClear.devState = device->devStateInfo.devState;
    device->devStateInfo = devStateInfoClear;


    /* Set the Memory Repair Start */
    recoveryAction =  adrv904x_Core_MemoryRepairStart_BfSet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "MemoryRepairStart Set Issue");
        goto cleanup;
    }

    /* Give Memory Repair time to work */
    recoveryAction = (adi_adrv904x_ErrAction_e) adi_common_hal_Wait_ms(&device->common, 1U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "HAL Wait(ms) Failed");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_MbiasBgCtat_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    0U,
                                                    0x09U);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing CTAT Trim0");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_Core_MbiasBgCtat_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    1U,
                                                    0x09U);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing CTAT Trim1");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_Core_MbiasBgPtat_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    0U,
                                                    0x1EU);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing PTAT Trim0");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_Core_MbiasBgPtat_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    1U,
                                                    0x1EU);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Issue while writing PTAT Trim1");
        goto cleanup;
    }
    
    recoveryAction = adi_adrv904x_SpiRuntimeOptionsSet(device, &init->spiOptionsInit);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Runtime Options Set Issue");
        goto cleanup;
    }
    
    /* Initializes the part in non-streaming mode. The responsibility is on the API functions internally
     * to use streaming when appropriate by calling adrv904x_SpiStreamingEntry when first entering the API
     * and adrv904x_SpiStreamingExit when leaving the API.
     * */
    recoveryAction = adrv904x_SpiStreamingExit(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI streaming exit issue");
        goto cleanup;
    }
    
    /* Initialize GP Interrupt settings for use during device initialization*/
    recoveryAction = adrv904x_GpIntPreMcsInit(device, &init->gpIntPreInit);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "GP Interrupt Initialization Issue");
        goto cleanup;
    }

    device->devStateInfo.gainIndexes.rx0MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;
    device->devStateInfo.gainIndexes.rx1MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;
    device->devStateInfo.gainIndexes.rx2MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;
    device->devStateInfo.gainIndexes.rx3MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;
    device->devStateInfo.gainIndexes.rx4MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;
    device->devStateInfo.gainIndexes.rx5MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;
    device->devStateInfo.gainIndexes.rx6MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;
    device->devStateInfo.gainIndexes.rx7MaxGainIndex = ADI_ADRV904X_RX_MAX_GAIN_INDEX;

    /* Disable TRX_CTRL Pin mode until after initialization to prevent any toggling
     * pins from interfering with the init sequence. SPI reg to actually enable the 
     * channels is 0 by default after Power On Reset */
    recoveryAction =  adrv904x_Core_RadioControlInterfaceTxSpiModeSel_BfSet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                            TX_SPIMODE_ENABLE_ALLCH);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable TX Radio Control SPI Mode");
        goto cleanup;
    }
        
    recoveryAction = adrv904x_Core_RadioControlInterfaceRxSpiModeSel_BfSet(device,
                                                                           NULL,
                                                                           (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                           RX_SPIMODE_ENABLE_ALLCH);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable RX Radio Control SPI Mode");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_Core_RadioControlInterfaceOrxSpiModeSel_BfSet(device,
                                                                            NULL,
                                                                            (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                            ORX_SPIMODE_ENABLE_ALLCH);
                
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to enable ORX Radio Control SPI Mode");
        goto cleanup;
    }
    
    /* Initialize the CPU memory bank usage */
    recoveryAction = adrv904x_CpuInitialize(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Initialization Issue");
        goto cleanup;
    }

    ADI_LIBRARY_MEMCPY( device->devStateInfo.cpu.cpuMemDumpBinaryInfo.filePath,
                        init->cpuMemDump.filePath,
                        ADI_ADRV904X_MAX_FILE_LENGTH);

    recoveryAction = adrv904x_ClocksSync(device, init);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "CPU Initialization Issue");
        goto cleanup;
    }

    /* Config Uart GPIO if enable */
    for (idx = 0U; idx < ADI_ADRV904X_MAX_UART; idx++)
    {		
        if (init->uart[idx].enable == ADI_TRUE)
        {		
            recoveryAction = adrv904x_UartCfg(device, init->uart[idx].pinSelect);
    
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_UartCfg Issue.");
                goto cleanup;
            }	
        }
    
    }

        /* Config DFE UART0 GPIOs */
    recoveryAction = adrv904x_DfeUartCfg(device, 0U, &init->dfeUart0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to configure DFE UART0 pins");
        goto cleanup;
    }

    /* Disable stream pin mode until after streams are loaded */
    /* Disable Tx pin mode for all Tx, Rx and ORx channels */
    recoveryAction =  adrv904x_Core_RadioControlInterfaceTxSpiEn_BfSet(device,
                                                                       NULL,
                                                                       (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                       0x00U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable Radio Control TX channels.");
        goto cleanup;
    }

    recoveryAction =  adrv904x_Core_RadioControlInterfaceRxSpiEn_BfSet(device,
                                                                       NULL,
                                                                       (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                       0x00U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable Radio Control RX channels.");
        goto cleanup;
    }

    recoveryAction =  adrv904x_Core_RadioControlInterfaceOrxSpiEn_BfSet(device,
                                                                        NULL,
                                                                        (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                        0x00U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to disable Radio Control ORX channels.");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_ProfilesVerify(device, init);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to profiles Verify.");
        goto cleanup;
    }

    /* Set Initialized Channels */
    if (device->devStateInfo.profilesValid & ADI_ADRV904X_TX_PROFILE_VALID)
    {
        device->devStateInfo.initializedChannels |= ((device->initExtract.tx.txInitChannelMask & ADI_ADRV904X_TXALL) << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET);
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID)
    {
        device->devStateInfo.initializedChannels |= (device->initExtract.rx.rxInitChannelMask & ADI_ADRV904X_RX_MASK_ALL);
    }

    if (device->devStateInfo.profilesValid & ADI_ADRV904X_ORX_PROFILE_VALID)
    {
        device->devStateInfo.initializedChannels |= (device->initExtract.rx.rxInitChannelMask & ADI_ADRV904X_ORX_MASK_ALL);
    }
    device->devStateInfo.hsDigClk_kHz = device->initExtract.clocks.hsDigClk_kHz;
    device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_INITIALIZED);


    /* set special pattern used flag value for RadioSequencer */
    ADI_LIBRARY_MEMSET((void*)(&(device->devStateInfo.specialSequencerPatternInfo)), 0U, sizeof(adi_adrv904x_RsSpecialSequencer_t));    

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Shutdown(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* TODO: Function not implemented yet. */

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_API_NOT_IMPLEMENTED_REPORT_GOTO(&device->common, cleanup);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_MultichipSyncSet(adi_adrv904x_Device_t* const    device,
                                                               const uint8_t                   enableSync)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_StartMcsResp_t startMcsResp = { ADRV904X_CPU_SYSTEM_SIMULATED_ERROR };
    adrv904x_CpuCmd_McsCompleteResp_t mcsCompleteResp = { ADRV904X_CPU_SYSTEM_SIMULATED_ERROR };
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Verify enableSync parameter */
    if ((enableSync != 0u) && (enableSync != 1U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, enableSync, "enableSync must be 0 or 1");
        goto cleanup;
    }

    if (enableSync == 1U)
    {
        /* Start MCS */

        /* Clear the MCS_STATUS register before starting. This register is write-1-clear. 
         * This register bit is only accessible over SPI & not AHB*/
        recoveryAction =  adrv904x_Core_McsStatus_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADI_ENABLE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to clear MCS status register");
            goto cleanup;
        }

        /* Send Start MCS command to CPU */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_START_MCS,
                                                NULL,
                                                0U,
                                                (void*)&startMcsResp,
                                                sizeof(startMcsResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(startMcsResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }
    else
    {
        /* Stop MCS */

        /* Notify CPU that MCS is complete */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                ADI_ADRV904X_CPU_TYPE_0,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_MCS_COMPLETE,
                                                NULL,
                                                0U,
                                                (void *)&mcsCompleteResp,
                                                sizeof(mcsCompleteResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(mcsCompleteResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_MultichipSyncSet_v2(adi_adrv904x_Device_t* const device,
                                                                  const adi_adrv904x_McsSyncMode_e mcsMode)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    adrv904x_CpuCmd_StartMcsResp_t startMcsResp = { ADRV904X_CPU_SYSTEM_SIMULATED_ERROR };
    adrv904x_CpuCmd_McsCompleteResp_t mcsCompleteResp = { ADRV904X_CPU_SYSTEM_SIMULATED_ERROR };
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    switch (mcsMode)
    {

        case ADI_ADRV904X_MCS_CARRIER_RECONFIG:
            device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_MCSCARRIERRECONFIG);

            /* Send ADRV904X_CPU_CMD_ID_START_MCS_CARRIER_RECONFIG Command */
            recoveryAction = adrv904x_CpuCmdSend(   device,
                                                    ADI_ADRV904X_CPU_TYPE_0,
                                                    ADRV904X_LINK_ID_0,
                                                    ADRV904X_CPU_CMD_ID_START_MCS_CARRIER_RECONFIG,
                                                    NULL,
                                                    0U,
                                                    (void*)&startMcsResp,
                                                    sizeof(startMcsResp),
                                                    &cmdStatus);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(startMcsResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
            }
            break;
        case ADI_ADRV904X_MCS_OFF:
            switch (device->devStateInfo.devState & ADI_ADRV904X_STATE_MCSCARRIERRECONFIG)
            {

                    case ADI_ADRV904X_STATE_MCSCARRIERRECONFIG:
                        /* Send ADRV904X_CPU_CMD_ID_MCS_CARRIER_RECONFIG_COMPLETE Command */
                        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                                ADI_ADRV904X_CPU_TYPE_0,
                                                                ADRV904X_LINK_ID_0,
                                                                ADRV904X_CPU_CMD_ID_MCS_CARRIER_RECONFIG_COMPLETE,
                                                                NULL,
                                                                0U,
                                                                (void *)&mcsCompleteResp,
                                                                sizeof(mcsCompleteResp),
                                                                &cmdStatus);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(mcsCompleteResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
                        }
                        /* clear the devState status of ADI_ADRV904X_STATE_MCSCARRIERRECONFIG */
                        device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState & (~ADI_ADRV904X_STATE_MCSCARRIERRECONFIG));
                        break;
                    default:
                        recoveryAction = adi_adrv904x_MultichipSyncSet(device, 0U);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_MultichipSyncSet Failed");
                            goto cleanup;
                        }
                        break;
            }
            break;
        case ADI_ADRV904X_MCS_START:
            recoveryAction = adi_adrv904x_MultichipSyncSet(device, 1U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "adi_adrv904x_MultichipSyncSet Failed");
                goto cleanup;
            }
            break;
        default:
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, mcsMode, "mcsMode is not valid");
            goto cleanup;
            break;
    }
cleanup:
        ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_MultichipSyncStatusGet(adi_adrv904x_Device_t* const    device,
                                                                     uint32_t* const                 mcsStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t mcsStatusRead = 0x0U;

    uint8_t jesdClkPllStatus = 0x0U;
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, mcsStatus, cleanup);

    recoveryAction =  adrv904x_Core_McsStatus_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    &mcsStatusRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get Mcs Status");
        /* If Warning we continue executing the API as normal, otherwise goto cleanup */
        goto cleanup;
    }

    recoveryAction =  adrv904x_PllMemMap_McsSpiStatus_BfGet(device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_JESD_CLKPLL,
                                                            &jesdClkPllStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get Mcs Spi Status");
        goto cleanup;
    }

    *mcsStatus = (uint32_t)mcsStatusRead;

    *mcsStatus = (uint32_t)((jesdClkPllStatus == 0x1U) ? (mcsStatusRead | (1U << 1U)) : (mcsStatusRead | (0U << 1U))) ;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ProfilesVerify(adi_adrv904x_Device_t* const        device,
                                                             const adi_adrv904x_Init_t* const    init)
{
        /* Set Recovery Action to Error State */
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common); /* Lock Device & Log Function Entry */

   /* Lock must be released before exiting API;
   * All Errors should result in 'goto cleanup'; Do not exit function early, i.e. return from API
   */

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, init, cleanup);

    if ((device->initExtract.tx.txInitChannelMask & ADI_ADRV904X_TXALL) != ADI_ADRV904X_TXOFF)
    {
        device->devStateInfo.profilesValid |= ADI_ADRV904X_TX_PROFILE_VALID;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }

    if ((device->initExtract.rx.rxInitChannelMask & ADI_ADRV904X_RX_MASK_ALL) != ADI_ADRV904X_RXOFF)
    {
        device->devStateInfo.profilesValid |= ADI_ADRV904X_RX_PROFILE_VALID;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }

    if ((device->initExtract.rx.rxInitChannelMask & ADI_ADRV904X_ORX_MASK_ALL) != ADI_ADRV904X_RXOFF)
    {
        device->devStateInfo.profilesValid |= ADI_ADRV904X_ORX_PROFILE_VALID;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiCfgSet(adi_adrv904x_Device_t* const            device,
                                                        const adi_adrv904x_SpiConfigSettings_t* const spiCtrlSettings)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;     /* Set Recovery Action to Error State */

    uint8_t spiConfigA = 0U;
    static const uint8_t SPICFG_MSBFIRST_OFF = 0U;
    static const uint8_t SPICFG_FOURWIREMODE_OFF = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);     /* Validate Device Only before Lock */
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, spiCtrlSettings, cleanup);

    ADI_ADRV904X_API_ENTRY(&device->common);     /* Lock Device & Log Function Entry */

    /* Lock must be released before exiting API;
     * All Errors should result in 'goto cleanup'; Do not exit function early, i.e. return from API
     */

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (spiCtrlSettings->msbFirst == SPICFG_MSBFIRST_OFF)
    {
        spiConfigA |= ADRV904X_CONFIG_A_SPI_LSB_FIRST;
    }
    
    spiConfigA |= ADRV904X_CONFIG_A_SPI_ADDR_ASCENSION;
    
    if (spiCtrlSettings->fourWireMode != SPICFG_FOURWIREMODE_OFF)
    {
        spiConfigA |= ADRV904X_CONFIG_A_SPI_SDO_ACTIVE;
    }

    recoveryAction = adi_adrv904x_Register32Write(device, NULL, ADRV904X_ADDR_SPI_INTERFACE_CONFIG_A, spiConfigA, 0xFFU);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set SPI Config");
        goto cleanup;
    }
    
    device->spiSettings.fourWireMode = spiCtrlSettings->fourWireMode;
    device->spiSettings.msbFirst = spiCtrlSettings->msbFirst;

    /* Setting CMOS PAD Driver Strength */
    if (device->spiSettings.fourWireMode == ADI_TRUE)
    {
        recoveryAction = adi_adrv904x_GpioDriveStrengthSet(device, ADI_ADRV904X_O_SPI_DO, spiCtrlSettings->cmosPadDrvStrength);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        { 
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set SPI DO drive strength");
            goto cleanup;
        }
    }
    else
    {
        recoveryAction = adi_adrv904x_GpioDriveStrengthSet(device, ADI_ADRV904X_IO_SPI_DIO, spiCtrlSettings->cmosPadDrvStrength);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        { 
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set SPI DIO drive strength");
            goto cleanup;
        }
    }

    device->spiSettings.cmosPadDrvStrength = spiCtrlSettings->cmosPadDrvStrength;

#if ADI_ADRV904X_SPI_HYSTERESIS
    {
        recoveryAction = adi_adrv904x_SpiHysteresisSet(device, ADI_ENABLE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "SPI Hysteresis Configuration Issue");
            goto cleanup;
        }
    }
#endif

    recoveryAction = adi_adrv904x_GpioDriveStrengthSet( device,
                                                        ADI_ADRV904X_I_RESETB,
                                                        (adi_adrv904x_CmosPadDrvStr_e) ADI_ADRV904X_RESETB_PIN_DRIVE_STRENGTH);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_GpioHysteresisSet(device, ADI_ADRV904X_I_RESETB, ADI_ENABLE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiCfgGet(adi_adrv904x_Device_t* const        device,
                                                        adi_adrv904x_SpiConfigSettings_t* const   spi)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, spi, cleanup);
    
    spi->fourWireMode = device->spiSettings.fourWireMode;
    spi->msbFirst = device->spiSettings.msbFirst;
    spi->cmosPadDrvStrength = device->spiSettings.cmosPadDrvStrength;
    
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AuxSpiCfgSet( adi_adrv904x_Device_t* const                device,
                                                            const adi_adrv904x_AuxSpiConfig_t* const    config)
{
        static const uint32_t NOCHAN_MASK = 0U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_AuxSpiConfig_t readConfig;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, config, cleanup);

    ADI_LIBRARY_MEMSET(&readConfig, 0, sizeof(adi_adrv904x_AuxSpiConfig_t));
    
    /* Range check enable */
    if ((config->enable != ADI_ENABLE) &&
        (config->enable != ADI_DISABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, config->enable, "Aux SPI enable can only be 0 or 1.");
        goto cleanup;
    }
    
    /* Range check GPIO selections */
    if ((config->enable == ADI_ENABLE) &&
        (config->gpioCsb != ADI_ADRV904X_GPIO_03) &&
        (config->gpioCsb != ADI_ADRV904X_GPIO_16))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, config->gpioCsb, "Aux SPI CSB GPIO pin can only be set to GPIO[3] or GPIO[16].");
        goto cleanup;
    }
    
    if ((config->enable == ADI_ENABLE) &&
        (config->gpioClk != ADI_ADRV904X_GPIO_02) &&
        (config->gpioClk != ADI_ADRV904X_GPIO_15))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, config->gpioClk, "Aux SPI CLK GPIO pin can only be set to GPIO[2] or GPIO[15].");
        goto cleanup;
    }
    
    if ((config->enable == ADI_ENABLE) &&
        (config->gpioSdio != ADI_ADRV904X_GPIO_00) &&
        (config->gpioSdio != ADI_ADRV904X_GPIO_13))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, config->gpioSdio, "Aux SPI SDIO GPIO pin can only be set to GPIO[0] or GPIO[13].");
        goto cleanup;
    }
    
    if ((config->enable == ADI_ENABLE) &&
        (config->gpioSdo != ADI_ADRV904X_GPIO_01) &&
        (config->gpioSdo != ADI_ADRV904X_GPIO_14) &&
        (config->gpioSdo != ADI_ADRV904X_GPIO_INVALID))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, config->gpioSdo, "Aux SPI SDO GPIO pin can only be set to GPIO[1], GPIO[14], or INVALID.");
        goto cleanup;
    }
    
    /* Get current config */
    recoveryAction = adi_adrv904x_AuxSpiCfgGet(device, &readConfig);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving current Aux SPI Configuration.");
        goto cleanup;
    }

    /* If Aux SPI is currently enabled, disable it */
    if (readConfig.enable)
    {
        recoveryAction = adrv904x_Core_Spi1En_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADI_DISABLE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error disabling Aux SPI interface.");
            goto cleanup;
        }

        if (readConfig.gpioCsb != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease( device,
                                                         readConfig.gpioCsb,
                                                         ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CSB,
                                                         NOCHAN_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error releasing Aux SPI CSB GPIO pin.");
                goto cleanup;
            }
        }
        
        if (readConfig.gpioClk != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease( device,
                                                         readConfig.gpioClk,
                                                         ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CLK,
                                                         NOCHAN_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error releasing Aux SPI CLK GPIO pin.");
                goto cleanup;
            }
        }
        
        if (readConfig.gpioSdio != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease( device,
                                                         readConfig.gpioSdio,
                                                         ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDIO,
                                                         NOCHAN_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error releasing Aux SPI SDIO GPIO pin.");
                goto cleanup;
            }
        }
        
        if (readConfig.gpioSdo != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease( device,
                                                         readConfig.gpioSdo,
                                                         ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDO,
                                                         NOCHAN_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error releasing Aux SPI SDO GPIO pin.");
                goto cleanup;
            }
        }
        
    }
    
    /* Enable Aux SPI, if appropriate */
    if (config->enable == ADI_ENABLE)
    {
        recoveryAction = adrv904x_GpioSignalSet( device,
                                                 config->gpioCsb,
                                                 ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CSB,
                                                 NOCHAN_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error routing Aux SPI CSB GPIO pin.");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_GpioSignalSet( device,
                                                 config->gpioClk,
                                                 ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CLK,
                                                 NOCHAN_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error routing Aux SPI CLK GPIO pin.");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_GpioSignalSet( device,
                                                 config->gpioSdio,
                                                 ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDIO,
                                                 NOCHAN_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error routing Aux SPI SDIO GPIO pin.");
            goto cleanup;
        }
        
        if (config->gpioSdo != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalSet( device,
                                                     config->gpioSdo,
                                                     ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDO,
                                                     NOCHAN_MASK);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error routing Aux SPI SDO GPIO pin.");
                goto cleanup;
            }
        }

        recoveryAction = adrv904x_Core_Spi1En_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADI_ENABLE);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error enabling Aux SPI interface.");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_AuxSpiCfgGet( adi_adrv904x_Device_t* const        device,
                                                            adi_adrv904x_AuxSpiConfig_t* const  config)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, config, cleanup);

    /* Read Aux SPI enable */
    recoveryAction = adrv904x_Core_Spi1En_BfGet(device,
                                                NULL,
                                                (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                &config->enable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error reading Aux SPI enable setting.");
        goto cleanup;
    }

    /* Find GPIOs routing Aux SPI signals */
    recoveryAction = adrv904x_GpioSignalFind(  device,
                                              &config->gpioCsb,
                                               ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CSB,
                                               ADI_ADRV904X_CHOFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving GPIO pin routing Aux SPI CSB signal.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_GpioSignalFind(  device,
                                              &config->gpioClk,
                                               ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CLK,
                                               ADI_ADRV904X_CHOFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving GPIO pin routing Aux SPI CLK signal.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_GpioSignalFind(  device,
                                              &config->gpioSdio,
                                               ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDIO,
                                               ADI_ADRV904X_CHOFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving GPIO pin routing Aux SPI SDIO signal.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_GpioSignalFind(  device,
                                              &config->gpioSdo,
                                               ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDO,
                                               ADI_ADRV904X_CHOFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error retrieving GPIO pin routing Aux SPI SDO signal.");
        goto cleanup;
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiRuntimeOptionsSet(adi_adrv904x_Device_t* const             device,
                                                                   const adi_adrv904x_SpiOptions_t* const   spiOptions)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, spiOptions, cleanup);

    if (spiOptions->allowAhbAutoIncrement > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                spiOptions->allowAhbAutoIncrement,
                                "allowAhbAutoIncrement can only be 0 or 1");
        goto cleanup;
    }
    
    if (spiOptions->allowAhbSpiFifoMode > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                spiOptions->allowAhbSpiFifoMode,
                                "allowAhbSpiFifoMode can only be 0 or 1");
        goto cleanup;
    }
    
    if (spiOptions->allowSpiStreaming > 1U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                spiOptions->allowSpiStreaming,
                                "allowSpiStreaming can only be 0 or 1");
        goto cleanup;
    }
    
    device->devStateInfo.spiOptions.allowAhbAutoIncrement = spiOptions->allowAhbAutoIncrement;
    device->devStateInfo.spiOptions.allowAhbSpiFifoMode = spiOptions->allowAhbSpiFifoMode;
    device->devStateInfo.spiOptions.allowSpiStreaming = spiOptions->allowSpiStreaming;

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiRuntimeOptionsGet(adi_adrv904x_Device_t* const     device,
                                                                   adi_adrv904x_SpiOptions_t* const spiOptions)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, spiOptions, cleanup);

    spiOptions->allowAhbAutoIncrement = device->devStateInfo.spiOptions.allowAhbAutoIncrement;
    spiOptions->allowAhbSpiFifoMode = device->devStateInfo.spiOptions.allowAhbSpiFifoMode;
    spiOptions->allowSpiStreaming = device->devStateInfo.spiOptions.allowSpiStreaming;

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


static adi_adrv904x_ErrAction_e adrv904x_SingleRegisterSpiVerify(adi_adrv904x_Device_t* const device)
{

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint16_t vendorIdReadback = 0U;
    uint8_t scratchPadReadback = 0U;
    static const uint16_t VENDOR_ID = 0x0456U;
    static const uint8_t TEST_WORD_1 = 0xB6U;
    static const uint8_t TEST_WORD_2 = 0x49U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    recoveryAction = adrv904x_Core_VendorId_BfGet(  device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    &vendorIdReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Vendor ID Read Issue; Check Platform SPI Driver");
        return recoveryAction;
    }

        if (vendorIdReadback != VENDOR_ID)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                vendorIdReadback,
                                "Vendor ID Readback Mismatch; Check Platform SPI Driver");
        return recoveryAction;
    }

    /* Writing, reading, then verifying Scratch Pad (Direct SPI access) */
    recoveryAction = adrv904x_Core_ScratchPadWord_BfSet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        TEST_WORD_1);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Scratchpad Byte Write (1) Issue; Check Platform SPI Driver");
        return recoveryAction;
    }

    recoveryAction = adrv904x_Core_ScratchPadWord_BfGet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        &scratchPadReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Scratchpad Byte Read (1) Issue; Check Platform SPI Driver");
        return recoveryAction;
    }

        if (scratchPadReadback != TEST_WORD_1)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                scratchPadReadback,
                                "Scratchpad Byte Readback Mismatch (1); Check Platform SPI Driver");
        return recoveryAction;
    }

    /* Writing, reading, then verifying Scratch Pad (Direct SPI access) */
    recoveryAction = adrv904x_Core_ScratchPadWord_BfSet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        TEST_WORD_2);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Scratchpad Byte Write (2) Issue; Check Platform SPI Driver");
        return recoveryAction;
    }

    recoveryAction = adrv904x_Core_ScratchPadWord_BfGet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        &scratchPadReadback);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Scratchpad Byte Read (2) Issue; Check Platform SPI Driver");
        return recoveryAction;
    }

        if (scratchPadReadback != TEST_WORD_2)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                scratchPadReadback,
                                "Scratchpad Byte Readback Mismatch (2) Issue; Check Platform SPI Driver");
        return recoveryAction;
    }

    return recoveryAction;
}

static adi_adrv904x_ErrAction_e adrv904x_MultipleRegisterSpiVerify(adi_adrv904x_Device_t* const device)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t i = 0U;

    uint32_t address[ADRV904X_SPI_VERIFY_BUFFER_SIZE]       = { 0U };
    uint32_t currentData[ADRV904X_SPI_VERIFY_BUFFER_SIZE]   = { 0U };
    uint32_t data[ADRV904X_SPI_VERIFY_BUFFER_SIZE]          = { 0U };
    uint32_t mask[ADRV904X_SPI_VERIFY_BUFFER_SIZE]          = { 0U };
    uint32_t readBack[ADRV904X_SPI_VERIFY_BUFFER_SIZE]      = { 0U };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common, ADI_HAL_LOG_API_PRIV);

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        /* Put the part into streaming mode to help facilitate the large, contiguous SPI writes */
        recoveryAction = adrv904x_SpiStreamingEntry(device);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue Entering Spi Streaming Mode");
            goto cleanup;
        }
    }

    for (i = 0U; i < ADRV904X_SPI_VERIFY_BUFFER_SIZE; ++i)
    {
        address[i]  = (ADRV904X_SPI_VERIFY_SCRATCHPAD_ADDR + (i * 4));

        data[i]     = (0x5U + (i * 4U));
        mask[i]     = ADRV904X_SPI_VERIFY_MASK;
    }

    recoveryAction = adi_adrv904x_Registers32Read(  device,
                                                    NULL,
                                                    ADRV904X_SPI_VERIFY_SCRATCHPAD_ADDR,
                                                    &currentData[0],
                                                    &mask[0],
                                                    ADRV904X_SPI_VERIFY_BUFFER_SIZE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Scratchpad Multi Register Read Issue; Check Platform SPI Driver");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Registers32Write( device,
                                                    NULL,
                                                    &address[0],
                                                    &data[0],
                                                    &mask[0],
                                                    ADRV904X_SPI_VERIFY_BUFFER_SIZE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Scratchpad Multi Register Write Issue; Check Platform SPI Driver");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Registers32Read(  device,
                                                    NULL,
                                                    ADRV904X_SPI_VERIFY_SCRATCHPAD_ADDR,
                                                    &readBack[0],
                                                    &mask[0],
                                                    ADRV904X_SPI_VERIFY_BUFFER_SIZE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Scratchpad Multi Register Read Issue; Check Platform SPI Driver");
        goto cleanup;
    }

        if (0 != ADI_LIBRARY_MEMCMP(&data[0], &readBack[0], sizeof(data)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, readBack[0], "Scratchpad Multi Register Readback Issue; Check Platform SPI Driver");
        goto cleanup;
    }

cleanup:
    if (ADI_ADRV904X_ERR_ACT_NONE != adi_adrv904x_Registers32Write( device,
                                                                    NULL,
                                                                    &address[0],
                                                                    &currentData[0],
                                                                    &mask[0],
                                                                    ADRV904X_SPI_VERIFY_BUFFER_SIZE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE;
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ScratchPad Multi Register Cleanup Issue");
    }

    if (device->devStateInfo.spiStreamingOn == ADI_TRUE)
    {
        if (ADI_ADRV904X_ERR_ACT_NONE != adrv904x_SpiStreamingExit(device))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_INTERFACE;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Spi Streaming Exit Issue");
        }
    }

    return recoveryAction;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiVerify(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_SingleRegisterSpiVerify(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Single Register Spi Verify Issue");
        goto cleanup;
    }

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_INITIALIZED) == ADI_ADRV904X_STATE_INITIALIZED)
    {
        recoveryAction = adrv904x_MultipleRegisterSpiVerify(device);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Multi Register Spi Verify Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ApiVersionGet(adi_adrv904x_Device_t* const        device,
                                                            adi_adrv904x_Version_t* const       apiVersion)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, apiVersion, cleanup);

    apiVersion->majorVer = ADI_ADRV904X_CURRENT_MAJOR_VERSION;
    apiVersion->minorVer = ADI_ADRV904X_CURRENT_MINOR_VERSION;
    apiVersion->maintenanceVer = ADI_ADRV904X_CURRENT_MAINTENANCE_VERSION;
    apiVersion->buildVer = ADI_ADRV904X_CURRENT_BUILD_VERSION;

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
       

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceRevGet(adi_adrv904x_Device_t* const     device,
                                                           uint8_t* const                   siRevision)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t major = 0U;
    uint8_t minor = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, siRevision, cleanup);

    *siRevision = 0U;

    /* Read product_id_1 from device to obtain silicon revision code. Comprised of 2 bitfields: major and minor */
    recoveryAction = adrv904x_Core_MaskRevisionMajor_BfGet( device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &major);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Major Silicon Revision Get Issue");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_MaskRevisionMinor_BfGet( device,
                                                            NULL,
                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                            &minor);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(   &device->common,
                                recoveryAction,
                                "Minor Silicon Revision Get Issue");
        goto cleanup;
    }

    /* Construct 8'b siRevision = {4'b major, 4'b minor}*/
    *siRevision = ( (major & 0xF) << 4) | (minor & 0xF);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ProductIdGet(adi_adrv904x_Device_t* const device,
                                                           uint8_t* const               productId)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint8_t EFUSE_READY_DEBOUNCE_LIMIT = 30U;
    uint8_t pidReady = 0U;
    uint8_t retryCount = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common); /* Lock Device & Log Function Entry */

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, productId, cleanup);

    *productId = 0U;

    /* Check the status of the Efuse State machine.
     * If Efuse is ready, read the product ID
     * otherwise return an error
     */
    for (retryCount = 0U; retryCount < EFUSE_READY_DEBOUNCE_LIMIT; ++retryCount)
    {
        recoveryAction =  adrv904x_Core_EfuseProductIdReady_BfGet(device,
                                                                  NULL,
                                                                  ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                  &pidReady);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Efuse Product Id Ready status");
            goto cleanup;
        }

        if (pidReady == 1U)
        {
            break;
        }
    }

    if (pidReady == 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_RESET_DEVICE;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               pidReady,
                               "Efuse Hardware Not Ready. Repeated Attempts Failed");
        goto cleanup;
    }

    /* Read the Product ID */
    recoveryAction =  adrv904x_Core_EfuseProductId_BfGet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                         productId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Product ID Get Issue");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DeviceCapabilityGet(adi_adrv904x_Device_t* const device,
                                                                  adi_adrv904x_CapabilityModel_t* const devCapability)
{
        /* local variables */
    static const uint32_t   ADRV904X_TX_MAX_BW_KHZ              = 800000U;
    static const uint32_t   ADRV9044NB_TX_MAX_BW_KHZ            = 600000U;
    static const uint32_t   ADRV9049_TX_MAX_BW_KHZ              = 600000U;
    static const uint32_t   ADRV9044WB_TX_MAX_BW_KHZ            = 200000U;
    static const uint32_t   ADRV904X_RX_MAX_BW_KHZ              = 600000U;
    static const uint32_t   ADRV9044WB_RX_MAX_BW_KHZ            = 200000U;
    static const uint32_t   ADRV904X_ORX_MAX_BW_KHZ             = 800000U;
    static const uint32_t   ADRV9044WB_ORX_MAX_BW_KHZ           = 430000U;
    static const uint32_t   ADRV904X_RF_FREQ_RANGE_MIN_KHZ      = 650000U;
    static const uint32_t   ADRV904X_RF_FREQ_RANGE_MAX_KHZ      = 7100000U;
    static const uint32_t   ADRV904X_RF_MAX_IBW_MHZ             = 400U;
    static const uint32_t   ADRV9044NB_RF_MAX_IBW_MHZ           = 200U;
    static const uint32_t   ADRV9044WB_RF_MAX_IBW_MHZ           = 600U;
    static const uint32_t   ADRV9049_RF_MAX_IBW_MHZ             = 600U;
    static const uint32_t   ADRV9049_RF_FREQ_RANGE_MAX_KHZ      = 6000000U;

    static const uint8_t    EXTERNAL_LO_DISABLE_POSITION        = 4U;
    static const uint8_t    INTERNAL_LO_DISABLE_POSITION        = 2U;

    uint8_t idx                                                 = 0U;
    uint8_t i                                                   = 0U;
    uint8_t count                                               = 0U;
    uint8_t temp                                                = 0U;
    uint8_t productId                                           = 0U;
    uint8_t siRevision                                          = 0U;
    uint8_t internalLomask                                      = 0U;
    uint8_t externalLomask                                      = 0U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_FEATURE;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common); /* Lock Device & Log Function Entry */

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, devCapability, cleanup);

    /* Read the Product ID */
    recoveryAction =  adi_adrv904x_ProductIdGet(device, &productId);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Product ID Get Issue");
        goto cleanup;
    }

    recoveryAction =  adi_adrv904x_DeviceRevGet(device, &siRevision);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Device Revision ID Get Issue");
        goto cleanup;
    }

    devCapability->productId = productId;
    devCapability->siRevision = siRevision;

    /* PID=0 for the EX an XX open feature products,other PID value have not filled in document. */
    switch (productId)
    {
        case 0U:
            devCapability->txMaxBw_kHz          =   ADRV904X_TX_MAX_BW_KHZ;
            devCapability->rxMaxBw_kHz          =   ADRV904X_RX_MAX_BW_KHZ;
            devCapability->orxMaxBw_kHz         =   ADRV904X_ORX_MAX_BW_KHZ;
            devCapability->rfFreqRangeMin_kHz   =   ADRV904X_RF_FREQ_RANGE_MIN_KHZ;
            devCapability->rfFreqRangeMax_kHz   =   ADRV904X_RF_FREQ_RANGE_MAX_KHZ;
            devCapability->rfMaxIbw_Mhz         =   ADRV904X_RF_MAX_IBW_MHZ;
            devCapability->featureMask          =   ADI_ADRV904X_FEATUREOFF;
            break;
   
        case 71U: /* AD80538 - 9049 */
            devCapability->txMaxBw_kHz          =   ADRV9049_TX_MAX_BW_KHZ;
            devCapability->rxMaxBw_kHz          =   ADRV904X_RX_MAX_BW_KHZ;
            devCapability->orxMaxBw_kHz         =   ADRV904X_ORX_MAX_BW_KHZ;
            devCapability->rfFreqRangeMin_kHz   =   ADRV904X_RF_FREQ_RANGE_MIN_KHZ;
            devCapability->rfFreqRangeMax_kHz   =   ADRV9049_RF_FREQ_RANGE_MAX_KHZ;
            devCapability->rfMaxIbw_Mhz         =   ADRV9049_RF_MAX_IBW_MHZ;
            devCapability->featureMask          =   ADI_ADRV904X_FEATUREOFF;
            break;
   
        case 72U: /* AD80539 - 9049 */
            devCapability->txMaxBw_kHz          =   ADRV9049_TX_MAX_BW_KHZ;
            devCapability->rxMaxBw_kHz          =   ADRV904X_RX_MAX_BW_KHZ;
            devCapability->orxMaxBw_kHz         =   ADRV904X_ORX_MAX_BW_KHZ;
            devCapability->rfFreqRangeMin_kHz   =   ADRV904X_RF_FREQ_RANGE_MIN_KHZ;
            devCapability->rfFreqRangeMax_kHz   =   ADRV9049_RF_FREQ_RANGE_MAX_KHZ;
            devCapability->rfMaxIbw_Mhz         =   ADRV9049_RF_MAX_IBW_MHZ;
            devCapability->featureMask          =   ADI_ADRV904X_FEATUREOFF;
            break;
            
        case 69U: /* 9044-NB */ 
            devCapability->txMaxBw_kHz          =   ADRV9044NB_TX_MAX_BW_KHZ;
            devCapability->rxMaxBw_kHz          =   ADRV904X_RX_MAX_BW_KHZ;
            devCapability->orxMaxBw_kHz         =   ADRV904X_ORX_MAX_BW_KHZ;
            devCapability->rfFreqRangeMin_kHz   =   ADRV904X_RF_FREQ_RANGE_MIN_KHZ;
            devCapability->rfFreqRangeMax_kHz   =   ADRV904X_RF_FREQ_RANGE_MAX_KHZ;
            devCapability->rfMaxIbw_Mhz         =   ADRV9044NB_RF_MAX_IBW_MHZ;
            devCapability->featureMask          =   ADI_ADRV904X_FEATUREOFF;
            break;
            
        case 70U: /* 9044-WB */
            devCapability->txMaxBw_kHz          =   ADRV9044WB_TX_MAX_BW_KHZ;
            devCapability->rxMaxBw_kHz          =   ADRV9044WB_RX_MAX_BW_KHZ;
            devCapability->orxMaxBw_kHz         =   ADRV9044WB_ORX_MAX_BW_KHZ;
            devCapability->rfFreqRangeMin_kHz   =   ADRV904X_RF_FREQ_RANGE_MIN_KHZ;
            devCapability->rfFreqRangeMax_kHz   =   ADRV904X_RF_FREQ_RANGE_MAX_KHZ;
            devCapability->rfMaxIbw_Mhz         =   ADRV9044WB_RF_MAX_IBW_MHZ;
            devCapability->featureMask          =   ADI_ADRV904X_FEATUREOFF;
            break;
               
        case 73U: /* AD80538 - 9049 */
            devCapability->txMaxBw_kHz          =   ADRV9049_TX_MAX_BW_KHZ;
            devCapability->rxMaxBw_kHz          =   ADRV904X_RX_MAX_BW_KHZ;
            devCapability->orxMaxBw_kHz         =   ADRV904X_ORX_MAX_BW_KHZ;
            devCapability->rfFreqRangeMin_kHz   =   ADRV904X_RF_FREQ_RANGE_MIN_KHZ;
            devCapability->rfFreqRangeMax_kHz   =   ADRV9049_RF_FREQ_RANGE_MAX_KHZ;
            devCapability->rfMaxIbw_Mhz         =   ADRV9049_RF_MAX_IBW_MHZ;
            devCapability->featureMask          =   ADI_ADRV904X_FEATUREOFF;
            break;

        default:
            break;
    }

    for (idx = 0; idx < ADI_ADRV904X_MAX_FEATUREMASK ; idx++)
    {
        switch (idx)
        {
            case ADI_ADRV904X_FEATURE_SERIALIZER :
                devCapability->serializerLaneEnableMask = device->initExtract.featureMask[idx] ^ 0xFFU;
                break;
            case ADI_ADRV904X_FEATURE_DESERIALIZER :
                devCapability->deserializerLaneEnableMask = device->initExtract.featureMask[idx] ^ 0xFFU;
                break;
            case ADI_ADRV904X_FEATURE_TX:
                temp = 1U;
                count = 0U;
                for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
                {
                    if ((device->initExtract.featureMask[idx] & temp) == 0U)
                        {
                            ++count;
                        }
                
                    temp = temp << 1U;
                }

                devCapability->txNumber = count;
                devCapability->txChannelMask = device->initExtract.featureMask[idx] ^ 0xFF;
                break;
            case ADI_ADRV904X_FEATURE_RX :
                temp = 1U;
                count = 0U;
                for (i = 0U; i < ADI_ADRV904X_MAX_RX_ONLY; ++i)
                {
                    if ((device->initExtract.featureMask[idx] & temp) == 0U)
                        {
                            ++count;
                        }
                
                    temp = temp << 1U;
                }

                devCapability->rxNumber = count;
                devCapability->rxChannelMask = device->initExtract.featureMask[idx] ^ 0xFF;
                break;
            case ADI_ADRV904X_FEATURE_ORX_LOMASK :
                temp = 1U;
                count = 0U;
                for (i = 0U; i < ADI_ADRV904X_MAX_ORX; ++i)
                {
                    if ((device->initExtract.featureMask[idx] & temp) == 0U)
                        {
                            ++count;
                        }

                    temp = temp << 1U;
                }

                devCapability->orxNumber = count;
                devCapability->orxChannelMask = device->initExtract.featureMask[idx] ^ 0x03U;

                internalLomask = (device->initExtract.featureMask[idx] >> INTERNAL_LO_DISABLE_POSITION) & 0x03U;
                externalLomask = (device->initExtract.featureMask[idx] >> EXTERNAL_LO_DISABLE_POSITION) & 0x04U;
                devCapability->loMask = (internalLomask | externalLomask) ^ 0x07U;
                break;

            default:
                break;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiDoutPadDriveStrengthSet(adi_adrv904x_Device_t* const   device,
                                                                         const uint8_t                  driveStrength)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
    static const uint8_t SPI_DOUT_PAD_DRIVE_STRENGTH_THREE_WIRE_MODE_ADDR_OFFSET    = 0x03U;
    static const uint8_t SPI_DOUT_PAD_DRIVE_STRENGTH_THREE_WIRE_MODE_MASK           = 0x02U;

    static const uint8_t SPI_DOUT_PAD_DRIVE_STRENGTH_FOUR_WIRE_MODE_ADDR_OFFSET     = 0x07U;
    static const uint8_t SPI_DOUT_PAD_DRIVE_STRENGTH_FOUR_WIRE_MODE_MASK            = 0x01U;

    static const uint8_t DRIVE_STRENGTH_MAX = 0x03U;
    uint8_t readBackVal = 0U;
    uint8_t registerAddrOffset = 0U;
    uint8_t registerMaskVal = 0U;

    static const uint8_t DS_ENUM_CONVERT[] = { 0U, 3U, 1U, 2U };

    static const uint8_t DS_VALUE_LUT[] = { 0x0U, 0x3U, 0x9U, 0xFU };
    uint8_t dsValue = DS_VALUE_LUT[DS_ENUM_CONVERT[driveStrength]];

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common); /* Lock Device & Log Function Entry */

    if (driveStrength > DRIVE_STRENGTH_MAX)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, driveStrength, "Drive strength is above the allowed value.");
        goto cleanup;
    }

    if (device->spiSettings.fourWireMode == 1U)
    {
        registerAddrOffset = SPI_DOUT_PAD_DRIVE_STRENGTH_FOUR_WIRE_MODE_ADDR_OFFSET;
        registerMaskVal = SPI_DOUT_PAD_DRIVE_STRENGTH_FOUR_WIRE_MODE_MASK;
    }
    else
    {
        registerAddrOffset = SPI_DOUT_PAD_DRIVE_STRENGTH_THREE_WIRE_MODE_ADDR_OFFSET;
        registerMaskVal = SPI_DOUT_PAD_DRIVE_STRENGTH_THREE_WIRE_MODE_MASK;
    }

    /* Configure SPI DOUT DS0 bit, we need to do RMW since its a single bit in DS0 register */
    recoveryAction = adrv904x_Core_PadDs0_BfGet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, &readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read DS0 bit");
        goto cleanup;
    }

    if (ADRV904X_BF_EQUAL(dsValue, 0x01U))
    {
        readBackVal |= (uint8_t) (1U << registerMaskVal);
    }
    else
    {
        readBackVal &= (uint8_t)(0U << registerMaskVal);
    }

    recoveryAction = adrv904x_Core_PadDs0_BfSet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write DS0 bit");
        goto cleanup;
    }

    /* Configure SPI DOUT DS1 bit, we need to do RMW since its a single bit in DS1 register */
    recoveryAction = adrv904x_Core_PadDs1_BfGet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, &readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read DS1 bit");
        goto cleanup;
    }

    if (ADRV904X_BF_EQUAL(dsValue, 0x02U))
    {
        readBackVal |= (uint8_t) (1U << registerMaskVal);
    }
    else
    {
        readBackVal &= (uint8_t)(0U << registerMaskVal);
    }

    recoveryAction =  adrv904x_Core_PadDs1_BfSet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write DS1 bit");
        goto cleanup;
    }

    /* Configure SPI DOUT DS2 bit, we need to do RMW since its a single bit in DS2 register */
    recoveryAction = adrv904x_Core_PadDs2_BfGet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, &readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read DS2 bit");
        goto cleanup;
    }

    if (ADRV904X_BF_EQUAL(dsValue, 0x04U))
    {
        readBackVal |= (uint8_t) (1U << registerMaskVal);
    }
    else
    {
        readBackVal &= (uint8_t)(0U << registerMaskVal);
    }

    recoveryAction = adrv904x_Core_PadDs2_BfSet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write DS2 bit");
        goto cleanup;
    }

    /* Configure SPI DOUT DS3 bit, we need to do RMW since its a single bit in DS3 register */
    recoveryAction = adrv904x_Core_PadDs3_BfGet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, &readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read DS3 bit");
        goto cleanup;
    }

    if (ADRV904X_BF_EQUAL(dsValue, 0x08U))
    {
        readBackVal |= (uint8_t) (1U << registerMaskVal);
    }
    else
    {
        readBackVal &= (uint8_t)(0U << registerMaskVal);
    }

    recoveryAction = adrv904x_Core_PadDs3_BfSet(device, NULL, (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR, registerAddrOffset, readBackVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    { 
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write DS3 bit");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
        
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiHysteresisSet( adi_adrv904x_Device_t* const    device,
                                                                const uint32_t                  enable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if (enable > ADI_ENABLE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                enable,
                                "enable can only be 0 or 1");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_GpioHysteresisSet(device, ADI_ADRV904X_I_SPI_ENB, enable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_GpioHysteresisSet(device, ADI_ADRV904X_I_SPI_CLK, enable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_GpioHysteresisSet(device, ADI_ADRV904X_IO_SPI_DIO, enable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_GpioHysteresisSet(device, ADI_ADRV904X_O_SPI_DO, enable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    /* SPI Configuration Validation */

#if ADI_ADRV904X_SPI_HYSTERESIS_DEBUG

    recoveryAction = adi_adrv904x_GpioHysteresisGet(device, ADI_ADRV904X_I_SPI_ENB, &enableFlag);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    enableMask |= (enableFlag << 0U);

    recoveryAction = adi_adrv904x_GpioHysteresisGet(device, ADI_ADRV904X_I_SPI_CLK, &enableFlag);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    enableMask |= (enableFlag << 1U);

    recoveryAction = adi_adrv904x_GpioHysteresisGet(device, ADI_ADRV904X_IO_SPI_DIO, &enableFlag);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    enableMask |= (enableFlag << 2U);

    recoveryAction = adi_adrv904x_GpioHysteresisGet(device, ADI_ADRV904X_O_SPI_DO, &enableFlag);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
        goto cleanup;
    }

    enableMask |= (enableFlag << 3U);

    if (((enable == ADI_ENABLE) ? 0x0FU : 0x00U) != enableMask)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                enableMask,
                                "Unexpected SPI Hysteresis Configuration");
    }

#endif

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DigitalHysteresisSet( adi_adrv904x_Device_t* const    device,
                                                                    const uint32_t                  enable)
{
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t i = 0U;
    uint8_t bfValue = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    for (i = 0U; i < ADRV904X_DIGITAL_PIN_GROUP_NUM; ++i)
    {
        recoveryAction = adrv904x_Core_PadSt_BfSet( device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    i,
                                                    ((enable == ADI_ENABLE) ? 0x3FU : 0x00U));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
            goto cleanup;
        }
    }

    for (i = 0U; i < ADRV904X_DIGITAL_PIN_GROUP_NUM; ++i)
    {
        recoveryAction = adrv904x_Core_PadSt_BfGet( device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                    i,
                                                    &bfValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, ADI_NO_ERROR_MESSAGE);
            goto cleanup;
        }

        if (bfValue != ((enable == ADI_ENABLE) ? 0x3FU : 0x00U))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    bfValue,
                                    "Hysteresis Enable Issue");

            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    i,
                                    "Pad Group Number");
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}