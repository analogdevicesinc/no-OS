/***************************************************************************//**
 *   @file   hmc630x.h
 *   @brief  hmc6300 and hmc6301 device driver header.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#ifndef _HMC630X_H_
#define _HMC630X_H_

#include <stdbool.h>
#include <stdint.h>
#include "no_os_util.h"
#include "no_os_gpio.h"

/* Code the row on highest 8 (of 16) bits and the field mask on lowest 8 bits. */
#define HMC630X_PARAM(row, mask)	(((((uint16_t)row) << 8)) | (mask))
#define HMC630X_ROW(param)		((param) >> 8)
#define HMC630X_MASK(param)		((param) & 0xff)

/* ROW0 */
#define HMC6301_LNA_PWRDWN		HMC630X_PARAM(0, NO_OS_BIT(7))
#define HMC6301_BBAMP_PWRDN_I		HMC630X_PARAM(0, NO_OS_BIT(6))
#define HMC6301_BBAMP_PWRDN_Q		HMC630X_PARAM(0, NO_OS_BIT(5))
#define HMC6301_DIVIDER_PWRDN		HMC630X_PARAM(0, NO_OS_BIT(4))
#define HMC6301_MIXER_PWRDN		HMC630X_PARAM(0, NO_OS_BIT(3))
#define HMC6301_IFMIXER_PWRDN_I		HMC630X_PARAM(0, NO_OS_BIT(2))
#define HMC6301_TRIPLER_PWRDN		HMC630X_PARAM(0, NO_OS_BIT(1))
#define HMC6301_IFVGA_PWRDN		HMC630X_PARAM(0, NO_OS_BIT(0))

/* ROW1 */
#define HMC6300_PA_SEL_VGBS		HMC630X_PARAM(1, NO_OS_GENMASK(7, 4))
#define HMC6300_PA_SEL_VREF		HMC630X_PARAM(1, NO_OS_GENMASK(3, 0))

#define HMC6301_IPC_PWRDWN		HMC630X_PARAM(1, NO_OS_BIT(7))
#define HMC6301_IFMIX_PWRDN_Q		HMC630X_PARAM(1, NO_OS_BIT(6))
#define HMC6301_IF_BGMUX_PWRDN		HMC630X_PARAM(1, NO_OS_BIT(5))
#define HMC6301_ASK_PWRDN		HMC630X_PARAM(1, NO_OS_BIT(4))
#define HMC6301_BBAMP_ATTEN1		HMC630X_PARAM(1, NO_OS_GENMASK(3, 2))
#define HMC6301_BBAMP_SELL_ASK		HMC630X_PARAM(1, NO_OS_BIT(1))
#define HMC6301_BBAMP_SIGSHORT		HMC630X_PARAM(1, NO_OS_BIT(0))

/* ROW2 */
#define HMC6300_PA_SEL_ALC_DAC		HMC630X_PARAM(2, NO_OS_GENMASK(7, 4))
#define HMC6300_PA_SEP_PA_PWRDWN_FAST	HMC630X_PARAM(2, NO_OS_BIT(3))
#define HMC6300_PA_PWRDWN_FAST		HMC630X_PARAM(2, NO_OS_BIT(2))
#define HMC6300_PA_SE_SEL		HMC630X_PARAM(2, NO_OS_BIT(1))
#define HMC6300_POWER_DET_PWRDN		HMC630X_PARAM(2, NO_OS_BIT(0))

#define HMC6301_BBAMP_ATTENFI		HMC630X_PARAM(2, NO_OS_GENMASK(7, 5))
#define HMC6301_BBAMP_ATTENFQ		HMC630X_PARAM(2, NO_OS_GENMASK(4, 2))
#define HMC6301_BBAMP_ATTEN2		HMC630X_PARAM(2, NO_OS_GENMASK(1, 0))

/* ROW3 */
#define HMC6300_DRIVER_BIAS		HMC630X_PARAM(3, NO_OS_GENMASK(7, 5))
#define HMC6300_DRIVER_BIAS2		HMC630X_PARAM(3, NO_OS_GENMASK(4, 2))
#define HMC6300_EN_IFMIX_HICG		HMC630X_PARAM(3, NO_OS_BIT(1))
#define HMC6300_EN_TEMPFLASH		HMC630X_PARAM(3, NO_OS_BIT(0))

