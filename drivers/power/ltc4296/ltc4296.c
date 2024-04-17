/***************************************************************************//**
 *   @file   ltc4296.c
 *   @brief  Implementation file for the LTC4296 Driver
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

#include <stdio.h>
#include <errno.h>
#include <math.h>
#include "ltc4296.h"
#include "ltc4296_sccp.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/* LTC4296_1_IGAIN / Rsense[port_no]
 * {(0.1/3), (0.1/1.5), (0.1/0.68), (0.1/0.25), (0.1/0.1)} */
const int ltc4296_spoe_rsense[LTC4296_MAX_PORTS] = {333, 666, 1470, 4000, 10000};

/* Value of RSense resistor for each port in mOhm */
const int ltc4296_spoe_sense_resistor[LTC4296_MAX_PORTS] = {3000, 1500, 680, 250, 100};

int ltc4296_spoe_vol_range_mv[12][2] = { {20000,30000},  /* SPoE Class 10         */
	{20000,30000},  /* SPoE Class 11         */
	{20000,30000},  /* SPoE Class 12         */
	{50000,58000},  /* SPoE Class 13         */
	{50000,58000},  /* SPoE Class 14         */
	{50000,58000},  /* SPoE Class 15         */
	{9600,15000},   /* APL Class A           */
	{9600,15000},   /* APL Class A noAutoNeg */
	{9600,15000},   /* APL Class C           */
	{46000,50000},  /* APL Class 3           */
	{9600,15000},   /* Production Power Test */
	{9600,15000}    /* APL Class A oldAPL    */
};

/* Stirng to print the different classes of LTC4296-1  */
char* ltc4296_class_str[12][25] = { {"SPOE CLASS 10"},
	{"SPOE CLASS 11"},
	{"SPOE CLASS 12"},
	{"SPOE CLASS 13"},
	{"SPOE CLASS 14"},
	{"SPOE CLASS 15"},
	{"APL CLASS A"},
	{"APL CLASS A"}, /* No Autoneg */
	{"APL CLASS C"},
	{"APL CLASS 3"},
	{"BOARD PRODUCTION TEST"},
	{"APL CLASS A"} /* OLD Demo */
};


uint8_t set_port_vout[LTC4296_MAX_PORTS] = {0x04, 0x06, 0x08, 0x0A, 0x0C};

static uint8_t ltc4296_get_pec_byte(uint8_t data, uint8_t seed)
{
	uint8_t pec = seed;
	uint8_t din, in0, in1, in2;
	int bit;
	for(bit = 7; bit >= 0; bit--) {
		din = (data>>bit) & 0x01;
		in0 = din ^ ( (pec >> 7) & 0x01 );
		in1 = in0 ^ ( pec & 0x01);
		in2 = in0 ^ ( (pec >> 1) & 0x01 );
		pec = (pec << 1);
		pec &= ~(0x07);
		pec = pec | in0 | (in1 << 1) | (in2 << 2);
	}
	return pec;
}

int ltc4296_reg_read(struct ltc4296_dev *dev, uint8_t reg, uint16_t *data)
{
	int ret;
	uint8_t r_buf[5] = {0};

	r_buf[0] = reg << 1 | 0x1;
	r_buf[1] = ltc4296_get_pec_byte(r_buf[0], 0x41);

	ret = no_os_spi_write_and_read(dev->spi_desc, r_buf, 5);
	if (ret)
		return ret;

	*data = r_buf[2] << 8 | r_buf[3];

	return 0;
}

int ltc4296_reg_write(struct ltc4296_dev *dev, uint8_t reg, uint16_t data)
{
	uint8_t w_buf[5] = {0};

	w_buf[0] = reg << 1 | 0x0;
	w_buf[1] = ltc4296_get_pec_byte(w_buf[0], 0x41);
	w_buf[2] = data >> 8;
	w_buf[3] = data & 0xFF;
	w_buf[4] = ltc4296_get_pec_byte(w_buf[3], ltc4296_get_pec_byte(w_buf[2], 0x41));

	return no_os_spi_write_and_read(dev->spi_desc, w_buf, 5);
}

int ltc4296_reset(struct ltc4296_dev *dev)
{
	int ret;

	ret = ltc4296_reg_write(dev, LTC4296_REG_GCMD,
				no_os_field_prep(LTC4296_SW_RESET_MSK, LTC4296_RESET_CODE));
	if (ret)
		return ret;

	no_os_mdelay(5);

	return 0;
}

int ltc4296_get_port_addr(enum ltc4296_port port_no,
			  enum ltc4296_port_reg_offset_e port_offset,
			  uint8_t *port_addr)
{
	*port_addr = (((port_no + 1) << 4) + port_offset);

	return 0;
}

int ltc4296_clear_global_faults(struct ltc4296_dev *dev)
{
	/* GFLTEV = clear all faults
	 * LTC4296_LOW_CKT_BRK_FAULT | LTC4296_MEMORY_FAULT |
	 * LTC4296_PEC_FAULT | LTC4296_COMMAND_FAULT | LTC4296_UVLO_DIGITAL */
	return ltc4296_reg_write(dev, LTC4296_REG_GFLTEV, 0x001F);
}

