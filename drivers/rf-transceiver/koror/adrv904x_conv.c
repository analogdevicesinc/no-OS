// SPDX-License-Identifier: GPL-2.0
/*
 * ADRV904x RF Transceiver
 *
 * Copyright 2020-2023 Analog Devices Inc.
 *
 */

#include "no_os_print_log.h"
#include "axi_adc_core.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "adrv904x.h"

 #ifdef CONFIG_CF_AXI_ADC
 int adrv904x_hdl_loopback(struct adrv904x_rf_phy *phy, bool enable)
 {
 	uint32_t reg, addr, chan, version, scratch_reg;

 	axi_adc_read(phy->rx_adc, 0x4000, &version);
 	pr_info("AXI ADC ver: %d.\n", version);

 	addr = 0x4418;

 	for (chan = 0; chan < 8; chan++) {
 		axi_adc_read(phy->rx_adc, addr + (chan)*0x40, &reg);

 		if (enable && reg != 0x8) {
 			scratch_reg = reg;
 			reg = 0x8;
 		} else if (reg == 0x8)
 			reg = scratch_reg;

 		axi_adc_write(phy->rx_adc, addr + (chan)*0x40, reg);
 	}

 	return 0;
 }

 int adrv904x_post_setup(struct adrv904x_rf_phy *phy)
 {
 	unsigned tmp, num_chan;
 	int i;

 	num_chan = 8;

 	axi_adc_write(phy->rx_adc, AXI_ADC_REG_CNTRL, 0);
 	axi_adc_read(phy->rx_adc, 0x4048, &tmp);

 	tmp &= ~NO_OS_BIT(5);
 	axi_adc_write(phy->rx_adc, 0x4048, tmp);
 	axi_adc_write(phy->rx_adc, 0x404c, 3); /* RATE */

 	for (i = 0; i < num_chan; i++) {
 		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL_1(i),
 			      AXI_ADC_DCFILT_OFFSET(0));
 		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL_2(i),
 			      (i & 1) ? 0x00004000 : 0x40000000);
 		axi_adc_write(phy->rx_adc, AXI_ADC_REG_CHAN_CNTRL(i),
 			      AXI_ADC_FORMAT_SIGNEXT | AXI_ADC_FORMAT_ENABLE |
 			      AXI_ADC_ENABLE | AXI_ADC_IQCOR_ENB);
 	}

 	return 0;
 }

 #else /* CONFIG_CF_AXI_ADC */

 int adrv9025_hdl_loopback(struct adrv9025_rf_phy *phy, bool enable)
 {
 	return -ENODEV;
 }

 #endif /* CONFIG_CF_AXI_ADC */
