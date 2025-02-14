/***************************************************************************//**
 *   @file   ade7758.h
 *   @brief  Header file of ADE7758 Driver.
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
#ifndef __ADE7758_H__
#define __ADE7758_H__

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
#define ADE7758_SPI_READ					NO_OS_BIT(7)

/* Version product */
#define ADE7758_VERSION						0x3F
/* Use a register that has a non-zero value at reset */
#define ADE7758_REG_VERSION_PRODUCT     			0x48

/* Miscellaneous Definitions */
#define IS_7BITS_REG(x) ((x) >= ADE7758_REG_APHCAL && (x) <= ADE7758_REG_CPHCAL)
#define IS_8BITS_REG(x) (((x) == ADE7758_REG_TEMP) || ((x) >= ADE7758_REG_OPMODE && (x) <= ADE7758_REG_LCYCMODE) \
        	|| ((x) >= ADE7758_REG_SAGCYC && (x) <= ADE7758_REG_GAIN) || ((x) >= ADE7758_REG_WDIV && (x) <= ADE7758_REG_VADIV) \
		||((x) >= ADE7758_REG_CHKSUM && (x) <= ADE7758_REG_VERSION))
#define IS_12BITS_REG(x) (((x) == ADE7758_REG_FREQ) || ((x) >= ADE7758_REG_AVRMSGAIN && (x) <= ADE7758_REG_CVAROS) \
		|| ((x) == ADE7758_REG_APCFDEN) || ((x) == ADE7758_REG_VARCFDEN))
#define IS_16BITS_REG(x) (((x) >= ADE7758_REG_AWATTHR && (x) <= ADE7758_REG_CVAHR) \
		|| ((x) >= ADE7758_REG_ZXTOUT && (x) <= ADE7758_REG_LINECYC) || ((x) == ADE7758_REG_APCFNUM) \
		|| ((x) == ADE7758_REG_VARCFNUM))
#define IS_24BITS_REG(x) (((x) >= ADE7758_REG_AIRMS && (x) <= ADE7758_REG_CVRMS) \
		|| ((x) == ADE7758_REG_WFORM) || ((x) >= ADE7758_REG_MASK && (x) <= ADE7758_REG_RSTATUS))

/* Reset Delay */
#define ADE7758_RESET_DEL					1