#define HMC6301_BBAMP_SELBW		HMC630X_PARAM(3, NO_OS_GENMASK(7, 6))
#define HMC6301_BBAMP_SELFASTREC	HMC630X_PARAM(3, NO_OS_GENMASK(5, 4))
#define HMC6301_BG_MONITOR_SEL		HMC630X_PARAM(3, NO_OS_GENMASK(3, 2))
#define HMC6301_IF_REFSEL		HMC630X_PARAM(3, NO_OS_BIT(1))
#define HMC6301_LNA_REFSEL		HMC630X_PARAM(3, NO_OS_BIT(0))

/* ROW4 */
#define HMC6300_DRIVER_PWRDN		HMC630X_PARAM(4, NO_OS_BIT(7))
#define HMC6300_UPMIXER_PWRDN		HMC630X_PARAM(4, NO_OS_BIT(6))
#define HMC6300_IFVGA_PWRDN		HMC630X_PARAM(4, NO_OS_BIT(5))
#define HMC6300_DIVIDER_PWRDN		HMC630X_PARAM(4, NO_OS_BIT(4))
#define HMC6300_PA_PWRDN		HMC630X_PARAM(4, NO_OS_BIT(3))
#define HMC6300_RFVGA_PWRDN		HMC630X_PARAM(4, NO_OS_BIT(2))
#define HMC6300_TRIPLER_PWRDN		HMC630X_PARAM(4, NO_OS_BIT(1))
#define HMC6300_IF_UPMIXER_PWRDN	HMC630X_PARAM(4, NO_OS_BIT(0))

#define HMC6301_IFVGA_BIAS		HMC630X_PARAM(4, NO_OS_GENMASK(7, 5))
#define HMC6301_IFVGA_TUNE		HMC630X_PARAM(4, NO_OS_GENMASK(4, 1))
#define HMC6301_ENDIGVGA		HMC630X_PARAM(4, NO_OS_BIT(0))

/* ROW5 */
#define HMC6300_TRIPLER_BIAS_HIGH	HMC630X_PARAM(5, NO_OS_GENMASK(7, 0))

#define HMC6301_IFVGA_VGA_ADJ		HMC630X_PARAM(5, NO_OS_GENMASK(7, 4))
#define HMC6301_RFMIX_TUNE		HMC630X_PARAM(5, NO_OS_GENMASK(3, 0))

/* ROW6 */
#define HMC6300_TRIPLER_BIAS_LOW	HMC630X_PARAM(6, NO_OS_GENMASK(7, 2))

#define HMC6301_TRIPLER_BIAS_HIGH	HMC630X_PARAM(6, NO_OS_GENMASK(7, 0))

/* ROW7 */
#define HMC6300_IFVGA_VGA_ADJ		HMC630X_PARAM(7, NO_OS_GENMASK(7, 4))
#define HMC6300_IFVGA_TUNE		HMC630X_PARAM(7, NO_OS_GENMASK(3, 0))

#define HMC6301_TRIPLER_BIAS_LOW	HMC630X_PARAM(7, NO_OS_GENMASK(7, 2))
#define HMC6301_BBAMP_SELFM		HMC630X_PARAM(7, NO_OS_BIT(1))
#define HMC6301_FM_PWRDN		HMC630X_PARAM(7, NO_OS_BIT(0))

/* ROW8 */
#define HMC6300_IFVGA_BIAS		HMC630X_PARAM(8, NO_OS_GENMASK(7, 4))
#define HMC6300_IF_UPMIXER_TUNE		HMC630X_PARAM(8, NO_OS_GENMASK(3, 0))

#define HMC6301_LNA_BIAS		HMC630X_PARAM(8, NO_OS_GENMASK(7, 5))
#define HMC6301_LNA_GAIN		HMC630X_PARAM(8, NO_OS_GENMASK(4, 3))
#define HMC6301_IFVGA_Q_CNTRL		HMC630X_PARAM(8, NO_OS_GENMASK(2, 0))

/* ROW9*/
#define HMC6300_IFVGA_Q_CNTRL		HMC630X_PARAM(9, NO_OS_GENMASK(7, 5))

#define HMC6301_ENANAV_LNA		HMC630X_PARAM(9, NO_OS_BIT(7))
#define HMC6301_ENBAR_TEMPS		HMC630X_PARAM(9, NO_OS_BIT(6))
#define HMC6301_EN_TEMPFLASH		HMC630X_PARAM(9, NO_OS_BIT(5))
#define HMC6301_EN_SEP_IFMIX_PWRDN_Q	HMC630X_PARAM(9, NO_OS_BIT(4))

