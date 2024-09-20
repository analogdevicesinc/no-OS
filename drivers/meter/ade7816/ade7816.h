/***************************************************************************//**
 *   @file   ade7816.h
 *   @brief  Header file of ADE7816 Driver.
 *   @author Radu Sabau (radu.sabau@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
 ******************************************************************************/
#ifndef __ADE7816_H__
#define __ADE7816_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_error.h"
#include "no_os_i2c.h"

#define ADE7816_VGAIN_REG			0x4380
#define ADE7816_IAGAIN_REG			0x4381
#define ADE7816_IBGAIN_REG			0x4382
#define ADE7816_ICGAIN_REG			0x4383
#define ADE7816_IDGAIN_REG			0x4384
#define ADE7816_IEGAIN_REG			0x4385
#define ADE7816_IFGAIN_REG			0x4386

#define ADE7816_DICOEFF_REG			0x4388
#define ADE7816_HPFDIS_REG			0x4389
#define ADE7816_VRMSOS_REG			0x438A
#define ADE7816_IARMSOS_REG			0x438B
#define ADE7816_IBRMSOS_REG			0x438C
#define ADE7816_ICRMSOS_REG			0x438D
#define ADE7816_IDRMSOS_REG			0x438E
#define ADE7816_IERMSOS_REG			0x438F
#define ADE7816_IFRMSOS_REG			0x4390
#define ADE7816_AWGAIN_REG			0x4391
#define ADE7816_AWATTOS_REG			0x4392
#define ADE7816_BWGAIN_REG			0x4393
#define ADE7816_BWATTOS_REG			0x4394
#define ADE7816_CWGAIN_REG			0x4395
#define ADE7816_CWATTOS_REG			0x4396
#define ADE7816_DWGAIN_REG			0x4397
#define ADE7816_DWATTOS_REG			0x4398
#define ADE7816_EWGAIN_REG			0x4399
#define ADE7816_EWATTOS_REG			0x439A
#define ADE7816_FWGAIN_REG			0x439B
#define ADE7816_FWATTOS_REG			0x439C
#define ADE7816_AVARGAIN_REG			0x439D
#define ADE7816_AVAROS_REG			0x439E
#define ADE7816_BVARGAIN_REG			0x439F
#define ADE7816_BVAROS_REG			0x43A0
#define ADE7816_CVARGAIN_REG			0x43A1
#define ADE7816_CVAROS_REG			0x43A2
#define ADE7816_DVARGAIN_REG			0x43A3
#define ADE7816_DVAROS_REG			0x43A4
#define ADE7816_EVARGAIN_REG			0x43A5
#define ADE7816_EVAROS_REG			0x43A6
#define ADE7816_FVARGAIN_REG			0x43A7
#define ADE7816_FVAROS_REG			0x43A8

#define ADE7816_WTHR1_REG			0x43AB
#define ADE7816_WTHR0_REG			0x43AC
#define ADE7816_VARTHR1_REG			0x43AD
#define ADE7816_VARTHR0_REG			0x43AE
#define ADE7816_APNOLOAD_REG			0x43AF
#define ADE7816_VARNOLOAD_REG			0x43B0
#define ADE7816_PCF_A_COEFF_REG			0x43B1
#define ADE7816_PCF_B_COEFF_REG			0x43B3
#define ADE7816_PCF_C_COEFF_REG			0x43B4
#define ADE7816_PCF_D_COEFF_REG			0x43B5
#define ADE7816_PCF_E_COEFF_REG			0x43B6
#define ADE7816_PCF_F_COEFF_REG			0x43B7

#define ADE7816_VRMS_REG			0x43C0
#define ADE7816_IARMS_REG			0x43C1
#define ADE7816_IBRMS_REG			0x43C2
#define ADE7816_ICRMS_REG			0x43C3
#define ADE7816_IDRMS_REG			0x43C4
#define ADE7816_IERMS_REG			0x43C5
#define ADE7816_IFRMS_REG			0x43C6