/* ADE7758 Register Map */
#define ADE7758_REG_AWATTHR					0x01
#define ADE7758_REG_BWATTHR					0x02
#define ADE7758_REG_CWATTHR					0x03
#define ADE7758_REG_AVARHR					0x04
#define ADE7758_REG_BVARHR					0x05
#define ADE7758_REG_CVARHR					0x06
#define ADE7758_REG_AVAHR					0x07
#define ADE7758_REG_BVAHR					0x08
#define ADE7758_REG_CVAHR              				0x09
#define ADE7758_REG_AIRMS					0x0A
#define ADE7758_REG_BIRMS					0x0B
#define ADE7758_REG_CIRMS					0x0C
#define ADE7758_REG_AVRMS					0x0D
#define ADE7758_REG_BVRMS					0X0E
#define ADE7758_REG_CVRMS					0x0F
#define ADE7758_REG_FREQ					0x10
#define ADE7758_REG_TEMP					0x11
#define ADE7758_REG_WFORM					0x12
#define ADE7758_REG_OPMODE					0x13
#define ADE7758_REG_MMODE					0x14
#define ADE7758_REG_WAVMODE					0x15
#define ADE7758_REG_COMPMODE					0x16
#define ADE7758_REG_LCYCMODE					0x17
#define ADE7758_REG_MASK					0x18
#define ADE7758_REG_STATUS					0x19
#define ADE7758_REG_RSTATUS					0x1A
#define ADE7758_REG_ZXTOUT					0X1B
#define ADE7758_REG_LINECYC					0x1C
#define ADE7758_REG_SAGCYC					0x1D
#define ADE7758_REG_SAGLVL					0x1E
#define ADE7758_REG_VPINTLVL					0x1F
#define ADE7758_REG_IPINTLVL					0x20
#define ADE7758_REG_VPEAK					0x21
#define ADE7758_REG_IPEAK					0x22
#define ADE7758_REG_GAIN					0x23
#define ADE7758_REG_AVRMSGAIN					0x24
#define ADE7758_REG_BVRMSGAIN					0x25
#define ADE7758_REG_CVRMSGAIN					0x26
#define ADE7758_REG_AIGAIN					0x27
#define ADE7758_REG_BIGAIN					0x28
#define ADE7758_REG_CIGAIN					0x29
#define ADE7758_REG_AWG						0x2A
#define ADE7758_REG_BWG						0x2B
#define ADE7758_REG_CWG						0x2C
#define ADE7758_REG_AVARG					0x2D
#define ADE7758_REG_BVARG					0x2E
#define ADE7758_REG_CVARG					0x2F
#define ADE7758_REG_AVAG					0x30
#define ADE7758_REG_BVAG					0x31
#define ADE7758_REG_CVAG					0x32
#define ADE7758_REG_AVRMSOS					0x33
#define ADE7758_REG_BVRMSOS					0x34
#define ADE7758_REG_CVRMSOS					0x35
#define ADE7758_REG_AIRMSOS					0x36
#define ADE7758_REG_BIRMSOS					0x37
#define ADE7758_REG_CIRMSOS					0x38
#define ADE7758_REG_AWATTOS					0x39
#define ADE7758_REG_BWATTOS					0x3A
#define ADE7758_REG_CWATTOS					0x3B
#define ADE7758_REG_AVAROS					0x3C
#define ADE7758_REG_BVAROS					0x3D
#define ADE7758_REG_CVAROS					0x3E
#define ADE7758_REG_APHCAL					0x3F
#define ADE7758_REG_BPHCAL					0x40
#define ADE7758_REG_CPHCAL					0x41
#define ADE7758_REG_WDIV					0x42
#define ADE7758_REG_VARDIV					0x43
#define ADE7758_REG_VADIV					0x44
#define ADE7758_REG_APCFNUM					0x45
#define ADE7758_REG_APCFDEN					0x46
#define ADE7758_REG_VARCFNUM					0x47
#define ADE7758_REG_VARCFDEN					0x48
#define ADE7758_REG_CHKSUM					0x7E
#define ADE7758_REG_VERSION					0x7F

/* ADE7758_REG_OPMODE Bit Definition */
#define ADE7758_SWRST_MSK					NO_OS_BIT(6)
#define ADE7758_DISMOD_MSK					NO_OS_GENMASK(5, 3)
#define ADE7758_DISCF_MSK					NO_OS_BIT(2)
#define ADE7758_DISLPF_MSK					NO_OS_BIT(1)
#define ADE7758_DISHPF_MSK					NO_OS_BIT(0)

/* ADE7758_REG_MMODE Bit Definition */
#define ADE7758_PKIRQSEL_MSK					NO_OS_GENMASK(7, 5)
#define ADE7758_PEAKSEL_MSK					NO_OS_GENMASK(4, 2)
#define ADE7758_FREQSEL_MSK					NO_OS_GENMASK(1, 0)

/* ADE7758_REG_WAVMODE Bit Definition */
#define ADE7758_VACF_MSK					NO_OS_BIT(7)
#define ADE7758_DTRT_MSK					NO_OS_GENMASK(6, 5)
#define ADE7758_WAVSEL_MSK					NO_OS_GENMASK(4, 2)
#define ADE7758_PHSEL_MSK					NO_OS_GENMASK(1, 0)

/* ADE7758_REG_COMPMODE Bit Definition */
#define ADE7758_NOLOAD_MSK					NO_OS_BIT(7)
#define ADE7758_SAVAR_MSK					NO_OS_BIT(6)
#define ADE7758_ABS_MSK						NO_OS_BIT(5)
#define ADE7758_TERMSEL_MSK					NO_OS_GENMASK(4, 2)
#define ADE7758_CONSEL_MSK					NO_OS_GENMASK(1, 0)

