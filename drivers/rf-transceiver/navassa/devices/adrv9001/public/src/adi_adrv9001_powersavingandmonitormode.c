/**
* \file
* \brief Contains Power saving and Monitor mode features related function implementation defined in
* adi_adrv9001_PowerSavingAndMonitorMode.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2021 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "adi_adrv9001_user.h"
#include "adi_adrv9001_powersavingandmonitormode.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_mcs.h"
#include "adi_adrv9001_cals.h"

#include "adrv9001_arm.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_arm_error_mapping.h"
#include "adrv9001_bf.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_validators.h"
#include "adrv9001_bf.h"

#include "object_ids.h"

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                                                           adi_common_ChannelNumber_e channel,
                                                                                                           adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    static const uint32_t TX_CHANNELS[] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
    static const uint32_t RX_CHANNELS[] = { ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 };
    uint8_t chan_index = 0;
    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;
    adi_adrv9001_GpioCfg_t gpio = { 0 };
    adi_adrv9001_GpioSignal_e signal = ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL1;
    if (ADI_CHANNEL_2 == channel)
    {
        signal = ADI_ADRV9001_GPIO_SIGNAL_POWER_SAVING_CHANNEL2;
    }

    ADI_NULL_PTR_RETURN(&device->common, powerSavingCfg);

    /* Check for valid channel */
    ADI_PERFORM_VALIDATION(adi_adrv9001_Channel_Validate, device, channel);

    /* 'channelDisabledPowerDownMode' and 'gpioPinPowerDownMode' are restricted between ADI_ADRV9001_POWERDOWNMODE_DISABLED and ADI_ADRV9001_POWERDOWNMODE_LDO */
    ADI_RANGE_CHECK(device,
                    powerSavingCfg->channelDisabledPowerDownMode,
                    ADI_ADRV9001_POWERSAVINGANDMONITORMODE_CHANNEL_MODE_DISABLED,
                    ADI_ADRV9001_POWERSAVINGANDMONITORMODE_CHANNEL_MODE_LDO);

    ADI_EXPECT(adi_adrv9001_gpio_Inspect, device, signal, &gpio);

    if (powerSavingCfg->gpioPinPowerDownMode != ADI_ADRV9001_POWERSAVINGANDMONITORMODE_CHANNEL_MODE_DISABLED)
    {
        if (ADI_ADRV9001_GPIO_UNASSIGNED == gpio.pin)
        {
            ADI_ERROR_REPORT(&device->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                powerSavingCfg->gpioPinPowerDownMode,
                "If gpioPinPowerDownMode is not POWERDOWN_DISABLED, then powerSavingGpio pin must be ASSIGNED.");
            ADI_API_RETURN(device)

        }
        /* gpioPinPowerDownMode must be greater than channelDisabledPowerDownMode */
        ADI_RANGE_CHECK(device,
                        powerSavingCfg->gpioPinPowerDownMode,
                        powerSavingCfg->channelDisabledPowerDownMode + 1,
                        ADI_ADRV9001_POWERSAVINGANDMONITORMODE_CHANNEL_MODE_LDO);
    }

    adi_common_channel_to_index(channel, &chan_index);
    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, RX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to configure power saving. Rx channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }


    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, TX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to configure power saving. Tx channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Configure(adi_adrv9001_Device_t *device,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    uint8_t armData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Configure_Validate, device, channel, powerSavingCfg);

    armData[0] = (uint8_t)channel;
    armData[1] = ADRV9001_ARM_HIGHPRIORITY_SET_POWER_SAVING_CONFIG;
    armData[2] = (uint8_t)powerSavingCfg->channelDisabledPowerDownMode;
    armData[3] = (uint8_t)powerSavingCfg->gpioPinPowerDownMode;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_HIGHPRIORITY_OPCODE, armData, sizeof(armData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
                                        armData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                                                         adi_common_ChannelNumber_e channel,
                                                                                                         adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    static const uint32_t TX_CHANNELS[] = { ADI_ADRV9001_TX1, ADI_ADRV9001_TX2 };
    static const uint32_t RX_CHANNELS[] = { ADI_ADRV9001_RX1, ADI_ADRV9001_RX2 };
    uint8_t chan_index = 0;

    adi_adrv9001_ChannelState_e state = ADI_ADRV9001_CHANNEL_STANDBY;

    /* Check device pointer and rxInterfaceGainCtrl are not null */
    ADI_ENTRY_PTR_EXPECT(device, powerSavingCfg);

    ADI_RANGE_CHECK(device, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    adi_common_channel_to_index(channel, &chan_index);

    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, RX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_RX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to inspect power saving configuration. Rx channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }

    if (ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, TX_CHANNELS[chan_index]))
    {
        ADI_EXPECT(adi_adrv9001_Radio_Channel_State_Get, device, ADI_TX, channel, &state);

        if (ADI_ADRV9001_CHANNEL_STANDBY == state)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_CHECK_PARAM,
                             currentState.channelStates[port_index][chan_index],
                             "Error while attempting to inspect power saving configuration. Specified channel is in wrong state.");
            ADI_API_RETURN(device)
        }
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Inspect(adi_adrv9001_Device_t *device,
                                                                          adi_common_ChannelNumber_e channel,
                                                                          adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerSavingCfg_t *powerSavingCfg)
{
    uint8_t armReadBack[2] = { 0 };
    uint8_t extData[5] = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_ChannelPowerSaving_Inspect_Validate, device, channel, powerSavingCfg);

    extData[0] = (uint8_t)channel;
    extData[1] = OBJID_GO_GET_POWER_SAVING_CONFIG;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false)

    powerSavingCfg->channelDisabledPowerDownMode = (adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerDownMode_e)armReadBack[0];
    powerSavingCfg->gpioPinPowerDownMode = (adi_adrv9001_PowerSavingAndMonitorMode_ChannelPowerDownMode_e)armReadBack[1];

    ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused SystemPowerSavingAndMonitorMode_Configure_Validate(adi_adrv9001_Device_t *device,
                                                                                 adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    adi_adrv9001_GpioCfg_t gpio = { 0 };

    /* Rx1 must be enabled to support Monitor Mode feature */
    if ((monitorModeCfg->detectionTime_us!= 0) &&
        (0 == ADRV9001_BF_EQUAL(device->devStateInfo.initializedChannels, ADI_ADRV9001_RX1)))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            device->devStateInfo.initializedChannels,
            "Rx1 channel must be initialized to support Monitor Mode feature");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_NULL_PTR_RETURN(&device->common, monitorModeCfg);

    ADI_RANGE_CHECK(device,
                    monitorModeCfg->powerDownMode,
                    ADI_ADRV9001_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_CLKPLL,
                    ADI_ADRV9001_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_ARM);

    ADI_RANGE_CHECK(device,
                    monitorModeCfg->detectionMode,
                    ADI_ADRV9001_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI,
                    ADI_ADRV9001_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_FFT);

    if (ADI_ADRV9001_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_SYNC == monitorModeCfg->detectionMode)
    {
        if (ADI_ADRV9001_POWERSAVINGANDMONITORMODE_SYSTEM_MODE_ARM == monitorModeCfg->powerDownMode)
        {
            if (!((ADI_ADRV9001_RX_FREQUENCY_DEVIATION == device->devStateInfo.rxOutputSignaling[0]) ||
                  (24000 == KILO_TO_BASE_UNIT(device->devStateInfo.rxOutputRate_kHz[0])) ||
                  (ADI_ADRV9001_RX1 == (device->devStateInfo.initializedChannels & ADI_ADRV9001_RX1))))
            {
                ADI_ERROR_REPORT(&device->common,
                    ADI_COMMON_ERRSRC_API,
                    ADI_COMMON_ERR_INV_PARAM,
                    ADI_COMMON_ACT_ERR_CHECK_PARAM,
                    monitorModeCfg->powerDownMode,
                    "Monitor mode sync is supported in 24 kHz, ADI_ADRV9001_RX_FREQUENCY_DEVIATION on Rx1 only");
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
                "ADI_ADRV9001_MONITOR_DETECTION_MODE_SYNC is only supported in ADI_ADRV9001_SYSTEM_MODE_ARM");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
    }

    if ((ADI_ADRV9001_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_FFT       == monitorModeCfg->detectionMode) ||
        (ADI_ADRV9001_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_SYNC == monitorModeCfg->detectionMode) ||
        (ADI_ADRV9001_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI_FFT  == monitorModeCfg->detectionMode))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeCfg->detectionMode,
            "Not supported yet");
        ADI_ERROR_RETURN(device->common.error.newAction);

    }

    ADI_EXPECT(adi_adrv9001_gpio_Inspect, device, ADI_ADRV9001_GPIO_SIGNAL_MON_ENABLE_SPS, &gpio);
    ADI_RANGE_CHECK(device, gpio.pin, ADI_ADRV9001_GPIO_DIGITAL_00, ADI_ADRV9001_GPIO_DIGITAL_15);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Configure(adi_adrv9001_Device_t *device,
                                                                                         adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    uint8_t armData[16] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    ADI_PERFORM_VALIDATION(SystemPowerSavingAndMonitorMode_Configure_Validate, device, monitorModeCfg);

    /* Ensure core.ahb_spi_bridge_enable is always set to 0x1 */
    ADI_EXPECT(adi_adrv9001_arm_AhbSpiBridge_Enable, device);

    adrv9001_LoadFourBytes(&offset, armData, monitorModeCfg->initialBatterySaverDelay_us);
    adrv9001_LoadFourBytes(&offset, armData, monitorModeCfg->detectionTime_us);
    adrv9001_LoadFourBytes(&offset, armData, monitorModeCfg->sleepTime_us);
    armData[offset++] = (uint8_t)monitorModeCfg->bbicWakeupLevelEnable;
    armData[offset++] = (uint8_t)monitorModeCfg->externalPllEnable;

    /* Write monitor mode configuration parameters to ARM data memory */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, device, ADRV9001_ADDR_ARM_HIGHPRIORITY_MAILBOX_SET, &armData[0], sizeof(armData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);

    extData[0] = 0;
    extData[1] = ADRV9001_ARM_HIGHPRIORITY_SET_MONITOR_MODE_CONFIG;
    extData[2] = (uint8_t)(monitorModeCfg->powerDownMode);
    extData[3] = monitorModeCfg->detectionFirst;
    extData[4] = (uint8_t)(monitorModeCfg->detectionMode);

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_HIGHPRIORITY_OPCODE, extData, sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_HIGHPRIORITY_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Inspect(adi_adrv9001_Device_t *device,
                                                                                       adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t *monitorModeCfg)
{
    uint8_t armReadBack[17] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;

    /* Check device pointer and rxInterfaceGainCtrl are not null */
    ADI_ENTRY_PTR_EXPECT(device, monitorModeCfg);

    extData[0] = 0;
    extData[1] = OBJID_GO_GET_MONITOR_CONFIG;

    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, &extData[0], sizeof(extData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        (uint8_t)ADRV9001_ARM_GET_OPCODE,
                                        extData[1],
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    /* read the ARM memory to get RSSI status */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read,
               device,
               ADRV9001_ADDR_ARM_MAILBOX_GET,
               &armReadBack[0],
               sizeof(armReadBack),
               false)

    monitorModeCfg->powerDownMode = (adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerDownMode_e)armReadBack[offset++];
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->initialBatterySaverDelay_us);
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->detectionTime_us);
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeCfg->sleepTime_us);
    monitorModeCfg->detectionFirst = armReadBack[offset++];
    monitorModeCfg->detectionMode = (adi_adrv9001_PowerSavingAndMonitorMode_MonitorDetectionMode_e)armReadBack[offset++];
    monitorModeCfg->bbicWakeupLevelEnable = (armReadBack[offset++] == 1) ? true : false;
    monitorModeCfg->externalPllEnable = (armReadBack[offset++] == 1) ? true : false;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingMode_Set(adi_adrv9001_Device_t *adrv9001,
                                                                         adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerDownMode_e mode)
{
    adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t config = {
        .initialBatterySaverDelay_us = 0,
        .detectionTime_us = 0,
        .sleepTime_us = 0xFFFFFFFF,
        .detectionFirst = 0,
        .detectionMode = ADI_ADRV9001_POWERSAVINGANDMONITORMODE_MONITOR_DETECTION_MODE_RSSI,
        .bbicWakeupLevelEnable = false,
    };

    config.powerDownMode = mode;
    ADI_EXPECT(adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Configure, adrv9001, &config);

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingMode_Get_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                                        adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerDownMode_e *mode)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, mode);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingMode_Get(adi_adrv9001_Device_t *adrv9001,
                                                                         adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerDownMode_e *mode)
{
    adi_adrv9001_PowerSavingAndMonitorMode_SystemPowerSavingAndMonitorModeCfg_t config = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingMode_Get_Validate, adrv9001, mode);

    ADI_EXPECT(adi_adrv9001_powerSavingAndMonitorMode_SystemPowerSavingAndMonitorMode_Inspect, adrv9001, &config);
    *mode = config.powerDownMode;

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                                            adi_adrv9001_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern)
{
    uint8_t readAddrOffset = 0;
    const uint32_t dpinfifoLength = 2048;

    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModePattern);

    /* The valid range for patternLength strictly depends on how the DPinFIFO is
     * configured. Exceeding this limit could result in errors that are very hard
     * to detect, so it is worthwhile to provide exact checking here. */
    ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoRdAddrOffset_Get, adrv9001, ADRV9001_BF_RX1_CORE, &readAddrOffset);
    ADI_RANGE_CHECK(adrv9001, monitorModePattern->patternLength, 0, dpinfifoLength - readAddrOffset);

    ADI_API_RETURN(adrv9001);
}

