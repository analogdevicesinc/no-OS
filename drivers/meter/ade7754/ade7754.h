/***************************************************************************//**
 *   @file   ade7754.h
 *   @brief  Header file of ADE7754 Driver.
 *   @author REtz (radu.etz@analog.com)
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
#ifndef __ADE7754_H__
#define __ADE7754_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/* SPI commands */
#define ADE7754_SPI_READ					NO_OS_BIT(7)

/* Version product */
#define ADE7754_VERSION						0x3F
/* Use a register that has a non-zero value at reset */
#define ADE7754_REG_VERSION_PRODUCT     			0x0D

/* Miscellaneous Definitions */
#define IS_5BITS_REG(x) ((x) >= ADE7754_REG_APHCAL && (x) <= ADE7754_REG_CPHCAL)
#define IS_8BITS_REG(x) (((x) == ADE7754_REG_TEMP) \
		|| ((x) >= ADE7754_REG_OPMODE && (x) <= ADE7754_REG_VAMODE) \
        	|| ((x) >= ADE7754_REG_SAGCYC && (x) <= ADE7754_REG_GAIN) \
		|| ((x) >= ADE7754_REG_WDIV && (x) <= ADE7754_REG_VADIV) \
		||((x) >= ADE7754_REG_CHKSUM && (x) <= ADE7754_REG_VERSION))
#define IS_12BITS_REG(x) (((x) >= ADE7754_REG_AWG && (x) <= ADE7754_REG_CVAG) \
		|| ((x) >= ADE7754_REG_AAPOS && (x) <= ADE7754_REG_CFDEN) \
		|| ((x) >= ADE7754_REG_AIRMSOS && (x) <= ADE7754_REG_CVGAIN))
#define IS_15BITS_REG(x) ((x) == ADE7754_REG_PERIOD)
#define IS_16BITS_REG(x) ((x) >= ADE7754_REG_IRQEN && (x) <= ADE7754_REG_LINCYC)
#define IS_24BITS_REG(x) (((x) >= ADE7754_REG_AENERGY && (x) <= ADE7754_REG_LVAENERGY) \
		|| ((x) == ADE7754_REG_WFORM) \
		|| ((x) >= ADE7754_REG_AIRMS && (x) <= ADE7754_REG_CVRMS))

/* Delay after reset */
/* A data transfer to the ADE7754 should
not take place for at least 18 µs
after a software reset. */
#define ADE7754_RESET_DEL					20

/* ADE7754 Register Map */
#define ADE7754_REG_AENERGY					0x01
#define ADE7754_REG_RAENERGY					0x02
#define ADE7754_REG_LAENERGY					0x03
#define ADE7754_REG_VAENERGY					0x04
#define ADE7754_REG_RVAENERGY					0x05
#define ADE7754_REG_LVAENERGY					0x06
#define ADE7754_REG_PERIOD					0x07
#define ADE7754_REG_TEMP					0x08
#define ADE7754_REG_WFORM              				0x09
#define ADE7754_REG_OPMODE					0x0A
#define ADE7754_REG_MMODE					0x0B
#define ADE7754_REG_WAVMODE					0x0C
#define ADE7754_REG_WATMODE					0x0D
#define ADE7754_REG_VAMODE					0X0E
#define ADE7754_REG_IRQEN					0x0F
#define ADE7754_REG_STATUS					0x10
#define ADE7754_REG_RSTATUS					0x11
#define ADE7754_REG_ZXTOUT					0x12
#define ADE7754_REG_LINCYC					0x13
#define ADE7754_REG_SAGCYC					0x14
#define ADE7754_REG_SAGLVL					0x15
#define ADE7754_REG_VPEAK					0x16
#define ADE7754_REG_IPEAK					0x17
#define ADE7754_REG_GAIN					0x18
#define ADE7754_REG_AWG						0x19
#define ADE7754_REG_BWG						0x1A
#define ADE7754_REG_CWG						0X1B
#define ADE7754_REG_AVAG					0x1C
#define ADE7754_REG_BVAG					0x1D
#define ADE7754_REG_CVAG					0x1E
#define ADE7754_REG_APHCAL					0x1F
#define ADE7754_REG_BPHCAL					0x20
#define ADE7754_REG_CPHCAL					0x21
#define ADE7754_REG_AAPOS					0x22
#define ADE7754_REG_BAPOS					0x23
#define ADE7754_REG_CAPOS					0x24
#define ADE7754_REG_CFNUM					0x25
#define ADE7754_REG_CFDEN					0x26
#define ADE7754_REG_WDIV					0x27
#define ADE7754_REG_VADIV					0x28
#define ADE7754_REG_AIRMS					0x29
#define ADE7754_REG_BIRMS					0x2A
#define ADE7754_REG_CIRMS					0x2B
#define ADE7754_REG_AVRMS					0x2C
#define ADE7754_REG_BVRMS					0x2D
#define ADE7754_REG_CVRMS					0x2E
#define ADE7754_REG_AIRMSOS					0x2F
#define ADE7754_REG_BIRMSOS					0x30
#define ADE7754_REG_CIRMSOS					0x31
#define ADE7754_REG_AVRMSOS					0x32
#define ADE7754_REG_BVRMSOS					0x33
#define ADE7754_REG_CVRMSOS					0x34
#define ADE7754_REG_AAPGAIN					0x35
#define ADE7754_REG_BAPGAIN					0x36
#define ADE7754_REG_CAPGAIN					0x37
#define ADE7754_REG_AVGAIN					0x38
#define ADE7754_REG_BVGAIN					0x39
#define ADE7754_REG_CVGAIN					0x3A
#define ADE7754_REG_CHKSUM					0x3E
#define ADE7754_REG_VERSION					0x3F