int  ltc4296_clear_ckt_breaker(struct ltc4296_dev *dev)
{
	int ret;
	uint16_t val = 0;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GFLTEV, &val);
	if (ret)
		return ret;

	val = val | LTC4296_LOW_CKT_BRK_FAULT_MSK;

	/* GFLTEV = clear LTC4296_LOW_CKT_BRK_FAULT by writing 1 to the bit*/
	return ltc4296_reg_write(dev, LTC4296_REG_GFLTEV, val);
}

int ltc4296_read_global_faults(struct ltc4296_dev *dev, uint16_t *g_events)
{
	/* GFLTEV = clear all faults
	 * LTC4296_LOW_CKT_BRK_FAULT | LTC4296_MEMORY_FAULT |
	 * LTC4296_PEC_FAULT | LTC4296_COMMAND_FAULT | LTC4296_UVLO_DIGITAL */
	return ltc4296_reg_read(dev, LTC4296_REG_GFLTEV, g_events);
}

int ltc4296_unlock(struct ltc4296_dev *dev)
{
	return ltc4296_reg_write(dev, LTC4296_REG_GCMD,
				 LTC4296_UNLOCK_KEY);/*GCMD = unlock_key,*/
}

int ltc4296_is_locked(struct ltc4296_dev *dev, enum ltc4296_state *state)
{
	uint16_t val16;
	int ret;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GCMD, &val16);
	if(ret) {
		return ret;
	}

	if( (val16 & LTC4296_UNLOCK_KEY) == LTC4296_UNLOCK_KEY) {
		*state = LTC_UNLOCKED;
	} else {
		*state = LTC_LOCKED;
	}

	return 0;
}

int ltc4296_read_gadc(struct ltc4296_dev *dev, int *port_voltage_mv)
{
	int ret;
	uint16_t val16;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GADCDAT, &val16);
	if (ret)
		return ret;
	if( (val16 & LTC4296_GADC_NEW_MSK) == LTC4296_GADC_NEW_MSK) {
		/* A new ADC value is available */
		*port_voltage_mv = (((val16 & LTC4296_GADC_MSK) - 2049) * LTC4296_VGAIN);
	} else {
		return ADI_LTC_INVALID_ADC_VOLTAGE;
	}

	return 0;
}

int ltc4296_set_gadc_vin(struct ltc4296_dev *dev)
{
	int ret;

	/* LTC4296-1 Set global ADC to measure Vin
	GADCCFG = ContModeLowGain | Vin */
	ret = ltc4296_reg_write(dev, LTC4296_REG_GADCCFG, 0x0041);
	if (ret)
		return ret;

	no_os_mdelay(4); /* Delay of 4ms*/

	return 0;
}

int ltc4296_is_vin_valid(struct ltc4296_dev *dev, int port_vin_mv,
			 enum ltc4296_board_class ltcboard_class, bool *vin_valid)
{

	if((port_vin_mv <= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMAX])
	    && (port_vin_mv >= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMIN]) ) {
		*vin_valid = true;
	} else {
		/* Voltage is out of range of the MIN/MAX value*/
		*vin_valid = false;
	}
	return 0;
}

int ltc4296_is_vout_valid(struct ltc4296_dev *dev, int port_vout_mv,
			  enum ltc4296_board_class ltcboard_class, bool *vout_valid)
{

	if((port_vout_mv <= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMAX])
	    && (port_vout_mv >= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMIN]) ) {
		*vout_valid =  true;
	} else {
		/* Voltage is out of range of the MIN/MAX value*/
		*vout_valid = false;
	}

	return 0;
}

int ltc4296_disable_gadc(struct ltc4296_dev *dev)
{
	int ret;

	ret = ltc4296_reg_write(dev, LTC4296_REG_GADCCFG, 0x0000);
	if (ret)
		return ret;

	no_os_mdelay(4); /* Delay of 4ms*/

	return 0;
}

int ltc4296_read_port_events(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     uint16_t *port_events)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_EVENTS, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_read(dev, port_addr, port_events);
}

int ltc4296_clear_port_events(struct ltc4296_dev *dev,
			      enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_EVENTS, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr, 0x3FF);
}

int ltc4296_read_port_status(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     uint16_t *port_status)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_read(dev, port_addr, port_status);
}

int ltc4296_is_port_disabled(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     enum ltc4296_port_status *port_chk)
{
	int ret;
	uint16_t port_status = 0;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, port_addr, &port_status);
	if( (port_status & LTC4296_PSE_STATUS_MSK) == LTC_PSE_STATUS_DISABLED) {
		*port_chk = LTC_PORT_DISABLED;
	} else {
		*port_chk = LTC_PORT_ENABLED;
	}

	return 0;
}

int ltc4296_port_disable(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;
	/* Write 0 to disable port*/
	return ltc4296_reg_write(dev, port_addr, 0x0000);
}

int ltc4296_is_port_deliver_pwr(struct ltc4296_dev *dev,
				enum ltc4296_port port_no,
				enum ltc4296_pse_status *pwr_status)
{
	int ret;
	uint16_t port_status;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, port_addr, &port_status);
	if (ret)
		return ret;

	if( (port_status & LTC4296_PSE_STATUS_MSK) == LTC_PSE_STATUS_DELIVERING) {
		*pwr_status = LTC_PSE_STATUS_DELIVERING;
	} else {
		*pwr_status = LTC_PSE_STATUS_UNKNOWN;
	}

	return 0;
}