#define ADE7816_RUN_REG				0xE228

#define ADE7816_AWATTHR_REG			0xE400
#define ADE7816_BWATTHR_REG			0xE401
#define ADE7816_CWATTHR_REG			0xE402
#define ADE7816_DWATTHR_REG			0xE403
#define ADE7816_EWATTHR_REG			0xE404
#define ADE7816_FWATTHR_REG			0xE405
#define ADE7816_AVARHR_REG			0xE406
#define ADE7816_BVARHR_REG			0xE407
#define ADE7816_CVARHR_REG			0xE408
#define ADE7816_DVARHR_REG			0xE409
#define ADE7816_EVARHR_REG			0xE40A
#define ADE7816_FVARHR_REG			0xE40B

#define ADE7816_IPEAK_REG			0xE500
#define ADE7816_VPEAK_REG			0xE501
#define ADE7816_STATUS0_REG			0xE502
#define ADE7816_STATUS1_REG			0xE503

#define ADE7816_OIVL_REG			0xE507
#define ADE7816_OVLVL_REG			0xE508
#define ADE7816_SAGLVL_REG			0xE509
#define ADE7816_MASK0_REG			0xE50A
#define ADE7816_MASK1_REG			0xE50B
#define ADE7816_IAVW_IDVW_REG			0xE50C
#define ADE7816_IBVW_IEVW_REG			0xE50D
#define ADE7816_ICVW_IFVW_REG			0xE50E

#define ADE7816_VWV_REG				0xE510

#define ADE7816_CHECKSUM_REG			0xE51F

#define ADE7816_CHSTATUS_REG			0xE600
#define ADE7816_ANGLE0_REG			0xE601
#define ADE7816_ANGLE1_REG			0xE602
#define ADE7816_ANGLE2_REG			0xE603

#define ADE7816_PERIOD_REG			0xE607
#define ADE7816_CHNOLOAD_REG			0xE608

#define ADE7816_LINECYC_REG			0xE60C
#define ADE7816_ZXTOUT_REG			0xE60D
#define ADE7816_COMPMODE_REG			0xE60E
#define ADE7816_GAIN_REG			0xE60F

#define ADE7816_CHSIGN_REG			0xE617
#define ADE7816_CONFIG_REG			0xE618

#define ADE7816_MMODE_REG			0xE700
#define ADE7816_ACCMODE_REG			0xE701
#define ADE7816_LCYCMODE_REG			0xE702
#define ADE7816_PEAKCYC_REG			0xE703
#define ADE7816_SAGCYC_REG			0xE704

#define ADE7816_HSDC_CFG_REG			0xE706
#define ADE7816_VERSION_REG			0xE707

#define ADE7816_CONFIG2_REG			0xEC01

//MASK MACROS
#define ADE7816_LSB_REG_MASK			NO_OS_GENMASK(7, 0)
#define ADE7816_MSB_REG_MASK			NO_OS_GENMASK(15, 8)

#define ADE7816_FOURTH_BYTE_MASK		NO_OS_GENMASK(31, 24)
#define ADE7816_THIRD_BYTE_MASK			NO_OS_GENMASK(23, 16)
#define ADE7816_SECOND_BYTE_MASK		NO_OS_GENMASK(15, 8)
#define ADE7816_FIRST_BYTE_MASK			NO_OS_GENMASK(7, 0)

#define ADE7816_SWRST_MASK			NO_OS_BIT(7)

#define ADE7816_I2C_LOCK_MASK			NO_OS_BIT(1)

#define ADE7816_ZSPE_MASK			NO_OS_GENMASK(27, 0)

#define ADE7816_THR_MSB_MASK			NO_OS_GENMASK(31, 24)
#define ADE7816_THR_LSB_MASK			NO_OS_GENMASK(23, 0)

#define ADE7816_RSTREAD_MASK			NO_OS_BIT(6)

#define ADE7816_LMASK				NO_OS_GENMASK(1, 0)
#define ADE7816_ZX_SEL_MASK			NO_OS_BIT(3)