/* ADE7754_REG_OPMODE Bit Definition */
#define ADE7754_SWRST_MSK					NO_OS_BIT(6)
#define ADE7754_DISMOD_MSK					NO_OS_GENMASK(5, 3)
#define ADE7754_DISCF_MSK					NO_OS_BIT(2)
#define ADE7754_DISLPF_MSK					NO_OS_BIT(1)
#define ADE7754_DISHPF_MSK					NO_OS_BIT(0)

/* ADE7754_REG_GAIN Bit Definition */
#define ADE7754_PGA2_MSK					NO_OS_GENMASK(6, 5)
#define ADE7754_NO_LOAD_MSK					NO_OS_BIT(3)
#define ADE7754_ABS_MSK						NO_OS_BIT(2)
#define ADE7754_PGA1_MSK					NO_OS_GENMASK(1, 0)

/* ADE7754_REG_CFNUM Bit Definition */
#define ADE7754_NEGC_MSK					NO_OS_BIT(14)
#define ADE7754_NEGB_MSK					NO_OS_BIT(13)
#define ADE7754_NEGA_MSK					NO_OS_BIT(12)
#define ADE7754_CFNUM_MSK					NO_OS_GENMASK(11, 0)

/* ADE7754_REG_MMODE Bit Definition */
#define ADE7754_ZXSEL_MSK					NO_OS_GENMASK(6, 4)
#define ADE7754_PEAKSEL_MSK					NO_OS_GENMASK(3, 2)
#define ADE7754_PERDSEL_MSK					NO_OS_GENMASK(1, 0)

/* ADE7754_REG_WAVMODE Bit Definition */
#define ADE7754_LVARSEL_MSK					NO_OS_BIT(5)
#define ADE7754_DTRT_MSK					NO_OS_GENMASK(4, 3)
#define ADE7754_WAVSEL_MSK					NO_OS_GENMASK(2, 0)

/* ADE7754_REG_WATMODE Bit Definition */
#define ADE7754_WATM_MSK					NO_OS_GENMASK(7, 6)
#define ADE7754_WATSEL_MSK					NO_OS_GENMASK(5, 3)
#define ADE7754_LWATSEL_MSK					NO_OS_GENMASK(2, 0)

/* ADE7754_REG_VAMODE Bit Definition */
#define ADE7754_VAMOD_MSK					NO_OS_GENMASK(7, 6)
#define ADE7754_VASEL_MSK					NO_OS_GENMASK(5, 3)
#define ADE7754_LVASEL_MSK					NO_OS_GENMASK(2, 0)