/* ROW10 */
#define HMC6300_ENABLE_FM		HMC630X_PARAM(10, NO_OS_BIT(7))
#define HMC6300_IF_REFSEL		HMC630X_PARAM(10, NO_OS_BIT(6))
#define HMC6300_BG_MONITOR		HMC630X_PARAM(10, NO_OS_BIT(5))
#define HMC6300_ENDIG_IFVGA_GAIN_CONTROL HMC630X_PARAM(10, NO_OS_BIT(4))
#define HMC6300_IPC_PWRDN		HMC630X_PARAM(10, NO_OS_BIT(3))
#define HMC6300_IF_BGMUX_PWRDN		HMC630X_PARAM(10, NO_OS_BIT(2))
#define HMC6300_UPMIX_CAL_PWRDN		HMC630X_PARAM(10, NO_OS_BIT(1))
#define HMC6300_TEMPSENSOR_PWRDN	HMC630X_PARAM(10, NO_OS_BIT(0))

/* ROW11 */
#define HMC6300_RFVGAGAIN		HMC630X_PARAM(11, NO_OS_GENMASK(7, 4))
#define HMC6300_ENRFVGA_ANA		HMC630X_PARAM(11, NO_OS_BIT(3))
#define HMC6300_RFVGA_ICTRL		HMC630X_PARAM(11, NO_OS_GENMASK(2, 0))

/* ROW12 */
#define HMC6300_UPMIX_CAL		HMC630X_PARAM(12, NO_OS_GENMASK(7, 0))

/* ROW16 */
#define HMC630X_BYP_SYNTH_LDO		HMC630X_PARAM(16, NO_OS_BIT(7))
#define HMC630X_EN_CPSHORT		HMC630X_PARAM(16, NO_OS_BIT(6))
#define HMC630X_EN_CPCMFB		HMC630X_PARAM(16, NO_OS_BIT(5))
#define HMC630X_EN_CP_DUMP		HMC630X_PARAM(16, NO_OS_BIT(4))
#define HMC630X_EN_CPTRIST		HMC630X_PARAM(16, NO_OS_BIT(3))
#define HMC630X_EN_CP			HMC630X_PARAM(16, NO_OS_BIT(2))
#define HMC630X_EN_SYNTH_LDO		HMC630X_PARAM(16, NO_OS_BIT(1))
#define HMC630X_ENBAR_SYNTHBG		HMC630X_PARAM(16, NO_OS_BIT(0))

/* ROW17 */
#define HMC630X_EN_LOCKD_CLK		HMC630X_PARAM(17, NO_OS_BIT(7))
#define HMC630X_EN_TEST_DIVOUT		HMC630X_PARAM(17, NO_OS_BIT(6))
#define HMC630X_EN_VTUNE_FLASH		HMC630X_PARAM(17, NO_OS_BIT(5))
#define HMC630X_EN_REBUF_DC		HMC630X_PARAM(17, NO_OS_BIT(4))
#define HMC630X_EN_REFBUF		HMC630X_PARAM(17, NO_OS_BIT(3))
#define HMC630X_EN_STICK_DIV		HMC630X_PARAM(17, NO_OS_BIT(2))
#define HMC630X_EN_FBDIV_CML2CMOS	HMC630X_PARAM(17, NO_OS_BIT(1))
#define HMC630X_EN_FBDIV		HMC630X_PARAM(17, NO_OS_BIT(0))

/* ROW18 */
#define HMC630X_EN_NB250M		HMC630X_PARAM(18, NO_OS_BIT(6))
#define HMC630X_BYP_VCO_LDO		HMC630X_PARAM(18, NO_OS_BIT(5))
#define HMC630X_EN_EXTLO		HMC630X_PARAM(18, NO_OS_BIT(4))
#define HMC630X_EN_VCOPK		HMC630X_PARAM(18, NO_OS_BIT(3))
#define HMC630X_EN_VCO			HMC630X_PARAM(18, NO_OS_BIT(2))
#define HMC630X_EN_VCO_REG		HMC630X_PARAM(18, NO_OS_BIT(1))
#define HMC630X_ENBAR_VCOGB		HMC630X_PARAM(18, NO_OS_BIT(0))

/* ROW19 */
#define HMC630X_REFSEL_SYNTHBG		HMC630X_PARAM(19, NO_OS_BIT(1))
#define HMC630X_MUXREF			HMC630X_PARAM(19, NO_OS_BIT(0))

