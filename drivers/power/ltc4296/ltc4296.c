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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdio.h>
#include <errno.h>
#include "ltc4296.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_util.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* LTC4296_1_IGAIN / Rsense[port_no]
 * {(0.1/3), (0.1/1.5), (0.1/0.68), (0.1/0.25), (0.1/0.1)} */
static const int ltc4296_spoe_rsense[LTC4296_MAX_PORTS] = {333, 666, 1470, 4000, 10000};

/* Value of RSense resistor for each port in mOhm */
static const int ltc4296_spoe_sense_resistor[LTC4296_MAX_PORTS] = {3000, 1500, 680, 250, 100};

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

static uint8_t set_port_vout[LTC4296_MAX_PORTS] = {0x04, 0x06, 0x08, 0x0A, 0x0C};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Compute the PEC.
 * @param data - The input data
 * @param seed - The seed.
 * @return the packet error code.
 */
static uint8_t ltc4296_get_pec_byte(uint8_t data, uint8_t seed)
{
	uint8_t pec = seed;
	uint8_t din, in0, in1, in2;
	int bit;
	for (bit = 7; bit >= 0; bit--) {
		din = (data >> bit) & 0x01;
		in0 = din ^ ( (pec >> 7) & 0x01 );
		in1 = in0 ^ ( pec & 0x01);
		in2 = in0 ^ ( (pec >> 1) & 0x01 );
		pec = (pec << 1);
		pec &= ~(0x07);
		pec = pec | in0 | (in1 << 1) | (in2 << 2);
	}

	return pec;
}

/**
 * @brief Register Read
 * @param dev - The device structure
 * @param reg - The register value
 * @param data - The data read from the device.
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_reg_read(struct ltc4296_dev *dev, uint8_t reg, uint16_t *data)
{
	int ret;
	uint8_t r_buf[5] = {0};

	if (!dev || !data)
		return -EINVAL;

	r_buf[0] = reg << 1 | LTC4296_SPI_READ;
	r_buf[1] = ltc4296_get_pec_byte(r_buf[0], 0x41);

	ret = no_os_spi_write_and_read(dev->spi_desc, r_buf, 5);
	if (ret)
		return ret;

	*data = no_os_get_unaligned_be16(&r_buf[2]);

	return 0;
}

/**
 * @brief Register Write
 * @param dev - The device structure
 * @param reg - The register value
 * @param data - The data written to the device
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_reg_write(struct ltc4296_dev *dev, uint8_t reg, uint16_t data)
{
	uint8_t w_buf[5] = {0};

	if (!dev)
		return -EINVAL;

	w_buf[0] = reg << 1 | LTC4296_SPI_WRITE;
	w_buf[1] = ltc4296_get_pec_byte(w_buf[0], 0x41);
	no_os_put_unaligned_be16(data, &w_buf[2]);
	w_buf[4] = ltc4296_get_pec_byte(w_buf[3], ltc4296_get_pec_byte(w_buf[2], 0x41));

	return no_os_spi_write_and_read(dev->spi_desc, w_buf, 5);
}

/**
 * @brief Software reset
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_reset(struct ltc4296_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_reg_write(dev, LTC4296_REG_GCMD,
				no_os_field_prep(LTC4296_SW_RESET_MSK, LTC4296_RESET_CODE));
	if (ret)
		return ret;

	/* Delay required after performing a software reset */
	no_os_mdelay(1);

	return 0;
}

/**
 * @brief Get port address
 * @param port_no - The port number
 * @param port_offset - The port offset
 * @param port_addr - The port address
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_get_port_addr(enum ltc4296_port port_no,
			  enum ltc4296_port_reg_offset_e port_offset,
			  uint8_t *port_addr)
{
	if (!port_addr)
		return -EINVAL;

	*port_addr = (((port_no + 1) << 4) + port_offset);

	return 0;
}

/**
 * @brief Clear global faults
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_clear_global_faults(struct ltc4296_dev *dev)
{
	if (!dev)
		return -EINVAL;

	/* GFLTEV = clear all faults
	 * LTC4296_LOW_CKT_BRK_FAULT | LTC4296_MEMORY_FAULT |
	 * LTC4296_PEC_FAULT | LTC4296_COMMAND_FAULT | LTC4296_UVLO_DIGITAL */
	return ltc4296_reg_write(dev, LTC4296_REG_GFLTEV, 0x001F);
}

