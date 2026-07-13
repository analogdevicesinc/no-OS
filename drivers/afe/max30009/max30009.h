/***************************************************************************//**
 *   @file   max30009.h
 *   @brief  Header file for MAX30009 driver
 *   @author Edelweise Escala (edelweise.escala@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#ifndef _MAX30009_H_
#define _MAX30009_H_

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#define MAX30009_REG_STATUS1                    0x00
#define   MAX30009_STATUS1_A_FULL_MSK           NO_OS_BIT(7)
#define   MAX30009_STATUS1_FIFO_DATA_RDY_MSK    NO_OS_BIT(5)
#define   MAX30009_STATUS1_FREQ_UNLOCK_MSK      NO_OS_BIT(4)
#define   MAX30009_STATUS1_FREQ_LOCK_MSK        NO_OS_BIT(3)
#define   MAX30009_STATUS1_PHASE_UNLOCK_MSK     NO_OS_BIT(2)
#define   MAX30009_STATUS1_PHASE_LOCK_MSK       NO_OS_BIT(1)
#define   MAX30009_STATUS1_PWR_RDY_MSK          NO_OS_BIT(0)

#define MAX30009_REG_STATUS2                    0x01
#define   MAX30009_STATUS2_LON_MSK              NO_OS_BIT(7)
#define   MAX30009_STATUS2_BIOZ_OVER_MSK        NO_OS_BIT(6)
#define   MAX30009_STATUS2_BIOZ_UNDR_MSK        NO_OS_BIT(5)
#define   MAX30009_STATUS2_DRV_OOR_MSK          NO_OS_BIT(4)
#define   MAX30009_STATUS2_DC_LOFF_PH_MSK       NO_OS_BIT(3)
#define   MAX30009_STATUS2_DC_LOFF_PL_MSK       NO_OS_BIT(2)
#define   MAX30009_STATUS2_DC_LOFF_NH_MSK       NO_OS_BIT(1)
#define   MAX30009_STATUS2_DC_LOFF_NL_MSK       NO_OS_BIT(0)

#define MAX30009_REG_FIFO_WRITE_POINTER         0x08
#define MAX30009_REG_FIFO_READ_POINTER          0x09

#define MAX30009_REG_FIFO_COUNTER1              0x0A
#define   MAX30009_FIFO_DATA_COUNT_MSB_MSK      NO_OS_BIT(7)
#define   MAX30009_OVF_COUNTER_MSK              NO_OS_GENMASK(6, 0)

#define MAX30009_REG_FIFO_COUNTER2              0x0B
#define   MAX30009_FIFO_DATA_COUNT_LSB_MSK      NO_OS_GENMASK(7, 0)

#define MAX30009_REG_FIFO_DATA_REGISTER         0x0C
#define   MAX30009_FIFO_DATA_MSK                NO_OS_GENMASK(7, 0)

#define MAX30009_REG_FIFO_CONFIGURATION1        0x0D
#define   MAX30009_FIFO_A_FULL_MSK              NO_OS_GENMASK(7, 0)

#define MAX30009_REG_FIFO_CONFIGURATION2        0x0E
#define   MAX30009_FIFO_MARK_MSK                NO_OS_BIT(5)
#define   MAX30009_FLUSH_FIFO_MSK               NO_OS_BIT(4)
#define   MAX30009_FIFO_STAT_CLR_MSK            NO_OS_BIT(3)
#define   MAX30009_A_FULL_TYPE_MSK              NO_OS_BIT(2)
#define   MAX30009_FIFO_RO_MSK                  NO_OS_BIT(1)

#define MAX30009_REG_SYSTEM_SYNC                0x10
#define   MAX30009_TIMING_SYS_RESET_MSK         NO_OS_BIT(7)

#define MAX30009_REG_SYSTEM_CONFIGURATION1      0x11
#define   MAX30009_MASTER_MSK                   NO_OS_BIT(7)
#define   MAX30009_DISABLE_I2C_MSK              NO_OS_BIT(6)
#define   MAX30009_SHDN_MSK                     NO_OS_BIT(1)
#define   MAX30009_RESET_MSK                    NO_OS_BIT(0)

#define MAX30009_REG_PIN_FUNC_CONFIGURATION     0x12
#define   MAX30009_INT_FCFG_MSK                 NO_OS_GENMASK(3, 2)
#define   MAX30009_TRIG_ICFG_MSK                NO_OS_BIT(0)

#define MAX30009_REG_OUTPUT_PIN_CONFIGURATION   0x13
#define   MAX30009_INT_OCFG_MSK                 NO_OS_GENMASK(3, 2)
#define   MAX30009_TRIG_OCFG_MSK                NO_OS_GENMASK(1, 0)

#define MAX30009_REG_I2C_BROADCAST_ADDRESS      0x14
#define   MAX30009_I2C_BCAST_ADDR_MSK           NO_OS_GENMASK(6, 1)
#define   MAX30009_I2C_BCAST_EN_MSK             NO_OS_BIT(0)

#define MAX30009_REG_PLL_CONFIGURATION1         0x17
#define   MAX30009_MDIV_MSB_MSK                 NO_OS_GENMASK(7, 6)
#define   MAX30009_NDIV_MSK                     NO_OS_BIT(5)
#define   MAX30009_KDIV_MSK                     NO_OS_GENMASK(4, 1)
#define   MAX30009_PLL_EN_MSK                   NO_OS_BIT(0)

#define MAX30009_REG_PLL_CONFIGURATION2         0x18
#define   MAX30009_MDIV_LSB_MSK                 NO_OS_GENMASK(7, 0)

#define MAX30009_REG_PLL_CONFIGURATION3         0x19
#define   MAX30009_PLL_LOCK_WNDW_MSK            NO_OS_BIT(0)

#define MAX30009_REG_PLL_CONFIGURATION4         0x1A
#define   MAX30009_REF_CLK_SEL_MSK              NO_OS_BIT(6)
#define   MAX30009_CLK_FREQ_SEL_MSK             NO_OS_BIT(5)
#define   MAX30009_CLK_FINE_TUNE_MSK            NO_OS_GENMASK(4, 0)

#define MAX30009_REG_BIOZ_CONFIGURATION1        0x20
#define   MAX30009_BIOZ_DAC_OSR_MSK             NO_OS_GENMASK(7, 6)
#define   MAX30009_BIOZ_ADC_OSR_MSK             NO_OS_GENMASK(5, 3)
#define   MAX30009_BIOZ_BG_EN_MSK               NO_OS_BIT(2)
#define   MAX30009_BIOZ_Q_EN_MSK                NO_OS_BIT(1)
#define   MAX30009_BIOZ_I_EN_MSK                NO_OS_BIT(0)

#define MAX30009_REG_BIOZ_CONFIGURATION2        0x21
#define   MAX30009_BIOZ_DHPF_MSK                NO_OS_GENMASK(7, 6)
#define   MAX30009_BIOZ_DLPF_MSK                NO_OS_GENMASK(5, 3)
#define   MAX30009_BIOZ_CMP_MSK                 NO_OS_GENMASK(2, 1)
#define   MAX30009_EN_BIOZ_THRESH_MSK           NO_OS_BIT(0)

#define MAX30009_REG_BIOZ_CONFIGURATION3        0x22
#define   MAX30009_BIOZ_EXT_RES_MSK             NO_OS_BIT(7)
#define   MAX30009_LOFF_RAPID_MSK               NO_OS_BIT(6)
#define   MAX30009_BIOZ_VDRV_MAG_MSK            NO_OS_GENMASK(5, 4)
#define   MAX30009_BIOZ_IDRV_RGE_MSK            NO_OS_GENMASK(3, 2)
#define   MAX30009_BIOZ_DRV_MODE_MSK            NO_OS_GENMASK(1, 0)

#define MAX30009_REG_BIOZ_CONFIGURATION4        0x23
#define   MAX30009_BIOZ_FAST_MANUAL_MSK         NO_OS_BIT(1)
#define   MAX30009_BIOZ_FAST_START_EN_MSK       NO_OS_BIT(0)

#define MAX30009_REG_BIOZ_CONFIGURATION5        0x24
#define   MAX30009_BIOZ_AHPF_MSK                NO_OS_GENMASK(7, 4)
#define   MAX30009_BIOZ_INA_MODE_MSK            NO_OS_BIT(3)
#define   MAX30009_BIOZ_DM_DIS_MSK              NO_OS_BIT(2)
#define   MAX30009_BIOZ_GAIN_MSK                NO_OS_GENMASK(1, 0)

#define MAX30009_REG_BIOZ_CONFIGURATION6        0x25
#define   MAX30009_BIOZ_EXT_CAP_MSK             NO_OS_BIT(7)
#define   MAX30009_BIOZ_DC_RESTORE_MSK          NO_OS_BIT(6)
#define   MAX30009_BIOZ_DRV_RESET_MSK           NO_OS_BIT(5)
#define   MAX30009_BIOZ_DAC_RESET_MSK           NO_OS_BIT(4)
#define   MAX30009_BIOZ_AMP_RGE_MSK             NO_OS_GENMASK(3, 2)
#define   MAX30009_BIOZ_AMP_BW_MSK              NO_OS_GENMASK(1, 0)

#define MAX30009_REG_BIOZ_LOW_THRESHOLD         0x26
#define   MAX30009_BIOZ_LO_THRESH_MSK           NO_OS_GENMASK(7, 0)

#define MAX30009_REG_BIOZ_HIGH_THRESHOLD        0x27
#define   MAX30009_BIOZ_HI_THRESH_MSK           NO_OS_GENMASK(7, 0)

#define MAX30009_REG_BIOZ_CONFIGURATION7        0x28
#define   MAX30009_BIOZ_STBYON_MSK              NO_OS_BIT(4)
#define   MAX30009_BIOZ_Q_CLK_PHASE_MSK         NO_OS_BIT(3)
#define   MAX30009_BIOZ_I_CLK_PHASE_MSK         NO_OS_BIT(2)
#define   MAX30009_BIOZ_INA_CHOP_EN_MSK         NO_OS_BIT(1)
#define   MAX30009_BIOZ_CH_FSEL_MSK             NO_OS_BIT(0)

#define MAX30009_REG_BIOZ_MUX_CONFIGURATION1    0x41
#define   MAX30009_BMUX_RSEL_MSK                NO_OS_GENMASK(7, 6)
#define   MAX30009_BMUX_BIST_EN_MSK             NO_OS_BIT(5)
#define   MAX30009_CONNECT_CAL_ONLY_MSK         NO_OS_BIT(2)
#define   MAX30009_MUX_EN_MSK                   NO_OS_BIT(1)
#define   MAX30009_CAL_EN_MSK                   NO_OS_BIT(0)

#define MAX30009_REG_BIOZ_MUX_CONFIGURATION2    0x42
#define   MAX30009_GSR_RSEL_MSK                 NO_OS_GENMASK(7, 6)
#define   MAX30009_GSR_LOAD_EN_MSK              NO_OS_BIT(5)
#define   MAX30009_EN_EXT_INLOAD_MSK            NO_OS_BIT(1)
#define   MAX30009_EN_INT_INLOAD_MSK            NO_OS_BIT(0)

#define MAX30009_REG_BIOZ_MUX_CONFIGURATION3    0x43
#define   MAX30009_BIP_ASSIGN_MSK               NO_OS_GENMASK(7, 6)
#define   MAX30009_BIN_ASSIGN_MSK               NO_OS_GENMASK(5, 4)
#define   MAX30009_DRVP_ASSIGN_MSK              NO_OS_GENMASK(3, 2)
#define   MAX30009_DRVN_ASSIGN_MSK              NO_OS_GENMASK(1, 0)

#define MAX30009_REG_BIOZ_MUX_CONFIGURATION4    0x44
#define   MAX30009_BIST_R_ERR_MSK               NO_OS_GENMASK(7, 0)

#define MAX30009_REG_DC_LEADS_CONFIGURATION     0x50
#define   MAX30009_EN_LON_DET_MSK               NO_OS_BIT(7)
#define   MAX30009_EN_LOFF_DET_MSK              NO_OS_BIT(6)
#define   MAX30009_EN_EXT_LOFF_MSK              NO_OS_BIT(5)
#define   MAX30009_EN_DRV_OOR_MSK               NO_OS_BIT(4)
#define   MAX30009_LOFF_IPOL_MSK                NO_OS_BIT(3)
#define   MAX30009_LOFF_IMAG_MSK                NO_OS_GENMASK(2, 0)

#define MAX30009_REG_DC_LEAD_DETECT_THRESHOLD   0x51
#define   MAX30009_LOFF_THRESH_MSK              NO_OS_GENMASK(3, 0)

#define MAX30009_REG_LEAD_BIAS_CONFIGURATION1   0x58
#define   MAX30009_RBIAS_VAL_MSK                NO_OS_GENMASK(3, 2)
#define   MAX30009_EN_RBIAS_BIP_MSK             NO_OS_BIT(1)
#define   MAX30009_EN_RBIAS_BIN_MSK             NO_OS_BIT(0)

#define MAX30009_REG_INTERRUPT_ENABLE1          0x80
#define   MAX30009_A_FULL_EN_MSK                NO_OS_BIT(7)
#define   MAX30009_FIFO_DATA_RDY_EN_MSK         NO_OS_BIT(5)
#define   MAX30009_FREQ_UNLOCK_EN_MSK           NO_OS_BIT(4)
#define   MAX30009_FREQ_LOCK_EN_MSK             NO_OS_BIT(3)
#define   MAX30009_PHASE_UNLOCK_EN_MSK          NO_OS_BIT(2)
#define   MAX30009_PHASE_LOCK_EN_MSK            NO_OS_BIT(1)

#define MAX30009_REG_INTERRUPT_ENABLE2          0x81
#define   MAX30009_LON_EN_MSK                   NO_OS_BIT(7)
#define   MAX30009_BIOZ_OVER_EN_MSK             NO_OS_BIT(6)
#define   MAX30009_BIOZ_UNDR_EN_MSK             NO_OS_BIT(5)
#define   MAX30009_DRV_OOR_EN_MSK               NO_OS_BIT(4)
#define   MAX30009_DC_LOFF_PH_EN_MSK            NO_OS_BIT(3)
#define   MAX30009_DC_LOFF_PL_EN_MSK            NO_OS_BIT(2)
#define   MAX30009_DC_LOFF_NH_EN_MSK            NO_OS_BIT(1)
#define   MAX30009_DC_LOFF_NL_EN_MSK            NO_OS_BIT(0)

#define MAX30009_REG_PART_ID                    0xFF

#define MAX30009_I2C_ADDRESS                    0x68
#define MAX30009_PART_ID_VALUE                  0x42
#define MAX30009_FIFO_DEPTH                     256
#define MAX30009_FIFO_DATA_SIZE                 3
#define MAX30009_PLL_LOCK_TIMEOUT_MS            1000

#define MAX30009_REF_CLK_32768_HZ               32768
#define MAX30009_REF_CLK_32000_HZ               32000

/* FIFO sample decode constants (24-bit sample: TAG[23:20], DATA[19:0]) */
#define MAX30009_FIFO_TAG_SHIFT                 20
#define MAX30009_FIFO_TAG_MASK                  NO_OS_GENMASK(3, 0)
#define MAX30009_FIFO_DATA_MASK                 NO_OS_GENMASK(19, 0)
#define MAX30009_FIFO_SIGN_BIT                  NO_OS_BIT(19)
#define MAX30009_FIFO_SIGN_EXT                  0xFFF00000UL