#define ADE7816_LENERGY_MASK			NO_OS_BIT(5)

#define ADE7816_CHANNEL_SEL_MASK		NO_OS_BIT(14)

#define ADE7816_DREADY_MASK			NO_OS_BIT(17)

#define ADE7816_MMODE_MASK			NO_OS_GENMASK(4, 2)

#define ADE7816_REVSEL_MASK			NO_OS_GENMASK(7, 6)

#define ADE7816_ANGLESEL_MASK			NO_OS_GENMASK(10, 9)

#define ADE7816_SIGN_MASK(x)			(((x) > 3) ? ((x) - 4) : (x))

#define ADE7816_INT_MASK(x)			NO_OS_BIT(x)

#define ADE7816_INIT_DELAY			40
#define ADE7816_HWRST_DELAY			10

#define ADE7816_LINECYC_VAL(enable)		((enable) ? 0x11 : 0x00)

#define ADE7816_CHECKSUM_VAL			0x33666787

enum ade7816_comm_type {
	ADE7816_I2C,
	ADE7816_SPI
};

enum ade7816_channel {
	ADE7816_CHANNEL_VOLTAGE,
	ADE7816_CHANNEL_A,
	ADE7816_CHANNEL_B,
	ADE7816_CHANNEL_C,
	ADE7816_CHANNEL_D,
	ADE7816_CHANNEL_E,
	ADE7816_CHANNEL_F,
};

enum ade7816_gain_type {
	ADE7816_CURRENT_GAIN,
	ADE7816_ACTIVE_POWER_GAIN,
	ADE7816_REACTIVE_POWER_GAIN,
};

enum ade7816_mmode_sel {
	ADE7816_PEAKSEL_0_AD,
	ADE7816_PEAKSEL_1_BE,
	ADE7816_PEAKSEL_2_CF,
};

enum ade7816_angle_sel {
	ADE7816_VOLTAGE_CURRENT,
	ADE7816_CURRENT_CURRENT = 2,
};

enum ade7816_status_int {
	ADE7816_AEHF1_INT,
	ADE7816_AEHF2_INT,
	ADE7816_REHF1_INT,
	ADE7816_REHF2_INT,
	ADE7816_LENERGY_INT = 5,
	ADE7816_REVAP1_INT,
	ADE7816_REVAP2_INT,
	ADE7816_REVAP3_INT,
	ADE7816_REVRP1_INT = 10,
	ADE7816_REVRP2_INT,
	ADE7816_REVRP3_INT,
	ADE7816_DREADY_INT = 17,
	ADE7816_NLOAD1_INT = 32,
	ADE7816_NLOAD2_INT,
	ADE7816_ZXTOV_INT = 35,
	ADE7816_ZXTOI1_INT = 38,
	ADE7816_ZXTOI2_INT,
	ADE7816_ZXTOI3_INT,
	ADE7816_ZXV_INT,
	ADE7816_ZXI1_INT = 44,
	ADE7816_ZXI2_INT,
	ADE7816_ZXI3_INT,
	ADE7816_RSTDONE_INT,
	ADE7816_SAG_INT,
	ADE7816_OI_INT,
	ADE7816_OV_INT,
	ADE7816_PKI_INT = 55,
	ADE7816_PKV_INT,
};

enum ade7816_active_irq {
	ADE7816_NO_IRQ,
	ADE7816_IRQ0,
	ADE7816_IRQ1,
	ADE7816_IRQ0_IRQ1,
};

enum ade7816_pcf_coeff {
	ADE7816_PCF_50HZ = 0x400CA4,
	ADE7816_PCF_60HZ = 0x401235,
};

struct ade7816_desc;

typedef int (*ade7816_reg_read)(struct ade7816_desc *desc, uint16_t reg,
				uint32_t *val);
typedef int (*ade7816_reg_write)(struct ade7816_desc *desc, uint16_t reg,
				 uint32_t val);

