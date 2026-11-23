/**
 * \file
 * \brief ADRV910X core functionality
 * 
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adi_adrv910x.h"
#include "adi_adrv910x_hal.h"
#include "adi_adrv910x_user.h"
#include "adi_adrv910x_version.h"

#ifndef NEVIS_PS2

int32_t adi_adrv910x_HwOpen(adi_adrv910x_Device_t *device, adi_adrv910x_SpiSettings_t *spiSettings)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_EXPECT(adi_adrv910x_HwOpenNoReset, device, spiSettings);

    /* Toggle RESETB pin, Configure and Verify SPI */
    ADI_MSG_EXPECT("Failed to reset device and set SPI Config", adi_adrv910x_HwReset, device);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_HwOpenNoReset(adi_adrv910x_Device_t *device, adi_adrv910x_SpiSettings_t *spiSettings)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

	ADI_NULL_DEVICE_PTR_RETURN(device);

    recoveryAction = adi_adrv910x_hal_open(device->common.devHalInfo);
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

int32_t adi_adrv910x_HwClose(adi_adrv910x_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    ADI_ENTRY_EXPECT(device);

    adi_adrv910x_hal_close(device->common.devHalInfo);
    recoveryAction = adi_adrv910x_hal_close(device->common.devHalInfo);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_ADI_HAL,
                     recoveryAction,
                     ADI_COMMON_ACT_ERR_RESET_INTERFACE,
                     NULL,
                     "Error closing HW");

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_HwReset(adi_adrv910x_Device_t *device)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    static const uint8_t RESETB_LEVEL_LOW = 0;
    static const uint8_t RESETB_LEVEL_HIGH = 1;
    static const uint32_t RESETB_WAIT_US = 100;
    static const uint32_t RESETB_DWELL_US = 100;

    ADI_ENTRY_EXPECT(device);

    /* toggle RESETB on device with matching spi chip select index */
    recoveryAction = adi_adrv910x_hal_resetbPin_set(device->common.devHalInfo, RESETB_LEVEL_LOW);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         NULL,
                         "GPIO Reset error");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_common_hal_Wait_us(&device->common, RESETB_DWELL_US);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_CHECK_TIMER,
                         NULL,
                         "Timer error");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_adrv910x_hal_resetbPin_set(device->common.devHalInfo, RESETB_LEVEL_HIGH);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_ADI_HAL,
                         device->common.error.errCode,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         NULL,
                         "GPIO Reset error");
        ADI_MUTEX_ERROR_RELEASE(device);
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
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

	device->devStateInfo.devState = ADI_ADRV910X_STATE_POWERON_RESET;
	
    /* Configure and Verify Spi */
    ADI_MSG_EXPECT("Failed to set SPI Config", adi_adrv910x_spi_Configure, device, &device->spiSettings);

    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_Shutdown(adi_adrv910x_Device_t *device)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);
	ADI_MUTEX_AQUIRE(device);
    ADI_MSG_EXPECT("Failed to reset device", adi_adrv910x_HwReset, device);

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_spi_Configure(adi_adrv910x_Device_t *device, adi_adrv910x_SpiSettings_t *spi)
{
    uint8_t spiConfigA = 0;
    uint8_t spiConfigB = 0;
    //uint8_t ioControl = 0;
    static const uint8_t SPICFG_MSBFIRST_OFF = 0;
    static const uint8_t SPICFG_AUTOINC_DOWN = 0;
    static const uint8_t SPICFG_FOURWIREMODE_OFF = 0;
    static const uint8_t SPICFG_ENSPISTREAMING_OFF = 0;

    ADI_ENTRY_PTR_EXPECT(device, spi);

    if ((spi->cmosPadDrvStrength != ADI_ADRV910X_CMOSPAD_DRV_WEAK) &&
        (spi->cmosPadDrvStrength != ADI_ADRV910X_CMOSPAD_DRV_STRONG))
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
        spiConfigB |= ADRV910X_CONFIG_B_SINGLE_INSTRUCTION;
    }
	ADI_MUTEX_AQUIRE(device);
    /* Force single instruction mode */
    ADRV910X_SPIWRITEBYTE(device, "SPI_INTERFACE_CONFIG_B", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_B, spiConfigB);

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (spi->msbFirst == SPICFG_MSBFIRST_OFF)
    {
        spiConfigA |= ADRV910X_CONFIG_A_SPI_LSB_FIRST;
    }

    if (spi->autoIncAddrUp != SPICFG_AUTOINC_DOWN)
    {
        spiConfigA |= ADRV910X_CONFIG_A_SPI_ADDR_ASCENSION;
    }

    if (spi->fourWireMode != SPICFG_FOURWIREMODE_OFF)
    {
        spiConfigA |= ADRV910X_CONFIG_A_SPI_SDO_ACTIVE;
    }

    ADRV910X_SPIWRITEBYTE(device, "SPI_INTERFACE_CONFIG_A", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_A, spiConfigA);

