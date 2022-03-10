// SPDX-License-Identifier: GPL-2.0
/**
 * \file ad917x_api.c
 *
 * \brief Contains AD917x APIs for DAC configuration and control
 *
 * release 1.1.x
 *
 * Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * By using this software you agree to the terms of the associated
 * Analog Devices Software License Agreement.
 */

#include "stddef.h"
#include "AD917x.h"
#include "ad917x_reg.h"
#include "api_errors.h"


#define HW_RESET_PERIOD_US 10
#define SPI_RESET_PERIOD_US 50
#define NVRAM_RESET_PERIOD_US 1000
#define DAC_9171_CLK_FREQ_MHZ_MAX 6000
#define DAC_CLK_FREQ_MHZ_MAX 12000
#define DAC_CLK_FREQ_MHZ_MIN 2900
#define REF_CLK_FREQ_MHZ_MIN 30
#define REF_CLK_FREQ_MHZ_MAX 2000
#define PFD_CLK_FREQ_MHZ_MIN 25
#define PFD_CLK_FREQ_MHZ_MAX 770
#define DLL_CLK_FREQ_THRES_HZ 4500000000ull
#define DAC_9171_CLK_FREQ_MAX_HZ 6000000000ull
#define DAC_CLK_FREQ_MAX_HZ 12000000000ull
#define DAC_CLK_FREQ_MIN_HZ 2900000000ull

/*======================================
 * Revision Data
 *=====================================*/
static uint8_t api_revision[3] = {1, 1, 1};
static uint16_t range_boundary[6] = {2910, 4140, 4370, 6210, 8740, 12420};

/*DataSheet Table 41*/
/*Engineering Sample Data*/
static struct adi_reg_data ADI_RECOMMENDED_BOOT_TBL[] = {
	{0x091, 0x00}, /*Power Up Clock Receiver*/
	{0x206, 0x01}, /*Power Up Phys*/
	{0x705, 0x01}, /*ADI INTERNAL:LOAD NVRAM FACTORY SETTINGS*/
	{0x090, 0x00}, /*TODO:Review Power on DACs and Bias CCT*/
};

static struct adi_reg_data ADI_RECOMMENDED_PLL_TBL_1[] = {
	{0x796, 0xE5}, /*DAC PLL Recommmended write*/
	{0x7A0, 0xBC}, /*DAC PLL Recommmended write*/
	{0x794, 0x08}, /*DAC PLL Charge Pump Current Recommmended write*/
	{0x797, 0x20}, /*DAC PLL Recommmended write*/
	{0x798, 0x10}, /*DAC PLL Recommmended write*/
	{0x7A2, 0x7F} /*DAC PLL Recommmended write*/


};

static struct adi_reg_data ADI_RECOMMENDED_DLL_TBL[] = {
	{0x0C0, 0x00}, /*Power Up delay Line*/
	{0x0DB, 0x00}, /*Implement Update*/
	{0x0DB, 0x01}, /*Implement Update*/
	{0x0DB, 0x00}, /*Implement Update*/
	{0x0C7, 0x01} /*Enable Status Read*/
};

static struct adi_reg_data ADI_RECOMMENDED_DAC_CAL_TBL[] = {
	{0x050, 0x2A}, /*ADI RECOMMENDED OPTIMIZED CALIBRATION*/
	{0x061, 0x68}, /*ADI RECOMMENDED OPTIMIZED CALIBRATION*/
	{0x051, 0x82}, /*ADI RECOMMENDED OPTIMIZED CALIBRATION*/
	{0x051, 0x83}, /*ADI RECOMMENDED OPTIMIZED CALIBRATION*/
	{0x081, 0x03} /*ADI RECOMMENDED OPTIMIZED CALIBRATION*/
};

static int32_t check_dac_clk_freq_range(ad917x_handle_t *h,
					uint64_t dac_clk_freq_hz)
{
	adi_chip_id_t adi_chip_id;
	int32_t err;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	err = ad917x_get_chip_id(h, &adi_chip_id);
	if (err != API_ERROR_OK)
		return err;
	if (adi_chip_id.prod_id == AD9171_ID) {
		if ((dac_clk_freq_hz > DAC_9171_CLK_FREQ_MAX_HZ) ||
		    (dac_clk_freq_hz < DAC_CLK_FREQ_MIN_HZ))
			return API_ERROR_INVALID_PARAM;
	} else {
		if ((dac_clk_freq_hz > DAC_CLK_FREQ_MAX_HZ) ||
		    (dac_clk_freq_hz < DAC_CLK_FREQ_MIN_HZ))
			return API_ERROR_INVALID_PARAM;
	}
	return API_ERROR_OK;
}
static int32_t spi_configure(ad917x_handle_t *h)
{
	switch (h->sdo) {
	case SPI_SDO:
		return ad917x_register_write(h, AD917X_IF_CFG_A_REG, 0x18);
	case SPI_SDIO:
		return ad917x_register_write(h, AD917X_IF_CFG_A_REG, 0x00);
	default:
		return API_ERROR_SPI_SDO;
		break;
	}
	return API_ERROR_SPI_SDO;
}