/* ADE7754_REG_IRQEN/ADE7754_REG_RSTATUS/ADE7754_REG_STATUS Bit Definition */
#define ADE7754_VAEHF_MSK					NO_OS_BIT(15)
#define ADE7754_WFSM_MSK					NO_OS_BIT(14)
#define ADE7754_PKI_MSK						NO_OS_BIT(13)
#define ADE7754_PKV_MSK						NO_OS_BIT(12)
/* bit 11 reserved in the IRQEN register*/
#define ADE7754_RESET_MSK					NO_OS_BIT(11)
#define ADE7754_LENERGY_MSK					NO_OS_BIT(10)
#define ADE7754_ZXC_MSK						NO_OS_BIT(9)
#define ADE7754_ZXB_MSK						NO_OS_BIT(8)
#define ADE7754_ZXA_MSK						NO_OS_BIT(7)
#define ADE7754_ZXTOC_MSK					NO_OS_BIT(6)
#define ADE7754_ZXTOB_MSK					NO_OS_BIT(5)
#define ADE7754_ZXTOA_MSK					NO_OS_BIT(4)
#define ADE7754_SAGC_MSK					NO_OS_BIT(3)
#define ADE7754_SAGB_MSK					NO_OS_BIT(2)
#define ADE7754_SAGA_MSK					NO_OS_BIT(1)
#define ADE7754_AEHF_MSK					NO_OS_BIT(0)

/**
 * @enum ade7754_dismod
 * @brief ADE7754 ADCs can be turned off by using these bits.
 */
enum ade7754_dismod {
	/* normal operation */
	ADE7754_NORMAL_OPERATION,
	/* switch off the current channel ADCs*/
	ADE7754_SWITCH_OFF_CURRENT_ADC,
	/* switch off the voltage channel ADCs */
	ADE7754_SWITCH_OFF_VOLTAGE_ADC,
	/* put the ADE7754 in sleep mode */
	ADE7754_SLEEP_MODE,
	/* swap voltage and currents channels */
	ADE7754_SWAP_VOLTAGE_CURRENT,
	/* switch off current channel ADCsand redirect the current
	input signal to the voltage channel signal paths*/
	ADE7754_OFF_CURRENT_ADCS_REDIRECTED_TO_VOLTAGE,
	/* switch off voltage channel ADCs and redirect the voltage
	input signal to the current channel signal paths */
	ADE7754_OFF_VOLTAGE_ADCS_REDIRECTED_TO_CURRENT,
	/* put the ADE7754 in power-down mode*/
	ADE7754_POWER_DOWN_MODE
};

/**
 * @enum ade7754_pga_gain
 * @brief ADE7754 select the PGA gain for current and voltage ch inputs.
 */
enum ade7754_pga_gain {
	/* Gain */
	/* gain 1 */
	ADE7754_PGA_GAIN_1,
	/* gain 2 */
	ADE7754_PGA_GAIN_2,
	/* gain 4 */
	ADE7754_PGA_GAIN_4
};

/**
 * @enum ade7754_phsel
 * @brief ADE7754 select the phase of peak detection or
 * the source for the voltage line period measurement.
 */
enum ade7754_phsel {
	/* phase A */
	ADE7754_PHASE_A,
	/* phase B */
	ADE7754_PHASE_B,
	/* phase C */
	ADE7754_PHASE_C
};

/**
 * @enum ade7754_formsel
 * @brief ADE7754 select each part of the formula separately
 * depending on the line active energy measurement method.
 */
enum ade7754_formsel {
	/* part 1 */
	ADE7754_FORMULA_PART_1,
	/* part 2 */
	ADE7754_FORMULA_PART_2,
	/* part 3 */
	ADE7754_FORMULA_PART_3
};

/**
 * @enum ade7754_wavsel
 * @brief ADE7754 select the source of the waveform sample.
 */
enum ade7754_wavsel {
	/* Voltage phase A */
	ADE7754_VOLTAGE_PHASE_A,
	/* Voltage phase B */
	ADE7754_VOLTAGE_PHASE_B,
	/* Voltage phase C */
	ADE7754_VOLTAGE_PHASE_C,
	/* Current phase A */
	ADE7754_CURRENT_PHASE_A,
	/* Current phase B */
	ADE7754_CURRENT_PHASE_B,
	/* Current phase C */
	ADE7754_CURRENT_PHASE_C
};