#if ADI_ADRV910X_PRE_MCS_BROADCAST_DISABLE > 0
    ADI_MSG_EXPECT("SPI Verify failed", adi_adrv910x_spi_Verify, device) ;
#endif

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_spi_Inspect(adi_adrv910x_Device_t *device, adi_adrv910x_SpiSettings_t *spi)
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

	ADI_MUTEX_AQUIRE(device);
    ADRV910X_SPIREADBYTE(device, "SPI_INTERFACE_CONFIG_A", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_A, &spiConfigA);

    /* core.spi_interface_config_A */
    /* SPI bit is 1 = LSB first */
    if (ADRV910X_BF_EQUAL(spiConfigA, ADRV910X_CONFIG_A_SPI_LSB_FIRST))
    {
        spi->msbFirst = SPICFG_MSBFIRST_OFF;
    }
    else
    {
        spi->msbFirst = SPICFG_MSBFIRST_ON;
    }

    if (ADRV910X_BF_EQUAL(spiConfigA, ADRV910X_CONFIG_A_SPI_ADDR_ASCENSION))
    {
        spi->autoIncAddrUp = SPICFG_AUTOINC_UP;
    }
    else
    {
        spi->autoIncAddrUp = SPICFG_AUTOINC_DOWN;
    }

    if (ADRV910X_BF_EQUAL(spiConfigA, ADRV910X_CONFIG_A_SPI_SDO_ACTIVE))
    {
        spi->fourWireMode = SPICFG_FOURWIREMODE_ON;
    }
    else
    {
        spi->fourWireMode = SPICFG_FOURWIREMODE_OFF;
    }

    /* Read single instruction mode */
    ADRV910X_SPIREADBYTE(device, "SPI_INTERFACE_CONFIG_B", ADRV910X_ADDR_SPI_INTERFACE_CONFIG_B, &spiConfigB);

    /* core.spi_interface_config_B */
    if (ADRV910X_BF_EQUAL(spiConfigB, ADRV910X_CONFIG_B_SINGLE_INSTRUCTION))
    {
        spi->enSpiStreaming = SPICFG_ENSPISTREAMING_OFF;
    }
    else
    {
        spi->enSpiStreaming = SPICFG_ENSPISTREAMING_ON;
    }

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

int32_t adi_adrv910x_spi_Verify(adi_adrv910x_Device_t *device)
{
    uint8_t spiReg = 0;
    static const uint8_t SCRATCH_PAD_1 = 0xB6; /* DATA 10110110 */
    static const uint8_t SCRATCH_PAD_2 = 0x49; /* DATA 01001001 */
    static const uint8_t SCRATCH_PAD_3 = 0xA5; /* DATA 10100101 */
    static const uint8_t VENDOR_ID_0   = 0x56;
    static const uint8_t VENDOR_ID_1   = 0x04;

    ADI_ENTRY_EXPECT(device);

	ADI_MUTEX_AQUIRE(device);
    /* Check SPI read - VENDOR_ID_0 */
    ADRV910X_SPIREADBYTE(device, "VENDOR_ID_0", ADRV910X_ADDR_VENDOR_ID_0, &spiReg);
#if ADI_ADRV910X_SW_TEST > 0
    if (device->devStateInfo.swTest == 1)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != VENDOR_ID_0)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot read from a low SPI address\n");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI read - VENDOR_ID_1 */
    spiReg = 0;
    ADRV910X_SPIREADBYTE(device, "VENDOR_ID_1", ADRV910X_ADDR_VENDOR_ID_1, &spiReg);

#if ADI_ADRV910X_SW_TEST > 0
    if (device->devStateInfo.swTest == 2)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != VENDOR_ID_1)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot read from a low SPI address\n");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCHPAD : Data = 10110110 */
    spiReg = 0;
    ADRV910X_SPIWRITEBYTE(device, "SCRATCH_PAD", ADRV910X_ADDR_SCRATCH_PAD, SCRATCH_PAD_1);

    ADRV910X_SPIREADBYTE(device, "SCRATCH_PAD", ADRV910X_ADDR_SCRATCH_PAD, &spiReg);

