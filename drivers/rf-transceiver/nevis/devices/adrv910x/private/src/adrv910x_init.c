/**
 * \file
 * \brief Contains ADRV910X init related private function implementations
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_adrv910x.h"
#include "adi_adrv910x_arm.h"
#include "adi_adrv910x_spi.h"
#include "adi_adrv910x_error.h"
#include "adrv910x_init.h"
#include "adrv910x_reg_addr_macros.h"
#include "adrv910x_arm_macros.h"
#include "adrv910x_bf.h"

static __maybe_unused int32_t adrv910x_RefClockEnable(adi_adrv910x_Device_t *device)
{
	// Write Bitfield: (ref_clk_adcdac_pd) to 0
	ADRV910X_SPIWRITEBYTE(device,
		"REFCLK_ADCDAC_CONTROL_0",
		ADRV910X_ADDR_REFCLK_ADCDAC_CONTROL_0,
		0); 
	
	// Write Bitfield: (ref_clk_adcdac_buffer_enable) to 1
	// Write Bitfield: (ref_clk_adcdac_devclk_divider_mcs_resetb) to 1
	ADRV910X_SPIWRITEBYTE(device,
		"REFCLK_ADCDAC_CONTROL_1",
		ADRV910X_ADDR_REFCLK_ADCDAC_CONTROL_1,
		(0x01 | 0x10));
	
	// Write Bitfield: (dig_devclk_divide_ratio_diff_rcv) to 0
	// Write Bitfield: (dig_devclk_divide_ratio_osc_rcv) to 0
	ADRV910X_SPIWRITEBYTE(device,
		"ANALOG_REFCLK_REG_1",
		ADRV910X_ADDR_ANALOG_REFCLK_REG_1,
		0);

	ADI_API_RETURN(device);
}

static __maybe_unused int32_t adrv910x_MCSInternalRestartRefClk(adi_adrv910x_Device_t *device)
{	
	ADI_EXPECT(adrv910x_NvsRegmapCore1_Armclk2RefClkSwitchEnable_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Hs2refClockSwitchEnable_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_AdcdacHs2refClockSwitchEnable_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_AhbSpiBridgeEn_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsCaptureReceivedClear_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsCaptureReceivedClear_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsInternalGenSel_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsCaptureEnable_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsInternalGen_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsInternalGen_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_AhbSpiBridgeEn_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NvsRegmapCore1_Armclk2RefClkSwitchEnable_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_Hs2refClockSwitchEnable_Set, device, 0x0);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_AdcdacHs2refClockSwitchEnable_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsInternalGen_Set, device, 0x1);
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_McsInternalGen_Set, device, 0x0);

	ADI_API_RETURN(device);
}


/**
* \brief Verifies the Rx profile members are valid (in range).
* \brief This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to Adrv910X device data structure
* \param rxProfile rxProfile settings to be verified
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
static __maybe_unused int32_t adrv910x_VerifyRxProfile(adi_adrv910x_Device_t *device,
													   rxConfig_t *rxProfile)
{
	static const uint32_t RX_OUTPUT_RATE_MIN = KILO_TO_BASE_UNIT(1);
	static const uint32_t RX_OUTPUT_RATE_MAX = MEGA_TO_BASE_UNIT(100);

	static const uint32_t RX_BANDWIDTH_MIN = 6250;
	static const uint32_t RX_BANDWIDTH_MAX = MEGA_TO_BASE_UNIT(41);

	ADI_ENTRY_PTR_EXPECT(device, rxProfile);

	/********************************/
	/* Check for a valid Rx profile */
	/********************************/
	ADI_RANGE_CHECK(device, rxProfile->rxOutputRate, RX_OUTPUT_RATE_MIN, RX_OUTPUT_RATE_MAX);

	ADI_RANGE_CHECK(device, rxProfile->primaryBw_Hz, RX_BANDWIDTH_MIN, RX_BANDWIDTH_MAX);

	ADI_API_RETURN(device);
}