int ltc4296_is_port_pwr_stable(struct ltc4296_dev *dev,
			       enum ltc4296_port port_no, bool *pwr_status)
{
	int ret;
	uint16_t port_status;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, port_addr, &port_status);
	if (ret)
		return ret;

	if( (port_status & LTC4296_POWER_STABLE_MSK) == LTC4296_POWER_STABLE_MSK) {
		*pwr_status =  true;
	} else {
		*pwr_status = false;
	}

	return 0;
}

int ltc4296_read_port_adc(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			  int *port_i_out_ma)
{
	int ret;
	uint8_t port_addr = 0;
	uint16_t val16;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_ADCDAT, &port_addr);
	if (ret)
		return ret;
	ret = ltc4296_reg_read(dev, port_addr, &val16);
	if (ret)
		return ret;
	if( (val16 & LTC4296_NEW_MSK) == LTC4296_NEW_MSK) {
		/* A new ADC value is available */
		*port_i_out_ma =( ((val16 & 0x0FFF) - 2049) * ltc4296_spoe_rsense[port_no] /
				  10);
	} else {
		return ADI_LTC_INVALID_ADC_PORT_CURRENT;
	}

	return 0;
}

int ltc4296_port_prebias(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			 enum ltc4296_config mode)
{
	int ret;
	uint8_t port_addr = 0;
	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG1, &port_addr);
	if (ret)
		return ret;

	if(mode == LTC_CFG_SCCP_MODE) {
		/* NOTE: We need to set prebias_overridegood bit
		because DEMO-ADIN1100D2Z board has all 5 port connected in parallel
		and the other ports are pulling down their outputs
		PxCFG1 = tinrush=56.2ms|prebias_overridegood */

		ret = ltc4296_reg_write(dev, port_addr, 0x0108);
		if (ret)
			return ret;
	} else if(mode == LTC_CFG_APL_MODE) {
		/* PxCFG1 = tinrush=56.2ms|prebias_overridegood|sig_override_good */
		ret = ltc4296_reg_write(dev, port_addr, 0x0109);
	}

	return ret;
}

int ltc4296_port_en(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr, (LTC4296_SW_EN_MSK |
				 LTC4296_SW_POWER_AVAILABLE_MSK |
				 LTC4296_SW_PSE_READY_MSK |
				 LTC4296_TMFVDO_TIMER_DISABLE_MSK));
}


int ltc4296_port_en_and_classification(struct ltc4296_dev *dev,
				       enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_PSE_READY_MSK |
				  LTC4296_SET_CLASSIFICATION_MODE_MSK));
}

int ltc4296_is_pd_compatible(struct ltc4296_dev *dev,
			     enum ltc4296_board_class pse_class,
			     uint16_t sccp_response_data, uint8_t *pd_class)
{
	int ret = ADI_LTC_SCCP_PD_CLASS_COMPATIBLE;

	ret = sccp_is_pd(dev, pse_class, sccp_response_data, pd_class);
	if(ret == ADI_LTC_SCCP_PD_CLASS_NOT_SUPPORTED) {
		pr_info("LTC4296-1 SCCP.. PD class not supported \n");
	} else if(ret == ADI_LTC_SCCP_PD_CLASS_NOT_COMPATIBLE) {
		pr_info("LTC4296-1 SCCP.. PD class not compatible \n");
	}
	return ret;
}

int ltc4296_set_port_mfvs(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;
	uint16_t val16 = 0;
	int mfvs_threshold = 0, val = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_ADCCFG, &port_addr);
	if (ret)
		return ret;
	/* LTC4296-1 Set Port ADC MFVS Threshold Value
	*/
	val = ltc4296_spoe_sense_resistor[port_no];
	mfvs_threshold = (625 * val / 10);
	/* Roundof to the nearest integer */
	val16 = round(mfvs_threshold) / 1000;

	return ltc4296_reg_write(dev, port_addr, val16);
}

int ltc4296_set_port_pwr(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;
	/* LTC4296-1 Port Set Power Available & End Classification
	* PxCFG0 = swen|power_available|pse_rdy|set_classification|end_classification
	*/
	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_POWER_AVAILABLE_MSK |
				  LTC4296_SW_PSE_READY_MSK | LTC4296_SET_CLASSIFICATION_MODE_MSK |
				  LTC4296_END_CLASSIFICATION_MSK));
}

int ltc4296_force_port_pwr(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;
	/* LTC4296-1 Port Set Power Available & End Classification
	* PxCFG0 = swen|power_available|pse_rdy|tmfvdo_timer_disable
	*/
	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_POWER_AVAILABLE_MSK |
				  LTC4296_SW_PSE_READY_MSK | LTC4296_TMFVDO_TIMER_DISABLE_MSK) );
}

int ltc4296_port_pwr_available(struct ltc4296_dev *dev,
			       enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;
	/* LTC4296-1 Port Clear Classification & PSE Ready, Set Power Available
	* PxCFG0=en|power_available
	*/
	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_POWER_AVAILABLE_MSK));
}

int ltc4296_set_gadc_vout(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	uint16_t val16 = 0;

	/* LTC4296-1 Set global ADC to measure Port Vout
		GADCCFG=ContModeLowGain|VoutPort2 */

	val16 = (0x001F & set_port_vout[port_no]);
	/* Set Continuous mode with LOW GAIN bit */
	val16 = (val16 | 0x40);

	return ltc4296_reg_write(dev, LTC4296_REG_GADCCFG, val16);
}

