/***************************************************************************//**
 *   @file   ltc4296_sccp.h
 *   @brief  Header file for the LTC4296 SCCP Driver
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef __LTC4296_SCCP_H__
#define __LTC4296_SCCP_H__

#include <stdint.h>

#define ms2tcks(ms)		((uint16_t)(ms*32))

#define CMD_BLINKEM				0xDD
#define CMD_RESET_PD			0x66 // not legit IEEE cmd
#define CMD_BROADCAST_ADDR		0xCC
#define CMD_READ_SCRATCHPAD		0xAA
#define CMD_READ_V_PWR			0xBB
#define CMD_READ_VOLT_INFO		0xBB
#define CMD_READ_PWR_INFO		0x77
#define CMD_WRITE_PWR_ASSIGN	0x99
#define CMD_READ_PWR_ASSIGN		0x81
#define CMD_RW_REFUSE			0x99 // not legit IEEE cmd, used to tell PC that rw was not performed
#define CLASS_TYPE_E			0x0C

#define T_BUG			200

/* COMMON TIMING */
#define T_REC			320
#define T_W1L			300
#define T_W0L			2000
#define T_R			250

/* RESET/PRESENSE PULSE TIMING */
#define T_RSTL_MIN		8000   /* length of PSE reset pulse */
#define T_RSTL_MAX		10500
#define T_RSTL_NOM		9000
#define T_PDH			1000   /* time between PSE releasing reset pulse and PD starting presense pulse */
#define T_PDL			3800   /* period that PD holds presense pulse */
#define T_MSP			2000     /* time between PSE releasing reset pulse and PSE sampling for PD's presense pulse */

#define T_POR_PULSE		450000

/* WRITE SLOT TIMING */
#define T_WRITESLOT		2750
#define T_SSW			1100   /* time between start of PSE Pull down and middle of PD capture window */

/* READ SLOT TIMING */
#define T_READSLOT		3000
#define T_READSLOT_MAX_TYPE_E	3830

#define T_MSR			1000     /* As per PD spec */
#define T_R0L			2000   /*time between start of PSE pull down and PD releasing a pull down response (PD writing 0 to PSE) */

#define HIGH			1
#define LOW			0

int sccp_read_write_pd(struct ltc4296_dev *dev, uint8_t addr, uint8_t cmd,
		       uint8_t* buf);
int sccp_reset_pulse(struct ltc4296_dev *dev);
int sccp_is_pd(struct ltc4296_dev *dev, uint8_t pse_class,
	       uint16_t sccp_response_data, uint8_t *pd_class);

#endif
