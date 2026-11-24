/**
* \file
* \brief API for interacting with the ADM1293 digital power monitor
*
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adm1293.h"
#include "adi_adm1293_hal.h"
#include "adi_common_error.h"

int32_t adi_adm1293_Configure(adi_adm1293_Device_t *adm1293)
{
    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
    uint8_t i = 0;
    adi_adrv910x_PowerMonitor_e powerMonitors[] = { 
        ADI_ADRV910X_POWERMONITOR_VDD_1P0,
        ADI_ADRV910X_POWERMONITOR_VDDA_1P0,
        ADI_ADRV910X_POWERMONITOR_VDD_1P8,
        ADI_ADRV910X_POWERMONITOR_VDDA_1P8
    };
    uint8_t wrData[][3] = {
        { 0xD4, 0x14, 0x07 },
        { 0xD4, 0x14, 0x07 },
        { 0xD4, 0x18, 0x07 },
        { 0xD4, 0x18, 0x07 },
        { 0xD4, 0x18, 0x07 },
    };
    
    ADI_NULL_DEVICE_PTR_RETURN(adm1293);
    
    for (i = 0; i < ADI_ARRAY_LEN(powerMonitors); i++)
    {
        recoveryAction = adi_adm1293_hal_i2c_write(adm1293->common.devHalInfo, powerMonitors[i], wrData[i], ADI_ARRAY_LEN(wrData[i]));
        ADI_ERROR_RETURN(recoveryAction);
    }
    
    return recoveryAction;
}

int32_t adi_adm1293_Read(adi_adm1293_Device_t *adm1293,
                         adi_adrv910x_PowerMonitor_e powerMonitor,
                         uint32_t *measuredVoltage_uV,
                         uint32_t *measuredCurrent_uA)
{
	int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
   
	recoveryAction = adi_adm1293_VoltageRead(adm1293, powerMonitor, measuredVoltage_uV);
	ADI_ERROR_RETURN(recoveryAction);
	recoveryAction = adi_adm1293_CurrentRead(adm1293, powerMonitor, measuredCurrent_uA);
	
	return recoveryAction;
}

int32_t adi_adm1293_VoltageRead(adi_adm1293_Device_t *adm1293,
								adi_adrv910x_PowerMonitor_e powerMonitor,
								uint32_t *measuredVoltage_uV)
{

	int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
	uint8_t buff[2] = { 0 };
	uint32_t voltage = 0;
    
	static const uint8_t VOLTAGE_ADDR = 0x88;
    
	ADI_NULL_DEVICE_PTR_RETURN(adm1293);
	ADI_NULL_PTR_RETURN(adm1293, measuredVoltage_uV);
    
	recoveryAction = adi_adm1293_hal_i2c_read(adm1293->common.devHalInfo, powerMonitor, &VOLTAGE_ADDR, 1, buff, 2);
	if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
	{
		ADI_ERROR_REPORT(adm1293,
			ADI_COMMON_ERRSRC_ADI_HAL,
			ADI_COMMON_ERR_API_FAIL,
			ADI_COMMON_ACT_ERR_RESET_FEATURE,
			NULL,
			"i2c read failed");
		ADI_ERROR_RETURN(recoveryAction);
	}
	voltage = (((uint16_t)buff[1] << 8) | buff[0]);
    
	if ((ADI_ADRV910X_POWERMONITOR_VDD_1P0 == powerMonitor) ||
	    (ADI_ADRV910X_POWERMONITOR_VDDA_1P0 == powerMonitor))
	{
		*measuredVoltage_uV = 300 * voltage + 150;
	}
	else
	{
		*measuredVoltage_uV = 1801 * voltage + 901;
	}
        
	return recoveryAction;
}

int32_t adi_adm1293_CurrentRead(adi_adm1293_Device_t *adm1293,
								adi_adrv910x_PowerMonitor_e powerMonitor,
								uint32_t *measuredCurrent_uA)
{
	int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;
	uint8_t buff[2] = { 0 };
	uint32_t current = 0;
    
	static const uint8_t CURRENT_ADDR = 0x8c;
    
	ADI_NULL_DEVICE_PTR_RETURN(adm1293);
	ADI_NULL_PTR_RETURN(adm1293, measuredCurrent_uA);
    
	recoveryAction = adi_adm1293_hal_i2c_read(adm1293->common.devHalInfo, powerMonitor, &CURRENT_ADDR, 1, buff, 2);
	if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
	{
		ADI_ERROR_REPORT(adm1293,
			ADI_COMMON_ERRSRC_ADI_HAL,
			ADI_COMMON_ERR_API_FAIL,
			ADI_COMMON_ACT_ERR_RESET_FEATURE,
			NULL,
			"i2c read failed");
		ADI_ERROR_RETURN(recoveryAction);
	}
	current = ((uint16_t)buff[1] << 8) | buff[0];
    
	if (current == 0xFFFF || current == 0xFFFE)
	{
		*measuredCurrent_uA = 0;
	}
	else
	{
			*measuredCurrent_uA = 1250 * current + 1250;
	}
    
	return recoveryAction;
}