int ltc4296_sccp_reset_pulse(struct ltc4296_dev *dev, uint8_t *pd_present)
{
	*pd_present = sccp_reset_pulse(dev);
	return ADI_LTC_SUCCESS;
}

int ltc4296_sccp_res_pd(struct ltc4296_dev *dev, uint16_t *res_data,
			uint8_t broadcast_addr,
			uint8_t read_scratchpad)
{
	uint8_t sccp_buf[3] = {0, 0, 0};
	int ret;

	ret = sccp_read_write_pd(dev, broadcast_addr, read_scratchpad, sccp_buf);
	if(ret == ADI_LTC_SCCP_PD_LINE_NOT_LOW) {
		pr_info("PD detection failed, PD line not LOW \n");
		ret = ADI_LTC_SCCP_PD_DETECTION_FAILED;
	} else if(ret == ADI_LTC_SCCP_PD_NOT_PRESENT) {
		pr_info("LTC4296-1 SCCP.. PD Not Present \n");
	} else if(ret == ADI_LTC_SCCP_PD_CRC_FAILED) {
		pr_info("PD classification failed, CRC ERROR \n");
	} else if(ret == ADI_LTC_SCCP_PD_LINE_NOT_HIGH) {
		pr_info("PD classification failed, PD line not HIGH \n");
	} else if(ret == ADI_LTC_SCCP_PD_PRESENT) {
		*res_data = (uint16_t)(sccp_buf[1]) << 8;
		*res_data |= (uint16_t)(sccp_buf[0]);
		//*pdCRC = sccp_buf[2];
	}

	return ret;
}

int ltc4296_sccp_pd(struct ltc4296_dev *dev, uint16_t *res_data,
		    uint8_t broadcast_addr,
		    uint8_t read_scratchpad)
{
	pr_info("\naddr = 0x%x, cmd = 0x%x \n  ", broadcast_addr, read_scratchpad);
	return ltc4296_sccp_res_pd(dev, res_data, broadcast_addr, read_scratchpad);
}

int ltc4296_print_global_faults(uint16_t g_events)
{
	if( (g_events & LTC4296_LOW_CKT_BRK_FAULT_MSK) == LTC4296_LOW_CKT_BRK_FAULT_MSK)
		pr_info("LTC4296-1 low side fault, too high current or negative voltage on output \n");
	else if( (g_events & LTC4296_MEMORY_FAULT_MSK) == LTC4296_MEMORY_FAULT_MSK)
		pr_info("LTC4296-1 memory fault \n");
	else if( (g_events & LTC4296_PEC_FAULT_MSK) == LTC4296_PEC_FAULT_MSK)
		pr_info("LTC4296-1 pec fault \n");
	else if( (g_events & LTC4296_COMMAND_FAULT_MSK) == LTC4296_COMMAND_FAULT_MSK)
		pr_info("LTC4296-1 command fault \n");
	else if( (g_events & LTC4296_UVLO_DIGITAL_MSK) == LTC4296_UVLO_DIGITAL_MSK)
		pr_info("LTC4296-1  UVLO, too low input voltage fault \n");

	return 0;
}

int ltc4296_print_port_events(enum ltc4296_port port_no, uint16_t port_events)
{
	if( (port_events & LTC4296_LSNS_REVERSE_FAULT_MSK) ==
	    LTC4296_LSNS_REVERSE_FAULT_MSK)
		pr_info("LTC4296-1 port%d LSNS_REVERSE_FAULT, negative low side current \n",
			port_no);

	if( (port_events & LTC4296_LSNS_FORWARD_FAULT_MSK) ==
	    LTC4296_LSNS_FORWARD_FAULT_MSK)
		pr_info("LTC4296-1 port%d LSNS_FORWARD_FAULT, too high low side current \n",
			port_no);

	if( (port_events & LTC4296_PD_WAKEUP_MSK) == LTC4296_PD_WAKEUP_MSK)
		pr_info("LTC4296-1 port%d PD_WAKEUP, wake up requested by PD \n",port_no);

	if( (port_events & LTC4296_TINRUSH_TIMER_DONE_MSK) ==
	    LTC4296_TINRUSH_TIMER_DONE_MSK)
		pr_info("LTC4296-1 port%d TINRUSH_TIMER_DONE, too long time to power on \n",
			port_no);

	if( (port_events & LTC4296_MFVS_TIMEOUT_MSK) == LTC4296_MFVS_TIMEOUT_MSK)
		pr_info("LTC4296-1 port%d MFVS_TIMEOUT, PD disconnected \n",port_no);

	if( (port_events & LTC4296_OVERLOAD_DETECTED_IPOWERED_MSK) ==
	    LTC4296_OVERLOAD_DETECTED_IPOWERED_MSK)
		pr_info("LTC4296-1 port%d OVERLOAD_DETECTED_IPOWERED, too high output current \n",
			port_no);

	if( (port_events & LTC4296_OVERLOAD_DETECTED_ISLEEP_MSK) ==
	    LTC4296_OVERLOAD_DETECTED_ISLEEP_MSK)
		pr_info("LTC4296-1 port%d OVERLOAD_DETECTED_ISLEEP, un-powered port overloaded \n",
			port_no);

	if( (port_events & LTC4296_TOFF_TIMER_DONE_MSK) == LTC4296_TOFF_TIMER_DONE_MSK)
		pr_info("LTC4296-1 port%d TOFF_TIMER_DONE, too long time to power off \n",
			port_no);

	return 0;
}