/**
 * @enum ade7754_data_rate
 * @brief ADE7754 select the data rate.
 */
enum ade7754_data_rate {
	/* 26 kSPS (CLKIN/3/128) */
	ADE7754_DATA_RATE_26_KSPS,
	/* 13 kSPS (CLKIN/3/256) */
	ADE7754_DATA_RATE_13_KSPS,
	/* 6.5 kSPS (CLKIN/3/512) */
	ADE7754_DATA_RATE_6_5KSPS,
	/* 3.3 kSPS (CLKIN/3/1024) */
	ADE7754_DATA_RATE_3_KSPS
};

/**
 * @enum ade7754_watm
 * @brief ADE7754 select the formula used for active energy calculation.
 */
enum ade7754_watm {
	/* Active energy calculation */
	/* formula 1 VA * IA + VB * IB + VC * IC */
	ADE7754_ACTIVE_E_FORMULA_1,
	/* formula 2 VA * (IA – IB) + 0 + VC * (IC – IB) */
	ADE7754_ACTIVE_E_FORMULA_2,
	/* formula 3 VA * (IA - IB) + 0 + VC * IC */
	ADE7754_ACTIVE_E_FORMULA_3
};

/**
 * @enum ade7754_vamod
 * @brief ADE7754 select the formula used for apparent energy calculation.
 * */
enum ade7754_vamod {
	/* Apparent energy calculation */
	/* formula 1 VArms * IArms + VBrms * IBrms + VCrms * ICrms */
	ADE7754_APP_E_FORMULA_1,
	/* formula 2 VArms * IArms + (VArms + VCrms)/2 * IBrms + VCrms * ICrms */
	ADE7754_APP_E_FORMULA_2,
	/* formula 3  VArms * IArms + VArms * IBrms + VCrms * ICrms */
	ADE7754_APP_E_FORMULA_3
};

/**
* @struct ade7754_init_param
* @brief ADE7754 Device initialization parameters.
*/
struct ade7754_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_init_param  	*gpio_reset;
	/** IRQ device descriptor used to handle interrupt routine for IRQ */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/**
* @struct ade7754_dev
* @brief ADE7754 Device structure.
*/
struct ade7754_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_desc  	*gpio_reset;
	/** IRQ status value */
	uint32_t 			irq_status;
	/** IRQ device descriptor used to handle interrupt routine for IRQN */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/**
* @struct ade7754_energy_values
* @brief ADE7754 energy registers values
*/
struct ade7754_energy_values {
	/** Active energy register value */
	int32_t active_energy_reg_val;
	/** Apparent energy register value */
	int32_t apparent_energy_reg_val;
};

/**
 * @struct ade7754_rms_values
 * @brief ADE7754 rms registers values
 */
struct ade7754_rms_values {
	/** Current rms register value phase A */
	int32_t current_rms_reg_val_phase_a;
	/** Voltage rms register value phase A */
	int32_t voltage_rms_reg_val_phase_a;
	/** Current rms register value phase B */
	int32_t current_rms_reg_val_phase_b;
	/** Voltage rms register value phase B */
	int32_t voltage_rms_reg_val_phase_b;
	/** Current rms register value phase C */
	int32_t current_rms_reg_val_phase_c;
	/** Voltage rms register value phase C */
	int32_t voltage_rms_reg_val_phase_c;
};

/**
 * @struct ade7754_period_value
 * @brief ADE7754 period value
 */
struct ade7754_period_value {
	/** Frequency/Period register value */
	int32_t per_reg_val;
};

/**
 * @struct ade7754_temp_value
 * @brief ADE7754 pq values
 */
struct ade7754_temp_value {
	/** Temperature register value */
	int32_t temp_reg_val;
};

/* Initialize the device. */
int ade7754_init(struct ade7754_dev **device,
		 struct ade7754_init_param init_param);

/* Setup device */
int ade7754_setup(struct ade7754_dev *dev);

/* Read device register. */
int ade7754_read(struct ade7754_dev *dev, uint16_t reg_addr,
		 int32_t *reg_data);

/* Write device register. */
int ade7754_write(struct ade7754_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data);

/* Update specific register bits. */
int ade7754_update_bits(struct ade7754_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data);

/* Remove the device and release resources. */
int ade7754_remove(struct ade7754_dev *dev);

