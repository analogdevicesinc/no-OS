/***************************************************************************//**
 *   @file   projects/fmcdaq3/src/examples/fmcdaq3/fmcdaq3_example.h
 *   @brief  example header for fmcdaq3 project
 *   @author EBalas (eliza.balas@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __FMCDAQ3_EXAMPLE_H__
#define __FMCDAQ3_EXAMPLE_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "common_data.h"

//TODO: move to FMCDAQ3_DMA_EXAMPLE
//struct axi_dmac_init ad9680_dmac_param = {
//	.name = "ad9680_dmac",
//	.base = RX_DMA_BASEADDR,
//	.irq_option = IRQ_DISABLED
//};
//
//struct axi_dmac_init ad9152_dmac_param = {
//	.name = "ad9152_dmac",
//	.base = TX_DMA_BASEADDR,
//	.irq_option = IRQ_DISABLED
//};

//extern struct axi_dmac_init ad9680_dmac_param;
//extern struct axi_dmac_init ad9680_dmac_param;

//
////
////#ifdef JESD_FSM_ON
////struct jesd204_clk rx_jesd_clk = {0};
////struct jesd204_clk tx_jesd_clk = {0};
//

struct fmcdaq3_dev {
	struct ad9528_dev *ad9528_device;
	struct ad9152_dev *ad9152_device;
	struct ad9680_dev *ad9680_device;

	struct ad9528_channel_spec ad9528_channels[8];

	struct no_os_gpio_desc *gpio_dac_txen;
	struct no_os_gpio_desc *gpio_adc_pd;

	struct adxcvr *ad9152_xcvr;
	struct adxcvr *ad9680_xcvr;

	struct axi_jesd204_tx *ad9152_jesd;
	struct axi_jesd204_rx *ad9680_jesd;

	struct axi_adc	*ad9680_core;
	struct axi_dac	*ad9152_core;
	struct axi_dac_channel  ad9152_channels[2];

	struct axi_dmac *ad9152_dmac;
	struct axi_dmac *ad9680_dmac;
};

struct fmcdaq3_init_param {
	struct ad9528_init_param ad9528_param;
	struct ad9152_init_param ad9152_param;
	struct ad9680_init_param ad9680_param;

	struct adxcvr_init ad9152_xcvr_param;
	struct adxcvr_init ad9680_xcvr_param;

	struct jesd204_tx_init ad9152_jesd_param;
	struct jesd204_rx_init ad9680_jesd_param;

	struct axi_adc_init ad9680_core_param;

	struct axi_dac_init ad9152_core_param;

	struct axi_dmac_init ad9152_dmac_param;
	struct axi_dmac_init ad9680_dmac_param;

#ifdef JESD_FSM_ON
	struct link_init_param	jrx_link_tx;
	struct link_init_param	jtx_link_rx;
#endif
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int fmcdaq3_example_main();

#endif /* __FMCDAQ3_EXAMPLE_H__ */