int ltc4296_chk_global_events(struct ltc4296_dev *dev)
{
	int ret;
	enum ltc4296_state state;
	uint16_t global_events = 0;

	/* Check if LTC4296-1 is locked */
	ret = ltc4296_is_locked(dev, &state);
	if (ret)
		return ret;
	if(state == LTC_LOCKED) {
		/* Unlock the LTC4296_1 */
		pr_info("PSE initiated ... \n");
		ret = ltc4296_reset(dev);
		if (ret)
			return ret;
		ret = ltc4296_unlock(dev);
		if (ret)
			return ret;
		ret = ltc4296_clear_global_faults(dev);
		if (ret)
			return ret;

		return ADI_LTC_DISCONTINUE_SCCP;
	} else if(state == LTC_UNLOCKED) {
		//pr_info("The LTC4296_1 is UNLocked \n");
		ret = ltc4296_read_global_faults(dev, &global_events);
		if (ret)
			return ret;
		ret = ltc4296_print_global_faults(global_events);
		if (ret)
			return ret;
		if( (global_events & LTC4296_LOW_CKT_BRK_FAULT_MSK) ==
		    LTC4296_LOW_CKT_BRK_FAULT_MSK) {
			ret = ltc4296_clear_ckt_breaker(dev);
			if (ret)
				return ret;
			return ADI_LTC_DISCONTINUE_SCCP;
		} else if( ((global_events & LTC4296_MEMORY_FAULT_MSK) ==
			    LTC4296_MEMORY_FAULT_MSK)   ||
			   ((global_events & LTC4296_PEC_FAULT_MSK) == LTC4296_PEC_FAULT_MSK) ||
			   ((global_events & LTC4296_COMMAND_FAULT_MSK) == LTC4296_COMMAND_FAULT_MSK) ||
			   ((global_events & LTC4296_UVLO_DIGITAL_MSK) == LTC4296_UVLO_DIGITAL_MSK)   ) {
			/* Global events other than circuit breaker ?*/
			ret = ltc4296_reset(dev);
			if (ret)
				return ret;
			ret = ltc4296_unlock(dev);
			if (ret)
				return ret;
			ret = ltc4296_clear_global_faults(dev);
			if (ret)
				return ret;
			return ADI_LTC_DISCONTINUE_SCCP;
		}
	}

	return 0;
}

int ltc4296_chk_port_events(struct ltc4296_dev *dev,
			    enum ltc4296_port ltc4296_port)
{
	int ret;
	uint16_t port_events = 0;

	/* Read the Port Events */
	ret = ltc4296_read_port_events(dev, ltc4296_port, &port_events);
	if (ret)
		return ret;
	/* Report only in case of port event other than signature */
	ret = ltc4296_print_port_events(ltc4296_port, port_events);
	if (ret)
		return ret;

	return ltc4296_clear_port_events(dev, ltc4296_port);
}

int ltc4296_do_apl(struct ltc4296_dev *dev,
		   enum ltc4296_board_class board_class,
		   enum ltc4296_port ltc4296_port, struct ltc4296_vi *ltc4296_vi)
{
	int ret = ADI_LTC_DISCONTINUE_APL;
	enum ltc4296_port_status port_status;
	enum ltc4296_pse_status port_pwr_status;
	int port_vout_mv, port_vin_mv, port_iout_ma;
	bool port_pwr_stable, vin_valid;

	ret = ltc4296_is_port_disabled(dev, ltc4296_port, &port_status);
	if (ret)
		return ret;