/* Reset the device using SW reset. */
int ade7754_sw_reset(struct ade7754_dev *dev);

/* Reset the device using HW reset. */
int ade7754_hw_reset(struct ade7754_dev *dev);

/* Version product */
int ade7754_version_product(struct ade7754_dev *dev,
			    uint32_t *data_read);

/* Write Ipeak val */
int ade7754_write_ipk_val(struct ade7754_dev *dev, uint32_t val);

/* Write Vpeak val */
int ade7754_write_vpk_val(struct ade7754_dev *dev, uint32_t val);

/* Get interrupt indicator from STATUS register */
int ade7754_get_int_status(struct ade7754_dev *dev, uint32_t msk,
			   uint8_t *status);

/* Clear irq STATUS flags */
int ade7754_clear_irq_status(struct ade7754_dev *dev,
			     int32_t *reg_data);

/* Enable/Disable interrupt */
int ade7754_enable_irq(struct ade7754_dev *dev,
		       uint32_t msk, uint8_t en);

/* Select waveform register update rate */
int ade7754_wave_update_rate(struct ade7754_dev *dev,
			     enum ade7754_data_rate sel);

/* Select source of sampled data for wave register */
int ade7754_wave_sample_data_source(struct ade7754_dev *dev,
				    enum ade7754_wavsel sel);

/* Select the gain of the current channels inputs. */
int ade7754_current_gain(struct ade7754_dev *dev,
			 enum ade7754_pga_gain sel);

/* Select the gain of the voltage channels inputs. */
int ade7754_voltage_gain(struct ade7754_dev *dev,
			 enum ade7754_pga_gain sel);

/* Select the formula used for active energy calculation */
int ade7754_active_energy_formula(struct ade7754_dev *dev,
				  enum ade7754_watm sel);

/* Select the formula used for apparent energy calculation */
int ade7754_apparent_energy_formula(struct ade7754_dev *dev,
				    enum ade7754_vamod sel);

/* Select the source of the measurement of the voltage line period. */
int ade7754_period_source(struct ade7754_dev *dev,
			  enum ade7754_phsel sel);

/* Select the phases used for zx, period, sag detection */
int ade7754_zxsel(struct ade7754_dev *dev,
		  enum ade7754_phsel sel);

/* Select part of the line active energy formula separately,
depending on the line active energy measurement method. */
int ade7754_lwat_formula_term(struct ade7754_dev *dev,
			      enum ade7754_formsel sel);

/* Select part of the line apparent energy formula separately,
depending on the line active energy measurement method. */
int ade7754_lva_formula_term(struct ade7754_dev *dev,
			     enum ade7754_formsel sel);

/* Select part of the active energy formula separately,
depending on the line active energy measurement method. */
int ade7754_wat_formula_term(struct ade7754_dev *dev,
			     enum ade7754_formsel sel);

/* Select part of the apparent energy formula separately,
depending on the line active energy measurement method. */
int ade7754_va_formula_term(struct ade7754_dev *dev,
			    enum ade7754_formsel sel);

/* Select the source of the voltage & current peak detection. */
int ade7754_peak_source(struct ade7754_dev *dev,
			enum ade7754_phsel sel);

/* Operation mode of the ADCs. */
int ade7754_adcs_mode(struct ade7754_dev *dev,
		      enum ade7754_dismod sel);

/* Read energy values */
int ade7754_energy_vals(struct ade7754_dev *dev,
			struct ade7754_energy_values *data);

/* Read rms values phase A */
int ade7754_rms_vals_phase_a(struct ade7754_dev *dev,
			     struct ade7754_rms_values *data);

/* Read rms values phase B */
int ade7754_rms_vals_phase_b(struct ade7754_dev *dev,
			     struct ade7754_rms_values *data);

/* Read rms values phase C */
int ade7754_rms_vals_phase_c(struct ade7754_dev *dev,
			     struct ade7754_rms_values *data);

/* Read period value */
int ade7754_period_val(struct ade7754_dev *dev,
		       struct ade7754_period_value *data);

/* Read temperature value */
int ade7754_temperature_val(struct ade7754_dev *dev,
			    struct ade7754_temp_value *data);

#endif /* __ADE7754_H__ */