#if ADI_ADRV910X_SW_TEST > 0
    if (device->devStateInfo.swTest == 3)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != SCRATCH_PAD_1)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot write to a low SPI address\n");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Check SPI write - SCRATCHPAD : Data = 01001001 */
    spiReg = 0;
    ADRV910X_SPIWRITEBYTE(device, "SCRATCH_PAD", ADRV910X_ADDR_SCRATCH_PAD, SCRATCH_PAD_2);

    ADRV910X_SPIREADBYTE(device, "SCRATCH_PAD", ADRV910X_ADDR_SCRATCH_PAD, &spiReg);

#if ADI_ADRV910X_SW_TEST > 0
    if (device->devStateInfo.swTest == 4)
    {
        spiReg = 0;
    }
#endif

    if (spiReg != SCRATCH_PAD_2)
    {
        ADI_ERROR_REPORT(&device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
                     ADI_COMMON_ACT_ERR_RESET_FULL, spiReg, "Cannot write to a low SPI address\n");
        ADI_MUTEX_ERROR_RELEASE(device);
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    
    /* Note: Only MONITOR REGMAP registers can be accessed and verified at initTime */

    /* TODO: Add code to check Spi Streaming when HAL support is available. */

    ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}

#endif /* NEVIS_PS2 */

static uint32_t number_extract(const char *src, uint8_t *location)
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

static void cacheInitInfo(adi_adrv910x_Device_t *adrv910x, deviceProfile_t *profile)
{

	adrv910x->devStateInfo.rxOutputSignaling[0] = profile->rxConfig[0].outputSignalingSel;
	adrv910x->devStateInfo.rxOutputSignaling[1] = profile->rxConfig[1].outputSignalingSel;
	adrv910x->devStateInfo.gainTableType[0] = profile->rxConfig[0].gainTableType;
	adrv910x->devStateInfo.gainTableType[1] = profile->rxConfig[1].gainTableType;

        if (adrv910x->devStateInfo.profilesValid & ADI_ADRV910X_RX_PROFILE_VALID)
        {
          adrv910x->devStateInfo.rxOutputRate_kHz[0] = profile->rxConfig[0].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxOutputRate_kHz[1] = profile->rxConfig[1].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxOutputRate_kHz[2] = profile->orxConfig[0].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxOutputRate_kHz[3] = profile->orxConfig[1].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxOutputRate_kHz[4] = profile->ilbConfig[0].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxOutputRate_kHz[5] = profile->ilbConfig[1].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxOutputRate_kHz[6] = profile->elbConfig[0].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxOutputRate_kHz[7] = profile->elbConfig[1].rxOutputRate / 1000;
          adrv910x->devStateInfo.rxNbAgcHighThresholdModeEnable = profile->rxNbConfig.gainControl.rxNbAgcHighThresholdModeEnable;
          adrv910x->devStateInfo.agcHighThresholdSettings.ApdPeakOverloadThresh = profile->rxNbConfig.gainControl.agcHighThresholdSettings.ApdPeakOverloadThresh;
          adrv910x->devStateInfo.agcHighThresholdSettings.ApdPeakUnderloadThresh = profile->rxNbConfig.gainControl.agcHighThresholdSettings.ApdPeakUnderloadThresh;
          adrv910x->devStateInfo.agcHighThresholdSettings.Hb2PeakOverloadThresh = profile->rxNbConfig.gainControl.agcHighThresholdSettings.Hb2PeakOverloadThresh;
          adrv910x->devStateInfo.agcHighThresholdSettings.Hb2PeakUnderloadThresh = profile->rxNbConfig.gainControl.agcHighThresholdSettings.Hb2PeakUnderloadThresh;
          adrv910x->devStateInfo.agcHighThresholdSettings.Hb2SecondaryUpperThresh = profile->rxNbConfig.gainControl.agcHighThresholdSettings.Hb2SecondaryUpperThresh; 
        }
    
	if (adrv910x->devStateInfo.profilesValid & ADI_ADRV910X_TX_PROFILE_VALID)
	{
		adrv910x->devStateInfo.txOutputSignaling[0] = profile->txConfig[0].outputSignalingSel;
		adrv910x->devStateInfo.txOutputSignaling[1] = profile->txConfig[1].outputSignalingSel;
		adrv910x->devStateInfo.txInputRate_kHz[0] = profile->txConfig[0].txInputRate / 1000;
		adrv910x->devStateInfo.txInputRate_kHz[1] = profile->txConfig[1].txInputRate / 1000;
	}
}

int32_t adi_adrv910x_apiVersion_get(adi_adrv910x_Device_t *adrv910x, adi_common_ApiVersion_t *apiVersion)
{
    char *version = ADI_ADRV910X_CURRENT_VERSION;
    uint8_t location = 0;
    
    ADI_ENTRY_PTR_EXPECT(adrv910x, apiVersion);
    
    apiVersion->major = number_extract(version, &location);
    apiVersion->minor = number_extract(version, &location);
    apiVersion->patch = number_extract(version, &location);
    
    ADI_API_RETURN(adrv910x);
}