	if(port_status == LTC_PORT_ENABLED) {
		ret = ltc4296_is_port_deliver_pwr(dev, ltc4296_port, &port_pwr_status);
		if (ret)
			return ret;

		if(port_pwr_status == LTC_PSE_STATUS_DELIVERING) {
			ret = ltc4296_is_port_pwr_stable(dev, ltc4296_port, &port_pwr_stable);
			if(port_pwr_stable == true) {
				ret = ltc4296_read_gadc(dev, &port_vout_mv);
				if(ret == 0) {
					ltc4296_vi->ltc4296_vout = port_vout_mv;
				} else {
					pr_info("LTC4296-1 port%d APL Vout N/A V \n",ltc4296_port);
					return ADI_LTC_DISCONTINUE_APL;
				}

				ret = ltc4296_read_port_adc(dev, ltc4296_port, &port_iout_ma);
				if(ret == 0) {
					pr_info("APL LTC4296-1 port %d - PSE Iout %dmA \n",ltc4296_port, port_iout_ma);
					ltc4296_vi->ltc4296_iout = port_iout_ma;
					return ret;
				} else {
					pr_info("LTC4296-1 port%d APL Iout N/A mA \n",ltc4296_port);
					return ADI_LTC_DISCONTINUE_APL;
				}
				return 0;
			} else {
				pr_info("LTC4296-1 port%d Output power unstable \n",ltc4296_port);
				return ADI_LTC_DISCONTINUE_APL;
			}
		} else {
			pr_info("LTC4296-1 port%d Port not delivering power \n",ltc4296_port);
			return ADI_LTC_DISCONTINUE_APL;
		}
	} else if(port_status == LTC_PORT_DISABLED) {
		/*ADC is set to a known state before setting it to read Vin*/
		ret = ltc4296_reg_write(dev, LTC4296_REG_GADCCFG, 0x0000);   /* Disable ADC */
		if (ret)
			return ret;
		no_os_mdelay(4);/* Delay of 4ms */
		ret = ltc4296_set_gadc_vin(dev);   /* To measure Vin*/
		if (ret)
			return ret;
		no_os_mdelay(4); /* Delay of 4ms */
		ret = ltc4296_read_gadc(dev,
					&port_vin_mv); /* Read Global ADC and calculate Port Vin*/
		if(ret) {
			pr_info("LTC4296-1 port%d Vin measurement not valid, power cannot be enabled \n",
				ltc4296_port);
			return ADI_LTC_DISCONTINUE_APL;
		}
		ltc4296_vi->ltc4296_vin = port_vin_mv;
		ret = ltc4296_is_vin_valid(dev, port_vin_mv,board_class, &vin_valid);
		if (ret)
			return ret;
		if(vin_valid == true) {
			if(ltc4296_vi->ltc4296_print_vin == true) {
				pr_info("LTC4296-1 Port%d Vin %dV\n", ltc4296_port, port_vin_mv);
				ltc4296_vi->ltc4296_print_vin = false;
			}
			/* NOTE: We are unconditionally turning on the port
			The APL devices do not perform any classification
			The APL devices expect the relevant voltage to be available to them when they get connected or at system power up
			The power stays on the port when the APL device is disconnected
			The power is always limited to the relevant max voltage and max output current
			*/
			ret = ltc4296_port_prebias(dev, ltc4296_port, LTC_CFG_APL_MODE);
			if (ret)
				return ret;
			ret = ltc4296_force_port_pwr(dev, ltc4296_port);
			if (ret)
				return ret;
			ret = ltc4296_set_gadc_vout(dev, ltc4296_port);
			if (ret)
				return ret;

			pr_info("LTC4296-1 port%d power output enabled for %s \n",ltc4296_port,
				*(ltc4296_class_str[board_class]));
			return ADI_LTC_APL_COMPLETE;
		} else {
			ltc4296_vi->ltc4296_print_vin = true;
			pr_info("LTC4296-1 port%d Vin %dV out of range, expected %dV to %dV \n",
				ltc4296_port, port_vin_mv, ltc4296_spoe_vol_range_mv[board_class][LTC4296_VMIN],
				ltc4296_spoe_vol_range_mv[board_class][LTC4296_VMAX]);
			return ADI_LTC_DISCONTINUE_APL;
		}
	}

	return 0;
}

int ltc4296_do_spoe_sccp(struct ltc4296_dev *dev,
			 enum ltc4296_board_class board_class,
			 enum ltc4296_port ltc4296_port, struct ltc4296_vi *ltc4296_vi)
{
	int ret;
	enum ltc4296_port_status port_chk;
	enum ltc4296_pse_status pse_pwr_status;
	int port_vout_mv, port_vin_mv, port_iout_ma;
	uint16_t port_status, sccp_response_data;
	uint8_t pd_class;
	bool vin_valid;

