/***************************************************************************//**
 *   @file   ade7753.h
 *   @brief  Header file of ADE7753 Driver.
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
#ifndef __ADE7753_H__
#define __ADE7753_H__

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
#define ADE7753_SPI_READ					NO_OS_BIT(7)

/* Version product */
#define ADE7753_VERSION						0x3F
#define ADE7753_REG_VERSION_PRODUCT     			0x14

/* Miscellaneous Definitions */
#define IS_6BITS_REG(x) (((x) == ADE7753_REG_PHCAL) || ((x) == ADE7753_REG_CHKSUM))
#define IS_8BITS_REG(x) (((x) >= ADE7753_REG_CH1OS && (x) <= ADE7753_REG_GAIN) \
        	|| ((x) == ADE7753_REG_WDIV) || ((x) == ADE7753_REG_VADIV) || \
		((x) >= ADE7753_REG_SAGCYC && (x) <= ADE7753_REG_VPKLVL) || ((x) == ADE7753_REG_TEMP) \
		|| ((x) == ADE7753_REG_TMODE) || ((x) == ADE7753_REG_DIEREV))
#define IS_12BITS_REG(x) (((x) == ADE7753_REG_WGAIN) || ((x) >= ADE7753_REG_IRMSOS && (x) <= ADE7753_REG_VAGAIN) \
		|| ((x) >= ADE7753_REG_CFNUM && (x) <= ADE7753_REG_CFDEN) || ((x) == ADE7753_REG_ZXTOUT))
#define IS_16BITS_REG(x) (((x) >= ADE7753_REG_MODE && (x) <= ADE7753_REG_RSTSTATUS) \
		|| ((x) == ADE7753_REG_APOS) || ((x) == ADE7753_REG_LINECYC) || ((x) == ADE7753_REG_PERIOD))
#define IS_24BITS_REG(x) (((x) >= ADE7753_REG_WAVEFORM && (x) <= ADE7753_REG_LVARENERGY) || \
		((x) >= ADE7753_REG_IRMS && (x) <= ADE7753_REG_VRMS) ||  \
		((x) >= ADE7753_REG_IPEAK && (x) <= ADE7753_REG_RSTVPEAK))

/* Reset Delay */
#define ADE7753_RESET_DEL					1

/* ENABLE and DISABLE */
#define ENABLE							1u
#define DISABLE							0u

/* ADE7753 Register Map */
#define ADE7753_REG_WAVEFORM					0x01
#define ADE7753_REG_AENERGY					0x02
#define ADE7753_REG_RAENERGY					0x03
#define ADE7753_REG_LAENERGY					0x04
#define ADE7753_REG_VAENERGY					0x05
#define ADE7753_REG_RVAENERGY					0x06
#define ADE7753_REG_LVAENERGY					0x07
#define ADE7753_REG_LVARENERGY					0x08
#define ADE7753_REG_MODE              				0x09
#define ADE7753_REG_IRQEN					0x0A
#define ADE7753_REG_STATUS					0x0B
#define ADE7753_REG_RSTSTATUS					0x0C
#define ADE7753_REG_CH1OS					0x0D
#define ADE7753_REG_CH2OS					0X0E
#define ADE7753_REG_GAIN					0x0F
#define ADE7753_REG_PHCAL					0x10
#define ADE7753_REG_APOS					0x11
#define ADE7753_REG_WGAIN					0x12
#define ADE7753_REG_WDIV					0x13
#define ADE7753_REG_CFNUM					0x14
#define ADE7753_REG_CFDEN					0x15
#define ADE7753_REG_IRMS					0x16
#define ADE7753_REG_VRMS					0x17
#define ADE7753_REG_IRMSOS					0x18
#define ADE7753_REG_VRMSOS					0x19
#define ADE7753_REG_VAGAIN					0x1A
#define ADE7753_REG_VADIV					0X1B
#define ADE7753_REG_LINECYC					0x1C
#define ADE7753_REG_ZXTOUT					0x1D
#define ADE7753_REG_SAGCYC					0x1E
#define ADE7753_REG_SAGLVL					0x1F
#define ADE7753_REG_IPKLVL					0x20
#define ADE7753_REG_VPKLVL					0x21
#define ADE7753_REG_IPEAK					0x22
#define ADE7753_REG_RSTIPEAK					0x23
#define ADE7753_REG_VPEAK					0x24
#define ADE7753_REG_RSTVPEAK					0x25
#define ADE7753_REG_TEMP					0x26
#define ADE7753_REG_PERIOD					0x27
#define ADE7753_REG_TMODE					0x3D
#define ADE7753_REG_CHKSUM					0x3E
#define ADE7753_REG_DIEREV					0x3F