/**
 * @brief Clear circuit break faults
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int  ltc4296_clear_ckt_breaker(struct ltc4296_dev *dev)
{
	int ret;
	uint16_t val = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GFLTEV, &val);
	if (ret)
		return ret;

	val = val | LTC4296_LOW_CKT_BRK_FAULT_MSK;

	/* GFLTEV = clear LTC4296_LOW_CKT_BRK_FAULT by writing 1 to the bit*/
	return ltc4296_reg_write(dev, LTC4296_REG_GFLTEV, val);
}

/**
 * @brief Read global faults
 * @param dev - The device structure
 * @param g_events - The global events data read
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_read_global_faults(struct ltc4296_dev *dev, uint16_t *g_events)
{
	if (!dev)
		return -EINVAL;

	/* GFLTEV = clear all faults
	 * LTC4296_LOW_CKT_BRK_FAULT | LTC4296_MEMORY_FAULT |
	 * LTC4296_PEC_FAULT | LTC4296_COMMAND_FAULT | LTC4296_UVLO_DIGITAL */
	return ltc4296_reg_read(dev, LTC4296_REG_GFLTEV, g_events);
}

/**
 * @brief Device unlock
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_unlock(struct ltc4296_dev *dev)
{
	if (!dev)
		return -EINVAL;

	return ltc4296_reg_write(dev, LTC4296_REG_GCMD,
				 LTC4296_UNLOCK_KEY);/*GCMD = unlock_key,*/
}

/**
 * @brief Check device state
 * @param dev - The device structure
 * @param state - The state of the device
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_is_locked(struct ltc4296_dev *dev, enum ltc4296_state *state)
{
	uint16_t val16;
	int ret;

	if (!dev || !state)
		return -EINVAL;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GCMD, &val16);
	if (ret)
		return ret;

	if ((val16 & LTC4296_UNLOCK_KEY) == LTC4296_UNLOCK_KEY)
		*state = LTC_UNLOCKED;
	else
		*state = LTC_LOCKED;

	return 0;
}

/**
 * @brief Read global ADC
 * @param dev - The device structure
 * @param port_voltage_mv - The port voltage in mV
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_read_gadc(struct ltc4296_dev *dev, int *port_voltage_mv)
{
	int ret;
	uint16_t val16;

	if (!dev || !port_voltage_mv)
		return -EINVAL;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GADCDAT, &val16);
	if (ret)
		return ret;
	if ((val16 & LTC4296_GADC_NEW_MSK) != LTC4296_GADC_NEW_MSK)
		return ADI_LTC_INVALID_ADC_VOLTAGE;

	/* A new ADC value is available */
	*port_voltage_mv = (((val16 & LTC4296_GADC_MSK) - LTC4296_ADC_OFFSET) *
			    LTC4296_VGAIN);

	return 0;
}

/**
 * @brief Set the global ADC to measure voltage
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_set_gadc_vin(struct ltc4296_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	/* LTC4296-1 Set global ADC to measure Vin
	GADCCFG = ContModeLowGain | Vin */
	ret = ltc4296_reg_write(dev, LTC4296_REG_GADCCFG, 0x0041);
	if (ret)
		return ret;

	no_os_mdelay(4); /* Delay of 4ms*/

	return 0;
}