/**
* \brief Verifies the Tx profile members are valid (in range).
* \brief This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to ADRV910X device data structure
* \param txProfile txProfile settings to be verified
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
static __maybe_unused int32_t adrv910x_VerifyTxProfile(adi_adrv910x_Device_t *device,
													   txConfig_t *txProfile)
{
	static const uint32_t TX_INPUT_RATE_MIN = KILO_TO_BASE_UNIT(1);
	static const uint32_t TX_INPUT_RATE_MAX = MEGA_TO_BASE_UNIT(100);

	static const uint32_t PRIMARY_SIGNAL_BW_MIN = 6250;
	static const uint32_t PRIMARY_SIGNAL_BW_MAX = MEGA_TO_BASE_UNIT(41);

	ADI_ENTRY_PTR_EXPECT(device, txProfile);

	/********************************/
	/* Check for a valid Tx profile */
	/********************************/
	ADI_RANGE_CHECK(device, txProfile->txInputRate, TX_INPUT_RATE_MIN, TX_INPUT_RATE_MAX);

	ADI_RANGE_CHECK(device, txProfile->primaryBw_Hz, PRIMARY_SIGNAL_BW_MIN, PRIMARY_SIGNAL_BW_MAX);

	ADI_API_RETURN(device);
}

int32_t adrv910x_ProfilesVerify(adi_adrv910x_Device_t *device, deviceProfile_t *profile)
{
	rxConfig_t *rxProfile = NULL;
	txConfig_t *txProfile = NULL;
	int i = 0;
	uint32_t TX_CHANNELS[] = { ADI_ADRV910X_TX1, ADI_ADRV910X_TXNB };
	uint32_t RX_CHANNELS[] = {
		ADI_ADRV910X_RX1,
		ADI_ADRV910X_RXNB,
		ADI_ADRV910X_ORX1,
		ADI_ADRV910X_ORXNB,
		ADI_ADRV910X_ILB1,
		ADI_ADRV910X_ILBNB,
		ADI_ADRV910X_ELB1,
		ADI_ADRV910X_ELBNB
	};
	uint8_t divHsDigClk = 0;

	ADI_ENTRY_PTR_EXPECT(device, profile);

	device->devStateInfo.profilesValid = 0;

	// Validate Rx channels
	for (i = 0; i < ADI_ADRV910X_MAX_RXCHANNELS; i++)
	{
		if (ADRV910X_BF_EQUAL(profile->chanConfig, RX_CHANNELS[i]))
		{
			int index = 1;
			switch (RX_CHANNELS[i])
			{
			case ADI_ADRV910X_RX1:  /* Falls through */
				index = 0;
			case ADI_ADRV910X_RXNB:
				ADI_EXPECT(adrv910x_VerifyRxProfile, device, &profile->rxConfig[index]);
				device->devStateInfo.profilesValid |= ADI_ADRV910X_RX_PROFILE_VALID;
				break;
			case ADI_ADRV910X_ORX1: /* Falls through */
				index = 0;
			case ADI_ADRV910X_ORXNB:
				ADI_EXPECT(adrv910x_VerifyRxProfile, device, &profile->orxConfig[index]);
				device->devStateInfo.profilesValid |= ADI_ADRV910X_ORX_PROFILE_VALID;
				break;
			case ADI_ADRV910X_ILB1: /* Defer ILB validation until Tx validation section */
			case ADI_ADRV910X_ILBNB:
				break;
			case ADI_ADRV910X_ELB1: /* Falls through */
				index = 0;
			case ADI_ADRV910X_ELBNB:
				ADI_EXPECT(adrv910x_VerifyRxProfile, device, &profile->elbConfig[index]);
				device->devStateInfo.profilesValid |= ADI_ADRV910X_ELB_PROFILE_VALID;
				break;
			default:
				ADI_SHOULD_NOT_EXECUTE(device);
			}

			device->devStateInfo.initializedChannels |= RX_CHANNELS[i];
		}
	}

	// Validate Tx channels
	for (i = 0; i < NUM_TX_CHANNELS; i++)
	{
		if (ADRV910X_BF_EQUAL(profile->chanConfig, TX_CHANNELS[i]))
		{          
			ADI_EXPECT(adrv910x_VerifyTxProfile, device, &profile->txConfig[i]);
			
			if (profile->txConfig[i].outputSignalingSel != FM_DM)	//TODO verify
			{
				/* Only validate ILB if Tx is enabled and not DIRECT_FM_FSK */
				ADI_EXPECT(adrv910x_VerifyRxProfile, device, &profile->ilbConfig[i]);
				device->devStateInfo.initializedChannels |= RX_CHANNELS[i + 4];
			}
                
			device->devStateInfo.profilesValid |= ADI_ADRV910X_TX_PROFILE_VALID;
			device->devStateInfo.initializedChannels |= TX_CHANNELS[i];
		}
	}
	ADI_API_RETURN(device);
}