struct ade7816_init_param {
	struct no_os_spi_init_param *spi_param;
	struct no_os_i2c_init_param *i2c_param;
	struct no_os_gpio_init_param *reset_param;
	struct no_os_gpio_init_param *ss_param;
	struct no_os_gpio_init_param *gpio_irq0_param;
	struct no_os_gpio_init_param *gpio_irq1_param;

	struct no_os_irq_ctrl_desc *irq_ctrl;

	enum ade7816_active_irq active_irq;
	enum ade7816_comm_type comm_type;

	uint32_t irq0_priority;
	uint32_t irq1_priority;

	void *handle_irq0;
	void *handle_irq1;

	/* External callback used to handle interrupt routine for GPIO
	 * IRQ0/IRQ1.
	 *
	 * Setting one of it to NULL means driver defined callback is used for
	 * it.
	 */
	void (*irq0_callback)(void *context);
	void (*irq1_callback)(void *context);
};

struct ade7816_desc {
	struct no_os_spi_desc *spi_desc;
	struct no_os_i2c_desc *i2c_desc;

	struct no_os_gpio_desc *reset_desc;
	struct no_os_gpio_desc *ss_desc;
	struct no_os_gpio_desc *gpio_irq0_desc;
	struct no_os_gpio_desc *gpio_irq1_desc;
	struct no_os_irq_ctrl_desc *irq_ctrl;
	struct no_os_callback_desc irq0_cb;
	struct no_os_callback_desc irq1_cb;

	ade7816_reg_read reg_read;
	ade7816_reg_write reg_write;

	enum ade7816_active_irq active_irq;
	enum ade7816_comm_type comm_type;
	enum ade7816_channel chan;

	uint32_t status0;
	uint32_t status1;

	bool lcycle_mode;
};

/** ADE7816 SPI Read Register function. */
int ade7816_spi_reg_read(struct ade7816_desc *desc, uint16_t reg,
			 uint32_t *val);

/** ADE7816 SPI Write Register function. */
int ade7816_spi_reg_write(struct ade7816_desc *desc, uint16_t reg,
			  uint32_t val);

/** ADE7816 I2C Read Register function. */
int ade7816_i2c_reg_read(struct ade7816_desc *desc, uint16_t reg,
			 uint32_t *val);

/** ADE7816 I2C Write Register function. */
int ade7816_i2c_reg_write(struct ade7816_desc *desc, uint16_t reg,
			  uint32_t val);

/** Register read wrapper function. */
int ade7816_read_reg(struct ade7816_desc *desc, uint16_t reg, uint32_t *val);

/** Register write wrapper function. */
int ade7816_write_reg(struct ade7816_desc *desc, uint16_t reg, uint32_t val);

/** Register update function. */
int ade7816_reg_update(struct ade7816_desc *desc, uint16_t reg, uint32_t mask,
		       uint32_t val);

/** ADE7816 software reset function. */
int ade7816_sw_reset(struct ade7816_desc *desc);

/** ADE7816 hardware reset function. */
int ade7816_hw_reset(struct ade7816_desc *desc);

/** ADE7816 set communication function. */
int ade7816_set_comm(struct ade7816_desc *desc, enum ade7816_comm_type type);

/** ADE7816 read active energy function. */
int ade7816_read_active_energy(struct ade7816_desc *desc,
			       enum ade7816_channel chan, int32_t *val);

/** ADE7816 set active energy threshold value function. */
int ade7816_set_active_thr(struct ade7816_desc *desc, uint16_t freq);

/** ADE7816 get active energy threshold value function. */
int ade7816_get_active_thr(struct ade7816_desc *desc, uint16_t *freq);

/** ADE7816 read reactive energy value function. */
int ade7816_read_reactive_energy(struct ade7816_desc *desc,
				 enum ade7816_channel chan, int32_t *val);

/** ADE7816 set reactive energy threshold value function. */
int ade7816_set_reactive_thr(struct ade7816_desc *desc, uint16_t freq);

