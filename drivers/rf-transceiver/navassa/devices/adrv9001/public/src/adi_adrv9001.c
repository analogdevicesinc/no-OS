/**
* \file
* \brief Contains Adrv9001 features related function implementation defined in
* adi_adrv9001.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adi_adrv9001.h"
#include "adi_adrv9001_hal.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_spi.h"
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_types.h"
#include "adi_adrv9001_version.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_arm_macros.h"

int32_t adi_adrv9001_HwOpen(adi_adrv9001_Device_t *device, adi_adrv9001_SpiSettings_t *spiSettings)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_EXPECT(adi_adrv9001_HwOpenNoReset, device, spiSettings);

    /* Toggle RESETB pin, Configure and Verify SPI */
    ADI_MSG_EXPECT("Failed to reset device and set SPI Config", adi_adrv9001_HwReset, device);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_HwOpenNoReset(adi_adrv9001_Device_t *device, adi_adrv9001_SpiSettings_t *spiSettings)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    ADI_ENTRY_EXPECT(device);

    recoveryAction = adi_adrv9001_hal_open(device->common.devHalInfo);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_ADI_HAL,
                     recoveryAction,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "Error opening HW");

    device->spiSettings.autoIncAddrUp		= spiSettings->autoIncAddrUp;
    device->spiSettings.cmosPadDrvStrength	= spiSettings->cmosPadDrvStrength;
    device->spiSettings.enSpiStreaming		= spiSettings->enSpiStreaming;
    device->spiSettings.fourWireMode		= spiSettings->fourWireMode;
    device->spiSettings.msbFirst			= spiSettings->msbFirst;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_HwClose(adi_adrv9001_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    ADI_ENTRY_EXPECT(device);

    adi_adrv9001_hal_close(device->common.devHalInfo);
    recoveryAction = adi_adrv9001_hal_close(device->common.devHalInfo);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_ADI_HAL,
                     recoveryAction,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "Error closing HW");

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_HwReset(adi_adrv9001_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t RESETB_LEVEL_LOW = 0;
    static const uint8_t RESETB_LEVEL_HIGH = 1;
    static const uint32_t RESETB_WAIT_US = 1000;

    ADI_ENTRY_EXPECT(device);

    /* toggle RESETB on device with matching spi chip select index */
    recoveryAction = adi_adrv9001_hal_resetbPin_set(device->common.devHalInfo, RESETB_LEVEL_LOW);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         NULL,
                         "GPIO Reset error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_common_hal_Wait_us(&device->common, RESETB_WAIT_US);

    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_TIMER,
                         NULL,
                         "Timer error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv9001_hal_resetbPin_set(device->common.devHalInfo, RESETB_LEVEL_HIGH);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         NULL,
                         "GPIO Reset error");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Configure and Verify Spi */
    ADI_MSG_EXPECT("Failed to set SPI Config", adi_adrv9001_spi_Configure, device, &device->spiSettings);

    device->devStateInfo.devState = ADI_ADRV9001_STATE_POWERON_RESET;

    ADI_API_RETURN(device);
}

static void cacheInitInfo(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_Init_t *init)
{
    uint8_t i = 0;

    for (i = 0; i < ADI_ADRV9001_MAX_RXCHANNELS; i++)
    {
        adrv9001->devStateInfo.rxOutputRate_kHz[i] = init->rx.rxChannelCfg[i].profile.rxOutputRate_Hz / 1000;
    }
    
    if (adrv9001->devStateInfo.profilesValid & ADI_ADRV9001_TX_PROFILE_VALID)
    {
        adrv9001->devStateInfo.outputSignaling[0] = init->tx.txProfile[0].outputSignaling;
        adrv9001->devStateInfo.outputSignaling[1] = init->tx.txProfile[1].outputSignaling;
        adrv9001->devStateInfo.txInputRate_kHz[0] = init->tx.txProfile[0].txInputRate_Hz / 1000;
        adrv9001->devStateInfo.txInputRate_kHz[1] = init->tx.txProfile[1].txInputRate_Hz / 1000;
    }
    
    /* Save whether we are in FH mode to state */
    adrv9001->devStateInfo.frequencyHoppingEnabled = init->sysConfig.fhModeOn;
}

