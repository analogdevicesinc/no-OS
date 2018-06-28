/***************************************************************************//**
 * @file jesd_core.h
 * @brief Header file of JESD Core Driver.
 * @author DBogdan (dragos.bogdan@analog.com)
 ********************************************************************************
 * Copyright 2017(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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
#ifndef JESD_CORE_H_
#define JESD_CORE_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "platform_drivers.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define RX_XCVR_BASEADDR		XPAR_AXI_AD9371_RX_XCVR_BASEADDR
#define TX_XCVR_BASEADDR		XPAR_AXI_AD9371_TX_XCVR_BASEADDR
#define RX_OS_XCVR_BASEADDR		XPAR_AXI_AD9371_RX_OS_XCVR_BASEADDR

#define RX_JESD_BASEADDR		XPAR_AXI_AD9371_RX_JESD_RX_AXI_BASEADDR
#define TX_JESD_BASEADDR		XPAR_AXI_AD9371_TX_JESD_TX_AXI_BASEADDR
#define RX_OS_JESD_BASEADDR		XPAR_AXI_AD9371_RX_OS_JESD_RX_AXI_BASEADDR

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
typedef struct {
	uint32_t	base_addr;
} jesd_device;

typedef enum {
	PM_200,
	PM_700,
	PM_1250,
} refclk_ppm;

typedef enum {
	XILINX_XCVR_TYPE_S7_GTX2,
	XILINX_XCVR_TYPE_US_GTH3,
	XILINX_XCVR_TYPE_US_GTH4,
} xilinx_xcvr_type;

struct xcvr_cpll_config {
	uint32_t refclk_div;
	uint32_t fb_div_N1;
	uint32_t fb_div_N2;
};

struct xcvr_qpll_config {
	uint32_t refclk_div;
	uint32_t fb_div;
	uint32_t band;
};

typedef struct {
	xilinx_xcvr_type	type;
	uint8_t				num_lanes;
	uint8_t				tx_enable;
	uint32_t			base_addr;
	refclk_ppm			ppm;
	uint16_t			encoding;
	uint8_t				lpm_enable;
	uint8_t				cpll_enable;
	uint32_t			lane_rate_khz;
	uint32_t			ref_rate_khz;
} xcvr_device;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int32_t xcvr_setup(mykonosDevice_t *myk_dev);
int32_t jesd_setup(mykonosDevice_t *myk_dev);
int32_t jesd_tx_enable(mykonosDevice_t *myk_dev);
int32_t jesd_rx_enable(mykonosDevice_t *myk_dev);
#endif