/* ADE7758_REG_LCYCMODE Bit Definition */
#define ADE7758_FREQSEL_MSK					NO_OS_BIT(7)
#define ADE7758_RSTREAD_MSK					NO_OS_BIT(6)
#define ADE7758_ZXSEL_MSK					NO_OS_GENMASK(5, 3)
#define ADE7758_LVA_MSK						NO_OS_BIT(2)
#define ADE7758_LVAR_MSK					NO_OS_BIT(1)
#define ADE7758_LWATT_MSK					NO_OS_BIT(0)

/* ADE7758_REG_STATUS/ADE7758_REG_RSTATUS/ADE7758_REG_MASK Bit Definition */
#define ADE7758_SEQERR_MSK					NO_OS_BIT(19)
#define ADE7758_REVPRP_MSK					NO_OS_BIT(18)
#define ADE7758_REVPAP_MSK					NO_OS_BIT(17)
#define ADE7758_WFSM_MSK					NO_OS_BIT(16)
#define ADE7758_PKI_MSK						NO_OS_BIT(15)
#define ADE7758_PKV_MSK						NO_OS_BIT(14)
/* bit 13 reserved in the mask register*/
#define ADE7758_RESET_MSK					NO_OS_BIT(13)
#define ADE7758_LENERGY_MSK					NO_OS_BIT(12)
#define ADE7758_ZXC_MSK						NO_OS_BIT(11)
#define ADE7758_ZXB_MSK						NO_OS_BIT(10)
#define ADE7758_ZXA_MSK						NO_OS_BIT(9)
#define ADE7758_ZXTOC_MSK					NO_OS_BIT(8)
#define ADE7758_ZXTOB_MSK					NO_OS_BIT(7)
#define ADE7758_ZXTOA_MSK					NO_OS_BIT(6)
#define ADE7758_SAGC_MSK					NO_OS_BIT(5)
#define ADE7758_SAGB_MSK					NO_OS_BIT(4)
#define ADE7758_SAGA_MSK					NO_OS_BIT(3)
#define ADE7758_VAEHF_MSK					NO_OS_BIT(2)
#define ADE7758_REHF_MSK					NO_OS_BIT(1)
#define ADE7758_AEHF_MSK					NO_OS_BIT(0)

/* ADE7758_REG_GAIN Bit Definition */
#define ADE7758_INTEGRATOR_EN_MSK				NO_OS_BIT(7)
#define ADE7758_PGA2_GAIN_MSK					NO_OS_GENMASK(6, 5)
#define ADE7758_FULL_SCALE_MSK					NO_OS_GENMASK(4, 3)
#define ADE7758_PGA1_GAIN_MSK					NO_OS_GENMASK(1, 0)

/**
 * @enum ade7758_dismod
 * @brief ADE7758 ADCs can be turned off by using these bits.
 */
enum ade7758_dismod {
	/* normal operation */
	ADE7758_NORMAL_OPERATION,
	/* switch off the current channel ADCs*/
	ADE7758_SWITCH_OFF_CURRENT_ADC,
	/* switch off the voltage channel ADCs */
	ADE7758_SWITCH_OFF_VOLTAGE_ADC,
	/* put the ADE7758 in sleep mode */
	ADE7758_SLEEP_MODE,
	/* swap voltage and currents channels */
	ADE7758_SWAP_VOLTAGE_CURRENT,
	/* switch off current channel ADCsand redirect the current
	input signal to the voltage channel signal paths*/
	ADE7758_OFF_CURRENT_ADCS_REDIRECTED_TO_VOLTAGE,
	/* switch off voltage channel ADCs and redirect the voltage
	input signal to the current channel signal paths */
	ADE7758_OFF_VOLTAGE_ADCS_REDIRECTED_TO_CURRENT,
	/* put the ADE7758 in power-down mode*/
	ADE7758_POWER_DOWN_MODE
};

/**
 * @enum ade7758_phsel
 * @brief ADE7758 select the phase of the waveform sample or
 * the source for the voltage line frequency measurement.
 */
enum ade7758_phsel {
	/* phase A */
	ADE7758_PHASE_A,
	/* phase B */
	ADE7758_PHASE_B,
	/* phase C */
	ADE7758_PHASE_C
};

/**
 * @enum ade7758_wavesel
 * @brief ADE7758 select the type of waveform.
 */
