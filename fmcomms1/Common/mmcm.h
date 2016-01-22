/***************************************************************************//**
 *   @file   mmcm.h
 *   @brief  Header file of MMCM Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#ifndef MMCM_H_
#define MMCM_H_

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_MMCM_REG_RESET			0x40
#define AXI_MMCM_REG_DRP_CNTRL		0x70
#define AXI_MMCM_REG_DRP_STATUS		0x74

#define AXI_MMCM_RESET_MMCM_ENABLE	(1 << 1)
#define AXI_MMCM_RESET_ENABLE		(1 << 0)

#define AXI_MMCM_DRP_CNTRL_SEL		(1 << 29)
#define AXI_MMCM_DRP_CNTRL_READ		(1 << 28)
#define AXI_MMCM_DRP_STATUS_BUSY	(1 << 16)

#define MMCM_REG_CLKOUT0_1		0x08
#define MMCM_REG_CLKOUT0_2		0x09
#define MMCM_REG_CLKOUT1_1		0x0A
#define MMCM_REG_CLKOUT1_2		0x0B
#define MMCM_REG_CLK_FB1		0x14
#define MMCM_REG_CLK_FB2		0x15
#define MMCM_REG_CLK_DIV		0x16
#define MMCM_REG_LOCK1			0x18
#define MMCM_REG_LOCK2			0x19
#define MMCM_REG_LOCK3			0x1a
#define MMCM_REG_FILTER1		0x4e
#define MMCM_REG_FILTER2		0x4f

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
void mmcm_init(uint32_t sel);
int32_t mmcm_set_rate(uint32_t rate,
				  uint32_t parent_rate);
uint32_t mmcm_get_rate(uint32_t parent_rate);
#endif // MMCM_H_
