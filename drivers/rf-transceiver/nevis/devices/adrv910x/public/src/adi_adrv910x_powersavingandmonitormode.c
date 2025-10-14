/**
* \file
* \brief Contains Power saving and Monitor mode features related function implementation defined in
* adi_adrv910x_PowerSavingAndMonitorMode.h
*
* ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
*/

/**
* Copyright 2021 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "adi_adrv910x_user.h"
#include "adi_adrv910x_powersavingandmonitormode.h"
#include "adi_adrv910x.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_error.h"
#include "adi_adrv910x_gpio.h"
#include "adi_adrv910x_spi.h"
#include "adi_adrv910x_radio.h"
#include "adi_adrv910x_cals.h"

#include "adrv910x_arm.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_arm_error_mapping.h"
#include "adrv910x_bf.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_validators.h"
#include "adrv910x_bf.h"

#include "object_ids.h"


static __maybe_unused int32_t __maybe_unused SystemPowerSavingAndMonitorMode_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                 adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    adi_adrv910x_GpioCfg_t gpio = { 0 };

    /* RxNb must be enabled to support Monitor Mode feature */
    if ((monitorModeCfg->detectionTime_us!= 0) &&
        (0 == ADRV910X_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV910X_RXNB)))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device->devStateInfo.initializedChannels,
            "RxNB channel must be initialized to support Monitor Mode feature");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_NULL_PTR_RETURN(&device->common, monitorModeCfg);

    ADI_RANGE_CHECK(device,
                    monitorModeCfg->powerDownMode,
                    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_CLKPLL,
                    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_ARM);

    ADI_RANGE_CHECK(device,
                    monitorModeCfg->detectionMode,
                    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI,
                    ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_FFT);

   if (ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_SYNC == monitorModeCfg->detectionMode)
   {
       if (ADI_ADRV910X_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_ARM == monitorModeCfg->powerDownMode)
       {
           if (!((RX_SIGNAL_TYPE_FREQUENCY_DEVIATION == device->devStateInfo.rxOutputSignaling[0]) ||
                 (24000 == KILO_TO_BASE_UNIT(device->devStateInfo.rxOutputRate_kHz[0])) ||
                 (ADI_ADRV910X_RXNB == (device->devStateInfo.initializedChannels & ADI_ADRV910X_RXNB))))
           {
               ADI_ERROR_REPORT(&device->common,
                   ADI_COMMON_ERRSRC_API,
                   ADI_COMMON_ERR_INV_PARAM,
                   ADI_COMMON_ACT_ERR_CHECK_PARAM,
                   monitorModeCfg->powerDownMode,
                   "Monitor mode sync is supported in 24 kHz, RX_SIGNAL_TYPE_FREQUENCY_DEVIATION on RxNb only");
               ADI_ERROR_RETURN(device->common.error.newAction);
           }
       }
       else
       {
           ADI_ERROR_REPORT(&device->common,
               ADI_COMMON_ERRSRC_API,
               ADI_COMMON_ERR_INV_PARAM,
               ADI_COMMON_ACT_ERR_CHECK_PARAM,
               monitorModeCfg->powerDownMode,
               "ADI_ADRV910X_MONITOR_DETECTION_MODE_SYNC is only supported in ADI_ADRV910X_SYSTEM_MODE_ARM");
           ADI_ERROR_RETURN(device->common.error.newAction);
       }
   }

    if ((ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_FFT       == monitorModeCfg->detectionMode) ||
        (ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_SYNC == monitorModeCfg->detectionMode) ||
        (ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_FFT  == monitorModeCfg->detectionMode))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeCfg->detectionMode,
            "Not supported yet");
        ADI_ERROR_RETURN(device->common.error.newAction);

    }

    ADI_EXPECT(adi_adrv910x_gpio_Ps1_Inspect, device, ADI_ADRV910X_GPIO_SIGNAL_MON_ENABLE_SPS, &gpio);
    ADI_RANGE_CHECK(device, gpio.pin, ADI_ADRV910X_GPIO_DIGITAL_00, ADI_ADRV910X_GPIO_DIGITAL_15);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Configure(adi_adrv910x_Device_t *device,
                                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    uint8_t armData[16] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(SystemPowerSavingAndMonitorMode_Configure_Validate, device, monitorModeCfg);

#ifndef NEVIS_PS2
    /* NOTE: this probably doesn't need to be here.  When API is executing on
     BBIC this will have been called as a result of loading images.  In PS2
     this function doesn't exist. */
    /* Ensure core.ahb_spi_bridge_enable is always set to 0x1 */
    ADI_EXPECT(adi_adrv910x_arm_AhbSpiBridge_Enable, device);
#endif
	
	ADI_MUTEX_AQUIRE(device);

    adrv910x_LoadFourBytes(&offset, armData, monitorModeCfg->initialBatterySaverDelay_us);
    adrv910x_LoadFourBytes(&offset, armData, monitorModeCfg->detectionTime_us);
    adrv910x_LoadFourBytes(&offset, armData, monitorModeCfg->sleepTime_us);
    armData[offset++] = (uint8_t)monitorModeCfg->bbicWakeupLevelEnable;
    armData[offset++] = (uint8_t)monitorModeCfg->externalPllEnable;

    /* Write monitor mode configuration parameters to ARM data memory */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, ADRV910X_ADDR_ARM_HIGHPRIORITY_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);

    extData[0] = 0;
    extData[1] = ADRV910X_ARM_HIGHPRIORITY_SET_MONITOR_MODE_CONFIG;
    extData[2] = (uint8_t)(monitorModeCfg->powerDownMode);
    extData[3] = monitorModeCfg->detectionFirst;
    extData[4] = (uint8_t)(monitorModeCfg->detectionMode);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_HIGHPRIORITY_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV910X_ARM_HIGHPRIORITY_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Inspect(adi_adrv910x_Device_t *device,
                                                                                       adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    uint8_t armReadBack[17] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    /* Check device pointer and rxInterfaceGainCtrl are not null */
    ADI_ENTRY_PTR_EXPECT(device, monitorModeCfg);

    extData[0] = 0;
    extData[1] = OBJID_GO_GET_MONITOR_CONFIG;

	ADI_MUTEX_AQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV910X_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
               device,
               ADRV910X_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false, ADI_PS1)

    monitorModeCfg->powerDownMode = (adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e)armReadBack[offset++];
    adrv910x_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->initialBatterySaverDelay_us);
    adrv910x_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->detectionTime_us);
    adrv910x_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->sleepTime_us);
    monitorModeCfg->detectionFirst = armReadBack[offset++];
    monitorModeCfg->detectionMode = (adi_adrv910x_PowerSavingAndMonitorMode_MonitorDetectionMode_e)armReadBack[offset++];
    monitorModeCfg->bbicWakeupLevelEnable = (armReadBack[offset++] == 1) ? true : false;
    monitorModeCfg->externalPllEnable = (armReadBack[offset++] == 1) ? true : false;

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingMode_Set(adi_adrv910x_Device_t *device,
                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e mode)
{
    adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t config = {
        .initialBatterySaverDelay_us = 0,
        .detectionTime_us = 0,
        .sleepTime_us = 0xFFFFFFFF,
        .detectionFirst = 0,
        .detectionMode = ADI_ADRV910X_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI,
        .bbicWakeupLevelEnable = false,
    };

    config.powerDownMode = mode;
	
	ADI_MUTEX_AQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Configure, device, &config);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingMode_Get_Validate(adi_adrv910x_Device_t *device,
                                                                                                        adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e *mode)
{
    ADI_NULL_PTR_RETURN(&device->common, mode);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingMode_Get(adi_adrv910x_Device_t *device,
                                                                         adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerDownMode_e *mode)
{
    adi_adrv910x_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t config = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingMode_Get_Validate, device, mode);

	ADI_MUTEX_AQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Inspect, device, &config);
    *mode = config.powerDownMode;

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                                            adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern)
{
    uint8_t readAddrOffset = 0;
    const uint32_t dpinfifoLength = 2048;

    ADI_NULL_PTR_RETURN(&device->common, monitorModePattern);

    /* The valid range for patternLength strictly depends on how the DPinFIFO is
     * configured. Exceeding this limit could result in errors that are very hard
     * to detect, so it is worthwhile to provide exact checking here. */
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoRdAddrOffset_Get, device, &readAddrOffset);
    ADI_RANGE_CHECK(device, monitorModePattern->patternLength, 0, dpinfifoLength - readAddrOffset);

    ADI_API_RETURN(device);
}

