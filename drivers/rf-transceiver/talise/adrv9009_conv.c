/*
 * ADRV9009 RF Transceiver
 *
 * Copyright 2020-2024 Analog Devices Inc.
 *
 */

#include "no_os_print_log.h"
#include "axi_adc_core.h"
#include "axi_dac_core.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "adrv9009.h"

int adrv9009_post_setup(struct adrv9009_rf_phy *phy)
{
	uint32_t tmp, ratio;
	unsigned num_chan;
	uint64_t clock_hz;
	uint32_t freq;
	int i;

	num_chan = TALISE_NUM_CHANNELS;

	// Write 0 to Rx ADC
	axi_adc_write(phy->rx_adc, AXI_ADC_REG_CNTRL, 0);
	// Write 0 to OBS Rx ADC
	axi_adc_write(phy->rx_adc, 0x8048 + AXI_ADC_REG_CNTRL, 0);
	// Read values from Tx DAC
	axi_adc_read(phy->rx_adc, 0x4048, &tmp);

	// Write no of RF channels and rate to Tx DAC
	tmp &= ~NO_OS_BIT(5);
	axi_adc_write(phy->rx_adc, 0x4048, tmp);
	axi_adc_write(phy->rx_adc, 0x404c, 3); /* RATE */

	// Rx
	for (i = 0; i < num_chan; i++) {
		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL_1(i),
			      AXI_ADC_DCFILT_OFFSET(0));
		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL_2(i),
			      (i & 1) ? 0x00004000 : 0x40000000);
		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL(i),
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE | AXI_ADC_IQCOR_ENB);
	}

	// ORx
	for (i = 0; i < num_chan / 2; i++) {
		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL_1(i) + 0x8000,
			      AXI_ADC_DCFILT_OFFSET(0));
		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL_2(i) + 0x8000,
			      (i & 1) ? 0x00004000 : 0x40000000);
		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL(i) + 0x8000,
			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
			      AXI_ADC_ENABLE | AXI_ADC_IQCOR_ENB);
	}

	no_os_mdelay(100);
	// Read frequency and ratio from DAC
	axi_adc_read(phy->rx_adc, 0x4054, &freq);
	axi_adc_read(phy->rx_adc, 0x4058, &ratio);
	clock_hz = freq * ratio;
	clock_hz = (clock_hz * 390625) >> 8;
	phy->tx_dac->clock_hz = clock_hz;

	return 0;
}