int32_t adi_adrv910x_PartNumber_Get(adi_adrv910x_Device_t *device, adi_adrv910x_PartNumber_e *partNumber)
{
    uint8_t     swConfig[2];

    ADI_ENTRY_PTR_EXPECT(device, partNumber);

    ADRV910X_SPIREADBYTE(device, "EFUSE_SW_CONFIG_0", ADRV910X_ADDR_EFUSE_SW_CONFIG_0, &swConfig[0]);
    ADRV910X_SPIREADBYTE(device, "EFUSE_SW_CONFIG_1", ADRV910X_ADDR_EFUSE_SW_CONFIG_1, &swConfig[1]);

    *partNumber = (adi_adrv910x_PartNumber_e)(((uint16_t)swConfig[1] << 8) | (uint16_t)swConfig[0]);
	if ((ADI_ADRV910X_PART_NUMBER_ADRV9101 != *partNumber) &&
        (ADI_ADRV910X_PART_NUMBER_ADRV9102 != *partNumber) &&
	    (ADI_ADRV910X_PART_NUMBER_ADRV9103 != *partNumber) &&
	    (ADI_ADRV910X_PART_NUMBER_ADRV9104 != *partNumber))
	{
        *partNumber = ADI_ADRV910X_PART_NUMBER_UNKNOWN;
        ADI_ERROR_REPORT(device, 
                         ADI_COMMON_ERRSRC_API, 
                         ADI_COMMON_ERR_API_FAIL, 
                         ADI_COMMON_ACT_ERR_RESET_FULL, 
                         NULL, 
                         "Unknown part number detected");
    }

    ADI_API_RETURN(device);
}

#ifndef NEVIS_PS2

int32_t adi_adrv910x_InitAnalog(adi_adrv910x_Device_t *device,
                                deviceProfile_t *profile,
                                adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor)
{
	adi_adrv910x_Info_t devStateInfoClear = { 0 };

	ADI_ENTRY_PTR_EXPECT(device, profile);

#ifdef API_MUTEX
	extern int adi_bbic_sem_lock(void *device);
	extern int adi_bbic_sem_unlock(void *device);
	device->common.error.locked = 0;
	device->common.error.lock = adi_bbic_sem_lock;
	device->common.error.unlock = adi_bbic_sem_unlock;
#endif /* API_MUTEX */
	
	device->devStateInfo = devStateInfoClear;
	ADI_EXPECT(adrv910x_InitAnalog, device, profile, adrv910xDeviceClockOutDivisor);

//	/* Disable stream pin mode until after streams are loaded */
//	/* Disable Tx pin mode for all Tx and Rx channels, ORx was defaulted with pin mode disabled */
//	ADRV910X_SPIWRITEBYTE(device, "BBIC_ENABLES", ADRV910X_ADDR_BBIC_ENABLES, 0x00);

	device->devStateInfo.devState = (adi_adrv910x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV910X_STATE_ANA_INITIALIZED);

	cacheInitInfo(device, profile);

    ADI_API_RETURN(device);
}

#else /* NEVIS_PS2 */

int32_t adi_adrv910x_InitDevPS2(adi_adrv910x_Device_t *device,
                                deviceProfile_t *profile, halTimer timerCallback)
{
  int32_t ret = 0;
  if (profile != NULL)
  {
    ret = adrv910x_ProfilesVerify(device, profile);
    if (ret == 0)
    {
        cacheInitInfo(device, profile);
    }
  }
  
  device->devStateInfo.devState = (adi_adrv910x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV910X_STATE_ANA_INITIALIZED);

  if (timerCallback != NULL)
  {
    adi_common_hal_Wait_us = timerCallback;
  }

#ifdef API_MUTEX
  extern int adi_ps2_sem_lock(void* device);
  extern int adi_ps2_sem_unlock(void* device);
  device->common.error.locked = 0;
  device->common.error.lock = adi_ps2_sem_lock;
  device->common.error.unlock = adi_ps2_sem_unlock;
#endif /* API_MUTEX */
  return ret;
}

#endif /* NEVIS_PS2 */

int32_t adi_adrv910x_SetDeviceClockOutDivider(adi_adrv910x_Device_t *device,
	adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor)
{
    ADI_MUTEX_AQUIRE(device);
	ADI_EXPECT(adrv910x_SetDeviceClockOutDivider, device, adrv910xDeviceClockOutDivisor);

	ADI_MUTEX_RELEASE(device);
    ADI_API_RETURN(device);
}