int32_t adi_adrv9001_InitAnalog(adi_adrv9001_Device_t *device,
                                adi_adrv9001_Init_t *init,
                                adi_adrv9001_DeviceClockDivisor_e deviceClockOutDivisor)
{
    adi_adrv9001_Info_t devStateInfoClear = { 0 };

    ADI_ENTRY_PTR_EXPECT(device, init);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest >  0)
    {
        devStateInfoClear.swTest = device->devStateInfo.swTest;
    }
#endif

    device->devStateInfo = devStateInfoClear;

    ADI_EXPECT(adrv9001_InitAnalog, device, init, deviceClockOutDivisor);

    /* Disable stream pin mode until after streams are loaded */
    /* Disable Tx pin mode for all Tx and Rx channels, ORx was defaulted with pin mode disabled */
    ADRV9001_SPIWRITEBYTE(device, "BBIC_ENABLES", ADRV9001_ADDR_BBIC_ENABLES, 0x00);

    device->devStateInfo.devState = (adi_adrv9001_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV9001_STATE_ANA_INITIALIZED);

    cacheInitInfo(device, init);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Shutdown(adi_adrv9001_Device_t *device)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_MSG_EXPECT("Failed to reset device", adi_adrv9001_HwReset, device);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Configure(adi_adrv9001_Device_t *device, adi_adrv9001_SpiSettings_t *spi)
{
    uint8_t spiConfigA = 0;
    uint8_t spiConfigB = 0;
    //uint8_t ioControl = 0;
    static const uint8_t SPICFG_MSBFIRST_OFF = 0;
    static const uint8_t SPICFG_AUTOINC_DOWN = 0;
    static const uint8_t SPICFG_FOURWIREMODE_OFF = 0;
    static const uint8_t SPICFG_ENSPISTREAMING_OFF = 0;

    ADI_ENTRY_PTR_EXPECT(device, spi);

    if ((spi->cmosPadDrvStrength != ADI_ADRV9001_CMOSPAD_DRV_WEAK) &&
        (spi->cmosPadDrvStrength != ADI_ADRV9001_CMOSPAD_DRV_STRONG))
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            spi->cmosPadDrvStrength,
            "Invalid CMOS Pad Drive Strength\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* core.spi_interface_config_B */
    if (spi->enSpiStreaming == SPICFG_ENSPISTREAMING_OFF)
    {
        spiConfigB |= ADRV9001_CONFIG_B_SINGLE_INSTRUCTION;
    }

    /* Force single instruction mode */
    ADRV9001_SPIWRITEBYTE(device, "SPI_INTERFACE_CONFIG_B", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_B, spiConfigB);

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (spi->msbFirst == SPICFG_MSBFIRST_OFF)
    {
        spiConfigA |= ADRV9001_CONFIG_A_SPI_LSB_FIRST;
    }

    if (spi->autoIncAddrUp != SPICFG_AUTOINC_DOWN)
    {
        spiConfigA |= ADRV9001_CONFIG_A_SPI_ADDR_ASCENSION;
    }

    if (spi->fourWireMode != SPICFG_FOURWIREMODE_OFF)
    {
        spiConfigA |= ADRV9001_CONFIG_A_SPI_SDO_ACTIVE;
    }

    ADRV9001_SPIWRITEBYTE(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, spiConfigA);

/* FIXME - Start: Vivek - In Tokelau, ADRV9001_ADDR_DIGITAL_IO_CONTROL is find in YODA file;
 * In Navassa, this is missing. Need to check whether it is needed.
 * Commenting this code snippet and related variables/constants used in this function for now
*/

    //if (spi->cmosPadDrvStrength == ADI_ADRV9001_CMOSPAD_DRV_STRONG)
    //{
        //ioControl |= ADRV9001_IO_CONTROL_SPI_OUTS_DRV_SEL;
    //}

    /* Force PAD mode */
    //ADRV9001_SPIWRITEBYTE(device, "DIGITAL_IO_CONTROL", ADRV9001_ADDR_DIGITAL_IO_CONTROL, ioControl);

/* FIXME - End: Vivek - In Tokelau, ADRV9001_ADDR_DIGITAL_IO_CONTROL is find in YODA file;
 * In Navassa, this is missing. Need to check whether it is needed.
 * Commenting this code snippet and related variables/constants used in this function for now
*/

#if ADI_ADRV9001_PRE_MCS_BROADCAST_DISABLE > 0
    ADI_MSG_EXPECT("SPI Verify failed", adi_adrv9001_spi_Verify, device) ;
#endif

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Inspect(adi_adrv9001_Device_t *device, adi_adrv9001_SpiSettings_t *spi)
{
    uint8_t spiConfigA = 0;
    uint8_t spiConfigB = 0;
    //uint8_t ioControl = 0;
    static const uint8_t SPICFG_MSBFIRST_OFF = 0;
    static const uint8_t SPICFG_AUTOINC_DOWN = 0;
    static const uint8_t SPICFG_FOURWIREMODE_OFF = 0;
    static const uint8_t SPICFG_ENSPISTREAMING_OFF = 0;
    static const uint8_t SPICFG_MSBFIRST_ON = 1;
    static const uint8_t SPICFG_AUTOINC_UP = 1;
    static const uint8_t SPICFG_FOURWIREMODE_ON = 1;
    static const uint8_t SPICFG_ENSPISTREAMING_ON = 1;

    ADI_ENTRY_PTR_EXPECT(device, spi);

    ADRV9001_SPIREADBYTE(device, "SPI_INTERFACE_CONFIG_A", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_A, &spiConfigA);

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (ADRV9001_BF_EQUAL(spiConfigA, ADRV9001_CONFIG_A_SPI_LSB_FIRST))
    {
        spi->msbFirst = SPICFG_MSBFIRST_OFF;
    }
    else
    {
        spi->msbFirst = SPICFG_MSBFIRST_ON;
    }

    if (ADRV9001_BF_EQUAL(spiConfigA, ADRV9001_CONFIG_A_SPI_ADDR_ASCENSION))
    {
        spi->autoIncAddrUp = SPICFG_AUTOINC_UP;
    }
    else
    {
        spi->autoIncAddrUp = SPICFG_AUTOINC_DOWN;
    }

    if (ADRV9001_BF_EQUAL(spiConfigA, ADRV9001_CONFIG_A_SPI_SDO_ACTIVE))
    {
        spi->fourWireMode = SPICFG_FOURWIREMODE_ON;
    }
    else
    {
        spi->fourWireMode = SPICFG_FOURWIREMODE_OFF;
    }

    /* Read single instruction mode */
    ADRV9001_SPIREADBYTE(device, "SPI_INTERFACE_CONFIG_B", ADRV9001_ADDR_SPI_INTERFACE_CONFIG_B, &spiConfigB);

    /* core.spi_interface_config_B */
    if (ADRV9001_BF_EQUAL(spiConfigB, ADRV9001_CONFIG_B_SINGLE_INSTRUCTION))
    {
        spi->enSpiStreaming = SPICFG_ENSPISTREAMING_OFF;
    }
    else
    {
        spi->enSpiStreaming = SPICFG_ENSPISTREAMING_ON;
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_spi_Verify(adi_adrv9001_Device_t *device)
{
    uint8_t spiReg = 0;
    static const uint8_t SCRATCH_PAD_1 = 0xB6; /* DATA 10110110 */
    static const uint8_t SCRATCH_PAD_2 = 0x49; /* DATA 01001001 */
    static const uint8_t SCRATCH_PAD_3 = 0xA5; /* DATA 10100101 */
    static const uint8_t VENDOR_ID_0   = 0x56;
    static const uint8_t VENDOR_ID_1   = 0x04;

    ADI_ENTRY_EXPECT(device);

    /* Check SPI read - VENDOR_ID_0 */
    ADRV9001_SPIREADBYTE(device, "VENDOR_ID_0", ADRV9001_ADDR_VENDOR_ID_0, &spiReg);
#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 1)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != VENDOR_ID_0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot read from a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI read - VENDOR_ID_1 */
    spiReg = 0;
    ADRV9001_SPIREADBYTE(device, "VENDOR_ID_1", ADRV9001_ADDR_VENDOR_ID_1, &spiReg);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 2)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != VENDOR_ID_1)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot read from a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCHPAD : Data = 10110110 */
    spiReg = 0;
    ADRV9001_SPIWRITEBYTE(device, "SCRATCH_PAD", ADRV9001_ADDR_SCRATCH_PAD, SCRATCH_PAD_1);

    ADRV9001_SPIREADBYTE(device, "SCRATCH_PAD", ADRV9001_ADDR_SCRATCH_PAD, &spiReg);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 3)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != SCRATCH_PAD_1)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot write to a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCHPAD : Data = 01001001 */
    spiReg = 0;
    ADRV9001_SPIWRITEBYTE(device, "SCRATCH_PAD", ADRV9001_ADDR_SCRATCH_PAD, SCRATCH_PAD_2);

    ADRV9001_SPIREADBYTE(device, "SCRATCH_PAD", ADRV9001_ADDR_SCRATCH_PAD, &spiReg);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 4)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != SCRATCH_PAD_2)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot write to a low SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI read - ADRV9001_ADDR_SCRATCH_PAD_READ_ONLY_UPPER_ADDRESS_SPACE : Data = 10100101 */
    spiReg = 0;
    ADRV9001_SPIREADBYTE(device, "SCRATCH_PAD_READ_ONLY_UPPER_ADDR", ADRV9001_ADDR_SCRATCH_PAD_READ_ONLY_UPPER_ADDRESS_SPACE, &spiReg);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 5)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != SCRATCH_PAD_3)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                        ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot read from a high SPI address\n");
                        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCH_PAD_UPPER_ADDRESS_SPACE : Data = 10110110 */
    spiReg = 0;
    ADRV9001_SPIWRITEBYTE(device, "SCRATCH_PAD_UPPER_ADDR", ADRV9001_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, SCRATCH_PAD_1);

    ADRV9001_SPIREADBYTE(device, "SCRATCH_PAD_UPPER_ADDR", ADRV9001_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, &spiReg);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 6)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != SCRATCH_PAD_1)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot write to a high SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCH_PAD_UPPER_ADDRESS_SPACE : Data = 01001001 */
    spiReg = 0;
    ADRV9001_SPIWRITEBYTE(device, "SCRATCH_PAD_UPPER_ADDR", ADRV9001_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, SCRATCH_PAD_2);

    ADRV9001_SPIREADBYTE(device, "SCRATCH_PAD_UPPER_ADDR", ADRV9001_ADDR_SCRATCH_PAD_UPPER_ADDRESS_SPACE, &spiReg);

