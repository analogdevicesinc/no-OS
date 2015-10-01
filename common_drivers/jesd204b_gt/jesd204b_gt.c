/***************************************************************************//**
* @file jesd204b_gt.c
* @brief Implementation of JESD204B GT Driver.
* @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
* Copyright 2014(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <xil_printf.h>
#include <xil_io.h>
#include "platform_drivers.h"
#include "jesd204b_gt.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint32_t jesd204b_gt_baseaddr;
uint8_t  num_of_config_regs;

/***************************************************************************//**
* @brief jesd204b_gt_read
*******************************************************************************/
int32_t jesd204b_gt_read(uint32_t reg_addr, uint32_t *reg_data)
{
	*reg_data = Xil_In32((jesd204b_gt_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_gt_write
*******************************************************************************/
int32_t jesd204b_gt_write(uint32_t reg_addr, uint32_t reg_data)
{
	Xil_Out32((jesd204b_gt_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_gt_clk_enable
*******************************************************************************/
int32_t jesd204b_gt_clk_enable(uint32_t num, uint32_t lane)
{
	uint32_t offs = (num == JESD204B_GT_TX ?
			JESD204B_GT_REG_TX_GT_RSTN(lane) - JESD204B_GT_REG_RX_GT_RSTN(lane) : 0);
	uint32_t data;

	jesd204b_gt_write(JESD204B_GT_REG_RX_GT_RSTN(lane) + offs, 0);
	jesd204b_gt_write(JESD204B_GT_REG_RX_RSTN(lane) + offs, 0);
	mdelay(10);

	jesd204b_gt_read(JESD204B_GT_REG_RX_STATUS(lane) + offs, &data);
	if(JESD204B_GT_TO_RX_PLL_LOCKED(data) != 0xFF)
	{
		xil_printf("%s PLL is unlocked.\n",
			num == JESD204B_GT_RX ? "JESD204B GT RX" : "JESD204B GT TX");
	}

	jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane) + offs, 0);
	jesd204b_gt_write(JESD204B_GT_REG_RX_SYNC_CTL(lane) + offs, JESD204B_GT_RX_SYNC);
	jesd204b_gt_write(JESD204B_GT_REG_RX_GT_RSTN(lane) + offs, JESD204B_GT_RX_GT_RSTN);
	jesd204b_gt_write(JESD204B_GT_REG_RX_RSTN(lane) + offs, JESD204B_GT_RX_RSTN);
	mdelay(40);

	jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane) + offs, JESD204B_GT_RX_SYSREF);
	jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane) + offs, 0);
	mdelay(50);

	xil_printf("%s CLK is enabled.\n",
		num == JESD204B_GT_RX ? "JESD204B GT RX" : "JESD204B GT TX");

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_gt_clk_enable
*******************************************************************************/
int32_t jesd204b_gt_clk_synchronize(uint32_t num, uint32_t lane)
{
	uint32_t offs = (num == JESD204B_GT_TX ?
			JESD204B_GT_REG_TX_GT_RSTN(lane) - JESD204B_GT_REG_RX_GT_RSTN(lane) : 0);
	uint32_t data;
	int8_t   timeout = 100;

	do
	{
		jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane) + offs, JESD204B_GT_RX_SYSREF);
		jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane) + offs, 0);
		mdelay(100);
		jesd204b_gt_read(JESD204B_GT_REG_RX_STATUS(lane) + offs, &data);
	}
	while((data != 0x1ffff) && (timeout--));

	if (timeout == -1) {
		xil_printf("Synchronization timeout. %s = 0x%x\n",
		num == JESD204B_GT_TX ? "JESD204B_GT_REG_TX_STATUS" : "JESD204B_GT_REG_RX_STATUS",
					data);
		return -1;
	}

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_gt_setup
*******************************************************************************/
int32_t jesd204b_gt_setup(uint32_t baseaddr, jesd204b_gt_state setup_param)
{
	uint32_t data;
	uint32_t jesd204b_gt_version;
	uint32_t lane;

	jesd204b_gt_baseaddr = baseaddr;

	jesd204b_gt_read(JESD204B_GT_REG_VERSION, &jesd204b_gt_version);
	if(JESD204B_GT_VERSION_MAJOR(jesd204b_gt_version) < 7)
		num_of_config_regs = 1;
	else
		num_of_config_regs = setup_param.num_of_lanes;

	for (lane = 0; lane < num_of_config_regs; lane++) {
		jesd204b_gt_write(JESD204B_GT_REG_CPLL_PD(lane),
				setup_param.use_cpll ? 0 : JESD204B_GT_CPLL_PD);

		if (setup_param.rx_sys_clk_sel || setup_param.rx_out_clk_sel)
			jesd204b_gt_write(JESD204B_GT_REG_RX_CLK_SEL(lane),
					JESD204B_GT_RX_SYS_CLK_SEL(setup_param.rx_sys_clk_sel) |
					JESD204B_GT_RX_OUT_CLK_SEL(setup_param.rx_out_clk_sel));

		if (setup_param.tx_sys_clk_sel || setup_param.tx_out_clk_sel)
			jesd204b_gt_write(JESD204B_GT_REG_TX_CLK_SEL(lane),
					JESD204B_GT_TX_SYS_CLK_SEL(setup_param.tx_sys_clk_sel) |
					JESD204B_GT_TX_OUT_CLK_SEL(setup_param.tx_out_clk_sel));

		jesd204b_gt_write(JESD204B_GT_REG_RSTN_1(lane), 0);
		jesd204b_gt_write(JESD204B_GT_REG_RX_GT_RSTN(lane), 0);
		jesd204b_gt_write(JESD204B_GT_REG_RX_RSTN(lane), 0);
		jesd204b_gt_write(JESD204B_GT_REG_TX_GT_RSTN(lane), 0);
		jesd204b_gt_write(JESD204B_GT_REG_TX_RSTN(lane), 0);
		jesd204b_gt_write(JESD204B_GT_REG_RSTN_1(lane),
				JESD204B_GT_DRP_RSTN | JESD204B_GT_PLL_RSTN);
	}

	mdelay(50);

	for (lane = 0; lane < num_of_config_regs; lane++) {
		if (setup_param.rx_sys_clk_sel || setup_param.rx_out_clk_sel) {
			jesd204b_gt_read(JESD204B_GT_REG_RX_STATUS(lane), &data);
			xil_printf("JESD204B GT RX %d PLL is %s.\n", lane,
				JESD204B_GT_TO_RX_PLL_LOCKED(data) == 0xFF ? "locked" : "unlocked");
		}

		if (setup_param.tx_sys_clk_sel || setup_param.tx_out_clk_sel) {
			jesd204b_gt_read(JESD204B_GT_REG_TX_STATUS(lane), &data);
			xil_printf("JESD204B GT TX %d PLL is %s.\n", lane,
				JESD204B_GT_TO_TX_PLL_LOCKED(data) == 0xFF ? "locked" : "unlocked");
		}

		jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane), 0);
		jesd204b_gt_write(JESD204B_GT_REG_RX_SYNC_CTL(lane), JESD204B_GT_RX_SYNC);
		jesd204b_gt_write(JESD204B_GT_REG_TX_SYSREF_CTL(lane), 0);
		jesd204b_gt_write(JESD204B_GT_REG_TX_SYNC_CTL(lane), JESD204B_GT_TX_SYNC);
		jesd204b_gt_write(JESD204B_GT_REG_RX_USER_READY(lane), JESD204B_GT_RX_USER_READY);
	}

	xil_printf("JESD204B GT successfully initialized.\n");

	return 0;
}