/** ADE7816 get reactive energy threshold value function. */
int ade7816_get_reactive_thr(struct ade7816_desc *desc, uint16_t *freq);

/** ADE7816 set line cycle mode function. */
int ade7816_set_lcycle_mode(struct ade7816_desc *desc, bool enable,
			    uint16_t cycles, bool lenergy);

/** ADE7816 read root mean square measurement value function. */
int ade7816_read_rms(struct ade7816_desc *desc, enum ade7816_channel chan,
		     uint32_t *rms);

/** ADE7816 set no load condition function. */
int ade7816_set_no_load(struct ade7816_desc *desc, uint16_t voltage,
			uint16_t current, bool enable);

/** ADE7816 set gain value for channel function. */
int ade7816_set_gain(struct ade7816_desc *desc, enum ade7816_channel chan,
		     int16_t scale, enum ade7816_gain_type gain);

/** ADE7816 get gain value for channel function. */
int ade7816_get_gain(struct ade7816_desc *desc, enum ade7816_channel chan,
		     int16_t *scale, enum ade7816_gain_type gain);

/** ADE7816 set offset value for channel function. */
int ade7816_set_offset(struct ade7816_desc *desc, enum ade7816_channel chan,
		       int16_t scale, enum ade7816_gain_type gain);

/** ADE7816 get offset value for channel function. */
int ade7816_get_offset(struct ade7816_desc *desc, enum ade7816_channel chan,
		       int16_t *scale, enum ade7816_gain_type gain);

/** ADE7816 set phase calibration coefficient function. */
int ade7816_set_phase(struct ade7816_desc *desc, enum ade7816_channel chan,
		      enum ade7816_pcf_coeff pcf_coeff);

/** ADE7816 root mean square measurement calibration function. */
int ade7816_calib_rms(struct ade7816_desc *desc, enum ade7816_channel chan,
		      int32_t rms);

/** ADE7816 group selection for channels function. */
int ade7816_group_sel(struct ade7816_desc *desc, enum ade7816_channel chan);

/** ADE7816 zero-crossing detection for channel function. */
int ade7816_zx_detect(struct ade7816_desc *desc, enum ade7816_channel chan);

/** ADE7816 zero-crossing detection timeout value function. */
int ade7816_zx_timeout(struct ade7816_desc *desc, uint32_t timeout_us);

/** ADE7816 set peak detection for channel function. */
int ade7816_peak_detect(struct ade7816_desc *desc, enum ade7816_channel chan,
			enum ade7816_mmode_sel mmode, uint8_t no_of_cycles);

/** ADE7816 set power direction for channel function. */
int ade7816_power_dir(struct ade7816_desc *desc, enum ade7816_channel chan);

/** ADE7816 get power direction for channel function. */
int ade7816_read_dir(struct ade7816_desc *desc, enum ade7816_channel chan,
		     bool *sign);

/** ADE7816 angle measurement function. */
int ade7816_angle_meas(struct ade7816_desc *desc, enum ade7816_channel chan,
		       enum ade7816_angle_sel sel);

/** ADE7816 read period value function. */
int ade7816_read_period(struct ade7816_desc *desc, uint32_t *period_us);

/** ADE7816 set interrupts function. */
int ade7816_set_interrupt(struct ade7816_desc *desc,
			  enum ade7816_status_int status_int, bool enable);

/** ADE7816 get interrupts function. */
int ade7816_get_interrupt(struct ade7816_desc *desc,
			  enum ade7816_status_int status_int, bool *enable);

/** ADE7816 root mean square measurement value conversion to microunits
 *  function.
 */
int ade7816_rms_to_micro(struct ade7816_desc *desc, enum ade7816_channel chan,
			 uint32_t rms, uint32_t *micro);

/** Initialize and configure the ADE7816 device. */
int ade7816_init(struct ade7816_desc **desc,
		 struct ade7816_init_param *init_param);

/** Free the resources allocated during init. */
int ade7816_remove(struct ade7816_desc *desc);

#endif /* __ADE7816_H__ */
