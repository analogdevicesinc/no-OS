/***************************************************************************//**
* @file jesd204b_v51.c
* @brief Implementation of JESD204B V51 Driver.
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
#include "jesd204b_v51.h"

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
uint32_t jesd204b_baseaddr;

/***************************************************************************//**
* @brief jesd204b_read
*******************************************************************************/
int32_t jesd204b_read(uint32_t reg_addr, uint32_t *reg_data)
{
	*reg_data = Xil_In32((jesd204b_baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_write
*******************************************************************************/
int32_t jesd204b_write(uint32_t reg_addr, uint32_t reg_data)
{
	Xil_Out32((jesd204b_baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief jesd204b_setup
*******************************************************************************/
int32_t jesd204b_setup(uint32_t baseaddr, jesd204b_state setup_param)
{
	jesd204b_baseaddr = baseaddr;
	jesd204b_write(JESD204B_REG_RESET, JESD204B_RESET);
	jesd204b_write(JESD204B_REG_ILA_CTRL,
			setup_param.lanesync_enable ? JESD204B_ILA_EN : 0);
	jesd204b_write(JESD204B_REG_SCR_CTRL,
			setup_param.scramble_enable ? JESD204B_SCR_EN : 0);
	jesd204b_write(JESD204B_REG_SYSREF_CTRL,
			setup_param.sysref_always_enable ? JESD204B_ALWAYS_SYSREF_EN : 0);
	jesd204b_write(JESD204B_REG_ILA_MFC,
			JESD204B_ILA_MFC(setup_param.frames_per_multiframe));
	jesd204b_write(JESD204B_REG_OCTETS_PER_FRAME,
			JESD204B_OCTETS_PER_FRAME(setup_param.bytes_per_frame));
	jesd204b_write(JESD204B_REG_FRAMES_PER_MFRAME,
			JESD204B_FRAMES_PER_MFRAME(setup_param.frames_per_multiframe));
	jesd204b_write(JESD204B_REG_SUBCLASS, setup_param.subclass);

	xil_printf("JESD204B successfully initialized.\n");

	return 0;
}