static __maybe_unused int32_t __maybe_unused adrv910x_InitAnalog_Validate(adi_adrv910x_Device_t *device,
                                                           deviceProfile_t *profile,
                                                           adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor)
{
	ADI_RANGE_CHECK(device, profile->devClock_Hz/64, 0, 200000000);
    ADI_RANGE_CHECK(device, adrv910xDeviceClockOutDivisor, ADI_ADRV910X_DEVICECLOCKDIVISOR_BYPASS, ADI_ADRV910X_DEVICECLOCKDIVISOR_DISABLED);
    ADI_API_RETURN(device);
}

int32_t adrv910x_InitAnalog(adi_adrv910x_Device_t *device,
                            deviceProfile_t *profile,
                            adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor)
{
	uint8_t sramAssignmentReg;

	ADI_PERFORM_VALIDATION(adrv910x_InitAnalog_Validate, device, profile, adrv910xDeviceClockOutDivisor);
	
	// PS1 Power Up - 
	// power enabled, power_iso_dis = 0, scan latch en = 1 held in soft reset
	ADRV910X_SPIWRITEBYTE(device,
		"PGSYS1_POWER_CTRL",
		ADRV910X_ADDR_PGSYS1_POWER_CTRL,
		(0x3));

	ADRV910X_SPIWRITEBYTE(device,
		"PGSYS1_POWER_CTRL",
		ADRV910X_ADDR_PGSYS1_POWER_CTRL,
		(0x07));

	ADRV910X_SPIWRITEBYTE(device,
		"PGSYS1_POWER_CTRL",
		ADRV910X_ADDR_PGSYS1_POWER_CTRL,
		(0x05));
	
	ADRV910X_SPIWRITEBYTE(device,
		"PGSYS1_POWER_CTRL",
		ADRV910X_ADDR_PGSYS1_POWER_CTRL,
		(0x0d));
	
	// Channel 1 power up
	// power enabled, power_iso_dis = 0, scan latch en = 1 release soft reset
	ADRV910X_SPIWRITEBYTE(device,
		"ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL",
		ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL,
		(0x33));
	ADRV910X_SPIWRITEBYTE(device,
		"ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL",
		ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL,
		(0x77));
	ADRV910X_SPIWRITEBYTE(device,
		"ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL",
		ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL,
		(0x55));
	ADRV910X_SPIWRITEBYTE(device,
		"ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL",
		ADRV910X_ADDR_PGCHANNEL1_POWER_CTRL,
		(0xdd));

	if (profile->ps2En != 0) 
	{
		// PS2 Power Up
		ADRV910X_SPIWRITEBYTE(device,
			"PGSYS2_POWER_CTRL",
			ADRV910X_ADDR_PGSYS2_POWER_CTRL,
			(0x03));
		ADRV910X_SPIWRITEBYTE(device,
			"PGSYS2_POWER_CTRL",
			ADRV910X_ADDR_PGSYS2_POWER_CTRL,
			(0x07));
		ADRV910X_SPIWRITEBYTE(device,
			"PGSYS2_POWER_CTRL",
			ADRV910X_ADDR_PGSYS2_POWER_CTRL,
			(0x05));
		ADRV910X_SPIWRITEBYTE(device,
			"PGSYS2_POWER_CTRL",
			ADRV910X_ADDR_PGSYS2_POWER_CTRL,
			(0x0d));
	}
	
	
	// TODO: This step needs to be remvoed when STREAM PROCESSORS are in use
	// Disable clocks to STREAM PROCESSORS
	ADRV910X_SPIWRITEBYTE(device,
		"STREAM_PROC_CLK_EN",
		ADRV910X_ADDR_STREAM_PROC_CLK_EN,
		(0x00)); // all off
	
	sramAssignmentReg = 0; /* all six banks to PS1 */
	ADRV910X_SPIWRITEBYTE(device, "SYS2_MEM_SEL_BYTE0", ADRV910X_ADDR_SYS2_MEM_SEL_BYTE0, sramAssignmentReg);
	
    ADI_EXPECT(adrv910x_RefClockEnable, device);

	uint32_t devclk = profile->devClock_Hz;
	uint8_t divider = 0; /* find fastest device clock less than 200mhz */
	/* devClock_Hz/64 < 200mhz is checked in validate funcion */
	while (devclk > 200000000 && divider < ADI_ADRV910X_DEVICECLOCKDIVISOR_DISABLED)
	{
		divider++;
		devclk /= (1 << divider);
	}
	if (divider < ADI_ADRV910X_DEVICECLOCKDIVISOR_DISABLED)
	{
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_RefClkIntDevclkDivideRatio_Set, device, divider);
	}
	
	ADI_EXPECT(adrv910x_SetDeviceClockOutDivider, device, adrv910xDeviceClockOutDivisor);
	
	/* TODO: re-enable when MCS required */
	//ADI_EXPECT(adrv910x_MCSInternalRestartRefClk, device);
	
	ADI_EXPECT(adrv910x_ProfilesVerify, device, profile);
	
	/* set ahb bridge resters to HSD/Ref Clock */
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_AhbSpiBridgeEn_Set, device, 0x1);

    ADI_API_RETURN(device);
}