static int32_t sysref_configure(ad917x_handle_t *h)
{
	uint8_t tmp_reg;
	int32_t err;

	err = ad917x_register_read(h, AD917X_SYSREF_CTRL_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~(AD917X_SYSREF_PD);
	if (h->sysref == COUPLING_DC)
		tmp_reg |= AD917X_SYSREF_PD;
	return ad917x_register_write(h, AD917X_SYSREF_CTRL_REG, tmp_reg);
}

static int32_t syncoutb_configure(ad917x_handle_t *h)
{
	if (h->syncoutb == SIGNAL_LVDS) {
		return ad917x_register_write(h,
					     AD917X_SYNCOUTB_CTRL_0_REG, AD917X_SYNCOUTB_MODE(0x1));

	} else {
		return ad917x_register_write(h,
					     AD917X_SYNCOUTB_CTRL_0_REG, AD917X_SYNCOUTB_MODE(0x0));
	}
}


static int32_t dac_init_sequence(ad917x_handle_t *h)
{
	int32_t err;
	uint8_t tmp_reg = 0x0;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	/*Boot from NVRAM & Check Boot Status*/
	err = ad917x_register_write_tbl(h, &ADI_RECOMMENDED_BOOT_TBL[0],
					NO_OS_ARRAY_SIZE(ADI_RECOMMENDED_BOOT_TBL));
	if (err != API_ERROR_OK)
		return err;

	if (h->delay_us != NULL) {
		err = h->delay_us(h->user_data, NVRAM_RESET_PERIOD_US);
		if (err != 0)
			return API_ERROR_US_DELAY;
	}
	err = ad917x_register_read(h, AD917X_NVM_LOADER_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	if (!(tmp_reg & AD917X_NVM_BLR_DONE))
		return API_ERROR_INIT_SEQ_FAIL;

	return API_ERROR_OK;
}
int32_t ad917x_init(ad917x_handle_t *h)
{
	int32_t err;
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (h->dev_xfer == NULL)
		return API_ERROR_INVALID_XFER_PTR;
	if (h->delay_us == NULL)
		return API_ERROR_INVALID_DELAYUS_PTR;
	if (h->sdo >= SPI_CONFIG_MAX)
		return API_ERROR_SPI_SDO;
	if (h->hw_open != NULL) {
		err = h->hw_open(h->user_data);
		if (err != 0)
			return API_ERROR_HW_OPEN;
	}

	err = spi_configure(h);
	if (err != API_ERROR_OK)
		return err;

	err = syncoutb_configure(h);
	if (err != API_ERROR_OK)
		return err;

	err = sysref_configure(h);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;
}

int32_t ad917x_deinit(ad917x_handle_t *h)
{
	int32_t err;
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (h->hw_close != NULL) {
		err = h->hw_close(h->user_data);
		if (err != 0)
			return API_ERROR_HW_CLOSE;
	}
	return API_ERROR_OK;
}

int32_t ad917x_get_chip_id(ad917x_handle_t *h, adi_chip_id_t *chip_id)
{
	int32_t err;
	uint8_t tmp_reg;
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (chip_id == NULL)
		return API_ERROR_INVALID_PARAM;
	err = ad917x_register_read(h, AD917X_CHIP_TYPE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->chip_type = tmp_reg;

	err = ad917x_register_read(h, AD917X_PROD_ID_MSB_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->prod_id = tmp_reg;
	chip_id->prod_id <<= 8;

	err = ad917x_register_read(h, AD917X_PROD_ID_LSB_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->prod_id |= tmp_reg;

	err = ad917x_register_read(h, AD917X_CHIP_GRADE_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	chip_id->prod_grade = (tmp_reg >> 4);
	chip_id->dev_revision = (tmp_reg & 0x0F);

	return API_ERROR_OK;
}

int32_t ad917x_reset(ad917x_handle_t *h, uint8_t hw_reset)
{
	int32_t err;
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	if (hw_reset > 1)
		return API_ERROR_INVALID_PARAM;
	if (hw_reset) {
		if (h->reset_pin_ctrl == NULL)
			return API_ERROR_INVALID_PARAM;
		err = h->reset_pin_ctrl(h->user_data, 0x1);
		if (err != 0)
			return API_ERROR_RESET_PIN_CTRL;
		if (h->delay_us != NULL) {
			err = h->delay_us(h->user_data, HW_RESET_PERIOD_US);
			if (err != 0)
				return API_ERROR_US_DELAY;
		}
		err = h->reset_pin_ctrl(h->user_data, 0x0);
		if (err != 0)
			return API_ERROR_RESET_PIN_CTRL;
	}

	switch (h->sdo) {
	case SPI_SDO:
		err = ad917x_register_write(h, AD917X_IF_CFG_A_REG, 0x99);
		if (err != API_ERROR_OK)
			return err;
		break;
	case SPI_SDIO:
		err = ad917x_register_write(h, AD917X_IF_CFG_A_REG, 0x81);
		if (err != API_ERROR_OK)
			return err;
		break;
	default:
		return API_ERROR_SPI_SDO;
		break;
	}

	if (h->delay_us != NULL) {
		err = h->delay_us(h->user_data, SPI_RESET_PERIOD_US);
		if (err != 0)
			return API_ERROR_US_DELAY;
	}

	err = syncoutb_configure(h);
	if (err != API_ERROR_OK)
		return err;

	err = sysref_configure(h);
	if (err != API_ERROR_OK)
		return err;

	return dac_init_sequence(h);

}

int32_t ad917x_get_revision(ad917x_handle_t *h, uint8_t *rev_major,
			    uint8_t *rev_minor, uint8_t *rev_rc)
{
	int32_t err = API_ERROR_OK;

	if (rev_major != NULL)
		*rev_major = api_revision[0];
	else
		err = API_ERROR_INVALID_PARAM;
	if (rev_minor != NULL)
		*rev_minor = api_revision[1];
	else
		err = API_ERROR_INVALID_PARAM;
	if (rev_rc != NULL)
		*rev_rc = api_revision[2];
	else
		err = API_ERROR_INVALID_PARAM;

	return err;
}


int32_t ad917x_set_dac_pll_config(ad917x_handle_t *h, uint8_t dac_pll_en,
				  uint8_t m_div, uint8_t n_div, uint8_t vco_div)
{

	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	/*check Parameter valid Ranges */
	if ((dac_pll_en > 1) || (m_div > 4) || (m_div < 1) || (n_div < 2) ||
	    (n_div > 50) || (vco_div < 1) || (vco_div > 3))
		return API_ERROR_INVALID_PARAM;
	err = ad917x_register_write(h,
				    AD917X_PLL_BYPASS_REG, AD917X_PLL_BYPASS(!dac_pll_en));
	if (err != API_ERROR_OK)
		return err;
	if (!dac_pll_en) {
		err = ad917x_register_write(h,
					    AD917X_DACPLL_CTRLX_REG, 0xFF); /*ADI REC WRITE*/
		if (err != API_ERROR_OK)
			return err;
		err = ad917x_register_write(h,
					    AD917X_DACPLL_CTRLY_REG, 0xFF); /*ADI REC WRITE*/
		if (err != API_ERROR_OK)
			return err;
		return API_ERROR_OK;
	} else {
		err = ad917x_register_write(h,
					    AD917X_DACPLL_CTRLX_REG, 0x00); /*ADI REC WRITE*/
		if (err != API_ERROR_OK)
			return err;
		err = ad917x_register_write(h,
					    AD917X_DACPLL_CTRLY_REG, 0x00); /*ADI REC WRITE*/
		if (err != API_ERROR_OK)
			return err;
	}

	/*Configure On-Chip PLL*/
	/*Initialise PLL*/
	/*Call PLL Recommended Init Sequence*/
	err = ad917x_register_write_tbl(h, &ADI_RECOMMENDED_PLL_TBL_1[0],
					NO_OS_ARRAY_SIZE(ADI_RECOMMENDED_PLL_TBL_1));
	if (err != API_ERROR_OK)
		return err;
	if (h->delay_us != NULL)
		h->delay_us(h->user_data, 100000);

	/*Set  M Divider*/
	err = ad917x_register_read(h, AD917X_DACPLL_CTRL1_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD917X_M_DIV(ALL);
	tmp_reg |= AD917X_M_DIV((m_div - 1));
	err = ad917x_register_write(h, AD917X_DACPLL_CTRL1_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/* Set N Divider */
	err = ad917x_register_read(h, AD917X_DACPLL_CTRL7_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD917X_N_DIV(ALL);
	tmp_reg |= AD917X_N_DIV(n_div);
	err = ad917x_register_write(h, AD917X_DACPLL_CTRL7_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Set PLL VCO DIV */
	tmp_reg = AD917X_PLL_VCO_DIV_EN(vco_div - 1);
	err = ad917x_register_write(h, AD917X_PLL_VCO_CTRL_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Reset VCO*/
	if (h->delay_us != NULL)
		h->delay_us(h->user_data, 1000);

	err = ad917x_register_read(h, AD917X_DACPLL_CTRL0_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg |= AD917X_RESET_VCO_DIV;
	err = ad917x_register_write(h, AD917X_DACPLL_CTRL0_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD917X_RESET_VCO_DIV;
	err = ad917x_register_write(h, AD917X_DACPLL_CTRL0_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	return API_ERROR_OK;

}


int32_t ad917x_set_dac_clk_freq(ad917x_handle_t *h,
				uint64_t dac_clk_freq_hz)
{
	int32_t err;
	uint8_t tmp_reg = 0x0;
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	err = check_dac_clk_freq_range(h, dac_clk_freq_hz);
	if (err != API_ERROR_OK)
		return err;

	h->dac_freq_hz = (uint64_t)(dac_clk_freq_hz);
	/*Call DLL Recommended Init Sequence*/
	err = ad917x_register_write_tbl(h, &ADI_RECOMMENDED_DLL_TBL[0],
					NO_OS_ARRAY_SIZE(ADI_RECOMMENDED_DLL_TBL));
	if (err != API_ERROR_OK)
		return err;

	/*Reset DLL*/
	if (dac_clk_freq_hz < DLL_CLK_FREQ_THRES_HZ)
		tmp_reg = 0x48;
	else
		tmp_reg = 0x68;
	err = ad917x_register_write(h, AD917X_DLL_CTRL0_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg |= AD917X_DLL_RST;
	err = ad917x_register_write(h, AD917X_DLL_CTRL0_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;

	/*Call DAC Calibration Recommended Init Sequence*/
	err = ad917x_register_write_tbl(h, &ADI_RECOMMENDED_DAC_CAL_TBL[0],
					NO_OS_ARRAY_SIZE(ADI_RECOMMENDED_DAC_CAL_TBL));
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}


int32_t ad917x_get_dac_clk_freq(ad917x_handle_t *h,
				uint64_t *dac_clk_freq_hz)
{
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (dac_clk_freq_hz == NULL)
		return API_ERROR_INVALID_PARAM;
	*dac_clk_freq_hz = h->dac_freq_hz;
	return API_ERROR_OK;
}

int32_t ad917x_get_dac_clk_status(ad917x_handle_t *h,
				  uint8_t *pll_lock_stat, uint8_t *dll_lock_stat)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (pll_lock_stat != NULL) {

		err = ad917x_register_read(h, AD917X_DACPLL_STATUS_REG, &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		*pll_lock_stat = tmp_reg & AD917X_DACPLL_LOCK;
	}
	if (dll_lock_stat != NULL) {

		err = ad917x_register_read(h, AD917X_DLL_STATUS_REG, &tmp_reg);
		if (err != API_ERROR_OK)
			return err;
		*dll_lock_stat = tmp_reg & AD917X_DLL_LOCK;
	}
	return API_ERROR_OK;
}

int32_t ad917x_set_clkout_config(ad917x_handle_t *h, uint8_t l_div)
{
	int32_t err;
	uint8_t tmp_reg;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((l_div < 1) || (l_div > 4))
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_DACPLL_CTRL7_REG, &tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	tmp_reg &= ~AD917X_L_DIV(ALL);
	tmp_reg |= AD917X_L_DIV((l_div - 1));
	err = ad917x_register_write(h, AD917X_DACPLL_CTRL7_REG, tmp_reg);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;
}

int32_t ad917x_set_dac_clk(ad917x_handle_t *h,
			   uint64_t dac_clk_freq_hz, uint8_t dac_pll_en, uint64_t ref_clk_freq_hz)
{
	int32_t err;
	uint8_t m_div, n_div, pll_vco_div;
	uint16_t n_div_tmp, ref_clk_freq_mhz, pfd_clk_freq_mhz, target_pfd,
		 fvco_freq_mhz = 0x0;
	uint64_t dac_clk_freq_mhz;

	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if (dac_pll_en > 1)
		return API_ERROR_INVALID_PARAM;

	err = check_dac_clk_freq_range(h, dac_clk_freq_hz);
	if (err != API_ERROR_OK)
		return err;

	err = ad917x_register_write(h,
				    AD917X_PLL_BYPASS_REG, AD917X_PLL_BYPASS(!dac_pll_en));
	if (err != API_ERROR_OK)
		return err;
	dac_clk_freq_mhz = NO_OS_DIV_U64(dac_clk_freq_hz, 1000000);
	if (dac_pll_en) {
		/*Generate On chip PLL Configuration*/
		/*Check REF CLK within 30MHz to 2GHz Range*/
		ref_clk_freq_mhz = NO_OS_DIV_U64(ref_clk_freq_hz, 1000000);
		if ((ref_clk_freq_mhz < REF_CLK_FREQ_MHZ_MIN) ||
		    (ref_clk_freq_mhz > REF_CLK_FREQ_MHZ_MAX))
			return API_ERROR_INVALID_PARAM;

		/*Determine FVCO Frequency and Check DAC CLK withing PLL Range*/
		if ((dac_clk_freq_mhz > range_boundary[0]) &&
		    (dac_clk_freq_mhz < range_boundary[1])) {
			fvco_freq_mhz = (dac_clk_freq_mhz * 3);
			pll_vco_div = 3;
		} else if ((dac_clk_freq_mhz > range_boundary[2]) &&
			   (dac_clk_freq_mhz < range_boundary[3])) {
			fvco_freq_mhz = (dac_clk_freq_mhz * 2);
			pll_vco_div = 2;
		} else if ((dac_clk_freq_mhz > range_boundary[4]) &&
			   (dac_clk_freq_mhz < range_boundary[5])) {
			fvco_freq_mhz = (dac_clk_freq_mhz * 1);
			pll_vco_div = 1;
		} else
			return API_ERROR_INVALID_PARAM;

		if ((fvco_freq_mhz >= 9960) && (fvco_freq_mhz <= 10870))
			target_pfd = 220; /* less than 225 */
		else
			target_pfd = 500; /* less than 770 */

		/*Determine divM and check PFD Frequency within Range*/
		m_div = ref_clk_freq_mhz / target_pfd;
		if (ref_clk_freq_mhz % target_pfd)
			m_div++;

		pfd_clk_freq_mhz = ref_clk_freq_mhz / m_div;

		if ((pfd_clk_freq_mhz < PFD_CLK_FREQ_MHZ_MIN) ||
		    (pfd_clk_freq_mhz > PFD_CLK_FREQ_MHZ_MAX))
			return API_ERROR_INVALID_PARAM;

		/*Calculate N Divider using FVCO Frequency*/
		n_div_tmp = (fvco_freq_mhz * (m_div));
		n_div_tmp = NO_OS_DIV_ROUND_CLOSEST(n_div_tmp * 1000,
						    (uint32_t) NO_OS_DIV_U64(ref_clk_freq_hz, 1000));
		n_div = NO_OS_DIV_ROUND_CLOSEST(n_div_tmp, 8);

		/*Initialise PLL*/
		/*Call PLL Recommended Init Sequence*/
		err = ad917x_register_write_tbl(h, &ADI_RECOMMENDED_PLL_TBL_1[0],
						NO_OS_ARRAY_SIZE(ADI_RECOMMENDED_PLL_TBL_1));
		if (err != API_ERROR_OK)
			return err;
		if (h->delay_us != NULL)
			h->delay_us(h->user_data, 100000);

		/*Set the PLL Dividers */
		err = ad917x_set_dac_pll_config(h, 1, m_div, n_div, pll_vco_div);
		if (err != API_ERROR_OK)
			return err;

	}
	/*Call Set Dac Frequency Sequence*/
	err = ad917x_set_dac_clk_freq(h, dac_clk_freq_hz);
	if (err != API_ERROR_OK)
		return err;
	return API_ERROR_OK;

}

int32_t ad917x_set_page_idx(ad917x_handle_t *h, const uint32_t dac,
			    const uint32_t channel)
{
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;

	return ad917x_register_write(h, AD917X_SPI_PAGEINDX_REG,
				     (uint8_t)((dac << 6) | channel));
}

int32_t ad917x_get_page_idx(ad917x_handle_t *h, int32_t *dac, int32_t *channel)
{
	int32_t err;
	uint8_t tmp;
	if (h == NULL)
		return API_ERROR_INVALID_HANDLE_PTR;
	if ((dac == NULL) || (channel == NULL))
		return API_ERROR_INVALID_PARAM;

	err = ad917x_register_read(h, AD917X_SPI_PAGEINDX_REG, &tmp);
	if (err != API_ERROR_OK)
		return err;
	*dac = tmp >> 6;
	*channel = tmp & 0x3F;
	return API_ERROR_OK;
}

