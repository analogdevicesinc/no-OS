/***************************************************************************//**
 *   @file   ADF4350_cfg.h
 *   @brief  Header file of ADF4350 Driver Configuration.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/
#ifndef __ADF4350_CFG_H__
#define __ADF4350_CFG_H__

static struct adf4350_platform_data adf4350_pdata_lpc =
{
	25000000,   	//clkin
	100000,    		//channel_spacing
	2500000000ul,   //power_up_frequency
    1,          	//ref_div_factor
    0,          	//ref_doubler_en
    0,          	//ref_div2_en
    ADF4350_REG2_PD_POLARITY_POS |
    ADF4350_REG2_CHARGE_PUMP_CURR_uA(312) |
    ADF4350_REG2_10BIT_R_CNT(1),  				//r2_user_settings
	ADF4350_REG3_12BIT_CLKDIV_MODE(0),      	//r3_user_settings
	ADF4350_REG4_OUTPUT_PWR(3) |
	ADF4350_REG4_RF_OUT_EN |
	ADF4350_REG4_AUX_OUTPUT_FUND |
	ADF4350_REG4_8BIT_BAND_SEL_CLKDIV(0xC8) |
	ADF4350_REG4_FEEDBACK_FUND,			        //r4_user_settings
	-1,         //gpio_lock_detect
};

#endif // __ADF4350_CFG_H__