/* ADE7753_REG_MODE Bit Definition */
#define ADE7753_POAM_MSK					NO_OS_BIT(15)
#define ADE7753_WAVSEL_MSK					NO_OS_GENMASK(14, 13)
#define ADE7753_DTRT_MSK					NO_OS_GENMASK(12, 11)
#define ADE7753_SWAP_MSK					NO_OS_BIT(10)
#define ADE7753_DISCH2_MSK					NO_OS_BIT(9)
#define ADE7753_DISCH1_MSK					NO_OS_BIT(8)
#define ADE7753_CYCMODE_MSK					NO_OS_BIT(7)
#define ADE7753_SWRST_MSK					NO_OS_BIT(6)
#define ADE7753_TEMPSEL_MSK					NO_OS_BIT(5)
#define ADE7753_ASUSPEND_MSK					NO_OS_BIT(4)
#define ADE7753_DISSAG_MSK					NO_OS_BIT(3)
#define ADE7753_DISCF_MSK					NO_OS_BIT(2)
#define ADE7753_DISLPF2_MSK					NO_OS_BIT(1)
#define ADE7753_DISHPF_MSK					NO_OS_BIT(0)

/* ADE7753_REG_STATUS/ADE7753_REG_RSTSTATUS/ADE7753_REG_IRQEN Bit Definition */
#define ADE7753_PNEG_MSK					NO_OS_BIT(14)
#define ADE7753_PPOS_MSK					NO_OS_BIT(13)
#define ADE7753_ZXTO_MSK					NO_OS_BIT(12)
#define ADE7753_VAEOF_MSK					NO_OS_BIT(11)
#define ADE7753_VAEHF_MSK					NO_OS_BIT(10)
#define ADE7753_PKI_MSK						NO_OS_BIT(9)
#define ADE7753_PKV_MSK						NO_OS_BIT(8)
#define ADE7753_AEOF_MSK					NO_OS_BIT(7)
#define ADE7753_RESET_MSK					NO_OS_BIT(6)
#define ADE7753_TEMP_MSK					NO_OS_BIT(5)
#define ADE7753_ZX_MSK						NO_OS_BIT(4)
#define ADE7753_WSMP_MSK					NO_OS_BIT(3)
#define ADE7753_CYCEND_MSK					NO_OS_BIT(2)
#define ADE7753_SAG_MSK						NO_OS_BIT(1)
#define ADE7753_AEHF_MSK					NO_OS_BIT(0)

/* ADE7753_REG_CH1OS Bit Definition */
#define ADE7753_INTEGRATOR_MSK					NO_OS_BIT(7)
#define ADE7753_OFFSET_MSK					NO_OS_GENMASK(5, 0)

/* ADE7753_REG_GAIN Bit Definition */
#define ADE7753_PGA2_GAIN_MSK					NO_OS_GENMASK(7, 5)
#define ADE7753_FULL_SCALE_MSK					NO_OS_GENMASK(4, 3)
#define ADE7753_PGA1_GAIN_MSK					NO_OS_GENMASK(2, 0)

/**
 * @enum ade7753_pga_gain_e
 * @brief ADE7753 select the PGA gain.
 */
enum ade7753_pga_gain_e {
	/* Gain */
	/* gain 1 */
	ADE7753_PGA_GAIN_1,
	/* gain 2 */
	ADE7753_PGA_GAIN_2,
	/* gain 4 */
	ADE7753_PGA_GAIN_4,
	/* gain 8 */
	ADE7753_PGA_GAIN_8,
	/* gain 16 */
	ADE7753_PGA_GAIN_16
};

/**
 * @enum ade7753_ch_full_scale_sel_e
 * @brief ADE7753 Channel 1 full-scale select.
 */
enum ade7753_ch_full_scale_sel_e {
	/* Full scale select */
	/* 0.5V */
	ADE7753_FS_0_5,
	/* 0.25V */
	ADE7753_FS_0_25,
	/* 0.125V */
	ADE7753_FS_0_125
};

/**
 * @enum ade7753_dtrt_e
 * @brief ADE7753 select the waveform register update rate.
 */
