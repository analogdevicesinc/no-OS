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
uint32_t jesd204b_gt_init_done = 0;
uint32_t jesd204b_gt_common_done = 0;

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
* @brief jesd204b_gt_setup
*******************************************************************************/
int32_t jesd204b_gt_setup(jesd204b_gt_link link_param)
{
  uint32_t data;
  int32_t lane;

  if(jesd204b_gt_init_done != 1)
  {
    xil_printf("JESD204B GT run initialize before setup.\n\r");
    return(-1);
  }

  if (jesd204b_gt_common_done == 0) {
  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    // need to check here if rx/tx are doing conflicting cpll/qpll usage.

    jesd204b_gt_write(JESD204B_GT_REG_RSTN(lane), 0);
    jesd204b_gt_write(JESD204B_GT_REG_LPM_QPLL(lane),
      JESD204B_GT_LPM_DFE(link_param.lpm_or_dfe) |
      JESD204B_GT_QPLL_CPLL(link_param.qpll_or_cpll));

   // if(link_param.tx_or_rx == JESD204B_GT_TX)
   // {
      jesd204b_gt_write(JESD204B_GT_REG_TX_GT_RSTN(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_TX_RSTN(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_TX_USER_READY(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_TX_SYNC_CTL(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_TX_SYSREF_CTL(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_TX_CLK_SEL(lane),
          JESD204B_GT_TX_SYS_CLK_SEL(link_param.sys_clk_sel) |
          JESD204B_GT_TX_OUT_CLK_SEL(link_param.out_clk_sel));
  //}
  //else
  //{
      jesd204b_gt_write(JESD204B_GT_REG_RX_GT_RSTN(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_RX_RSTN(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_RX_USER_READY(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_RX_SYNC_CTL(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane), 0);
      jesd204b_gt_write(JESD204B_GT_REG_RX_CLK_SEL(lane),
          JESD204B_GT_RX_SYS_CLK_SEL(link_param.sys_clk_sel) |
          JESD204B_GT_RX_OUT_CLK_SEL(link_param.out_clk_sel));
   // }

    jesd204b_gt_write(JESD204B_GT_REG_RSTN(lane),
        JESD204B_GT_DRP_RSTN | JESD204B_GT_PLL_RSTN);
  }
  }
  jesd204b_gt_common_done = 1;

  // reconfigure gt -

  // check pll status

  mdelay(10);

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    if(link_param.tx_or_rx == JESD204B_GT_TX)
    {
      jesd204b_gt_read(JESD204B_GT_REG_TX_STATUS(lane), &data);
      if((data & 0xff) != 0xff)
      {
        xil_printf("JESD204B GT TX %d PLL is NOT locked (0x%02x).\n\r", lane, data);
      }
    }
    else
    {
      jesd204b_gt_read(JESD204B_GT_REG_RX_STATUS(lane), &data);
      if((data & 0xff) != 0xff)
      {
        xil_printf("JESD204B GT RX %d PLL is NOT locked (0x%02x).\n\r", lane, data);
      }
    }
  }

  // bring phy out of reset

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    if(link_param.tx_or_rx == JESD204B_GT_TX)
    {
      jesd204b_gt_write(JESD204B_GT_REG_TX_GT_RSTN(lane), JESD204B_GT_TX_RSTN);
    }
    else
    {
      jesd204b_gt_write(JESD204B_GT_REG_RX_GT_RSTN(lane), JESD204B_GT_RX_RSTN);
    }
  }

  mdelay(1);

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    if(link_param.tx_or_rx == JESD204B_GT_TX)
    {
      jesd204b_gt_write(JESD204B_GT_REG_TX_USER_READY(lane), JESD204B_GT_TX_USER_READY);
    }
    else
    {
      jesd204b_gt_write(JESD204B_GT_REG_RX_USER_READY(lane), JESD204B_GT_RX_USER_READY);
    }
  }

  // check reset-done & pll status

  mdelay(10);

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    if(link_param.tx_or_rx == JESD204B_GT_TX)
    {
      jesd204b_gt_read(JESD204B_GT_REG_TX_STATUS(lane), &data);
      if((data & 0xffff) != 0xffff)
      {
        xil_printf("JESD204B GT TX %d RESET-DONE and PLL NOT locked (0x%02x).\n\r", lane, data);
      }
    }
    else
    {
      jesd204b_gt_read(JESD204B_GT_REG_RX_STATUS(lane), &data);
      if((data & 0xffff) != 0xffff)
      {
        xil_printf("JESD204B GT RX %d RESET-DONE and PLL NOT locked (0x%02x).\n\r", lane, data);
      }
    }
  }

  // bring data-path out of reset

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    if(link_param.tx_or_rx == JESD204B_GT_TX)
    {
      jesd204b_gt_write(JESD204B_GT_REG_TX_RSTN(lane), JESD204B_GT_TX_RSTN);
    }
    else
    {
      jesd204b_gt_write(JESD204B_GT_REG_RX_RSTN(lane), JESD204B_GT_RX_RSTN);
    }
  }

  mdelay(10);

  // enable sync & sysref

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    if(link_param.tx_or_rx == JESD204B_GT_TX)
    {
      jesd204b_gt_write(JESD204B_GT_REG_TX_SYNC_CTL(lane), JESD204B_GT_TX_SYNC);
      if(link_param.sysref_int_or_ext == JESD204B_GT_SYSREF_INT)
      {
        jesd204b_gt_write(JESD204B_GT_REG_TX_SYSREF_CTL(lane), JESD204B_GT_TX_SYSREF_ON);
        jesd204b_gt_write(JESD204B_GT_REG_TX_SYSREF_CTL(lane), JESD204B_GT_TX_SYSREF_OFF);
      }
      else
      {
        jesd204b_gt_write(JESD204B_GT_REG_TX_SYSREF_CTL(lane), JESD204B_GT_TX_SYSREF_EXTERNAL);
      }
    }
    else
    {
      jesd204b_gt_write(JESD204B_GT_REG_RX_SYNC_CTL(lane), JESD204B_GT_RX_SYNC);
      if(link_param.sysref_int_or_ext == JESD204B_GT_SYSREF_INT)
      {
        jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane), JESD204B_GT_RX_SYSREF_ON);
        jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane), JESD204B_GT_RX_SYSREF_OFF);
      }
      else
      {
        jesd204b_gt_write(JESD204B_GT_REG_RX_SYSREF_CTL(lane), JESD204B_GT_RX_SYSREF_EXTERNAL);
      }
    }
  }

  mdelay(10);

  // check status

  for (lane = link_param.last_lane; lane >= link_param.first_lane; lane--)
  {
    if(link_param.tx_or_rx == JESD204B_GT_TX)
    {
      jesd204b_gt_read(JESD204B_GT_REG_TX_STATUS(lane), &data);
      if((data & 0x1ffff) != 0x1ffff)
      {
        xil_printf("JESD204B GT TX %d Invalid status (0x%02x).\n\r", lane, data);
      }
    }
    else
    {
      jesd204b_gt_read(JESD204B_GT_REG_RX_STATUS(lane), &data);
      if((data & 0x1ffff) != 0x1ffff)
      {
        xil_printf("JESD204B GT RX %d Invalid status (0x%02x).\n\r", lane, data);
      }
    }
  }

  return(0);
}

/***************************************************************************//**
* @brief jesd204b_gt_initialize
*******************************************************************************/
int32_t jesd204b_gt_initialize(uint32_t baseaddr, uint32_t num_of_lanes)
{
  uint32_t jesd204b_gt_version;
  int32_t lane;

  jesd204b_gt_baseaddr = baseaddr;
  jesd204b_gt_init_done = 0;
  jesd204b_gt_common_done = 0;
  jesd204b_gt_read(JESD204B_GT_REG_VERSION, &jesd204b_gt_version);
  if(JESD204B_GT_VERSION_MAJOR(jesd204b_gt_version) < 7)
  {
    xil_printf("JESD204B GT Version %d is NOT supported.\n\r", jesd204b_gt_version);
    return(-1);
  }

  // reset only

//for (lane = 0; lane < num_of_lanes; lane++)
//{
//  jesd204b_gt_write(JESD204B_GT_REG_RSTN_1(lane), 0);
//  jesd204b_gt_write(JESD204B_GT_REG_RSTN_1(lane),
//      JESD204B_GT_DRP_RSTN | JESD204B_GT_PLL_RSTN);
//}

  jesd204b_gt_init_done = 1;
  return(0);
}


