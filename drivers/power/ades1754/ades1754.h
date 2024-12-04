/***************************************************************************//**
 *   @file   ades1754.h
 *   @brief  Header file for the ADES1754 Driver
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __ADES1754_H__
#define __ADES1754_H__

#include "no_os_spi.h"
#include "no_os_uart.h"
#include "no_os_util.h"
#include "no_os_units.h"

#define ADES1754_VERSION_REG			0x00
#define ADES1754_ADDRESS_REG			0x01
#define ADES1754_STATUS1_REG			0x02
#define ADES1754_STATUS2_REG			0x03
#define ADES1754_STATUS3_REG			0x04
#define ADES1754_FMEA1_REG			0x05
#define ADES1754_FMEA2_REG			0x06
#define ADES1754_ALRTSUM_REG			0x07
#define ADES1754_ALRTOVCELL_REG			0x08
#define ADES1754_ALRTUVCELL_REG			0x09
#define ADES1754_MINMAXCELL_REG			0x0A
#define ADES1754_ALRTAUXPRTCT_REG		0x0B
#define ADES1754_ALRTUAUXOV_REG			0x0C
#define ADES1754_ALRTAUXUV_REG			0x0D
#define ADES1754_ALRTCOMPOVR_REG		0x0E
#define ADES1754_ALRTCOMPUVR_REG		0x0F
#define ADES1754_ALRTCOMPAUXOVR_REG		0x10
#define ADES1754_ALRTCOMPAUXUV_REG		0x11
#define ADES1754_ALRTBALSWRE_REG		0x12
#define ADES1754_SWACTION_REG			0x13
#define ADES1754_DEVCFG_REG			0x14
#define ADES1754_DEVCFG2_REG			0x15
#define ADES1754_AUXGPIOCFG_REG			0x16
#define ADES1754_GPIOCFG_REG			0x17
#define ADES1754_PACKCFG_REG			0x18

#define ADES1754_ALRTIRQEN_REG			0x19
#define ADES1754_ALRTOVEN_REG			0x1A
#define ADES1754_ALRTUVEN_REG			0x1B
#define ADES1754_ALRTAUXOVEN_REG		0x1C
#define ADES1754_ALRTAUXUVEN_REG		0x1D
#define ADES1754_ALRTCALTST_REG			0x1E

#define ADES1754_OVTHCLR_REG			0x1F
#define ADES1754_OVTHSET_REG			0x20
#define ADES1754_UVTHCLR_REG			0x21
#define ADES1754_UVTHSET_REG			0x22
#define ADES1754_MSMTCH_REG			0x23
#define ADES1754_BIPOVTHCLR_REG			0x24
#define ADES1754_BIPOVTHSET_REG			0x25
#define ADES1754_BIPUVTHCLR_REG			0x26
#define ADES1754_BIPUVTHSET_REG			0x27
#define ADES1754_BLKOVTHCLR_REG			0x28
#define ADES1754_BLKOVTHSET_REG			0x29
#define ADES1754_BLKUVTHCLR_REG			0x2A
#define ADES1754_BLKUVTHSET_REG			0x2B
#define ADES1754_AUXROVTHCLR_REG		0x30
#define ADES1754_AUXROVTHSET_REG		0x31
#define ADES1754_AUXRUVTHCLR_REG		0x32
#define ADES1754_AUXRUVTHSET_REG		0x33
#define ADES1754_AUXAOVTHCLR_REG		0x34
#define ADES1754_AUXAOVTHSET_REG		0x35
#define ADES1754_AUXAUVTHCLR_REG		0x36
#define ADES1754_AUXAUVTHSET_REG		0x37
#define ADES1754_COMPOVTH_REG			0x38
#define ADES1754_COMPUVTH_REG			0x39
#define ADES1754_COMPAUXROVTH_REG		0x3A
#define ADES1754_COMPAUXRUVTH_REG		0x3B
#define ADES1754_COMPAUXAOVTH_REG		0x3C
#define ADES1754_COMPAUXAUVTH_REG		0x3D

#define ADES1754_COMPOPNTH_REG			0x3E
#define ADES1754_COMPAUXROPNTH_REG		0x3F
#define ADES1754_COMPAUXOPNTH_REG		0x40
#define ADES1754_COMPACCOVTH_REG		0x41
#define ADES1754_COMPACCUVTH_REG		0x42
#define ADES1754_BALSHRTTHR_REG			0x43
#define ADES1754_BALLOWTHR_REG			0x44
#define ADES1754_BALHIGHTHR_REG			0x45

#define ADES1754_CELLN_REG(n)			(0x47 + (n))
#define ADES1754_BLOCK_REG			0x55

#define ADES1754_TOTAL_REG			0x56
#define ADES1754_DIAG1_REG			0x57
#define ADES1754_DIAG2_REG			0x58
#define ADES1754_AUX0_REG			0x59
#define ADES1754_AUX1_REG			0x5A
#define ADES1754_AUX2_REG			0x5B
#define ADES1754_AUX3_REG			0x5C
#define ADES1754_AUX4_REG			0x5D
#define ADES1754_AUX5_REG			0x5E

#define ADES1754_POLARITYCTRL_REG		0x5F
#define ADES1754_AUXREFCTRL_REG			0x60
#define ADES1754_AUXTIME_REG			0x61
#define ADES1754_ACQCFG_REG			0x62
#define ADES1754_BALSWDLY_REG			0x63

#define ADES1754_MEASUREEN1_REG			0x64
#define ADES1754_MEASUREEN2_REG			0x65
#define ADES1754_SCANCTRL_REG			0x66

#define ADES1754_ADCTEST1A_REG			0x67
#define ADES1754_ADCTEST1B_REG			0x68
#define ADES1754_ADCTEST2A_REG			0x69
#define ADES1754_ADCTEST2B_REG			0x6A
#define ADES1754_DIAGCFG_REG			0x6B
#define ADES1754_CTSTCFG_REG			0x6C
#define ADES1754_AUXTSTCFG_REG			0x6D
#define ADES1754_DIAGGENCFG_REG			0x6E

#define ADES1754_BALSWCTRL_REG			0x6F
#define ADES1754_BALEXP1_REG			0x70
#define ADES1754_BALEXP2_REG			0x71
#define ADES1754_BALEXP3_REG			0x72
#define ADES1754_BALEXP4_REG			0x73
#define ADES1754_BALEXP5_REG			0x74
#define ADES1754_BALEXP6_REG			0x75
#define ADES1754_BALEXP7_REG			0x76
#define ADES1754_BALEXP8_REG			0x77
#define ADES1754_BALEXP9_REG			0x78
#define ADES1754_BALEXP10_REG			0x79
#define ADES1754_BALEXP11_REG			0x7A
#define ADES1754_BALEXP12_REG			0x7B
#define ADES1754_BALEXP13_REG			0x7C
#define ADES1754_BALEXP14_REG			0x7D
#define ADES1754_BALAUTOUVTH_REG		0x7E
#define ADES1754_BALDLYCTRL_REG			0x7F
#define ADES1754_BALCTRL_REG			0x80
#define ADES1754_BALSTAT_REG			0x81
#define ADES1754_BALUVSTAT_REG			0x82
#define ADES1754_BALDATA_REG			0x83

#define ADES1754_I2CPNTR_REG			0x84
#define ADES1754_I2CWDATA1_REG			0x85
#define ADES1754_I2CWDATA2_REG			0x86
#define ADES1754_I2CRDATA1_REG			0x87
#define ADES1754_I2CRDATA2_REG			0x88
#define ADES1754_I2CCFG_REG			0x89
#define ADES1754_I2CSTAT_REG			0x8A
#define ADES1754_I2CSEND_REG			0x8B

#define ADES1754_ID1_REG			0x8C
#define ADES1754_ID2_REG			0x8D
#define ADES1754_ID3_REG			0x8E
#define ADES1754_OTP3_REG			0x8F
#define ADES1754_OTP4_REG			0x90
#define ADES1754_OTP5_REG			0x91
#define ADES1754_OTP6_REG			0x92
#define ADES1754_OTP7_REG			0x93
#define ADES1754_OTP8_REG			0x94
#define ADES1754_OTP9_REG			0x95
#define ADES1754_OTP10_REG			0x96
#define ADES1754_OTP11_REG			0x97
#define ADES1754_OTP12_REG			0x98

#define ADES1754_PREAMBLE_BYTE			0x15
#define ADES1754_STOP_BYTE			0x54
#define ADES1754_HELLO_ALL_BYTE			0x57

#define ADES1754_DEV_ADDR_MAX			0x1F
#define ADES1754_WR_ALL				0x02
#define ADES1754_RD_ALL				0x03
#define ADES1754_DEFAULT_DC_BYTE_SEED		0x00
#define ADES1754_FILL_BYTE_C2			0xC2
#define ADES1754_FILL_BYTE_D3			0xD3

#define ADES1754_WR_MASK			NO_OS_BIT(2)
#define ADES1754_RD_MASK			ADES1754_WR_MASK | NO_OS_BIT(0)
#define ADES1754_BL_MASK			NO_OS_GENMASK(2, 1)
#define ADES1754_RW_ADDR_MASK			NO_OS_GENMASK(7, 3)

#define ADES1754_LSB_MASK			NO_OS_GENMASK(7, 0)
#define ADES1754_MSB_MASK			NO_OS_GENMASK(15, 8)
#define ADES1754_LOWER_NIBBLE_MASK		NO_OS_GENMASK(3, 0)
#define ADES1754_UPPER_NIBBLE_MASK		NO_OS_GENMASK(7, 4)

#define ADES1754_ADC_METHOD_MASK		NO_OS_BIT(1)
#define ADES1754_SCAN_MODE_MASK			NO_OS_GENMASK(8, 6)
#define ADES1754_SCAN_REQUEST_MASK		NO_OS_BIT(0)
#define ADES1754_IIR_MASK			NO_OS_GENMASK(15, 13)
#define ADES1754_ALRTFILT_MASK			NO_OS_BIT(2)
#define ADES1754_AMENDFILT_MASK			NO_OS_BIT(1)
#define ADES1754_RDFILT_MASK			NO_OS_BIT(0)
#define ADES1754_IIR_SCAN_MASK			NO_OS_GENMASK(11, 9)
#define ADES1754_DBLBUFEN_MASK			NO_OS_BIT(0)
#define ADES1754_CBMODE_MASK			NO_OS_GENMASK(13, 11)
#define ADES1754_CBMEASEN_MASK			NO_OS_GENMASK(1, 0)
#define ADES1754_CBCALDLY_MASK			NO_OS_GENMASK(2, 0)
#define ADES1754_UARTCFG_MASK			NO_OS_GENMASK(15, 14)
#define ADES1754_CELL_POLARITY_MASK		NO_OS_BIT(15)

enum ades1754_id {
	ID_ADES1754,
	ID_ADES1755,
	ID_ADES1756,
	ID_ADES1751,
	ID_ADES1752,
};

enum ades1754_scan_method {
	ADES1754_PYRAMID_METHOD,
	ADES1754_RAMP_METHOD,
};

enum ades1754_scan_mode {
	ADES1754_SCAN_ADC,
	ADES1754_SCAN_COMP,
	ADES1754_SCAN_ADC_COMP,
	ADES1754_ON_DEMAND_CALIB,
	ADES1754_BALANCING_SW_SHRT,
	ADES1754_BALANCING_SW_OPEN,
	ADES1754_CELL_SENSE_OPEN_ODDS,
	ADES1754_CELL_SENSE_OPEN_EVENS
};

enum ades1754_cell_polarity {
	ADES1754_UNIPOLAR,
	ADES1754_BIPOLAR,
};

enum ades1754_iir_filter_coef {
	ADES1754_IIR_0_125,
	ADES1754_IIR_0_250,
	ADES1754_IIR_0_375,
	ADES1754_IIR_0_500,
	ADES1754_IIR_0_625,
	ADES1754_IIR_0_750,
	ADES1754_IIR_0_875,
	ADES1754_IIR_1_000_OFF,
};

enum ades1754_buffer_mode {
	ADES1754_SINGLE_BUFF,
	ADES1754_DOUBLE_BUFF,
};

enum ades1754_alert {
	ADES1754_CELL_OV,
	ADES1754_CELL_UV,
	ADES1754_BIPOLAR_OV,
	ADES1754_BIPOLAR_UV,
	ADES1754_BLOCK_OV,
	ADES1754_BLOCK_UV,
	ADES1754_CELL_MISMATCH,
	ADES1754_AUXIN_OV,
	ADES1754_AUXIN_UV,
};

enum ades1754_bal_mode {
	ADES1754_BAL_DISABLED,
	ADES1754_BAL_EMERGENCY,
	ADES1754_BAL_MANUAL_SEC,
	ADES1754_BAL_MANUAL_MIN,
	ADES1754_BAL_AUTO_SEC,
	ADES1754_BAL_AUTO_MIN,
	ADES1754_BAL_AUTO_GROUP_SEC,
	ADES1754_BAL_AUTO_GROUP_MIN,
};

enum ades1754_bal_meas {
	ADES1754_ALL_MEAS_DISABLED = 1,
	ADES1754_ADC_CAL_ON_CBUVTHR_OFF,
	ADES1754_ALL_MEAS_ENABLED,
};

enum ades1754_bal_calib {
	ADES1754_BAL_CALIB_DISABLED,
	ADES1754_BAL_2_CYCLE,
	ADES1754_BAL_4_CYCLE,
	ADES1754_BAL_8_CYCLE,
	ADES1754_BAL_12_CYCLE,
	ADES1754_BAL_16_CYCLE,
	ADES1754_BAL_24_CYCLE,
};

struct ades1754_init_param {
	struct no_os_uart_init_param *uart_param;
	enum ades1754_id id;
	bool uart_bridge;
	uint8_t dev_addr;
	uint8_t no_dev;
};

struct ades1754_desc {
	struct no_os_uart_desc *uart_desc;
	enum ades1754_id id;
	bool uart_bridge;
	enum ades1754_scan_mode scan_mode;
	enum ades1754_cell_polarity cell_polarity;
	uint8_t dev_addr;
	uint8_t no_dev;
	bool alive;
};

int ades1754_hello_all(struct ades1754_desc *desc);

int ades1754_write_dev(struct ades1754_desc *desc, uint8_t reg, uint16_t data);

int ades1754_write_all(struct ades1754_desc *desc, uint8_t reg, uint16_t data);

int ades1754_read_dev(struct ades1754_desc *desc, uint8_t reg, uint16_t *data);

int ades1754_read_all(struct ades1754_desc *desc, uint8_t reg, uint16_t *data);

int ades1754_read_block(struct ades1754_desc *desc, uint8_t block, uint8_t reg,
			uint16_t *data, bool double_size);

int ades1754_update_dev(struct ades1754_desc *desc, uint8_t reg, uint16_t mask,
			uint16_t val);

int ades1754_set_adc_method(struct ades1754_desc *desc,
			    enum ades1754_scan_method scan_method);

int ades1754_switch_scan_mode(struct ades1754_desc *desc,
			      enum ades1754_scan_mode mode);

int ades1754_set_cell_pol(struct ades1754_desc *desc,
			  enum ades1754_cell_polarity cell_polarity);

int ades1754_start_scan(struct ades1754_desc *desc, bool meas,
			uint16_t cell_mask);

int ades1754_get_cell_data(struct ades1754_desc *desc, uint8_t cell_nb,
			   int32_t *cell_voltage);

int ades1754_set_iir(struct ades1754_desc *desc,
		     enum ades1754_iir_filter_coef coef);

int ades1754_set_iir_ctrl(struct ades1754_desc *desc, bool alrtfilt, bool acc,
			  bool output);

int ades1754_set_buffer_mode(struct ades1754_desc *desc,
			     enum ades1754_buffer_mode mode);

int ades1754_set_alert_thr(struct ades1754_desc *desc,
			   enum ades1754_alert alert,
			   uint16_t thr);

int ades1754_get_alert(struct ades1754_desc *desc, enum ades1754_alert alert,
		       bool *enable);

int ades1754_set_balancing_mode(struct ades1754_desc *desc,
				enum ades1754_bal_mode mode);

int ades1754_set_balancing_meas(struct ades1754_desc *desc,
				enum ades1754_bal_meas meas);

int ades1754_set_balancing_calib(struct ades1754_desc *desc,
				 enum ades1754_bal_calib calib);

int ades1754_init(struct ades1754_desc **desc,
		  struct ades1754_init_param *init_param);

int ades1754_remove(struct ades1754_desc *desc);

#endif /* __ADES1754_H__ */
