/***************************************************************************//**
 *   @file   hmc6300.h
 *   @brief  hmc6300 device driver.
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
#ifndef _HMC6300_H_
#define _HMC6300_H_

#include <stdbool.h>
#include <stdint.h>
#include "no_os_util.h"
#include "no_os_gpio.h"

#define HMC6300_PARAM(row, mask) ((((uint16_t)row) << 8) | (mask))
#define HMC6300_MASK(setting) ((setting) & 0xff)
#define HMC6300_ROW(setting) ((setting) >> 8)

/* ROW1 */
#define HMC6300_PA_SEL_VGBS HMC6300_PARAM(1, NO_OS_GENMASK(7, 4))
#define HMC6300_PA_SEL_VREF HMC6300_PARAM(1, NO_OS_GENMASK(3, 0))

/* ROW2 */
#define HMC6300_PA_SEL_ALC_DAC HMC6300_PARAM(2, NO_OS_GENMASK(7, 4))
#define HMC6300_PA_SEP_PA_PWRDWN_FAST HMC6300_PARAM(2, NO_OS_BIT(3))
#define HMC6300_PA_PWRDWN_FAST HMC6300_PARAM(2, NO_OS_BIT(2))
#define HMC6300_PA_SE_SEL HMC6300_PARAM(2, NO_OS_BIT(1))
#define HMC6300_POWER_DET_PWRDN HMC6300_PARAM(2, NO_OS_BIT(0))

/* ROW3 */
#define HMC6300_DRIVER_BIAS HMC6300_PARAM(3, NO_OS_GENMASK(7, 5))
#define HMC6300_DRIVER_BIAS2 HMC6300_PARAM(3, NO_OS_GENMASK(4, 2))
#define HMC6300_EN_IFMIX_HICG HMC6300_PARAM(3, NO_OS_BIT(1))
#define HMC6300_EN_TEMPFLASH HMC6300_PARAM(3, NO_OS_BIT(0))

/* ROW4 */
#define HMC6300_DRIVER_PWRDN HMC6300_PARAM(4, NO_OS_BIT(7))
#define HMC6300_UPMIXER_PWRDN HMC6300_PARAM(4, NO_OS_BIT(6))
#define HMC6300_IFVGA_PWRDN HMC6300_PARAM(4, NO_OS_BIT(5))
#define HMC6300_DIVIDER_PWRDN HMC6300_PARAM(4, NO_OS_BIT(4))
#define HMC6300_PA_PWRDN HMC6300_PARAM(4, NO_OS_BIT(3))
#define HMC6300_RFVGA_PWRDN HMC6300_PARAM(4, NO_OS_BIT(2))
#define HMC6300_TRIPLER_PWRDN HMC6300_PARAM(4, NO_OS_BIT(1))
#define HMC6300_IF_UPMIXER_PWRDN HMC6300_PARAM(4, NO_OS_BIT(0))

/* ROW5 */
#define HMC6300_TRIPLER_BIAS_HIGH HMC6300_PARAM(5, NO_OS_GENMASK(7, 0))

/* ROW6 */
#define HMC6300_TRIPLER_BIAS_LOW HMC6300_PARAM(6, NO_OS_GENMASK(7, 2))

/* ROW7 */
#define HMC6300_IFVGA_VGA_ADJ HMC6300_PARAM(7, NO_OS_GENMASK(7, 4))
#define HMC6300_IFVGA_TUNE HMC6300_PARAM(7, NO_OS_GENMASK(3, 0))

/* ROW8 */
#define HMC6300_IFVGA_BIAS HMC6300_PARAM(8, NO_OS_GENMASK(7, 4))
#define HMC6300_IF_UPMIXER_TUNE HMC6300_PARAM(8, NO_OS_GENMASK(3, 0))

/* ROW9*/
#define HMC6300_IFVGA_Q_CNTRL HMC6300_PARAM(9, NO_OS_GENMASK(7, 5))