int32_t adrv910x_SetDeviceClockOutDivider(adi_adrv910x_Device_t *device,
	adi_adrv910x_DeviceClockDivisor_e adrv910xDeviceClockOutDivisor)
{
	ADI_RANGE_CHECK(device, adrv910xDeviceClockOutDivisor, ADI_ADRV910X_DEVICECLOCKDIVISOR_BYPASS, ADI_ADRV910X_DEVICECLOCKDIVISOR_DISABLED);
	uint8_t div;
	volatile uint8_t zero = 0;
	volatile uint8_t one = 1;
		
	if (adrv910xDeviceClockOutDivisor != ADI_ADRV910X_DEVICECLOCKDIVISOR_DISABLED)
	{
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_PdDigclk_Set, device, zero);
		div = (((uint8_t)adrv910xDeviceClockOutDivisor << 4) | (uint8_t)adrv910xDeviceClockOutDivisor);
	}
	else
	{
		ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_PdDigclk_Set, device, one);
		div = 0;
	}
		
	/* set divider values */
	ADRV910X_SPIWRITEBYTE(device,
		"ANALOG_REFCLK_REG_1",
		ADRV910X_ADDR_ANALOG_REFCLK_REG_1,
		div);	
		
	/* force spi setting for dev clk out divider selection */
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_ForceDigDevClkDivSourceSelFromSpi_Set, device, one);
		
	/* select which divider to use */
	ADI_EXPECT(adrv910x_NevisMonitorRegmapCore_DigDevClkDivSourceSelFromSpi_Set, device, one);

	ADI_API_RETURN(device);
}