/**
 * @brief Check if input voltage is valid.
 * @param dev - The device structure
 * @param port_vin_mv - The input voltage in mV
 * @param ltcboard_class - The board class
 * @param vin_valid - Valid input voltage
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_is_vin_valid(struct ltc4296_dev *dev, int port_vin_mv,
			 enum ltc4296_board_class ltcboard_class, bool *vin_valid)
{
	if (!dev || !vin_valid)
		return -EINVAL;

	if ((port_vin_mv <= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMAX])
	    && (port_vin_mv >= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMIN]) ) {
		*vin_valid = true;
	} else {
		/* Voltage is out of range of the MIN/MAX value*/
		*vin_valid = false;
	}
	return 0;
}

/**
 * @brief Check if output voltage is valid.
 * @param dev - The device structure
 * @param port_vout_mv - The output voltage in mV
 * @param ltcboard_class - The board class
 * @param vout_valid - Valid input voltage
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_is_vout_valid(struct ltc4296_dev *dev, int port_vout_mv,
			  enum ltc4296_board_class ltcboard_class, bool *vout_valid)
{
	if (!dev || !vout_valid)
		return -EINVAL;

	if ((port_vout_mv <= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMAX])
	    && (port_vout_mv >= ltc4296_spoe_vol_range_mv[ltcboard_class][LTC4296_VMIN]) ) {
		*vout_valid =  true;
	} else {
		/* Voltage is out of range of the MIN/MAX value*/
		*vout_valid = false;
	}

	return 0;
}

/**
 * @brief Disable the global ADC.
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_disable_gadc(struct ltc4296_dev *dev)
{
	int ret;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_reg_write(dev, LTC4296_REG_GADCCFG, 0x0000);
	if (ret)
		return ret;

	no_os_mdelay(4); /* Delay of 4ms*/

	return 0;
}

/**
 * @brief Read Port Events.
 * @param dev - The device structure
 * @param port_no - The port number
 * @param port_events - The port events read
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_read_port_events(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     uint16_t *port_events)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev || !port_events)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_EVENTS, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_read(dev, port_addr, port_events);
}

/**
 * @brief Clear Port Events.
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_clear_port_events(struct ltc4296_dev *dev,
			      enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_EVENTS, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr, 0x3FF);
}

/**
 * @brief Read Port Status.
 * @param dev - The device structure
 * @param port_no - The port number
 * @param port_status - The port status
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_read_port_status(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     uint16_t *port_status)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev || !port_status)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_read(dev, port_addr, port_status);
}

/**
 * @brief Check if port is enabled/disabled
 * @param dev - The device structure
 * @param port_no - The port number
 * @param port_chk - The port is enabled/disabled
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_is_port_disabled(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     enum ltc4296_port_status *port_chk)
{
	int ret;
	uint16_t port_status = 0;
	uint8_t port_addr = 0;

	if (!dev || !port_chk)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, port_addr, &port_status);
	if (ret)
		return ret;

	if ((port_status & LTC4296_PSE_STATUS_MSK) == LTC_PSE_STATUS_DISABLED) {
		*port_chk = LTC_PORT_DISABLED;
	} else {
		*port_chk = LTC_PORT_ENABLED;
	}

	return 0;
}

/**
 * @brief Check if port is disabled
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_port_disable(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;
	/* Write 0 to disable port*/
	return ltc4296_reg_write(dev, port_addr, 0x0000);
}