/* ROW10 */
#define HMC6300_ENABLE_FM HMC6300_PARAM(10, NO_OS_BIT(7))
#define HMC6300_IF_REFSEL HMC6300_PARAM(10, NO_OS_BIT(6))
#define HMC6300_BG_MONITOR HMC6300_PARAM(10, NO_OS_BIT(5))
#define HMC6300_ENDIG_IFVGA_GAIN_CONTROL HMC6300_PARAM(10, NO_OS_BIT(4))
#define HMC6300_IPC_PWRDN HMC6300_PARAM(10, NO_OS_BIT(3))
#define HMC6300_IF_BGMUX_PWRDN HMC6300_PARAM(10, NO_OS_BIT(2))
#define HMC6300_UPMIX_CAL_PWRDN HMC6300_PARAM(10, NO_OS_BIT(1))
#define HMC6300_TEMPSENSOR_PWRDN HMC6300_PARAM(10, NO_OS_BIT(0))

/* ROW11 */
#define HMC6300_RFVGAGAIN HMC6300_PARAM(11, NO_OS_GENMASK(7, 4))
#define HMC6300_ENRFVGA_ANA HMC6300_PARAM(11, NO_OS_BIT(3))
#define HMC6300_RFVGA_ICTRL HMC6300_PARAM(11, NO_OS_GENMASK(2, 0))

/* ROW12 */
#define HMC6300_UPMIX_CAL HMC6300_PARAM(12, NO_OS_GENMASK(7, 0))

/* ROW16 */
#define HMC6300_BYP_SYNTH_LDO HMC6300_PARAM(16, NO_OS_BIT(7))
#define HMC6300_EN_CPSHORT HMC6300_PARAM(16, NO_OS_BIT(6))
#define HMC6300_EN_CPCMFB HMC6300_PARAM(16, NO_OS_BIT(5))
#define HMC6300_EN_CP_DUMP HMC6300_PARAM(16, NO_OS_BIT(4))
#define HMC6300_EN_CPTRIST HMC6300_PARAM(16, NO_OS_BIT(3))
#define HMC6300_EN_CP HMC6300_PARAM(16, NO_OS_BIT(2))
#define HMC6300_EN_SYNTH_LDO HMC6300_PARAM(16, NO_OS_BIT(1))
#define HMC6300_ENBAR_SYNTHBG HMC6300_PARAM(16, NO_OS_BIT(0))

/* ROW17 */
#define HMC6300_EN_LOCKD_CLK HMC6300_PARAM(17, NO_OS_BIT(7))
#define HMC6300_EN_TEST_DIVOUT HMC6300_PARAM(17, NO_OS_BIT(6))
#define HMC6300_EN_VTUNE_FLASH HMC6300_PARAM(17, NO_OS_BIT(5))
#define HMC6300_EN_REBUF_DC HMC6300_PARAM(17, NO_OS_BIT(4))
#define HMC6300_EN_REFBUF HMC6300_PARAM(17, NO_OS_BIT(3))
#define HMC6300_EN_STICK_DIV HMC6300_PARAM(17, NO_OS_BIT(2))
#define HMC6300_EN_FBDIV_CML2CMOS HMC6300_PARAM(17, NO_OS_BIT(1))
#define HMC6300_EN_FBDIV HMC6300_PARAM(17, NO_OS_BIT(0))

/* ROW18 */
#define HMC6300_EN_NB250M HMC6300_PARAM(18, NO_OS_BIT(6))
#define HMC6300_BYP_VCO_LDO HMC6300_PARAM(18, NO_OS_BIT(5))
#define HMC6300_EN_EXTLO HMC6300_PARAM(18, NO_OS_BIT(4))
#define HMC6300_EN_VCOPK HMC6300_PARAM(18, NO_OS_BIT(3))
#define HMC6300_EN_VCO HMC6300_PARAM(18, NO_OS_BIT(2))
#define HMC6300_EN_VCO_REG HMC6300_PARAM(18, NO_OS_BIT(1))
#define HMC6300_ENBAR_VCOGB HMC6300_PARAM(18, NO_OS_BIT(0))