enum ade7753_dtrt_e {
	/* update rate */
	/* 27.9 kSPS (CLKIN/128) */
	ADE7753_UPDATE_27_9KSPS,
	/* 14 kSPS (CLKIN/256) */
	ADE7753_UPDATE_14KSPS,
	/* 7 kSPS (CLKIN/512) */
	ADE7753_UPDATE_7KSPS,
	/* 3.5 kSPS (CLKIN/1024) */
	ADE7753_UPDATE_3_5KSPS
};

/**
 * @enum ade7753_wavsel_e
 * @brief ADE7753 select the source of the sampled data for the waveform register.
 */
enum ade7753_wavsel_e {
	/* 24 bits active power signal (output of LPF2) */
	ADE7753_24BITS_ACTIVE_POWER,
	/* 24 bits Channel 1 */
	ADE7753_24BITS_CH1 = 2,
	/* 24 bits Channel 2 */
	ADE7753_24BITS_CH2,
};

/**
* @struct ade7753_init_param
* @brief ADE7753 Device initialization parameters.
*/
struct ade7753_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_init_param  	*gpio_reset;
	/** IRQ device descriptor used to handle interrupt routine for IRQ */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/**
* @struct ade7753_dev
* @brief ADE7753 Device structure.
*/
struct ade7753_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_desc  	*gpio_reset;
	/** IRQ device descriptor used to handle interrupt routine for IRQN */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
	/** IRQ device descriptor used to handle interrupt routine for ZX */
	struct no_os_irq_ctrl_desc 	*zx_ctrl;
};

/**
* @struct ade7753_energy_values
* @brief ADE7753 energy registers values
*/
struct ade7753_energy_values {
	/** Active energy register value */
	int32_t active_energy_reg_val;
	/** Accumulated reactive energy register value */
	int32_t acc_reactive_energy_reg_val;
	/** Apparent energy register value */
	int32_t apparent_energy_reg_val;
};

/**
 * @struct ade7753_rms_values
 * @brief ADE7753 rms registers values
 */
struct ade7753_rms_values {
	/** Current rms register value */
	uint32_t current_rms_reg_val;
	/** Voltage rms register value */
	uint32_t voltage_rms_reg_val;
};

/**
 * @struct ade7753_period_value
 * @brief ADE7753 pq values
 */
struct ade7753_period_value {
	/** Period register value */
	uint32_t period_reg_val;
};

/* Initialize the device. */
int ade7753_init(struct ade7753_dev **device,
		 struct ade7753_init_param init_param);

/* Read device register. */
int ade7753_read(struct ade7753_dev *dev, uint16_t reg_addr,
		 int32_t *reg_data);

/* Write device register. */
int ade7753_write(struct ade7753_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data);

/* Update specific register bits. */
int ade7753_update_bits(struct ade7753_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data);

/* Remove the device and release resources. */
int ade7753_remove(struct ade7753_dev *dev);

/* Reset the device using SW reset. */
int ade7753_sw_reset(struct ade7753_dev *dev);

/* Reset the device using HW reset. */
int ade7753_hw_reset(struct ade7753_dev *dev);

/* Version product */
int ade7753_version_product(struct ade7753_dev *dev, uint32_t *data_read);

/* Read with reset Ipeak val */
int ade7753_read_reset_iapk_val(struct ade7753_dev *dev, uint32_t *val);

/* Read with reset Vpeak val */
int ade7753_reset_vpk_val(struct ade7753_dev *dev, uint32_t *val);

/* Get interrupt indicator from STATUS register */
int ade7753_get_int_status(struct ade7753_dev *dev, uint32_t msk,
			   uint8_t *status);

/* Clear irq STATUS flags */
int ade7753_clear_irq_status(struct ade7753_dev *dev, int32_t *reg_data);

/* Enable/Disable interrupt */
int ade7753_enable_irq(struct ade7753_dev *dev, uint32_t msk, uint8_t en);

/* Select waveform register update rate */
int ade7753_wave_update_rate(struct ade7753_dev *dev,
			     enum ade7753_dtrt_e sel);

/* Select source of sampled data for wave register */
int ade7753_wave_sample_data_source(struct ade7753_dev *dev,
				    enum ade7753_wavsel_e sel);

/* Read energy values */
int ade7753_energy_vals(struct ade7753_dev *dev,
			struct ade7753_energy_values *data);

/* Read rms values */
int ade7753_rms_vals(struct ade7753_dev *dev,
		     struct ade7753_rms_values *data);

/* Read period value */
int ade7753_period_val(struct ade7753_dev *dev,
		       struct ade7753_period_value *data);

#endif /* __ADE7753_H__ */