	/* Read the Port Status */
	ret = ltc4296_is_port_disabled(dev, ltc4296_port, &port_chk);
	if (ret)
		return ret;
	if(port_chk == LTC_PORT_ENABLED) {
		ret = ltc4296_is_port_deliver_pwr(dev, ltc4296_port, &pse_pwr_status);
		if (ret)
			return ret;
		if(pse_pwr_status == LTC_PSE_STATUS_DELIVERING) {
			ret = ltc4296_read_gadc(dev, &port_vout_mv);
			if(ret == 0) {
				ltc4296_vi->ltc4296_vout = port_vout_mv;
			} else {
				pr_err("LTC4296-1 Port%d Vout N/A \n",ltc4296_port);
				return ADI_LTC_DISCONTINUE_SCCP;
			}
			ret = ltc4296_read_port_adc(dev, ltc4296_port, &port_iout_ma);
			if(ret == 0) {
				ltc4296_vi->ltc4296_iout = port_iout_ma;
				return 0;
			} else {
				pr_err("LTC4296-1 Port%d Iout N/A \n",ltc4296_port);
				return ADI_LTC_DISCONTINUE_SCCP;
			}

			return ADI_LTC_DISCONTINUE_SCCP;
		} else if(pse_pwr_status == LTC_PSE_STATUS_UNKNOWN) {
			ret = ltc4296_port_disable(dev, ltc4296_port);
			if (ret)
				return ret;
			pr_err("LTC4296-1 port %d disabling output \n",ltc4296_port);
			return ADI_LTC_DISCONTINUE_SCCP;
		}
	} else if(port_chk == LTC_PORT_DISABLED) {
		/*ADC is set to a known state before setting it to read Vin*/
		ret = ltc4296_disable_gadc(dev);   /* Disable ADC */
		if (ret)
			return ret;
		no_os_mdelay(4);/* Delay of 4ms */
		ret = ltc4296_set_gadc_vin(dev);   /* To measure Vin */
		if (ret)
			return ret;
		no_os_mdelay(4); /* Delay of 4ms */
		ret = ltc4296_read_gadc(dev,
					&port_vin_mv); /* Read Global ADC and calculate Port Vin*/
		if (ret) {
			pr_err("LTC4296-1 Port%d Vin measurement not valid, power cannot be enabled \n",
			       ltc4296_port);
			return ADI_LTC_DISCONTINUE_SCCP;
		}

		ltc4296_vi->ltc4296_vin = port_vin_mv;
		ret = ltc4296_is_vin_valid(dev, port_vin_mv, board_class, &vin_valid);
		if (ret)
			return ret;
		if(vin_valid == true) {
			if(ltc4296_vi->ltc4296_print_vin == true) {
				pr_info("LTC4296-1 Port%d Vin %dV\n", ltc4296_port, port_vin_mv);
				ltc4296_vi->ltc4296_print_vin = false;
			}
			ret = ltc4296_port_prebias(dev, ltc4296_port, LTC_CFG_SCCP_MODE);
			if (ret)
				return ret;
			ret = ltc4296_port_en_and_classification(dev, ltc4296_port);
			if (ret)
				return ret;
			no_os_mdelay(4); /* Delay of 4ms */

			ret = ltc4296_read_port_status(dev, ltc4296_port, &port_status);
			if (ret)
				return ret;
			if( (port_status & LTC4296_PSE_STATUS_MSK) == LTC_PSE_STATUS_SEARCHING) {
				/* This condition check is specific to DEMO-ADIN1100D2Z,
					* as the board has all 5 port connected in parallel
					*/
				if( (port_status & LTC4296_DET_VLOW_MSK) == LTC4296_DET_VLOW_MSK) {
					ret = ltc4296_port_disable(dev, ltc4296_port);
					pr_err("LTC4296-1 Port%d output voltage for classification too low \n",
					       ltc4296_port);
					return ADI_LTC_DISCONTINUE_SCCP;
				}
				/* Perform Classification */
				ret = ltc4296_sccp_res_pd(dev, &sccp_response_data, CMD_BROADCAST_ADDR,
							  CMD_READ_SCRATCHPAD);
				if(ret == ADI_LTC_SCCP_PD_PRESENT) {
					ret = ltc4296_is_pd_compatible(dev, board_class, sccp_response_data, &pd_class);
					if(ret == ADI_LTC_SCCP_PD_CLASS_COMPATIBLE) {
						ret = ltc4296_set_port_mfvs(dev, ltc4296_port);
						ret = ltc4296_set_port_pwr(dev, ltc4296_port);

						no_os_mdelay(5); /* Delay of 5ms */

						ret = ltc4296_port_pwr_available(dev, ltc4296_port);
						ret = ltc4296_set_gadc_vout(dev, ltc4296_port);
						pr_info("LTC4296-1 Port%d SCCP completed, PD class %d detected, PSE output enabled \n",
							ltc4296_port, pd_class);
						return ADI_LTC_SCCP_COMPLETE;
					} else { /* ADIN_LTC_SCCP_PD_OUTOFRANGE or ADIN_LTC_SCCP_PD_NOT_COMPATIBLE */
						ret = ltc4296_port_disable(dev, ltc4296_port);
						pr_err("LTC4296-1 Port%d detected PD class %d, not compatible with PSE %s \n",
						       ltc4296_port, pd_class, *(ltc4296_class_str[board_class]));
						return ADI_LTC_DISCONTINUE_SCCP;
					}
				} else if(ret ==
					  ADI_LTC_SCCP_PD_NOT_PRESENT) { /* ADIN_LTC_SCCP_PD_NOT_PRESENT */
					ret = ltc4296_port_disable(dev, ltc4296_port);
					return ADI_LTC_SCCP_PD_NOT_PRESENT;
				} else {/* Error case - ADIN_LTC_SCCP_PD_DETECTION_FAILED or ADIN_LTC_SCCP_PD_CRC_FAILED or ADI_LTC_SCCP_PD_LINE_NOT_HIGH */
					ret = ltc4296_port_disable(dev, ltc4296_port);
					return ADI_LTC_DISCONTINUE_SCCP;
				}
			} else {
				ret = ltc4296_port_disable(dev, ltc4296_port);
				pr_err("LTC4296-1 Port%d could not enter classification mode \n",ltc4296_port);
				return ADI_LTC_DISCONTINUE_SCCP;
			}
		} else {
			ltc4296_vi->ltc4296_print_vin = true;
			pr_err("LTC4296-1 Port%d Vin %dV out of range, expected %dV to %dV \n",
			       ltc4296_port, port_vin_mv, ltc4296_spoe_vol_range_mv[board_class][LTC4296_VMIN],
			       ltc4296_spoe_vol_range_mv[board_class][LTC4296_VMAX]);
			return ADI_LTC_DISCONTINUE_SCCP;
		}
	}
	return ret;
}

int ltc4296_pwr_test(struct ltc4296_dev *dev,
		     enum ltc4296_board_class board_class)
{
	int ret;
	enum ltc4296_port port_no = LTC_PORT0;
	int port_vout_mv, port_vin_mv, port_iout_ma, i_expected_ma;
	bool vin_valid, vout_valid;
	uint8_t i = 0;
	bool test_rets[LTC4296_MAX_PORTS];

