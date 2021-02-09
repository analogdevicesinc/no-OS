/**
 * \file
 * \brief Low Dropout Regulator (LDO) configuration
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2020 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_adrv9001_powermanagement.h"
#include "adi_adrv9001_arm.h"
#include "adrv9001_arm_macros.h"

static int32_t adi_adrv9001_ldo_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                   adi_adrv9001_PowerManagementSettings_t *powerManagementSettings)
{
    uint8_t i = 0;
    ADI_NULL_PTR_RETURN(&adrv9001->common, powerManagementSettings);
    
    for (i = 0; i < ADI_ADRV9001_NUM_LDOS; i++)
    {
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoPowerSavingModes[i],
                        ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
                        ADI_ADRV9001_LDO_POWER_SAVING_MODE_5);
    }
    
    for (i = 0; i < ADI_ADRV9001_NUM_LDOS_CONFIGURABLE; i++)
    {
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoConfigs[i].shuntResistanceOff,
                        ADI_ADRV9001_LDO_SHUNT_RESISTANCE_HIGH_Z,
                        ADI_ADRV9001_LDO_SHUNT_RESISTANCE_300_OHM);
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoConfigs[i].shuntResistancePowerSave,
                        ADI_ADRV9001_LDO_SHUNT_RESISTANCE_HIGH_Z,
                        ADI_ADRV9001_LDO_SHUNT_RESISTANCE_300_OHM);
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoConfigs[i].shuntResistanceNormal,
                        ADI_ADRV9001_LDO_SHUNT_RESISTANCE_HIGH_Z,
                        ADI_ADRV9001_LDO_SHUNT_RESISTANCE_300_OHM);
        
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoConfigs[i].diffPairBiasOff,
                        ADI_ADRV9001_LDO_DIFFERENTIAL_PAIR_BIAS_25_PERCENT,
                        ADI_ADRV9001_LDO_DIFFERENTIAL_PAIR_BIAS_100_PERCENT);
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoConfigs[i].diffPairBiasPowerSave,
                        ADI_ADRV9001_LDO_DIFFERENTIAL_PAIR_BIAS_25_PERCENT,
                        ADI_ADRV9001_LDO_DIFFERENTIAL_PAIR_BIAS_100_PERCENT);
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoConfigs[i].diffPairBiasNormal,
                        ADI_ADRV9001_LDO_DIFFERENTIAL_PAIR_BIAS_25_PERCENT,
                        ADI_ADRV9001_LDO_DIFFERENTIAL_PAIR_BIAS_100_PERCENT);
    }
    
    ADI_API_RETURN(adrv9001);
}


int32_t adi_adrv9001_powermanagement_Configure(adi_adrv9001_Device_t *adrv9001,
                                               adi_adrv9001_PowerManagementSettings_t *powerManagementSettings)
{
    uint8_t addrData[8] = { 0 };
    uint32_t offset = 0;
    uint32_t ldoPowerSavingModesAddr = 0;
    uint32_t ldoConfigsAddr = 0;
    uint8_t i = 0;
    uint8_t modesData[ADI_ADRV9001_NUM_LDOS] = { 0 };
    uint8_t configData[ADI_ADRV9001_NUM_LDOS_CONFIGURABLE * 6] = { 0 };
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_ldo_Configure_Validate, adrv9001, powerManagementSettings);
    
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, LDO_POWER_SAVING_MODES_LOCATION, addrData, sizeof(addrData), ADI_ADRV9001_ARM_MEM_AUTO_INCR);
    adrv9001_ParseFourBytes(&offset, addrData, &ldoPowerSavingModesAddr);
    adrv9001_ParseFourBytes(&offset, addrData, &ldoConfigsAddr);
    
    for (i = 0; i < ADI_ADRV9001_NUM_LDOS; i++)
    {
        modesData[i] = powerManagementSettings->ldoPowerSavingModes[i];
    }
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ldoPowerSavingModesAddr, modesData, sizeof(modesData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    
    for (i = 0; i < ADI_ADRV9001_NUM_LDOS_CONFIGURABLE; i++)
    {
        configData[i * 6 + 0] = powerManagementSettings->ldoConfigs[i].shuntResistanceOff;
        configData[i * 6 + 1] = powerManagementSettings->ldoConfigs[i].diffPairBiasOff;
        configData[i * 6 + 2] = powerManagementSettings->ldoConfigs[i].shuntResistancePowerSave;
        configData[i * 6 + 3] = powerManagementSettings->ldoConfigs[i].diffPairBiasPowerSave;
        configData[i * 6 + 4] = powerManagementSettings->ldoConfigs[i].shuntResistanceNormal;
        configData[i * 6 + 5] = powerManagementSettings->ldoConfigs[i].diffPairBiasNormal;
    }
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ldoConfigsAddr, configData, sizeof(configData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    
    ADI_API_RETURN(adrv9001);
}