enum max30009_bioz_gain {
	MAX30009_BIOZ_GAIN_1,
	MAX30009_BIOZ_GAIN_2,
	MAX30009_BIOZ_GAIN_5,
	MAX30009_BIOZ_GAIN_10,
};

enum max30009_bioz_drv_mode {
	MAX30009_CURRENT_DRV_MODE,
	MAX30009_VOLTAGE_DRV_MODE,
	MAX30009_H_BRIDGE_DRV_MODE,
	MAX30009_STANDBY_MODE,
};

enum max30009_bioz_bw {
	MAX30009_BIOZ_LOW_BW,
	MAX30009_BIOZ_MEDIUM_LOW_BW,
	MAX30009_BIOZ_MEDIUM_HIGH_BW,
	MAX30009_BIOZ_HIGH_BW
};

enum max30009_dac_osr {
	MAX30009_DAC_OSR_32,
	MAX30009_DAC_OSR_64,
	MAX30009_DAC_OSR_128,
	MAX30009_DAC_OSR_256,
};

enum max30009_adc_osr {
	MAX30009_ADC_OSR_8,
	MAX30009_ADC_OSR_16,
	MAX30009_ADC_OSR_32,
	MAX30009_ADC_OSR_64,
	MAX30009_ADC_OSR_128,
	MAX30009_ADC_OSR_256,
	MAX30009_ADC_OSR_512,
	MAX30009_ADC_OSR_1024,
};