/* ROW19 */
#define HMC6300_REFSEL_SYNTHBG HMC6300_PARAM(19, NO_OS_BIT(1))
#define HMC6300_MUXREF HMC6300_PARAM(19, NO_OS_BIT(0))

/* ROW20 */
#define HMC6300_FBDIV_CODE HMC6300_PARAM(20, NO_OS_GENMASK(6, 0))

/* ROW21 */
#define HMC6300_REFSEL_VCOBG HMC6300_PARAM(21, NO_OS_BIT(4))
#define HMC6300_VCO_BIASTRIM HMC6300_PARAM(21, NO_OS_GENMASK(3, 0))

/* ROW22 */
#define HMC6300_VCO_BANDSEL HMC6300_PARAM(22, NO_OS_GENMASK(6, 0))

/* ROW23 */
#define HMC6300_ICP_BIASTRIM HMC6300_PARAM(23, NO_OS_GENMASK(7, 5))
#define HMC6300_VCO_OFFSET HMC6300_PARAM(23, NO_OS_GENMASK(4, 0))

/* ROW24 */
#define HMC6300_LOCKDET HMC6300_PARAM(24, NO_OS_BIT(3))
#define HMC6300_DN HMC6300_PARAM(24, NO_OS_BIT(2))
#define HMC6300_UP HMC6300_PARAM(24, NO_OS_BIT(1))
#define HMC6300_CENTER HMC6300_PARAM(24, NO_OS_BIT(0))

/* ROW25 */
#define HMC6300_VTUNE_FLASHP HMC6300_PARAM(25, NO_OS_GENMASK(7, 0))

/* ROW26 */
#define HMC6300_VTUNE_FLASHN HMC6300_PARAM(26, NO_OS_GENMASK(7, 0))

/* ROW27 */
#define HMC6300_TEMPS HMC6300_PARAM(27, NO_OS_GENMASK(4, 0))

enum hmc6300_ref_clk {
	HMC6300_REF_CLK_71p42857MHz,
	HMC6300_REF_CLK_75MHz,
	HMC6300_REF_CLK_142p8571MHz,
	HMC6300_REF_CLK_154p2857MHz,
};

struct hmc6300_vco {
	uint64_t *freqs;
	uint8_t *fbdiv;
	uint8_t entries;
};

struct hmc6300_init_param {
	enum hmc6300_ref_clk ref_clk;
	struct no_os_gpio_init_param en;
	struct no_os_gpio_init_param clk;
	struct no_os_gpio_init_param data;
	struct no_os_gpio_init_param scanout;
};

struct hmc6300_dev;

int hmc6300_init(struct hmc6300_dev **dev, struct hmc6300_init_param *init);
int hmc6300_remove(struct hmc6300_dev *dev);

int hmc6300_write_row(struct hmc6300_dev *dev, uint8_t row, uint8_t val);
int hmc6300_read_row(struct hmc6300_dev *dev, uint8_t row, uint8_t *val);
int hmc6300_write(struct hmc6300_dev *dev, uint16_t param, uint8_t val);
int hmc6300_read(struct hmc6300_dev *dev, uint16_t param, uint8_t *val);
int hmc6300_write_regmap(struct hmc6300_dev *dev, const uint8_t *regmap);
int hmc6300_read_regmap(struct hmc6300_dev *dev, uint8_t *regmap);

int hmc6300_enable_temp(struct hmc6300_dev *dev, bool enable);
int hmc6300_get_temp(struct hmc6300_dev *dev, uint8_t *temp);
int hmc6300_enable_fm(struct hmc6300_dev *dev, bool enable);
int hmc6300_enable(struct hmc6300_dev *dev, bool enable);
int hmc6300_set_if_attn(struct hmc6300_dev *dev, uint8_t attn);
int hmc6300_set_rf_attn(struct hmc6300_dev *dev, uint8_t attn);
int hmc6300_set_vco(struct hmc6300_dev *dev, uint64_t frequency);

#endif
