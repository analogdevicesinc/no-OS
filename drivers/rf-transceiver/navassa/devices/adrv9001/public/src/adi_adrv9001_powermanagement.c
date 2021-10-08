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

static __maybe_unused int32_t adi_adrv9001_powermanagement_Configure_Validate(adi_adrv9001_Device_t *adrv9001,
                                                                              adi_adrv9001_PowerManagementSettings_t *powerManagementSettings)
{
    uint8_t i = 0;
    uint8_t wrData[4] = { 0xDE, 0xAD, 0xBE, 0xEF };
    uint8_t rdData[4] = { 0 };
    uint32_t fixedPattern = 0;
    uint32_t readPattern = 0;
    uint32_t offset = 0;

    ADI_NULL_PTR_RETURN(&adrv9001->common, powerManagementSettings);
    
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ADRV9001_ADDR_ARM_START_DATA, wrData, sizeof(wrData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, ADRV9001_ADDR_ARM_START_DATA, rdData, sizeof(rdData), ADI_ADRV9001_ARM_MEM_AUTO_INCR);

    adrv9001_ParseFourBytes(&offset, wrData, &fixedPattern);
    offset = 0;
    adrv9001_ParseFourBytes(&offset, rdData, &readPattern);

    if (readPattern != fixedPattern)
    {
        ADI_ERROR_REPORT(&adrv9001->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            readPattern,
            "ARM memory is not accessible. Check whether device clock and/or power is connected to the ADRV9001 board");
        ADI_ERROR_RETURN(adrv9001->common.error.newAction);
    }

    for (i = 0; i < ADI_ADRV9001_NUM_LDOS; i++)
    {
        ADI_RANGE_CHECK(adrv9001, 
                        powerManagementSettings->ldoPowerSavingModes[i],
                        ADI_ADRV9001_LDO_POWER_SAVING_MODE_1,
                        ADI_ADRV9001_LDO_POWER_SAVING_MODE_5);
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
    
    ADI_PERFORM_VALIDATION(adi_adrv9001_powermanagement_Configure_Validate, adrv9001, powerManagementSettings);
    
    ADI_EXPECT(adi_adrv9001_arm_Memory_Read, adrv9001, LDO_POWER_SAVING_MODES_LOCATION, addrData, sizeof(addrData), ADI_ADRV9001_ARM_MEM_AUTO_INCR);
    adrv9001_ParseFourBytes(&offset, addrData, &ldoPowerSavingModesAddr);
    adrv9001_ParseFourBytes(&offset, addrData, &ldoConfigsAddr);
    
    for (i = 0; i < ADI_ADRV9001_NUM_LDOS; i++)
    {
        modesData[i] = powerManagementSettings->ldoPowerSavingModes[i];
    }
    ADI_EXPECT(adi_adrv9001_arm_Memory_Write, adrv9001, ldoPowerSavingModesAddr, modesData, sizeof(modesData), ADI_ADRV9001_ARM_SINGLE_SPI_WRITE_MODE_STANDARD_BYTES_4);
    
    ADI_API_RETURN(adrv9001);
}