enum max30009_dhpf {
	MAX30009_DHPF_BYPASS,
	MAX30009_DHPF_0_00025_SR,
	MAX30009_DHPF_0_002_SR,
};

enum max30009_dlpf {
	MAX30009_DLPF_BYPASS,
	MAX30009_DLPF_0_005_SR,
	MAX30009_DLPF_0_02_SR,
	MAX30009_DLPF_0_08_SR,
	MAX30009_DLPF_0_25_SR,
};

enum max30009_ahpf {
	MAX30009_AHPF_100HZ,
	MAX30009_AHPF_200HZ,
	MAX30009_AHPF_500HZ,
	MAX30009_AHPF_1KHZ,
	MAX30009_AHPF_2KHZ,
	MAX30009_AHPF_5KHZ,
	MAX30009_AHPF_10KHZ,
	MAX30009_AHPF_BYPASS,
	MAX30009_AHPF_42M4K_OHM,
	MAX30009_AHPF_21M2K_OHM,
	MAX30009_AHPF_8M4K_OHM,
	MAX30009_AHPF_4M2K_OHM,
	MAX30009_AHPF_2M2K_OHM,
	MAX30009_AHPF_848K_OHM,
};

enum max30009_drive_mode {
	MAX30009_DRIVE_SINE_CURRENT,
	MAX30009_DRIVE_SINE_VOLTAGE,
	MAX30009_DRIVE_HBRIDGE,
	MAX30009_DRIVE_STANDBY,
};