/**
 * @brief Check if port delivers power
 * @param dev - The device structure
 * @param port_no - The port number
 * @param pwr_status - The PSE Status of the port
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_is_port_deliver_pwr(struct ltc4296_dev *dev,
				enum ltc4296_port port_no,
				enum ltc4296_pse_status *pwr_status)
{
	int ret;
	uint16_t port_status;
	uint8_t port_addr = 0;

	if (!dev || !pwr_status)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, port_addr, &port_status);
	if (ret)
		return ret;

	if ((port_status & LTC4296_PSE_STATUS_MSK) == LTC_PSE_STATUS_DELIVERING) {
		*pwr_status = LTC_PSE_STATUS_DELIVERING;
	} else {
		*pwr_status = LTC_PSE_STATUS_UNKNOWN;
	}

	return 0;
}

/**
 * @brief Check if port has stable power
 * @param dev - The device structure
 * @param port_no - The port number
 * @param pwr_status - The power status
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_is_port_pwr_stable(struct ltc4296_dev *dev,
			       enum ltc4296_port port_no, bool *pwr_status)
{
	int ret;
	uint16_t port_status;
	uint8_t port_addr = 0;

	if (!dev || !pwr_status)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_STATUS, &port_addr);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, port_addr, &port_status);
	if (ret)
		return ret;

	if ((port_status & LTC4296_POWER_STABLE_MSK) == LTC4296_POWER_STABLE_MSK) {
		*pwr_status =  true;
	} else {
		*pwr_status = false;
	}

	return 0;
}

/**
 * @brief Read port ADC
 * @param dev - The device structure
 * @param port_no - The port number
 * @param port_i_out_ma - The port current in mA.
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_read_port_adc(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			  int *port_i_out_ma)
{
	int ret;
	uint8_t port_addr = 0;
	uint16_t val16;

	if (!dev || !port_i_out_ma)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_ADCDAT, &port_addr);
	if (ret)
		return ret;

	ret = ltc4296_reg_read(dev, port_addr, &val16);
	if (ret)
		return ret;

	if ((val16 & LTC4296_NEW_MSK) == LTC4296_NEW_MSK)
		/* A new ADC value is available */
		*port_i_out_ma =( ((val16 & 0x0FFF) - LTC4296_ADC_OFFSET) *
				  ltc4296_spoe_rsense[port_no] /
				  10);
	else
		return ADI_LTC_INVALID_ADC_PORT_CURRENT;

	return 0;
}

/**
 * @brief Stimulate valid wake-up signature for a port
 * @param dev - The device structure
 * @param port_no - The port number
 * @param mode - The device configuration mode
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_port_prebias(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			 enum ltc4296_config mode)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG1, &port_addr);
	if (ret)
		return ret;

	if (mode == LTC_CFG_SCCP_MODE)
		return ltc4296_reg_write(dev, port_addr, 0x0108);
	else if (mode == LTC_CFG_APL_MODE)
		return ltc4296_reg_write(dev, port_addr, 0x0109);
	else
		return -EINVAL;
}

/**
 * @brief Enable specific port
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_port_en(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr, (LTC4296_SW_EN_MSK |
				 LTC4296_SW_POWER_AVAILABLE_MSK |
				 LTC4296_SW_PSE_READY_MSK |
				 LTC4296_TMFVDO_TIMER_DISABLE_MSK));
}

/**
 * @brief Enable specific port and classify
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_port_en_and_classification(struct ltc4296_dev *dev,
				       enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_PSE_READY_MSK |
				  LTC4296_SET_CLASSIFICATION_MODE_MSK));
}

/**
 * @brief Set port to maintain full voltage signature.
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_set_port_mfvs(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;
	uint16_t val16;
	int mfvs_threshold, val;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_ADCCFG, &port_addr);
	if (ret)
		return ret;

	/* LTC4296-1 Set Port ADC MFVS Threshold Value */
	val = ltc4296_spoe_sense_resistor[port_no];
	mfvs_threshold = (625 * val / 10);
	/* Round of to the nearest integer */
	val16 = NO_OS_DIV_ROUND_CLOSEST(mfvs_threshold, 1000);

	return ltc4296_reg_write(dev, port_addr, val16);
}

/**
 * @brief Set port to power mode.
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_set_port_pwr(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_POWER_AVAILABLE_MSK |
				  LTC4296_SW_PSE_READY_MSK | LTC4296_SET_CLASSIFICATION_MODE_MSK |
				  LTC4296_END_CLASSIFICATION_MSK));
}

/**
 * @brief Force port to power mode.
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_force_port_pwr(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;

	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_POWER_AVAILABLE_MSK |
				  LTC4296_SW_PSE_READY_MSK | LTC4296_TMFVDO_TIMER_DISABLE_MSK) );
}

/**
 * @brief Set port is able to source power.
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_port_pwr_available(struct ltc4296_dev *dev,
			       enum ltc4296_port port_no)
{
	int ret;
	uint8_t port_addr = 0;

	if (!dev)
		return -EINVAL;

	ret = ltc4296_get_port_addr(port_no, LTC_PORT_CFG0, &port_addr);
	if (ret)
		return ret;
	/* LTC4296-1 Port Clear Classification & PSE Ready, Set Power Available
	* PxCFG0=en|power_available
	*/
	return ltc4296_reg_write(dev, port_addr,
				 (LTC4296_SW_EN_MSK | LTC4296_SW_POWER_AVAILABLE_MSK));
}