#if ADI_ADRV9001_SW_TEST > 0
    if (device->devStateInfo.swTest == 7)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != SCRATCH_PAD_2)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot write to a high SPI address\n");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* TODO: Add code to check Spi Streaming when HAL support is available. */

    ADI_API_RETURN(device);
}

static uint32_t adi_adrv9001_Number_Extract(const char *src, uint8_t *location)
{
    uint8_t i = 0;
    uint32_t num = 0;
    char *source;
    source = (char *)src;

    i = *location;
    source += i;
    for (; *source != '\0'; i++, source++)
    {
        if ('.' == *source)
        {
            *location = i + 1;
            return num;
        }
        num = (num * 10) + (*source - 48);
    }
    *location = i;
    return num;
}

int32_t adi_adrv9001_ApiVersion_Get(adi_adrv9001_Device_t *device, adi_common_ApiVersion_t *apiVersion)
{
    char *version = ADI_ADRV9001_CURRENT_VERSION;
    uint8_t location = 0;

    ADI_ENTRY_PTR_EXPECT(device, apiVersion);

    apiVersion->major = adi_adrv9001_Number_Extract(version, &location);
    apiVersion->minor = adi_adrv9001_Number_Extract(version, &location);
    apiVersion->patch = adi_adrv9001_Number_Extract(version, &location);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_SiliconVersion_Get(adi_adrv9001_Device_t *device, adi_adrv9001_SiliconVersion_t *siliconVersion)
{
    uint8_t siVersion = 0;

    ADI_ENTRY_PTR_EXPECT(device, siliconVersion);

    ADRV9001_SPIREADBYTE(device, "PRODUCT_ID_1", ADRV9001_ADDR_PRODUCT_ID_1, &siVersion);

    siliconVersion->major = (siVersion >> 4) & 0xF;
    siliconVersion->minor = siVersion & 0xF;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Profiles_Verify(adi_adrv9001_Device_t *device, adi_adrv9001_Init_t *init)
{
    ADI_ENTRY_PTR_EXPECT(device, init);

    ADI_EXPECT(adrv9001_ProfilesVerify, device, init);

    ADI_API_RETURN(device);
}

static int32_t __maybe_unused adrv9001_TemperatureGetValidate(adi_adrv9001_Device_t *device,
                                                              int16_t *temperature_C)
{
    ADI_NULL_PTR_RETURN(&device->common, temperature_C);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Temperature_Get(adi_adrv9001_Device_t *device, int16_t *temperature_C)
{
    uint8_t channelMask = 0;
    uint8_t armExtData[2] = {0};
    uint8_t armReadBack[2] = {0};

    ADI_PERFORM_VALIDATION(adrv9001_TemperatureGetValidate, device, temperature_C);

    *temperature_C = 0;

    /* Channel mask is not used */
    armExtData[0] = channelMask;
    armExtData[1] = ADRV9001_ARM_OBJECTID_TEMP_SENSOR;

    /* send ARM GET opcode */
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, (uint8_t)ADRV9001_ARM_GET_OPCODE, armExtData, sizeof(armExtData));

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_GET_OPCODE,
                                        armExtData[1],
                                        ADI_ADRV9001_READ_TEMP_SENSOR_TIMEOUT_US,
                                        ADI_ADRV9001_READ_TEMP_SENSOR_INTERVAL_US);

    /* read the ARM memory to get temperature */
    /* TODO: Enable auto increment if it works with non multiples of 4 */
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, device, ADRV9001_ADDR_ARM_MAILBOX_GET, armReadBack,
        sizeof(armReadBack), false)

    /* Reconstruct temperature */
    *temperature_C = (int16_t)(((int16_t)armReadBack[0] << 0) |
                               ((int16_t)armReadBack[1] << 8));

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_PartNumber_Get(adi_adrv9001_Device_t *device, adi_adrv9001_PartNumber_e *partNumber)
{
    uint8_t     swConfig[2];

    ADI_ENTRY_PTR_EXPECT(device, partNumber);

    ADRV9001_SPIREADBYTE(device, "EFUSE_SW_CONFIG_0", ADRV9001_ADDR_EFUSE_SW_CONFIG_0, &swConfig[0]);
    ADRV9001_SPIREADBYTE(device, "EFUSE_SW_CONFIG_1", ADRV9001_ADDR_EFUSE_SW_CONFIG_1, &swConfig[1]);

    *partNumber = (adi_adrv9001_PartNumber_e)(((uint16_t)swConfig[1] << 8) | (uint16_t)swConfig[0]);
    if ((ADI_ADRV9001_PART_NUMBER_ADRV9002 != *partNumber) &&
        (ADI_ADRV9001_PART_NUMBER_ADRV9003 != *partNumber) &&
        (ADI_ADRV9001_PART_NUMBER_ADRV9004 != *partNumber))
    {
        *partNumber = ADI_ADRV9001_PART_NUMBER_UNKNOWN;
        ADI_ERROR_REPORT(device, 
                         ADI_COMMON_ERRSRC_API, 
                         ADI_COMMON_ERR_API_FAIL, 
                         ADI_COMMON_ACT_ERR_RESET_FULL, 
                         NULL, 
                         "Unknown part number detected");
    }

    ADI_API_RETURN(device);
}