enum max30009_bia_rsel {
	MAX30009_BIA_5100_OHM,
	MAX30009_BIA_900_OHM,
	MAX30009_BIA_600_OHM,
	MAX30009_BIA_280_OHM,
};

enum max30009_gsr_rsel {
	MAX30009_GSR_25K7_OHM,
	MAX30009_GSR_101K_OHM,
	MAX30009_GSR_505K_OHM,
	MAX30009_GSR_1M_OHM,
};

enum max30009_loff_current {
	MAX30009_LOFF_DISABLE,
	MAX30009_LOFF_5NA,
	MAX30009_LOFF_10NA,
	MAX30009_LOFF_20NA,
	MAX30009_LOFF_50NA,
	MAX30009_LOFF_100NA,
};

enum max30009_rbias_val {
	MAX30009_RBIAS_50M_OHM,
	MAX30009_RBIAS_100M_OHM,
	MAX30009_RBIAS_200M_OHM,
	MAX30009_RBIAS_RESERVED,
};

enum max30009_pll_ndiv {
	MAX30009_NDIV_512,
	MAX30009_NDIV_1024
};

enum max30009_pll_kdiv {
	MAX30009_KDIV_1,
	MAX30009_KDIV_2,
	MAX30009_KDIV_4,
	MAX30009_KDIV_8,
	MAX30009_KDIV_16,
	MAX30009_KDIV_32,
	MAX30009_KDIV_64,
	MAX30009_KDIV_128,
	MAX30009_KDIV_256,
	MAX30009_KDIV_512,
	MAX30009_KDIV_1024,
	MAX30009_KDIV_2048,
	MAX30009_KDIV_4096,
	MAX30009_KDIV_8192,
};