/* TODO: Change this function to be private when a function to measure
 * the value from CALIBRATED and program the value to the ADRV9001 is added */
int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                             adi_adrv9001_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern)
{
    uint16_t pattern = 0;
    uint16_t patternLength = 0;
    uint32_t readPattern = 0;

    int32_t halError = 0;
    uint32_t numEventChecks = 1;
    uint32_t eventCheck = 0;
    uint8_t testDataUpdate = 0;
    bool updateCompleted = false;

    static const uint32_t ADI_ADRV9001_MONITOR_PATTERN_WRITE_INTERVAL_US = 1000;
    static const uint32_t ADI_ADRV9001_MONITOR_PATTERN_WRITE_TIMEOUT_US = 50000;    /* < 50 us expected */

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure_Validate, adrv9001, monitorModePattern);
    /* Select DpinFIFO test pattern; 1: select test pattern. 0: select datapath data */
    ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataSel_Set, adrv9001, ADRV9001_BF_RX1_CORE, 0x1);
    /* Set dpinfifo_control: dpinfifo_en, detected, not rd_startstop, not wr_startstop. */
    ADI_EXPECT(adi_bf_hal_Register_Write, adrv9001, ADRV9001_BF_RX1_CORE + 0xac, 0x81);

    patternLength = monitorModePattern->patternLength;
    numEventChecks = ADI_ADRV9001_MONITOR_PATTERN_WRITE_TIMEOUT_US / ADI_ADRV9001_MONITOR_PATTERN_WRITE_INTERVAL_US;

    for (pattern = 0; pattern < patternLength; pattern++)
    {
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataI_Set, adrv9001, ADRV9001_BF_RX1_CORE, monitorModePattern->patternI[pattern]);
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataI_Get, adrv9001, ADRV9001_BF_RX1_CORE, &readPattern);
        if (readPattern != monitorModePattern->patternI[pattern])
        {
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                readPattern,
                "Test pattern I is not written properly in DpInFIFO.");
            ADI_ERROR_RETURN(adrv9001->common.error.newAction);
        }
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataQ_Set, adrv9001, ADRV9001_BF_RX1_CORE, monitorModePattern->patternQ[pattern]);
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestdataQ_Get, adrv9001, ADRV9001_BF_RX1_CORE, &readPattern);
        if (readPattern != monitorModePattern->patternQ[pattern])
        {
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_INV_PARAM,
                ADI_COMMON_ACT_ERR_CHECK_PARAM,
                readPattern,
                "Test pattern Q is not written properly in DpInFIFO.");
            ADI_ERROR_RETURN(adrv9001->common.error.newAction);
        }

        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestDataUpdate_Set, adrv9001, ADRV9001_BF_RX1_CORE, 0x1);

        /* timeout event check loop, waiting for 'test_data_update' bit to clear */
        ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestDataUpdate_Get, adrv9001, ADRV9001_BF_RX1_CORE, &testDataUpdate);
        updateCompleted = (testDataUpdate == 0u);
        for (eventCheck = 0; (eventCheck <= numEventChecks) && !updateCompleted; eventCheck++)
        {
            /* polling delay */
            halError = adi_common_hal_Wait_us(&adrv9001->common, ADI_ADRV9001_MONITOR_PATTERN_WRITE_INTERVAL_US);
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_ADRV9001_SRC_ARMCMD,
                halError,
                ADI_COMMON_ACT_ERR_CHECK_TIMER,
                device,
                "Timer not working in adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure()");
            ADI_ERROR_RETURN(adrv9001->common.error.newAction);

            /* check whether 'test_data_update' bit is reset to '0' after delay */
            ADI_EXPECT(adrv9001_NvsRegmapRx_DpinfifoTestDataUpdate_Get, adrv9001, ADRV9001_BF_RX1_CORE, &testDataUpdate);
            updateCompleted = (testDataUpdate == 0u);
        }

        /* report if the write failed */
        if (!updateCompleted)
        {
            ADI_ERROR_REPORT(&adrv9001->common,
                ADI_ADRV9001_SRC_ARMCMD,
                ADI_COMMON_ERR_API_FAIL,
                ADI_ADRV9001_ACT_ERR_RESET_ARM,
                device,
                "Time out!!! adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure() failed due to 'test_data_update' bit not getting reset after write operation.");
            ADI_ERROR_RETURN(adrv9001->common.error.newAction);
        }

    }
    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                                           adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
{
    static uint16_t VECTOR_MASK_MAX = 0x3FFF;
    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModeVector);
    ADI_RANGE_CHECK(adrv9001, monitorModeVector->vectorMask, 0x1, VECTOR_MASK_MAX);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                            adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
{
    uint8_t mask = 0;
    uint8_t nibSel = 0;
    uint8_t vectorCrumb = 0;
    uint8_t vectorNibble = 0;
    uint16_t bfValue = 0;
    uint32_t vectorConverted = 0;
    static const uint8_t MONITOR_MODE_VECTOR_MAX = 14;
    static const uint8_t MAX_NIBBLE_PER_VECTOR = 12;

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Configure_Validate, adrv9001, monitorModeVector);

    /* Hard code D14 and D15 = 0x1 */
    bfValue = monitorModeVector->vectorMask | 0xC000;
    ADI_EXPECT(adrv9001_NvsRegmapRx_CorrCtrl_Set, adrv9001, ADRV9001_BF_RX1_CORE, bfValue);

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
            ADI_EXPECT(adrv9001_NvsRegmapRx_Vcorrsig_Set, adrv9001, ADRV9001_BF_RX1_CORE, mask, vectorConverted);
            vectorConverted = 0;
        }
    }

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                                         adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModeVector);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                                          adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector)
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

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Inspect_Validate, adrv9001, monitorModeVector);

    ADI_EXPECT(adrv9001_NvsRegmapRx_CorrCtrl_Get, adrv9001, ADRV9001_BF_RX1_CORE, &bfValue);
    monitorModeVector->vectorMask = bfValue & 0x3FFF;

    for (mask = 0; mask < MONITOR_MODE_VECTOR_MAX; mask++)
    {
        if (bfValue & (1 << mask))
        {
            ADI_EXPECT(adrv9001_NvsRegmapRx_Vcorrsig_Get, adrv9001, ADRV9001_BF_RX1_CORE, mask, &readvector);

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

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                                         adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    adi_adrv9001_RadioState_t state = { 0 };
    uint8_t port = 0;
    uint8_t channel = 0;
    uint8_t port_index = 0;
    uint8_t chan_index = 0;
    uint32_t MEASUREMENT_DURATION_SAMPLES_MAX = 0x1FFFFF;
    int32_t  DETECTION_THRESHOLD_MIN = -140000;
    adi_common_Port_e ports[2] = { ADI_RX, ADI_TX };
    adi_common_ChannelNumber_e channels[2] = { ADI_CHANNEL_1, ADI_CHANNEL_2 };
    ADI_NULL_PTR_RETURN(&adrv9001->common, monitorModeRssiCfg);

    ADI_RANGE_CHECK(adrv9001, monitorModeRssiCfg->measurementDuration_samples, 0x2, MEASUREMENT_DURATION_SAMPLES_MAX);
    ADI_RANGE_CHECK(adrv9001, monitorModeRssiCfg->detectionThreshold_mdBFS, DETECTION_THRESHOLD_MIN, 0);

    if (0 == monitorModeRssiCfg->numberOfMeasurementsToAverage)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeRssiCfg->numberOfMeasurementsToAverage,
            "'numberOfMeasurementsToAverage' cannot be 0.");
        ADI_ERROR_RETURN(adrv9001->common.error.newAction);
    }

    /* TODO: Remove this check in future.
     * measurementsStartPeriod_ms = 0: Continuous RSSI measurements (currently not supported by FW) */
    if (0 == monitorModeRssiCfg->measurementsStartPeriod_ms)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            monitorModeRssiCfg->measurementsStartPeriod_ms,
            "'measurementsStartPeriod_ms = 0' is currently not supported by FW.");
        ADI_ERROR_RETURN(adrv9001->common.error.newAction);
    }

    /* Validate state is STANDBY */
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &state);
    for (port = 0; port < ADI_ARRAY_LEN(ports); port++)
    {
        for (channel = 0; channel < ADI_ARRAY_LEN(channels); channel++)
        {
            adi_common_port_to_index(ports[port], &port_index);
            adi_common_channel_to_index(channels[channel], &chan_index);
	        if (ADI_ADRV9001_CHANNEL_RF_ENABLED == state.channelStates[port_index][chan_index])
            {
                ADI_ERROR_REPORT(&adrv9001->common,
                                 ADI_COMMON_ERRSRC_API,
                                 ADI_COMMON_ERR_INV_PARAM,
                                 ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                 channel,
                                 "Invalid channel state. Channel must be in STANDBY, CALIBRATED or PRIMED state");
                ADI_ERROR_RETURN(adrv9001->common.error.newAction);
            }
        }
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                          adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    uint8_t armData[16] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 0;
    static const uint8_t OBJID_CFG_MONITOR_MODE_RSSI = 0xAD;

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Configure_Validate, adrv9001, monitorModeRssiCfg);

    offset += 4;
    armData[offset++] = monitorModeRssiCfg->numberOfMeasurementsToAverage;
    armData[offset++] = monitorModeRssiCfg->measurementsStartPeriod_ms;
    offset += 2;
    adrv9001_LoadFourBytes(&offset, armData, monitorModeRssiCfg->measurementDuration_samples);
    adrv9001_LoadFourBytes(&offset, armData, monitorModeRssiCfg->detectionThreshold_mdBFS);

    extData[0] = 0;
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_MONITOR_MODE_RSSI;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect_Validate(adi_adrv9001_Device_t *device,
                                                                                                       adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    ADI_NULL_PTR_RETURN(&device->common, monitorModeRssiCfg);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                                        adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeRssiCfg_t *monitorModeRssiCfg)
{
    uint8_t armReadBack[12] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;
    static const uint8_t OBJID_CFG_MONITOR_MODE_RSSI = 0xAD;

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Rssi_Inspect_Validate, adrv9001, monitorModeRssiCfg);

    ADI_EXPECT(adi_adrv9001_arm_Config_Read, adrv9001, OBJID_CFG_MONITOR_MODE_RSSI, channelMask, offset, armReadBack, sizeof(armReadBack))

    monitorModeRssiCfg->numberOfMeasurementsToAverage = armReadBack[offset++];
    monitorModeRssiCfg->measurementsStartPeriod_ms = armReadBack[offset++];
    offset += 2;
    adrv9001_ParseFourBytes(&offset, armReadBack, &monitorModeRssiCfg->measurementDuration_samples);
    adrv9001_ParseFourBytes(&offset, armReadBack, (uint32_t *)(&monitorModeRssiCfg->detectionThreshold_mdBFS));

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare(adi_adrv9001_Device_t *adrv9001)
{
    uint8_t armData[4] = { 0 };
    uint8_t extData[5] = { 0 };

    extData[0] = 1; /* channelMask = Rx1 */
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_IC_RX_DMR_PD;

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                       uint8_t *initCalsError)
{
    adi_adrv9001_RadioState_t currentState = { 0 };
    uint8_t chanId = 0u;  /* Always Rx1 */

    ADI_NULL_PTR_RETURN(&adrv9001->common, initCalsError);

    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    if (currentState.channelStates[ADI_RX][chanId] != ADI_ADRV9001_CHANNEL_CALIBRATED)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         currentState.channelStates[ADI_RX][chanId],
                         "Channel Rx1 must be in CALIBRATED");
        ADI_API_RETURN(adrv9001)
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run(adi_adrv9001_Device_t *adrv9001,
                                                                       uint32_t timeout_ms,
                                                                       uint8_t *initCalsError)
{
    adi_adrv9001_InitCals_t initCals = { 0 };

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run_Validate, adrv9001, initCalsError);

    initCals.sysInitCalMask = 0;
    initCals.chanInitCalMask[0] = ADI_ADRV9001_INIT_CAL_RX_DMR_PATH_DELAY;
    initCals.chanInitCalMask[1] = 0;
    initCals.calMode = ADI_ADRV9001_INIT_CAL_MODE_ALL;

    ADI_EXPECT(adi_adrv9001_cals_InitCals_Run, adrv9001, &initCals, timeout_ms, initCalsError);

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Calibrate(adi_adrv9001_Device_t *adrv9001,
                                                                             adi_adrv9001_PowerSavingAndMonitorMode_MonitorModePatternCfg_t *monitorModePattern,
                                                                             adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeVectorCfg_t *monitorModeVector,
                                                                             uint32_t timeout_ms,
                                                                             uint8_t *initCalsError,
                                                                             uint32_t *pathDelay)
{
    ADI_EXPECT(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Prepare, adrv9001);
    ADI_EXPECT(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Pattern_Configure, adrv9001, monitorModePattern);
    ADI_EXPECT(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_Vector_Configure, adrv9001, monitorModeVector);
    ADI_EXPECT(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Run, adrv9001, timeout_ms, initCalsError);
    ADI_EXPECT(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get, adrv9001, pathDelay);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_RxDmrPd_Get(adi_adrv9001_Device_t *adrv9001,
                                                                       uint32_t *pathDelay)
{
    uint8_t armReadBack[4] = { 0 };
    uint8_t channelMask = 0;
    uint32_t offset = 0;

    ADI_ENTRY_PTR_EXPECT(adrv9001, pathDelay);
    ADI_EXPECT(adi_adrv9001_arm_MailBox_Get, adrv9001, OBJID_GO_CAL_STATUS, OBJID_IC_RX_DMR_PD, channelMask, offset, armReadBack, sizeof(armReadBack))

    adrv9001_ParseFourBytes(&offset, armReadBack, pathDelay);

    ADI_API_RETURN(adrv9001);
}

static __maybe_unused int32_t __maybe_unused adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                                                              adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg)
{
    ADI_NULL_PTR_RETURN(&adrv9001->common, dmrSearchCfg);
    ADI_RANGE_CHECK(adrv9001, dmrSearchCfg->pathDelay, 0, 2047);
    ADI_RANGE_CHECK(adrv9001, dmrSearchCfg->magcorrTh, 0, 0x7FFFFFFF);
    ADI_RANGE_CHECK(adrv9001, dmrSearchCfg->detCnt1, 0, 2047);
    ADI_RANGE_CHECK(adrv9001, dmrSearchCfg->detCnt2, dmrSearchCfg->detCnt1, 2047);
    ADI_RANGE_CHECK(adrv9001, dmrSearchCfg->detTgtMin, 0, 0x7FFFFFFF);
    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure(adi_adrv9001_Device_t *adrv9001,
                                                                               adi_adrv9001_PowerSavingAndMonitorMode_MonitorModeDmrSearchCfg_t *dmrSearchCfg)
{
    uint8_t armData[24] = { 0 };
    uint8_t extData[5] = { 0 };
    uint32_t offset = 4;     /* Skip size - not used */

    ADI_PERFORM_VALIDATION(adi_adrv9001_powerSavingAndMonitorMode_MonitorMode_DmrSearch_Configure_Validate, adrv9001, dmrSearchCfg);

    extData[0] = 0; /* channelMask = Rx1 */
    extData[1] = OBJID_GS_CONFIG;
    extData[2] = OBJID_CFG_MONITOR_DMR_SEARCH;

    adrv9001_LoadFourBytes(&offset, armData, dmrSearchCfg->pathDelay);
    adrv9001_LoadFourBytes(&offset, armData, dmrSearchCfg->magcorrTh);
    adrv9001_LoadTwoBytes(&offset, armData, dmrSearchCfg->detCnt1);
    adrv9001_LoadTwoBytes(&offset, armData, dmrSearchCfg->detCnt2);
    offset += 4;        /* Skip four unused bytes */
    adrv9001_LoadFourBytes(&offset, armData, dmrSearchCfg->detTgtMin);

    ADI_EXPECT(adi_adrv9001_arm_Config_Write, adrv9001, armData, sizeof(armData), extData, sizeof(extData))

    ADI_API_RETURN(adrv9001);
}
