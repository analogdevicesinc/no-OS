/**
 * \file
 * \brief Contains FPGA910X API configuration and run-time type definitions
 *
 * Copyright 2018-2021 Analog Devices Inc.
 * Released under the FPGA910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 *
 */

#include "adi_common_error.h"

#include "adi_fpga910x.h"
#include "adi_fpga910x_gpio.h"
#include "adi_fpga910x_ssi.h"
#include "adi_adrv910x_types.h"
#include "adrv910x_reg_addr_macros.h"
#include "fpga910x_utilities.h"

int32_t adi_fpga910x_VersionGet(adi_fpga910x_Device_t *fpga910x,
                                adi_fpga910x_Version_t *version)
{
    uint32_t regVal = 0;

	axi_sysid_top_sys_version_get((void *)fpga910x, AXI_SYSID_ID, &regVal);
    version->major = (uint8_t)((regVal >> 16) & 0xff);
    version->minor = (uint8_t)((regVal >>  8) & 0xff);
    version->patch = (uint8_t)((regVal >>  0) & 0xff);
	ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_Initialize(adi_fpga910x_Device_t *fpga910x,
                                deviceProfile_t *profile,
                                adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration,
                                adi_fpga910x_Mmcm_ClockOutput_Divisor_e adrv910xDeviceClockDivisor)
{
	uint8_t i = 0;
	uint32_t version = 0;
	uint32_t locked = 0;
	adi_fpga910x_SsiConfig_t ssi_config = { 0 };
	axi_adrv9102_clk_params_t clk_params = { 0 };

	adi_fpga910x_GpioSmaCfg_t smaConfig = { 0 };
    
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

	bool ssiDelayConfigured = false;
	bool rx1SsiConfigured = false;
	bool rx2SsiConfigured = false;    
    
	uint32_t BITM_RX1 = ADI_ADRV910X_RX1 | ADI_ADRV910X_ORX1 | ADI_ADRV910X_ILB1 | ADI_ADRV910X_ELB1;
	uint32_t BITM_RX2 = ADI_ADRV910X_RXNB | ADI_ADRV910X_ORXNB | ADI_ADRV910X_ILBNB | ADI_ADRV910X_ELBNB;

	/* Check fpga910x info is valid */
	ADI_ENTRY_EXPECT(fpga910x);

	for (i = 0; ((i < NUM_RX_CHANNELS) && (ssiDelayConfigured == false)); i++)
	{
		if (ADRV910X_BF_EQUAL(profile->chanConfig, RX_CHANNELS[i]))
		{
			ADI_EXPECT(adi_fpga910x_ssi_Delay_Configure,
				fpga910x,
				(adi_fpga910x_SsiType_e)(profile->rxConfig[i].rxSsiConfig.ssiType),
				ssiCalibration);

			ssiDelayConfigured = true;
		}
	}

	/* ideally you want application to report versions, this is meant to be a */
	/* developer friendly api, and users are free to use any version at their own risk */
	/* do not block, simply inform -- better to change it to a NOTE. */
	axi_sysid_top_sys_version_get((void *)fpga910x, AXI_SYSID_ID, &version);
	ADI_ERROR_REPORT(&fpga910x->common,
		ADI_COMMON_ERRSRC_API,
		0,
		0,
		version,
		"INFO: Platform FPGA version.");

	/* deassert reset to the fpga910x */
	axi_adrv9102_top_resetb_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x1);

	smaConfig.smaPin1 = true;
	smaConfig.smaPin2 = true;
	ADI_EXPECT(adi_fpga910x_gpio_smapin_Configure, fpga910x, &smaConfig);
    
	/* Configure the RX ssi port using the first found valid port */
	for (i = 0; i < NUM_RX_CHANNELS; i++)
	{
		ADI_EXPECT(adi_fpga910x_ssi_ConvertSsiConfig, fpga910x, &(profile->rxConfig[i].rxSsiConfig), &ssi_config);
		if (ADRV910X_BF_EQUAL(profile->chanConfig, RX_CHANNELS[i]))
		{
			if ((BITM_RX1 & RX_CHANNELS[i]) && (!rx1SsiConfigured))
			{
				ADI_EXPECT(adi_fpga910x_ssi_Configure, fpga910x, ADI_RX, ADI_CHANNEL_1, profile->rxConfig[i].rxInterfaceSampleRate, profile->rxConfig[i].rxOutputRate, &ssi_config);
				rx1SsiConfigured = true;
			}
			if ((BITM_RX2 & RX_CHANNELS[i]) && (!rx2SsiConfigured))
			{
				ADI_EXPECT(adi_fpga910x_ssi_Configure, fpga910x, ADI_RX, ADI_CHANNEL_2, profile->rxConfig[i].rxInterfaceSampleRate, profile->rxConfig[i].rxOutputRate, &ssi_config);
				rx2SsiConfigured = true;
			}
		}
	}
	
	/* Configure either Tx1 of TxNB ssi port if enabled */
	if (ADRV910X_BF_EQUAL(profile->chanConfig, ADI_ADRV910X_TX1))
	{
		ADI_EXPECT(adi_fpga910x_ssi_ConvertSsiConfig, fpga910x, &(profile->txConfig[0].txSsiConfig), &ssi_config);
		ADI_EXPECT(adi_fpga910x_ssi_Configure, fpga910x, ADI_TX, ADI_CHANNEL_1, profile->txConfig[0].txInterfaceSampleRate, profile->txConfig[0].txInputRate, &ssi_config);		
	}
	else if (ADRV910X_BF_EQUAL(profile->chanConfig, ADI_ADRV910X_TXNB))
	{
		ADI_EXPECT(adi_fpga910x_ssi_ConvertSsiConfig, fpga910x, &(profile->txConfig[1].txSsiConfig), &ssi_config);
		ADI_EXPECT(adi_fpga910x_ssi_Configure, fpga910x, ADI_TX, ADI_CHANNEL_1, profile->txConfig[1].txInterfaceSampleRate, profile->txConfig[1].txInputRate, &ssi_config);		
	}

	axi_adrv9102_top_mcs_swreset_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x0);
    axi_adrv9102_top_reg_mcs_rx_resetn_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, (uint32_t) - 1);
    axi_adrv9102_top_reg_mcs_tx_resetn_set(fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, (uint32_t) - 1);

	clk_params.ref0_clk_freq_hz = profile->devClock_Hz;

	if (ADI_FPGA910X_MMCM_CLKDIV_DISABLED == adrv910xDeviceClockDivisor)
	{
		clk_params.ref1_clk_freq_hz = -1;
	}
	else
	{
		clk_params.ref1_clk_freq_hz = profile->devClock_Hz >> adrv910xDeviceClockDivisor;
	}

	clk_params.gen_dev_clk_freq_hz = profile->devClock_Hz;

	if (axi_adrv9102_clk_config((void *)fpga910x, AXI_ADRV9102_ID, &clk_params, &locked) != 0)
	{
		ADI_ERROR_REPORT(&fpga910x->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_API_FAIL,
			ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
			0,
			"function axi_adrv9102_clk_config has failed");
		ADI_API_RETURN(fpga910x);
	}

	if (locked != 1)
	{
		ADI_ERROR_REPORT(&fpga910x->common,
			ADI_COMMON_ERRSRC_API,
			ADI_COMMON_ERR_API_FAIL,
			ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
			0,
			"platform VCO clock NOT locked");
		ADI_API_RETURN(fpga910x);
	}

    axi_adrv9102_top_trig0_ext_oe_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x1);
    axi_adrv9102_top_trig1_ext_oe_set((void *)fpga910x, AXI_ADRV9102_ID, AXI_ADRV9102_TOP_OFFSET, 0x1);

	ADI_API_RETURN(fpga910x);
}