struct max30009_pll_config {
	uint16_t mdiv;
	enum max30009_pll_ndiv ndiv;
	enum max30009_pll_kdiv kdiv;
	bool pll_lock_wndw;
	bool use_external_clk;
	bool use_32768_hz;
	uint8_t clk_fine_tune;
	bool pll_enabled;
};

struct max30009_fifo_config {
	uint8_t watermark;
	bool fifo_rollover;
	bool a_full_type;
	bool fifo_stat_clr;
};

struct max30009_bioz_config {
	enum max30009_dac_osr dac_osr;
	enum max30009_adc_osr adc_osr;
	enum max30009_dhpf dhpf;
	enum max30009_dlpf dlpf;
	uint8_t cmp_mode;
	bool en_bioz_thresh;
	bool ext_res;
	bool loff_rapid;
	uint8_t vdrv_mag;
	uint8_t idrv_rge;
	enum max30009_drive_mode drv_mode;
	bool fast_manual;
	bool fast_start_en;
	enum max30009_ahpf ahpf;
	bool ina_mode;
	bool dm_dis;
	enum max30009_bioz_gain gain;
	bool ext_cap;
	bool dc_restore;
	bool drv_reset;
	bool dac_reset;
	uint8_t amp_rge;
	uint8_t amp_bw;
	uint8_t lo_thresh;
	uint8_t hi_thresh;
	bool stbyon;
	bool q_clk_phase;
	bool i_clk_phase;
	bool ina_chop_en;
	bool ch_fsel;
};

struct max30009_bioz_mux_config {
	enum max30009_bia_rsel bmux_rsel;
	bool bmux_bist_en;
	bool connect_cal_only;
	bool mux_en;
	bool cal_en;
	enum max30009_gsr_rsel bmux_gsr_rsel;
	bool gsr_load_en;
	bool en_ext_inload;
	bool en_int_inload;
	uint8_t bip_assign;
	uint8_t bin_assign;
	uint8_t drvp_assign;
	uint8_t drvn_assign;
};

struct max30009_lead_detect_config {
	bool en_lon_det;
	bool en_loff_det;
	bool en_ext_loff;
	bool en_drv_oor;
	bool loff_ipol;
	enum max30009_loff_current loff_imag;
	uint8_t loff_thresh;
	enum max30009_rbias_val rbias_value;
	bool en_rbias_bip;
	bool en_rbias_bin;
};

struct max30009_dev {
	struct no_os_i2c_desc *i2c_desc;
	struct no_os_spi_desc *spi_desc;
	struct no_os_gpio_desc *int_gpio;
	struct no_os_gpio_desc *csb_gpio_desc;
	struct max30009_pll_config pll_config;
	struct max30009_fifo_config fifo_config;
	struct max30009_bioz_config bioz_config;
	struct max30009_bioz_mux_config bioz_mux_config;
	struct max30009_lead_detect_config lead_detect_config;
	uint32_t ref_clk_freq;
	uint8_t *fifo_read_buf;
};

struct max30009_init_param {
	bool use_i2c;
	struct no_os_i2c_init_param i2c_init;
	struct no_os_spi_init_param spi_init;
	struct no_os_gpio_init_param *int_gpio_init;
	struct no_os_gpio_init_param csb_gpio_init;
};

struct max30009_bioz_data {
	int32_t i_data;
	int32_t q_data;
};

struct max30009_status {
	bool a_full;
	bool fifo_data_rdy;
	bool freq_unlock;
	bool freq_lock;
	bool phase_unlock;
	bool phase_lock;
	bool pwr_rdy;
	bool leads_on;
	bool bioz_over;
	bool bioz_undr;
	bool drv_oor;
};

struct max30009_cal_coeffs {
	float i_offset;
	float q_offset;
	float i_coef;
	float q_coef;
	float i_phase_coef;
	float q_phase_coef;
};

struct max30009_lead_status {
	bool leads_on;
	bool dc_loff_ph;
	bool dc_loff_pl;
	bool dc_loff_nh;
	bool dc_loff_nl;
	bool drv_oor;
	bool bioz_over;
	bool bioz_undr;
};