enum ade7758_wavesel {
	/* Current */
	ADE7758_CURRENT,
	/* Voltage */
	ADE7758_VOLTAGE,
	/* Active power */
	ADE7758_ACTIVE_POWER,
	/* Reactive power */
	ADE7758_REACTIVE_POWER,
	/* Apparent power */
	ADE7758_APPARENT_POWER
};

/**
 * @enum ade7758_data_rate
 * @brief ADE7758 select the data rate.
 */
enum ade7758_data_rate {
	/* 26.04 kSPS (CLKIN/3/128) */
	ADE7758_DATA_RATE_26_04KSPS,
	/* 13.02 kSPS (CLKIN/3/256) */
	ADE7758_DATA_RATE_13_02KSPS,
	/* 6.51 kSPS (CLKIN/3/512) */
	ADE7758_DATA_RATE_6_51KSPS,
	/* 3.25 kSPS (CLKIN/3/1024) */
	ADE7758_DATA_RATE_3_25KSPS
};

/**
 * @enum ade7758_consel
 * @brief ADE7758 select the input to the energy accumulation registers.
 */
enum ade7758_consel {
	/* Consel 0 */
	ADE7758_CONSEL_0,
	/* Consel 1 */
	ADE7758_CONSEL_1,
	/* Consel 2 */
	ADE7758_CONSEL_2
};

/**
 * @enum ade7758_pga_gain
 * @brief ADE7758 select the PGA gain.
 */
enum ade7758_pga_gain {
	/* Gain */
	/* gain 1 */
	ADE7758_PGA_GAIN_1,
	/* gain 2 */
	ADE7758_PGA_GAIN_2,
};

/**
 * @enum ade7758_ch_full_scale_sel
 * @brief ADE7758 Current input full-scale select.
 */
enum ade7758_ch_full_scale_sel {
	/* Full scale select */
	/* 0.5V */
	ADE7758_FS_0_5,
	/* 0.25V */
	ADE7758_FS_0_25,
	/* 0.125V */
	ADE7758_FS_0_125
};

/**
* @struct ade7758_init_param
* @brief ADE7758 Device initialization parameters.
*/
struct ade7758_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/** IRQ device descriptor used to handle interrupt routine for IRQ */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/**
* @struct ade7758_dev
* @brief ADE7758 Device structure.
*/
struct ade7758_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** IRQ status value */
	uint32_t irq_status;
	/** IRQ device descriptor used to handle interrupt routine for IRQN */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/**
* @struct ade7758_energy_values
* @brief ADE7758 energy registers values
*/
struct ade7758_energy_values {
	/** Active energy register value phase a */
	int32_t active_energy_reg_val_phase_a;
	/** Active energy register value phase b */
	int32_t active_energy_reg_val_phase_b;
	/** Active energy register value phase c */
	int32_t active_energy_reg_val_phase_c;
	/** Accumulated reactive energy register value phase a */
	int32_t acc_reactive_energy_reg_val_phase_a;
	/** Accumulated reactive energy register value phase b */
	int32_t acc_reactive_energy_reg_val_phase_b;
	/** Accumulated reactive energy register value phase c */
	int32_t acc_reactive_energy_reg_val_phase_c;
	/** Apparent energy register value phase a */
	int32_t apparent_energy_reg_val_phase_a;
	/** Apparent energy register value phase b */
	int32_t apparent_energy_reg_val_phase_b;
	/** Apparent energy register value phase c */
	int32_t apparent_energy_reg_val_phase_c;
};

/**
 * @struct ade7758_rms_values
 * @brief ADE7758 rms registers values
 */
struct ade7758_rms_values {
	/** Current rms register value phase A */
	uint32_t current_rms_reg_val_phase_a;
	/** Voltage rms register value phase A */
	uint32_t voltage_rms_reg_val_phase_a;
	/** Current rms register value phase B */
	uint32_t current_rms_reg_val_phase_b;
	/** Voltage rms register value phase B */
	uint32_t voltage_rms_reg_val_phase_b;
	/** Current rms register value phase C */
	uint32_t current_rms_reg_val_phase_c;
	/** Voltage rms register value phase C */
	uint32_t voltage_rms_reg_val_phase_c;
};