/* ROW20 */
#define HMC630X_FBDIV_CODE		HMC630X_PARAM(20, NO_OS_GENMASK(6, 0))

/* ROW21 */
#define HMC630X_REFSEL_VCOBG		HMC630X_PARAM(21, NO_OS_BIT(4))
#define HMC630X_VCO_BIASTRIM		HMC630X_PARAM(21, NO_OS_GENMASK(3, 0))

/* ROW22 */
#define HMC630X_VCO_BANDSEL		HMC630X_PARAM(22, NO_OS_GENMASK(6, 0))

/* ROW23 */
#define HMC630X_ICP_BIASTRIM		HMC630X_PARAM(23, NO_OS_GENMASK(7, 5))
#define HMC630X_VCO_OFFSET		HMC630X_PARAM(23, NO_OS_GENMASK(4, 0))

/* ROW24 */
#define HMC630X_LOCKDET			HMC630X_PARAM(24, NO_OS_BIT(3))
#define HMC630X_DN			HMC630X_PARAM(24, NO_OS_BIT(2))
#define HMC630X_UP			HMC630X_PARAM(24, NO_OS_BIT(1))
#define HMC630X_CENTER			HMC630X_PARAM(24, NO_OS_BIT(0))

/* ROW25 */
#define HMC630X_VTUNE_FLASHP		HMC630X_PARAM(25, NO_OS_GENMASK(7, 0))

/* ROW26 */
#define HMC630X_VTUNE_FLASHN		HMC630X_PARAM(26, NO_OS_GENMASK(7, 0))

/* ROW27 */
#define HMC630X_TEMPS			HMC630X_PARAM(27, NO_OS_GENMASK(4, 0))

/* Devices supported by this driver. */
enum hmc630x_type {
	HMC6300,
	HMC6301,
};

/* Possible values for receiver LNA attenuator. */
enum hmc6301_lna_attn {
	HMC6301_LNA_ATTN_0dB,
	HMC6301_LNA_ATTN_6dB,
	HMC6301_LNA_ATTN_12dB,
	HMC6301_LNA_ATTN_18dB
};

/* Possible baseband attenuation values. */
enum hmc6301_bb_attn {
	HMC6301_BB_ATTN_0dB,
	HMC6301_BB_ATTN_12dB,
	HMC6301_BB_ATTN_6dB,
	HMC6301_BB_ATTN_18dB
};

/* Possible baseband fine attenuation adjustment values. */
enum hmc6301_bb_attn_fine {
	HMC6301_BB_ATTN_FINE_0dB,
	HMC6301_BB_ATTN_FINE_4dB,
	HMC6301_BB_ATTN_FINE_2dB,
	HMC6301_BB_ATTN_FINE_RESERVED1, /* Do not use. */
	HMC6301_BB_ATTN_FINE_1dB,
	HMC6301_BB_ATTN_FINE_5dB,
	HMC6301_BB_ATTN_FINE_3dB,
	HMC6301_BB_ATTN_FINE_RESERVED2 /* Do not use. */
};

/* Possible values for the low-pass corner of the baseband amplifiers. */
enum hmc6301_bb_lpc {
	HMC6301_BB_LPC_1400MHz,
	HMC6301_BB_LPC_300MHz,
	HMC6301_BB_LPC_500MHz,
	HMC6301_BB_LPC_200MHz
};

/* Possible values for the high-pass corner of the baseband amplifiers. */
enum hmc6301_bb_hpc {
	HMC6301_BB_HPC_45kHz,
	HMC6301_BB_HPC_1600kHz,
	HMC6301_BB_HPC_350kHz,
	HMC6301_BB_HPC_RESERVED /* Do not use. */
};

/* Possible values for the external reference clock. */
enum hmc630x_ref_clk {
	HMC6300_REF_CLK_71p42857MHz,
	HMC6300_REF_CLK_75MHz,
	HMC6300_REF_CLK_142p8571MHz,
	HMC6300_REF_CLK_154p2857MHz,
};

struct hmc6300_attr {
	uint8_t rf_attn;
};

struct hmc6301_attr {
	enum hmc6301_bb_attn bb_attn1;
	enum hmc6301_bb_attn bb_attn2;
	enum hmc6301_bb_attn_fine bb_attni_fine;
	enum hmc6301_bb_attn_fine bb_attnq_fine;
	enum hmc6301_lna_attn lna_attn;
	enum hmc6301_bb_lpc bb_lpc;
	enum hmc6301_bb_hpc bb_hpc;
};