enum max30009_interrupt {
	MAX30009_INT_A_FULL,
	MAX30009_INT_FIFO_DATA_RDY,
	MAX30009_INT_FREQ_UNLOCK,
	MAX30009_INT_FREQ_LOCK,
	MAX30009_INT_PHASE_UNLOCK,
	MAX30009_INT_PHASE_LOCK,
	MAX30009_INT_LON,
	MAX30009_INT_BIOZ_OVER,
	MAX30009_INT_BIOZ_UNDR,
	MAX30009_INT_DRV_OOR,
	MAX30009_INT_DC_LOFF_PH,
	MAX30009_INT_DC_LOFF_PL,
	MAX30009_INT_DC_LOFF_NH,
	MAX30009_INT_DC_LOFF_NL,
};

enum max30009_fifo_sample_type {
	MAX30009_FIFO_SAMPLE_IN_PHASE = 0x1,
	MAX30009_FIFO_SAMPLE_QUADRATURE = 0x2,
	MAX30009_FIFO_SAMPLE_MARKER = 0xE,
	MAX30009_FIFO_SAMPLE_INVALID = 0xF,
};

struct max30009_fifo_sample {
	enum max30009_fifo_sample_type type;
	int32_t data;
};

/** Initialize the MAX30009 device */
int max30009_init(struct max30009_dev **device,
		  struct max30009_init_param *init_param);

/** Remove the MAX30009 device and free resources */
int max30009_remove(struct max30009_dev *device);

/** Read multiple bytes from register */
int max30009_reg_read_multiple(struct max30009_dev *device, uint8_t reg_addr,
			       uint8_t *data, uint32_t len);

/** Read a single byte from register */
int max30009_reg_read(struct max30009_dev *device, uint8_t reg_addr,
		      uint8_t *data);

/** Write multiple bytes to register */
int max30009_reg_write_multiple(struct max30009_dev *device, uint8_t reg_addr,
				uint8_t *data, uint32_t len);

/** Write a single byte to register */
int max30009_reg_write(struct max30009_dev *device, uint8_t reg_addr,
		       uint8_t data);

/** Update specific bits in a register */
int max30009_reg_update(struct max30009_dev *device, uint8_t reg_addr,
			uint8_t mask, uint8_t value);

/** Perform software reset */
int max30009_soft_reset(struct max30009_dev *device);

/** Read device part ID */
int max30009_verify_part_id(struct max30009_dev *device);

/** Set power mode - shutdown or active */
int max30009_set_power_mode(struct max30009_dev *device, bool shutdown);

/** Get device status */
int max30009_get_status(struct max30009_dev *device,
			struct max30009_status *status);

/** Check lead-off detection status */
int max30009_check_lead_off(struct max30009_dev *device, bool *leads_on);

/** Flush FIFO contents */
int max30009_fifo_flush(struct max30009_dev *device);

/** Get FIFO data count */
int max30009_fifo_get_count(struct max30009_dev *device, uint16_t *count);

/** Get FIFO overflow data count */
int max30009_fifo_get_ovf_count(struct max30009_dev *device, uint16_t *count);

/** Read data from FIFO */
int max30009_read_fifo_data(struct max30009_dev *device, uint8_t *data,
			    uint32_t len);

/** Decode a 3-byte FIFO sample into type and data */
int max30009_fifo_decode_sample(uint8_t *raw_data,
				struct max30009_fifo_sample *sample);

/** Get FIFO read and write pointers */
int max30009_get_fifo_pointers(struct max30009_dev *device,
			       uint8_t *write_ptr,
			       uint8_t *read_ptr);

/** Set FIFO almost-full threshold */
int max30009_set_fifo_watermark(struct max30009_dev *device,
				uint8_t watermark);

/** Gets the FIFO watermark level of the device. */
int max30009_get_fifo_watermark(struct max30009_dev *desc, uint8_t *watermark);

/** Set FIFO rollover mode */
int max30009_set_fifo_rollover(struct max30009_dev *device, bool rollover);

/** Set almost-full interrupt type */
int max30009_set_a_full_type(struct max30009_dev *device, bool type);

/** Insert marker into FIFO data stream */
int max30009_insert_fifo_marker(struct max30009_dev *device);

/** Clear FIFO status flags */
int max30009_clear_fifo_status(struct max30009_dev *device);

/** Trigger timing system reset for PLL synchronization */
int max30009_timing_system_reset(struct max30009_dev *device);

/** Set master/slave mode for PLL synchronization */
int max30009_set_master_mode(struct max30009_dev *device, bool is_master);

/** Configure INT pin function and output */
int max30009_configure_int_pin(struct max30009_dev *device,
			       uint8_t func_cfg,
			       uint8_t out_cfg);

/** Configure TRIG pin input and output */
int max30009_configure_trig_pin(struct max30009_dev *device,
				uint8_t input_cfg,
				uint8_t output_cfg);