/**
 * @struct ade7758_freq_per_value
 * @brief ADE7758 pq values
 */
struct ade7758_freq_per_value {
	/** Frequency/Period register value */
	uint32_t freq_per_reg_val;
};

/**
 * @struct ade7758_temp_value
 * @brief ADE7758 pq values
 */
struct ade7758_temp_value {
	/** Temperature register value */
	uint32_t temp_reg_val;
};

/* Initialize the device. */
int ade7758_init(struct ade7758_dev **device,
		 struct ade7758_init_param init_param);

/* Setup device */
int ade7758_setup(struct ade7758_dev *dev);

/* Read device register. */
int ade7758_read(struct ade7758_dev *dev, uint16_t reg_addr,
		 int32_t *reg_data);

/* Write device register. */
int ade7758_write(struct ade7758_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data);

/* Update specific register bits. */
int ade7758_update_bits(struct ade7758_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data);

/* Remove the device and release resources. */
int ade7758_remove(struct ade7758_dev *dev);

/* Reset the device using SW reset. */
int ade7758_sw_reset(struct ade7758_dev *dev);

/* Version product */
int ade7758_version_product(struct ade7758_dev *dev, uint32_t *data_read);

/* Read Ipeak val */
int ade7758_read_ipk_val(struct ade7758_dev *dev, uint32_t *val);

/* Read Vpeak val */
int ade7758_vpk_val(struct ade7758_dev *dev, uint32_t *val);

/* Get interrupt indicator from STATUS register */
int ade7758_get_int_status(struct ade7758_dev *dev, uint32_t msk,
			   uint8_t *status);

/* Clear irq STATUS flags */
int ade7758_clear_irq_status(struct ade7758_dev *dev, int32_t *reg_data);

/* Enable/Disable interrupt */
int ade7758_enable_irq(struct ade7758_dev *dev, uint32_t msk, uint8_t en);

/* Select waveform register update rate */
int ade7758_wave_update_rate(struct ade7758_dev *dev,
			     enum ade7758_data_rate sel);

/* Select source of sampled data for wave register */
int ade7758_wave_sample_data_source(struct ade7758_dev *dev,
				    enum ade7758_wavesel sel);

/* Select the phase of sampled data for wave register */
int ade7758_wave_phase_sel(struct ade7758_dev *dev,
			   enum ade7758_phsel sel);

/* Select the input to the energy accumulation registers */
int ade7758_consel(struct ade7758_dev *dev,
		   enum ade7758_consel sel);

/* Select the source of the measurement of the voltage line frequency */
int ade7758_freq_source(struct ade7758_dev *dev,
			enum ade7758_phsel sel);

/* Operation mode of the ADCs */
int ade7758_adcs_mode(struct ade7758_dev *dev,
		      enum ade7758_dismod sel);

/* Read energy values phase A */
int ade7758_energy_vals_phase_a(struct ade7758_dev *dev,
				struct ade7758_energy_values *data);

/* Read energy values phase B */
int ade7758_energy_vals_phase_b(struct ade7758_dev *dev,
				struct ade7758_energy_values *data);

/* Read energy values phase C */
int ade7758_energy_vals_phase_c(struct ade7758_dev *dev,
				struct ade7758_energy_values *data);

/* Read rms values phase A */
int ade7758_rms_vals_phase_a(struct ade7758_dev *dev,
			     struct ade7758_rms_values *data);

/* Read rms values phase B */
int ade7758_rms_vals_phase_b(struct ade7758_dev *dev,
			     struct ade7758_rms_values *data);

/* Read rms values phase C */
int ade7758_rms_vals_phase_c(struct ade7758_dev *dev,
			     struct ade7758_rms_values *data);

/* Read period value */
int ade7758_period_val(struct ade7758_dev *dev,
		       struct ade7758_period_value *data);

/* Read temperature value */
int ade7758_temp_val(struct ade7758_dev *dev,
		     struct ade7758_temp_value *data);

#endif /* __ADE7758_H__ */