/* Initialization parameters for hmc6300_init(). */
struct hmc630x_init_param {
	enum hmc630x_type type;
	enum hmc630x_ref_clk ref_clk; /* Reference clock value. */
	struct no_os_gpio_init_param en; /* EN GPIO signal of the digital interface. */
	struct no_os_gpio_init_param
		clk; /* CLK GPIO signal of the digital interface. */
	struct no_os_gpio_init_param
		data; /* DATA GPIO signal of the digital interface. */
	struct no_os_gpio_init_param
		scanout; /* SCANOUT GPIO signal of the digital interface. */
	bool enabled;
	bool temp_en;
	uint64_t vco;
	uint8_t if_attn;
	union {
		struct hmc6300_attr tx;
		struct hmc6301_attr rx;
	};
};

struct hmc630x_dev;
struct hmc630x_vco;

/* Device driver init/remove API. */
int hmc630x_init(struct hmc630x_dev **dev, struct hmc630x_init_param *init);
int hmc630x_remove(struct hmc630x_dev *dev);
enum hmc630x_type hmc630x_type(struct hmc630x_dev *dev);

/* Register access API. */
int hmc630x_write_row(struct hmc630x_dev *dev, uint8_t row, uint8_t val);
int hmc630x_read_row(struct hmc630x_dev *dev, uint8_t row, uint8_t *val);
int hmc630x_write(struct hmc630x_dev *dev, uint16_t param, uint8_t val);
int hmc630x_read(struct hmc630x_dev *dev, uint16_t param, uint8_t *val);
int hmc630x_write_regmap(struct hmc630x_dev *dev, const uint8_t *regmap);
int hmc630x_read_regmap(struct hmc630x_dev *dev, uint8_t *regmap);

/* hmc6300/hmc6301 API. */
int hmc630x_set_temp_en(struct hmc630x_dev *dev, bool enable);
int hmc630x_get_temp_en(struct hmc630x_dev *dev, bool *enable);
int hmc630x_get_temp(struct hmc630x_dev *dev, uint8_t *temp);
int hmc630x_set_enable(struct hmc630x_dev *dev, bool enable);
int hmc630x_get_enable(struct hmc630x_dev *dev, bool *enable);
int hmc630x_set_if_attn(struct hmc630x_dev *dev, uint8_t attn);
int hmc630x_get_if_attn(struct hmc630x_dev *dev, uint8_t *attn);
int hmc630x_set_vco(struct hmc630x_dev *dev, uint64_t frequency);
int hmc630x_get_vco(struct hmc630x_dev *dev, uint64_t *frequency);
int hmc630x_get_avail_vco(struct hmc630x_dev *dev, const uint64_t **avail,
			  uint8_t *avail_num);

/* hmc6300-only API. */
int hmc6300_set_fm_en(struct hmc630x_dev *dev, bool enable);
int hmc6300_get_fm_en(struct hmc630x_dev *dev, bool *enable);
int hmc6300_set_rf_attn(struct hmc630x_dev *dev, uint8_t attn);
int hmc6300_get_rf_attn(struct hmc630x_dev *dev, uint8_t *attn);

/* hmc6301-only API. */
int hmc6301_set_lna_gain(struct hmc630x_dev *dev, enum hmc6301_lna_attn gain);
int hmc6301_get_lna_gain(struct hmc630x_dev *dev, enum hmc6301_lna_attn *gain);
int hmc6301_set_bb_attn(struct hmc630x_dev *dev, enum hmc6301_bb_attn attn1,
			enum hmc6301_bb_attn attn2);
int hmc6301_get_bb_attn(struct hmc630x_dev *dev, enum hmc6301_bb_attn *attn1,
			enum hmc6301_bb_attn *attn2);
int hmc6301_set_bb_attn_fine(struct hmc630x_dev *dev,
			     enum hmc6301_bb_attn_fine attn_i,
			     enum hmc6301_bb_attn_fine attn_q);
int hmc6301_get_bb_attn_fine(struct hmc630x_dev *dev,
			     enum hmc6301_bb_attn_fine *attn_i,
			     enum hmc6301_bb_attn_fine *attn_q);
int hmc6301_set_bb_lpc_hpc(struct hmc630x_dev *dev, enum hmc6301_bb_lpc lpc,
			   enum hmc6301_bb_hpc hpc);
int hmc6301_get_bb_lpc_hpc(struct hmc630x_dev *dev, enum hmc6301_bb_lpc *lpc,
			   enum hmc6301_bb_hpc *hpc);

#endif