/** Configure I2C broadcast address */
int max30009_configure_i2c_broadcast(struct max30009_dev *device,
				     bool enable,
				     uint8_t broadcast_addr);

/** Disable I2C interface - SPI only mode */
int max30009_disable_i2c(struct max30009_dev *device);

/** Configure PLL dividers */
int max30009_pll_set_dividers(struct max30009_dev *device,
			      uint16_t mdiv,
			      enum max30009_pll_ndiv ndiv,
			      enum max30009_pll_kdiv kdiv);

/** Enable or disable PLL */
int max30009_pll_enable(struct max30009_dev *device, bool enable);

/** Set PLL lock window */
int max30009_pll_set_lock_window(struct max30009_dev *device, bool lock_window);

/** Configure clock source */
int max30009_set_clock_source(struct max30009_dev *device, bool use_external,
			      bool use_32768_hz);

/** Fine tune internal clock frequency */
int max30009_clock_fine_tune(struct max30009_dev *device, uint8_t tune_val);

/** Enable or disable BioZ measurement */
int max30009_bioz_enable(struct max30009_dev *device, bool enable);

/** Enable or disable BioZ I channel */
int max30009_bioz_i_enable(struct max30009_dev *device, bool enable);

/** Enable or disable BioZ Q channel */
int max30009_bioz_q_enable(struct max30009_dev *device, bool enable);

/** Enable or disable BioZ bandgap */
int max30009_bioz_bg_enable(struct max30009_dev *device, bool enable);

/** Set DAC oversampling ratio */
int max30009_set_dac_osr(struct max30009_dev *device,
			 enum max30009_dac_osr osr);

/** Set ADC oversampling ratio */
int max30009_set_adc_osr(struct max30009_dev *device,
			 enum max30009_adc_osr osr);

/** Set digital high-pass filter */
int max30009_set_digital_hpf(struct max30009_dev *device,
			     enum max30009_dhpf dhpf);

/** Set digital low-pass filter */
int max30009_set_digital_lpf(struct max30009_dev *device,
			     enum max30009_dlpf dlpf);

/** Set analog high-pass filter */
int max30009_set_analog_hpf(struct max30009_dev *device,
			    enum max30009_ahpf ahpf);

/** Set drive mode - current, voltage, H-bridge, or standby */
int max30009_set_drive_mode(struct max30009_dev *device,
			    enum max30009_drive_mode mode);

/** Set drive current with detailed control */
int max30009_set_drive_current_detailed(struct max30009_dev *device,
					uint8_t idrv_range,
					uint8_t vdrv_mag);

/** Enable or disable external resistor for current drive */
int max30009_enable_external_resistor(struct max30009_dev *device, bool enable);

/** Enable or disable rapid lead-off detection */
int max30009_enable_rapid_lead_off(struct max30009_dev *device, bool enable);

/** Enable or disable fast start mode */
int max30009_enable_fast_start(struct max30009_dev *device,
			       bool enable,
			       bool manual);

/** Set INA (Instrumentation Amplifier) mode */
int max30009_set_ina_mode(struct max30009_dev *device, bool low_power);

/** Disable differential mode - enable single-ended */
int max30009_disable_differential_mode(struct max30009_dev *device,
				       bool disable);

/** Set amplifier range */
int max30009_set_amp_range(struct max30009_dev *device, uint8_t amp_range);

/** Set amplifier bandwidth */
int max30009_set_amp_bandwidth(struct max30009_dev *device, uint8_t amp_bw);

/** Enable or disable external capacitor */
int max30009_enable_external_cap(struct max30009_dev *device, bool enable);

/** Enable or disable DC restore circuit */
int max30009_enable_dc_restore(struct max30009_dev *device, bool enable);

/** Enable or disable drive reset */
int max30009_enable_drive_reset(struct max30009_dev *device, bool enable);

/** Enable or disable DAC reset */
int max30009_enable_dac_reset(struct max30009_dev *device, bool enable);

/** Set comparison mode for threshold detection */
int max30009_set_comparison_mode(struct max30009_dev *device, uint8_t cmp_mode);

/** Enable or disable BioZ threshold detection */
int max30009_enable_bioz_threshold(struct max30009_dev *device, bool enable);

/** Set BioZ threshold values */
int max30009_set_bioz_thresholds(struct max30009_dev *device,
				 uint8_t low_thresh, uint8_t high_thresh);

/** Set demodulation clock phases */
int max30009_set_demod_clk_phases(struct max30009_dev *device,
				  bool i_clk_phase, bool q_clk_phase);

/** Enable or disable INA chopping */
int max30009_enable_ina_chopping(struct max30009_dev *device, bool enable);

/** Set channel frequency select */
int max30009_set_channel_freq_select(struct max30009_dev *device, bool fsel);