/**
 * @brief Configure Global ADC to read output voltage
 * @param dev - The device structure
 * @param port_no - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_set_gadc_vout(struct ltc4296_dev *dev, enum ltc4296_port port_no)
{
	uint16_t val16;

	if (!dev)
		return -EINVAL;

	/* LTC4296-1 Set global ADC to measure Port Vout
		GADCCFG=ContModeLowGain|VoutPort2 */

	val16 = (0x001F & set_port_vout[port_no]);
	/* Set Continuous mode with LOW GAIN bit */
	val16 = (val16 | 0x40);

	return ltc4296_reg_write(dev, LTC4296_REG_GADCCFG, val16);
}

/**
 * @brief Print global faults
 * @param g_events - The global events data
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_print_global_faults(uint16_t g_events)
{
	if ((g_events & LTC4296_LOW_CKT_BRK_FAULT_MSK) == LTC4296_LOW_CKT_BRK_FAULT_MSK)
		pr_info("LTC4296-1 low side fault, too high current or negative voltage on output \n");
	else if ((g_events & LTC4296_MEMORY_FAULT_MSK) == LTC4296_MEMORY_FAULT_MSK)
		pr_info("LTC4296-1 memory fault \n");
	else if ((g_events & LTC4296_PEC_FAULT_MSK) == LTC4296_PEC_FAULT_MSK)
		pr_info("LTC4296-1 pec fault \n");
	else if ((g_events & LTC4296_COMMAND_FAULT_MSK) == LTC4296_COMMAND_FAULT_MSK)
		pr_info("LTC4296-1 command fault \n");
	else if ((g_events & LTC4296_UVLO_DIGITAL_MSK) == LTC4296_UVLO_DIGITAL_MSK)
		pr_info("LTC4296-1  UVLO, too low input voltage fault \n");

	return 0;
}

/**
 * @brief Print port events
 * @param port_no - The port number
 * @param port_events - The port events data
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_print_port_events(enum ltc4296_port port_no, uint16_t port_events)
{
	if ((port_events & LTC4296_LSNS_REVERSE_FAULT_MSK) ==
	    LTC4296_LSNS_REVERSE_FAULT_MSK)
		pr_info("LTC4296-1 port%d LSNS_REVERSE_FAULT, negative low side current \n",
			port_no);

	if ((port_events & LTC4296_LSNS_FORWARD_FAULT_MSK) ==
	    LTC4296_LSNS_FORWARD_FAULT_MSK)
		pr_info("LTC4296-1 port%d LSNS_FORWARD_FAULT, too high low side current \n",
			port_no);

	if ((port_events & LTC4296_PD_WAKEUP_MSK) == LTC4296_PD_WAKEUP_MSK)
		pr_info("LTC4296-1 port%d PD_WAKEUP, wake up requested by PD \n",port_no);

	if ((port_events & LTC4296_TINRUSH_TIMER_DONE_MSK) ==
	    LTC4296_TINRUSH_TIMER_DONE_MSK)
		pr_info("LTC4296-1 port%d TINRUSH_TIMER_DONE, too long time to power on \n",
			port_no);

	if ((port_events & LTC4296_MFVS_TIMEOUT_MSK) == LTC4296_MFVS_TIMEOUT_MSK)
		pr_info("LTC4296-1 port%d MFVS_TIMEOUT, PD disconnected \n",port_no);

	if ((port_events & LTC4296_OVERLOAD_DETECTED_IPOWERED_MSK) ==
	    LTC4296_OVERLOAD_DETECTED_IPOWERED_MSK)
		pr_info("LTC4296-1 port%d OVERLOAD_DETECTED_IPOWERED, too high output current \n",
			port_no);

	if ((port_events & LTC4296_OVERLOAD_DETECTED_ISLEEP_MSK) ==
	    LTC4296_OVERLOAD_DETECTED_ISLEEP_MSK)
		pr_info("LTC4296-1 port%d OVERLOAD_DETECTED_ISLEEP, un-powered port overloaded \n",
			port_no);

	if ((port_events & LTC4296_TOFF_TIMER_DONE_MSK) == LTC4296_TOFF_TIMER_DONE_MSK)
		pr_info("LTC4296-1 port%d TOFF_TIMER_DONE, too long time to power off \n",
			port_no);

	return 0;
}

/**
 * @brief Check global events
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_chk_global_events(struct ltc4296_dev *dev)
{
	int ret;
	enum ltc4296_state state;
	uint16_t global_events = 0;

	if (!dev)
		return -EINVAL;

	/* Check if LTC4296-1 is locked */
	ret = ltc4296_is_locked(dev, &state);
	if (ret)
		return ret;

	if (state == LTC_LOCKED) {
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
	} else {
		ret = ltc4296_read_global_faults(dev, &global_events);
		if (ret)
			return ret;

		ret = ltc4296_print_global_faults(global_events);
		if (ret)
			return ret;

		if ((global_events & LTC4296_LOW_CKT_BRK_FAULT_MSK) ==
		    LTC4296_LOW_CKT_BRK_FAULT_MSK) {
			ret = ltc4296_clear_ckt_breaker(dev);
			if (ret)
				return ret;

			return ADI_LTC_DISCONTINUE_SCCP;
		} else if (((global_events & LTC4296_MEMORY_FAULT_MSK) ==
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

/**
 * @brief Check port events
 * @param dev - The device structure
 * @param ltc4296_port - The port number
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_chk_port_events(struct ltc4296_dev *dev,
			    enum ltc4296_port ltc4296_port)
{
	int ret;
	uint16_t port_events = 0;

	if (!dev)
		return -EINVAL;

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

/**
 * @brief Device initialization
 * @param device - The device structure
 * @param init_param - The initialization parameters
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_init(struct ltc4296_dev **device,
		 struct ltc4296_init_param *init_param)
{
	struct ltc4296_dev *dev;
	uint16_t value;
	int ret;

	if ((!device) || (!init_param))
		return -EINVAL;

	dev = (struct ltc4296_dev*)no_os_calloc(1, sizeof(struct ltc4296_dev));
	if (!dev)
		return  -ENOMEM;

	ret = no_os_spi_init(&dev->spi_desc, init_param->spi_init);
	if (ret)
		goto err;

	ret = ltc4296_reset(dev);
	if (ret)
		goto err_spi;

	ret = ltc4296_reg_write(dev, LTC4296_REG_GCMD, LTC4296_UNLOCK_KEY);
	if (ret)
		goto err_spi;

	ret = ltc4296_reg_read(dev, LTC4296_REG_GCMD, &value);
	if (ret)
		goto err_spi;

	if (value != LTC4296_UNLOCK_KEY) {
		pr_err("Device locked. Write Access is disabled");
		ret = -EINVAL;
		goto err_spi;
	}

	*device = dev;

	return 0;

err_spi:
	no_os_spi_remove(dev);
err:
	no_os_free(dev);

	return ret;
}

/**
 * @brief Free allocated resources
 * @param dev - The device structure
 * @return 0 in case of success, negative code otherwise
 */
int ltc4296_remove(struct ltc4296_dev* dev)
{
	int ret;

	ret = no_os_spi_remove(dev->spi_desc);
	if (ret)
		return ret;

	no_os_free(dev);

	return 0;
}