/* TODO: Change this function to be private when a function to measure
 * the value from CALIBRATED and program the value to the ADRV910X is added */
int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure(adi_adrv910x_Device_t *device,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern)
{
    uint16_t pattern = 0;
    uint16_t patternLength = 0;
    uint32_t readPattern = 0;

    int32_t halError = 0;
    uint32_t numEventChecks = 1;
    uint32_t eventCheck = 0;
    uint8_t testDataUpdate = 0;
    bool updateCompleted = false;

    static const uint32_t ADI_ADRV910X_MONITOR_PATTERN_WRITE_INTERVAL_US = 1000;
    static const uint32_t ADI_ADRV910X_MONITOR_PATTERN_WRITE_TIMEOUT_US = 50000;    /* < 50 us expected */

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure_Validate, device, monitorModePattern);
	
	ADI_MUTEX_AQUIRE(device);
	
    /* Select DpinFIFO test pattern; 1: select test pattern. 0: select datapath data */
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestdataSel_Set, device, 0x1);
    /* Set dpinfifo_control: dpinfifo_en, detected, not rd_startstop, not wr_startstop. */
	ADI_EXPECT(adi_bf_hal_Register_Write, device, ADRV910X_BF_RX_NB_CORE_2 + 0x95, 0x81);


    patternLength = monitorModePattern->patternLength;
    numEventChecks = ADI_ADRV910X_MONITOR_PATTERN_WRITE_TIMEOUT_US / ADI_ADRV910X_MONITOR_PATTERN_WRITE_INTERVAL_US;

    for (pattern = 0; pattern < patternLength; pattern++)
    {
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestdataI_Set, device, monitorModePattern->patternI[pattern]);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestdataI_Get, device, &readPattern);
        if (readPattern != monitorModePattern->patternI[pattern])
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                readPattern,
                "Test pattern I is not written properly in DpInFIFO.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestdataQ_Set, device, monitorModePattern->patternQ[pattern]);
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestdataQ_Get, device, &readPattern);
        if (readPattern != monitorModePattern->patternQ[pattern])
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                readPattern,
                "Test pattern Q is not written properly in DpInFIFO.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

	    ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestDataUpdate_Set, device, 0x1);

        /* timeout event check loop, waiting for 'test_data_update' bit to clear */
	    ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestDataUpdate_Get, device, &testDataUpdate);
        updateCompleted = (testDataUpdate == 0u);
        for (eventCheck = 0; (eventCheck <= numEventChecks) && !updateCompleted; eventCheck++)
        {
            /* polling delay */
            halError = adi_common_hal_Wait_us(&device->common, ADI_ADRV910X_MONITOR_PATTERN_WRITE_INTERVAL_US);
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV910X_SRC_ARMCMD,
                halError,
                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                device,
                "Timer not working in adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure()");
            ADI_ERROR_RETURN(device->common.error.newAction);

            /* check whether 'test_data_update' bit is reset to '0' after delay */
	        ADI_EXPECT(adrv910x_NvsRegmapRxnb_DpinfifoTestDataUpdate_Get, device, &testDataUpdate);
            updateCompleted = (testDataUpdate == 0u);
        }

        /* report if the write failed */
        if (!updateCompleted)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_ADRV910X_SRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV910X_ACT_ERR_RESET_ARM,
                device,
                "Time out!!! adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure() failed due to 'test_data_update' bit not getting reset after write operation.");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }

    }
	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                                           adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
{
    static uint16_t VECTOR_MASK_MAX = 0x3FFF;
    ADI_NULL_PTR_RETURN(&device->common, monitorModeVector);
    ADI_RANGE_CHECK(device, monitorModeVector->vectorMask, 0x1, VECTOR_MASK_MAX);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Configure(adi_adrv910x_Device_t *device,
                                                                            adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
{
    uint8_t mask = 0;
    uint8_t nibSel = 0;
    uint8_t vectorCrumb = 0;
    uint8_t vectorNibble = 0;
    uint16_t bfValue = 0;
    uint32_t vectorConverted = 0;
    static const uint8_t MONITOR_MODE_VECTOR_MAX = 14;
    static const uint8_t MAX_NIBBLE_PER_VECTOR = 12;

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Configure_Validate, device, monitorModeVector);

	ADI_MUTEX_AQUIRE(device);
	
    /* Hard code D14 and D15 = 0x1 */
    bfValue = monitorModeVector->vectorMask | 0xC000;
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_CorrCtrl_Set, device, bfValue);

    for (mask = 0; mask < MONITOR_MODE_VECTOR_MAX; mask++)
    {
        if (bfValue & (1 << mask))
        {
            for (nibSel = 0; nibSel < MAX_NIBBLE_PER_VECTOR; nibSel++)
            {
                vectorNibble = (monitorModeVector->vector[mask] >> (nibSel * 4)) & 0xF;
                /* bit3 and bit1 are extracted in each nibble and combined for conversion */
                /* 0x5: 0x00, 0x7:0x1, 0xD:0x2, 0xF:0x3 */
                vectorCrumb = ((vectorNibble & 0x8) >> 2) | ((vectorNibble & 0x2) >> 1);
                vectorConverted |= (vectorCrumb << (nibSel * 2));
            }
	        ADI_EXPECT(adrv910x_NvsRegmapRxnb_Vcorrsig_Set, device, mask, vectorConverted);
            vectorConverted = 0;
        }
    }

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect_Validate(adi_adrv910x_Device_t *device,
                                                                                                         adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
{
    ADI_NULL_PTR_RETURN(&device->common, monitorModeVector);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect(adi_adrv910x_Device_t *device,
                                                                          adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
{
    uint8_t mask = 0;
    uint8_t crumbSel = 0;
    uint8_t vectorCrumb = 0;
    uint8_t vectorLut[4] = { 0x5, 0x7, 0xD, 0xF };
    uint16_t bfValue = 0;
    uint32_t readvector = 0;
    uint64_t vectorConverted = 0;
    uint64_t tempVector = 0;
    static const uint8_t MONITOR_MODE_VECTOR_MAX = 14;
    static const uint8_t MAX_CRUMB_PER_READVECTOR = 12;

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect_Validate, device, monitorModeVector);

	ADI_MUTEX_AQUIRE(device);
	
	ADI_EXPECT(adrv910x_NvsRegmapRxnb_CorrCtrl_Get, device, &bfValue);
    monitorModeVector->vectorMask = bfValue & 0x3FFF;

    for (mask = 0; mask < MONITOR_MODE_VECTOR_MAX; mask++)
    {
        if (bfValue & (1 << mask))
        {
	        ADI_EXPECT(adrv910x_NvsRegmapRxnb_Vcorrsig_Get, device, mask, &readvector);

            for (crumbSel = 0; crumbSel < MAX_CRUMB_PER_READVECTOR; crumbSel++)
            {
                vectorCrumb = (readvector >> (crumbSel * 2)) & 0x3;
                tempVector = (uint64_t)vectorLut[vectorCrumb] << (crumbSel * 4);
                vectorConverted |= tempVector;
            }
            monitorModeVector->vector[mask] = vectorConverted;
            vectorConverted = 0;
        }
    }

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                                         adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port = 0;
    uint8_t channel = 0;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    uint32_t MEASUREMENT_DURATION_SAMPLES_MAX = 0x1FFFFF;
    int32_t  DETECTION_THRESHOLD_MIN = -140000;
    adi_common_Port_e ports[2] = { ADI_RX, ADI_TX };
    adi_common_ChannelNumber_e channels[2] = { ADI_CHANNEL_1, ADI_CHANNEL_2 };
    ADI_NULL_PTR_RETURN(&device->common, monitorModeRssiCfg);

    ADI_RANGE_CHECK(device, monitorModeRssiCfg->measurementDuration_samples, 0x2, MEASUREMENT_DURATION_SAMPLES_MAX);
    ADI_RANGE_CHECK(device, monitorModeRssiCfg->detectionThreshold_mdBFS, DETECTION_THRESHOLD_MIN, 0);

    if (0 == monitorModeRssiCfg->numberOfMeasurementsToAverage)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeRssiCfg->numberOfMeasurementsToAverage,
            "'numberOfMeasurementsToAverage' cannot be 0.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* TODO: Remove this check in future.
     * measurementsStartPeriod_ms = 0: Continuous RSSI measurements (currently not supported by FW) */
    if (0 == monitorModeRssiCfg->measurementsStartPeriod_ms)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeRssiCfg->measurementsStartPeriod_ms,
            "'measurementsStartPeriod_ms = 0' is currently not supported by FW.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Validate state is STANDBY */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    for (port = 0; port < ADI_ARRAY_LEN(ports); port++)
    {
        for (channel = 0; channel < ADI_ARRAY_LEN(channels); channel++)
        {
            adi_common_port_to_index(ports[port], &port_index);
            adi_common_channel_to_index(channels[channel], &chan_index);
	        if (ADI_ADRV910X_CHANNEL_RF_ENABLED == state.channelStates[port_index][chan_index])
            {
                ADI_ERROR_REPORT(&device->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channel,
                                 "Invalid channel state. Channel must be in STANDBY, CALIBRATED or PRIMED state");
                ADI_ERROR_RETURN(device->common.error.newAction);
            }
        }
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure(adi_adrv910x_Device_t *device,
                                                                          adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    uint8_t armData[16] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    static const uint8_t OBJID_CFG_MONITOR_MODE_RSSI = 0xAD;

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure_Validate, device, monitorModeRssiCfg);

	ADI_MUTEX_AQUIRE(device);
	
    offset += 4;
    armData[offset++] = monitorModeRssiCfg->numberOfMeasurementsToAverage;
    armData[offset++] = monitorModeRssiCfg->measurementsStartPeriod_ms;
    offset += 2;
    adrv910x_LoadFourBytes(&offset, armData, monitorModeRssiCfg->measurementDuration_samples);
    adrv910x_LoadFourBytes(&offset, armData, monitorModeRssiCfg->detectionThreshold_mdBFS);

    extData[0] = 0;
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_MONITOR_MODE_RSSI;

    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_SET_OPCODE, extData, sizeof(extData));

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect_Validate(adi_adrv910x_Device_t *device,
                                                                                                       adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    ADI_NULL_PTR_RETURN(&device->common, monitorModeRssiCfg);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect(adi_adrv910x_Device_t *device,
                                                                        adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    uint8_t armReadBack[12] = { 0 };
    uint8_t channelMask = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2);;
    uint32_t offset = 0;
    static const uint8_t OBJID_CFG_MONITOR_MODE_RSSI = 0xAD;

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect_Validate, device, monitorModeRssiCfg);

	ADI_MUTEX_AQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_arm_MailBox_Get, device, OBJID_GS_CONFIG, OBJID_CFG_MONITOR_MODE_RSSI, channelMask, offset, armReadBack, sizeof(armReadBack));
    monitorModeRssiCfg->numberOfMeasurementsToAverage = armReadBack[offset++];
    monitorModeRssiCfg->measurementsStartPeriod_ms = armReadBack[offset++];
    offset += 2;
    adrv910x_ParseFourBytes(&offset, armReadBack, &monitorModeRssiCfg->measurementDuration_samples);
    adrv910x_ParseFourBytes(&offset, armReadBack, (uint32_t *)(&monitorModeRssiCfg->detectionThreshold_mdBFS));

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare(adi_adrv910x_Device_t *device)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[5] = { 0 };

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2); /* channelMask = RxNB */
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_IC_RX_DMR_PD;
	
	ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_SET_OPCODE, extData, sizeof(extData));

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run_Validate(adi_adrv910x_Device_t *device,
                                                                                       uint8_t *initCalsError)
{
    adi_adrv910x_RadioState_t currentState = { 0 };
    uint8_t chanId = 0;

    ADI_NULL_PTR_RETURN(&device->common, initCalsError);

    adi_common_channel_to_index(ADI_CHANNEL_2, &chanId); /* Always RxNB */

    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &currentState);
    if (currentState.channelStates[ADI_RX][chanId] != ADI_ADRV910X_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_RX][chanId],
                         "Channel Rx2 must be in CALIBRATED");
        ADI_API_RETURN(device)
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run(adi_adrv910x_Device_t *device,
                                                                       uint32_t timeout_ms,
                                                                       uint8_t *initCalsError)
{
    initCals_t initCals = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run_Validate, device, initCalsError);
	
	ADI_MUTEX_AQUIRE(device);

    initCals.sysInitCalMask = 0;
    initCals.chanInitCalMask[0] = 0;
    initCals.chanInitCalMask[1] = INIT_CAL_RX_DMR_PATH_DELAY;
    initCals.calMode = INIT_CAL_MODE_ALL;

    ADI_EXPECT(adi_adrv910x_cals_InitCals_Run, device, &initCals, timeout_ms, initCalsError);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Calibrate(adi_adrv910x_Device_t *device,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern,
                                                                             adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector,
                                                                             uint32_t timeout_ms,
                                                                             uint8_t *initCalsError,
                                                                             uint16_t *pathDelay,
                                                                             uint8_t *syncIndex)
{
	ADI_MUTEX_AQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare, device);
    ADI_EXPECT(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure, device, monitorModePattern);
    ADI_EXPECT(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_Vector_Configure, device, monitorModeVector);
    ADI_EXPECT(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run, device, timeout_ms, initCalsError);
    ADI_EXPECT(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get, device, pathDelay, syncIndex);
    
	ADI_MUTEX_RELEASE(device);
	ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get(adi_adrv910x_Device_t *device,
                                                                       uint16_t *pathDelay, uint8_t *syncIndex)
{
    uint8_t armReadBack[4] = { 0 };
    uint8_t channelMask = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2);
    uint32_t offset = 0;

    ADI_ENTRY_PTR_EXPECT(device, pathDelay);
	ADI_ENTRY_PTR_EXPECT(device, syncIndex);
	
	ADI_MUTEX_AQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_arm_MailBox_Get, device, OBJID_GO_CAL_STATUS, OBJID_IC_RX_DMR_PD, channelMask, offset, armReadBack, sizeof(armReadBack))

    adrv910x_ParseTwoBytes(&offset, armReadBack, pathDelay);
    *syncIndex = armReadBack[offset];

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure_Validate(adi_adrv910x_Device_t *device,
                                                                                                              adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg)
{
	static const int32_t MIN_POWER_THRESHOLD = (int32_t)(-256*(1<<23)); // -256 in S9.23
	static const uint32_t MAX_DETECT_MULTIPLIER = 0x7FFFFFFF;
	
	ADI_NULL_PTR_RETURN(&device->common, dmrSearchCfg);
    ADI_RANGE_CHECK(device, dmrSearchCfg->pathDelay, 0, 2047);
	ADI_RANGE_CHECK(device, dmrSearchCfg->powerThreshold_dBm, MIN_POWER_THRESHOLD, 0);
	ADI_RANGE_CHECK(device, dmrSearchCfg->detectMultiplier, 0, MAX_DETECT_MULTIPLIER);
	ADI_RANGE_CHECK(device, dmrSearchCfg->preCount, 0, 65535);
	ADI_RANGE_CHECK(device, dmrSearchCfg->postCount, 0, 65535);
    ADI_RANGE_CHECK(device, dmrSearchCfg->algoMode, ADI_ADRV910X_POWERSAVINGANDMONITORMODE_DMRSEARCH_DETECTION_MODE_CORR, ADI_ADRV910X_POWERSAVINGANDMONITORMODE_DMRSEARCH_DETECTION_MODE_ERR);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure(adi_adrv910x_Device_t *device,
                                                                               adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg)
{
    uint8_t armData[22] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 4;

    ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure_Validate, device, dmrSearchCfg);

    extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2); /* channelMask = RxNB */
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_MONITOR_DMR_SEARCH;

    adrv910x_LoadFourBytes(&offset, armData, dmrSearchCfg->pathDelay);
	adrv910x_LoadFourBytes(&offset, armData, dmrSearchCfg->powerThreshold_dBm);
	adrv910x_LoadFourBytes(&offset, armData, dmrSearchCfg->detectMultiplier);
    adrv910x_LoadTwoBytes(&offset, armData, dmrSearchCfg->preCount);
    adrv910x_LoadTwoBytes(&offset, armData, dmrSearchCfg->postCount);
    armData[offset++] = (uint8_t)dmrSearchCfg->algoMode;

	ADI_MUTEX_AQUIRE(device);
	
    ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, armData, sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, ADRV910X_ARM_SET_OPCODE, extData, sizeof(extData));

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Inspect(adi_adrv910x_Device_t *device,
	adi_adrv910x_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg)
{
	uint8_t armReadBack[22] = { 0 };
	uint8_t extData[5] = { 0 };
	uint32_t offset = 0;
	
	ADI_ENTRY_PTR_EXPECT(device, dmrSearchCfg);	

	extData[0] = adi_adrv910x_Radio_MailboxChannel_Get(ADI_RX, ADI_CHANNEL_2); /* channelMask = RxNB */
	extData[1] = OBJID_GS_CONFIG;
	extData[2] = OBJID_CFG_MONITOR_DMR_SEARCH;
    
	ADI_MUTEX_AQUIRE(device);

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

	/* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
		(uint8_t)ADRV910X_ARM_GET_OPCODE,
		extData[1],
		(uint32_t)ADI_ADRV910X_DEFAULT_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_DEFAULT_INTERVAL_US);

	/* read the ARM memory to get DmrSearch status */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		armReadBack,
		sizeof(armReadBack),
		false,
		ADI_PS1)

	adrv910x_ParseFourBytes(&offset, armReadBack, &dmrSearchCfg->pathDelay);
	adrv910x_ParseFourBytes(&offset, armReadBack, &dmrSearchCfg->powerThreshold_dBm);
	adrv910x_ParseFourBytes(&offset, armReadBack, &dmrSearchCfg->detectMultiplier);
	adrv910x_ParseTwoBytes(&offset, armReadBack, &dmrSearchCfg->preCount);
	adrv910x_ParseTwoBytes(&offset, armReadBack, &dmrSearchCfg->postCount);
	dmrSearchCfg->algoMode = (adi_adrv910x_PowerSavingAndMonitorMode_MonitorDetectionMode_e)armReadBack[offset++];

	ADI_MUTEX_RELEASE(device);
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Set_Validate(adi_adrv910x_Device_t *device,
																																  bool syncDetectionEnable, adi_adrv910x_GpioPin_e detectedPin){
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_RANGE_CHECK(device, syncDetectionEnable, false, true);
	ADI_RANGE_CHECK(device, detectedPin, ADI_ADRV910X_GPIO_UNASSIGNED, ADI_ADRV910X_GPIO_DIGITAL_15);

    /* Validate state is CALIBRATED, PRIMED, or RF_ENABLED */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(ADI_CHANNEL_2, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_PRIMED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_RF_ENABLED != state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
		    ADI_CHANNEL_2,
            "RxNB must be in CALIBRATED, PRIMED, or RF_ENABLED state");
    }
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Set(adi_adrv910x_Device_t *device,
																					bool syncDetectionEnable, adi_adrv910x_GpioPin_e detectedPin)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[2] = { 0 };

	ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Set_Validate, device, syncDetectionEnable, detectedPin);
    
    armData[0] = (uint8_t)syncDetectionEnable;  // 1: Enabled; 0: Disabled  
	armData[1] = (uint8_t)detectedPin - 1; //PintoMailbox Conversion due to enum

	if (detectedPin != ADI_ADRV910X_GPIO_UNASSIGNED)
	{
		adi_adrv910x_GpioCfg_t gpioCfg = {
			.pin = detectedPin,
			.polarity = ADI_ADRV910X_GPIO_POLARITY_NORMAL,
			.master = ADI_ADRV910X_GPIO_MASTER_ADRV910X
		};
		ADI_EXPECT(adi_adrv910x_gpio_Ps1_Configure, device, ADI_ADRV910X_GPIO_SIGNAL_CONTINUOUS_SYNC_DETECTION, &gpioCfg);
	}

	ADI_MUTEX_AQUIRE(device);

    /* Write Continuous Sync Detection Enable to ARM mailbox*/
	ADI_EXPECT(adi_adrv910x_arm_Memory_Write, device, (uint32_t)ADRV910X_ADDR_ARM_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV910X_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4, ADI_PS1);
    
    extData[1] = OBJID_GS_CONTINUOUS_SYNC_DETECTION;
    
	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_SET_OPCODE, &extData[0], sizeof(extData));
    
    /* Wait for command to finish executing */
	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
    (uint8_t)ADRV910X_ARM_SET_OPCODE,
    extData[1],
		(uint32_t)ADI_ADRV910X_READARMCFG_TIMEOUT_US,
		(uint32_t)ADI_ADRV910X_READARMCFG_INTERVAL_US);
    
    ADI_MUTEX_RELEASE(device);

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Get_Validate(adi_adrv910x_Device_t *device,
																																  bool *syncDetectionEnable, adi_adrv910x_GpioPin_e *detectedPin)
{
    adi_adrv910x_RadioState_t state = { 0 };
    uint8_t port_index = 0;
    uint8_t chan_index = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Validate state is CALIBRATED, PRIMED, or RF_ENABLED */
    ADI_EXPECT(adi_adrv910x_Radio_State_Get, device, &state);
    adi_common_port_to_index(ADI_RX, &port_index);
    adi_common_channel_to_index(ADI_CHANNEL_2, &chan_index);
    if ((ADI_ADRV910X_CHANNEL_CALIBRATED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_PRIMED != state.channelStates[port_index][chan_index]) &&
        (ADI_ADRV910X_CHANNEL_RF_ENABLED != state.channelStates[port_index][chan_index]))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
		    ADI_CHANNEL_2,
		    "RxNB must be in CALIBRATED, PRIMED, or RF_ENABLED state");
		}
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Get(adi_adrv910x_Device_t *device,
																					bool *syncDetectionEnable, adi_adrv910x_GpioPin_e *detectedPin)
{
    uint8_t armReadBack[4] = { 0 };	
    uint8_t extData[2] = { 0 };

	ADI_PERFORM_VALIDATION(adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Get_Validate, device, syncDetectionEnable, detectedPin);

    extData[1] = OBJID_GS_CONTINUOUS_SYNC_DETECTION;

    ADI_MUTEX_AQUIRE(device);

    ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, device, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(device,
        (uint8_t)ADRV910X_ARM_GET_OPCODE,
        extData[1],
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
        (uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

	/* read the ARM memory to get continuous sync detect enable */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
		device,
		ADRV910X_ADDR_ARM_MAILBOX_GET,
		&armReadBack[0],
		sizeof(armReadBack),
		false, ADI_PS1);
		
	*syncDetectionEnable = (bool)(armReadBack[0] & 0x1);
	if (detectedPin != NULL)
	{
		adi_adrv910x_GpioCfg_t gpioCfg = {0};

		ADI_EXPECT(adi_adrv910x_gpio_Ps1_Inspect, device, ADI_ADRV910X_GPIO_SIGNAL_CONTINUOUS_SYNC_DETECTION, &gpioCfg);
		*detectedPin = gpioCfg.pin;
	}

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_powerSavingAndMonitorMode_MonitorMode_ContinuousSyncDetect_Status_Get(adi_adrv910x_Device_t *adrv910x,
																						   adi_adrv910x_PowerSavingAndMonitorMode_SyncDetectionResult_t *syncDetectionResult)
{
	uint8_t extData[2] = {0};

	ADI_NULL_PTR_RETURN(&adrv910x->common, syncDetectionResult);

	extData[1] = OBJID_GO_SYNCDETECT_DATA_GET;

	ADI_MUTEX_AQUIRE(adrv910x);

	ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));

	ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
										(uint8_t)ADRV910X_ARM_GET_OPCODE,
										extData[1],
										(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
										(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);

	/* read the ARM memory to get continuous sync detect enable */
	ADI_EXPECT(adi_adrv910x_arm_Memory_Read,
			   adrv910x,
			   ADRV910X_ADDR_ARM_MAILBOX_GET,
			   (uint8_t*)syncDetectionResult,
			   sizeof(adi_adrv910x_PowerSavingAndMonitorMode_SyncDetectionResult_t),
			   false, ADI_PS1);
	
	/* send Ack if in sync */
	if (syncDetectionResult->syncDetected)
	{
		extData[1] = OBJID_GO_SYNCDETECT_DATA_READ_DONE;
		ADI_EXPECT(adi_adrv910x_arm_Cmd_Write, adrv910x, (uint8_t)ADRV910X_ARM_GET_OPCODE, &extData[0], sizeof(extData));
		
		ADRV910X_ARM_CMD_STATUS_WAIT_EXPECT(adrv910x,
			(uint8_t)ADRV910X_ARM_GET_OPCODE,
			extData[1],
			(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_TIMEOUT_US,
			(uint32_t)ADI_ADRV910X_RX_INTERFACE_CONTROL_INTERVAL_US);
	}

	ADI_MUTEX_RELEASE(adrv910x);
	ADI_API_RETURN(adrv910x);
}