/** Enable or disable standby mode */
int max30009_enable_standby_mode(struct max30009_dev *device,
				 bool keep_rx_on);

/** Enable or disable calibration mode */
int max30009_enable_calibration(struct max30009_dev *device, bool enable);

/** Enable or disable calibration MUX */
int max30009_enable_calibration_mux(struct max30009_dev *device, bool enable);

/** Set calibration connect-only mode */
int max30009_calibration_connect_only(struct max30009_dev *device,
				      bool cal_only);

/** Set BIA Body Impedance Analysis load resistor */
int max30009_set_bia_load_resistor(struct max30009_dev *device,
				   enum max30009_bia_rsel rsel);

/** Enable or disable BIA BIST (Built-In Self Test) */
int max30009_enable_bia_bist(struct max30009_dev *device, bool enable);

/** Set GSR (Galvanic Skin Response) load resistor */
int max30009_set_gsr_load_resistor(struct max30009_dev *device,
				   enum max30009_gsr_rsel rsel);

/** Enable or disable GSR load resistor */
int max30009_enable_gsr_load(struct max30009_dev *device, bool enable);

/** Enable or disable internal load resistor */
int max30009_enable_internal_load(struct max30009_dev *device, bool enable);

/** Enable or disable external load connection */
int max30009_enable_external_load(struct max30009_dev *device, bool enable);

/** Assign electrodes to measurement channels */
int max30009_assign_electrodes(struct max30009_dev *device,
			       uint8_t bip_assign,
			       uint8_t bin_assign,
			       uint8_t drvp_assign,
			       uint8_t drvn_assign);

/** Read BIST error value */
int max30009_read_bist_error(struct max30009_dev *device, uint8_t *error);

/** Configure lead-off detection */
int max30009_lead_off_config(struct max30009_dev *device, bool enable,
			     uint8_t threshold);

/** Set lead-off detection current magnitude */
int max30009_set_lead_off_current(struct max30009_dev *device,
				  enum max30009_loff_current current);

/** Set lead-off detection current polarity */
int max30009_set_lead_off_polarity(struct max30009_dev *device, bool positive);

/** Enable or disable external lead-off detection for AC-coupled electrodes */
int max30009_enable_external_lead_off(struct max30009_dev *device, bool enable);

/** Enable or disable drive out-of-range detection */
int max30009_enable_drive_oor_detect(struct max30009_dev *device, bool enable);

/** Enable or disable ultra-low-power lead-on detection */
int max30009_enable_ultra_low_power_lead_on(struct max30009_dev *device,
		bool enable);

/** Get detailed lead detection status */
int max30009_get_detailed_lead_status(struct max30009_dev *device,
				      struct max30009_lead_status *status);

/** Configure lead bias resistor and enable for receive electrodes */
int max30009_set_lead_bias(struct max30009_dev *device,
			   enum max30009_rbias_val rbias_value,
			   bool enable_bip,
			   bool enable_bin);

/** Set complete PLL configuration */
int max30009_set_pll_config(struct max30009_dev *device,
			    struct max30009_pll_config *pll_config);

/** Get complete PLL configuration */
int max30009_get_pll_config(struct max30009_dev *device,
			    struct max30009_pll_config *pll_config);

/** Set complete FIFO configuration */
int max30009_set_fifo_config(struct max30009_dev *device,
			     struct max30009_fifo_config *fifo_config);

/** Get complete FIFO configuration */
int max30009_get_fifo_config(struct max30009_dev *device,
			     struct max30009_fifo_config *fifo_config);

/** Set complete BioZ configuration */
int max30009_set_bioz_config(struct max30009_dev *device,
			     struct max30009_bioz_config *bioz_config);

/** Get complete BioZ configuration */
int max30009_get_bioz_config(struct max30009_dev *device,
			     struct max30009_bioz_config *bioz_config);

/** Set complete BioZ MUX configuration */
int max30009_set_bioz_mux_config(struct max30009_dev *device,
				 struct max30009_bioz_mux_config *mux_config);

/** Get complete BioZ MUX configuration */
int max30009_get_bioz_mux_config(struct max30009_dev *device,
				 struct max30009_bioz_mux_config *mux_config);

/** Set complete lead detection configuration */
int max30009_set_lead_detect_config(struct max30009_dev *device,
				    struct max30009_lead_detect_config *lead_config);

/** Get complete lead detection configuration */
int max30009_get_lead_detect_config(struct max30009_dev *device,
				    struct max30009_lead_detect_config *lead_config);

/** Configure interrupt enables */
int max30009_interrupt_config(struct max30009_dev *device, uint8_t int1_mask,
			      uint8_t int2_mask);

/** Enable or disable a specific interrupt */
int max30009_enable_interrupt(struct max30009_dev *device,
			      enum max30009_interrupt interrupt,
			      bool enable);

/** Clear interrupt status registers */
int max30009_clear_status(struct max30009_dev *device);

#endif /* _MAX30009_H_ */