	/*ADC is set to a known state before setting it to read Vin*/
	ret = ltc4296_disable_gadc(dev);   /* Disable ADC */
	if (ret)
		return ret;
	no_os_mdelay(4); /* Delay of 4ms */
	ret = ltc4296_set_gadc_vin(dev);   /* To measure Vin */
	if (ret)
		return ret;
	no_os_mdelay(4); /* Delay of 4ms */
	ret = ltc4296_read_gadc(dev,
				&port_vin_mv); /* Read Global ADC and calculate Port Vin*/
	if(ret) {
		pr_err("Vin measurement not valid, power cannot be enabled, test discontinued\n");
		return ADI_LTC_DISCONTINUE_APL;
	}
	ret = ltc4296_is_vin_valid(dev, port_vin_mv, board_class, &vin_valid);
	if (ret)
		return ret;
	if(vin_valid == true) {
		for(i = 0; i < LTC4296_MAX_PORTS; i++) {
			port_no = i;
			ret = ltc4296_port_prebias(dev, port_no, LTC_CFG_APL_MODE);
			if (ret)
				return ret;
			ret = ltc4296_force_port_pwr(dev, port_no);
			if (ret)
				return ret;
			ret = ltc4296_set_gadc_vout(dev, port_no);
			if (ret)
				return ret;
			no_os_mdelay(10);/* Delay of 10ms */

			ret = ltc4296_read_gadc(dev, &port_vout_mv);
			if (ret)
				return ret;

			if(ret == ADI_LTC_SUCCESS) {
				ret = ltc4296_is_vout_valid(dev, port_vout_mv, board_class, &vout_valid);
				if (ret) {
					return ret;
				}
				if(vout_valid == true) {
					pr_info("LTC4296-1 Port%d Vout %dV \n",port_no, port_vout_mv);
				} else {
					pr_err("LTC4296-1 Port%d Vout %dV out of limits \n",port_no, port_vout_mv);
				}
			} else {
				pr_info("LTC4296-1 Port%d Vout N/A \n",port_no);
			}
			ret = ltc4296_read_port_adc(dev, port_no, &port_iout_ma);
			if(ret == 0) {
				pr_info("LTC4296-1 Port%d Iout %dmA\n", port_no, port_iout_ma);
				i_expected_ma = (port_vout_mv / RTESTLOAD); // port_vout is already in mV
				ret = ltc4296_port_disable(dev, port_no);
				if (ret) {
					return ret;
				}
				/* IOUT should be +5% or -5% of the Iexpected*/
				if( (port_iout_ma <= (i_expected_ma + (i_expected_ma * 5 / 100)))
				    && (port_iout_ma >= (i_expected_ma - (i_expected_ma * 5 / 100))) ) {
					pr_info("LTC4296-1 Port%d Iexpected %dmA\n",port_no, i_expected_ma);
					test_rets[port_no] = true;
					pr_info("LTC4296-1 Port%d TEST PASSED\n",port_no);
				} else {
					pr_info("LTC4296-1 Port%d Iexpected %dmA, Iout is out of range \n",port_no,
						i_expected_ma);
					test_rets[port_no] = false;
					pr_info("LTC4296-1 Port%d - TEST FAILED\n",port_no);
				}
			} else {
				pr_info("LTC4296-1 Port%d Iout N/A \n",port_no);
				test_rets[port_no] = false;
				pr_info("LTC4296-1 Port%d TEST FAILED\n",port_no);
			}
		}
	} else {
		pr_info("LTC4296-1 Vin %dV out of range, expected %dV to %dV \n", port_vin_mv,
			ltc4296_spoe_vol_range_mv[board_class][LTC4296_VMIN],
			ltc4296_spoe_vol_range_mv[board_class][LTC4296_VMAX]);
		return ADI_LTC_DISCONTINUE_TEST;
	}

	pr_info("LTC4296-1 Production PWR TEST Results... ");
	for(i = 0; i < LTC4296_MAX_PORTS; i++) {
		if(test_rets[i] == false) {
			pr_info("FAILED\n");
			return ADI_LTC_TEST_FAILED;
		}
	}

	pr_info("PASSED\n");
	return ADI_LTC_TEST_COMPLETE;
}

int ltc4296_init(struct ltc4296_dev **device,
		 struct ltc4296_init_param *init_param)
{
	struct ltc4296_dev *dev;
	uint16_t value;
	int ret;

	//Input parameter NULL checks
	if((!device) || (!init_param))
		return -EINVAL;

	dev = (struct ltc4296_dev*)no_os_calloc(1, sizeof(struct ltc4296_dev));
	if(!dev)
		return  -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if(ret)
		return ret;

	ret = no_os_gpio_get_optional(&dev->sccpi_gpio,
				      init_param->sccpi_gpio);
	if (ret)
		return ret;

	ret = no_os_gpio_direction_input(dev->sccpi_gpio);
	if (ret)
		return ret;

	ret = no_os_gpio_get_optional(&dev->sccpo_gpio,
				      init_param->sccpo_gpio);
	if (ret)
		return ret;

	ret = ltc4296_reset(dev);
	if (ret)
		return ret;

	ret = ltc4296_reg_write(dev, LTC4296_REG_GCMD, LTC4296_UNLOCK_KEY);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GCMD, &value);
	if (ret)
		return ret;

	if (value != LTC4296_UNLOCK_KEY) {
		pr_err("Device locked. Write Access is disabled");
		return -EINVAL;
	}

	*device = dev;

	return 0;
}

int ltc4296_remove(struct ltc4296_dev* dev)
{

	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

