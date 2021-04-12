/*******************************************************************************
 * @file altera_pll_common.c
 * @brief Implementation of a10-mypll Driver.
 * @author andrei.grozav@analog.com)
 ********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 * of one or more patent holders. This license does not release you
 * from the requirement that you obtain separate licenses from these
 * patent holders to use this software.
 * - Use of the software either in source or binary form, must be run
 * on or directly connected to an Analog Devices Inc. component.
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

#ifdef ALTERA
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "altera_pll_common.h"

/*******************************************************************************
* @brief a10_pll_read
*******************************************************************************/
int32_t a10_pll_read(xcvr_pll *mypll,
		uint32_t reg_addr,
		uint32_t *reg_data)
{
	reg_addr = reg_addr << 2; // left shift 2 (byte align address)
	*reg_data = ad_reg_read((mypll->base_address + reg_addr));

	return 0;
}

/*******************************************************************************
* @brief a10_pll_write
*******************************************************************************/
int32_t a10_pll_write(xcvr_pll *mypll,
		uint32_t reg_addr,
		uint32_t reg_data)
{
	reg_addr = reg_addr << 2; // left shift 2 (byte align address)
	ad_reg_write((mypll->base_address + reg_addr), reg_data);

	return 0;
}

/*******************************************************************************
* @brief pll_update
*******************************************************************************/
void pll_update(xcvr_pll *mypll,
	uint32_t addr, uint32_t mask, uint32_t val)
{
	uint32_t rval;

	a10_pll_read(mypll, addr, &rval);
	rval &= ~mask;
	rval |= val;
	a10_pll_write(mypll, addr, rval);
}

/*******************************************************************************
* @brief altera_a10_acquire_arbitration
*******************************************************************************/
uint32_t altera_a10_acquire_arbitration(xcvr_pll *mypll)
{
	uint32_t timeout = 0;
	uint32_t status;
	uint32_t arb_status;

	switch (mypll->type) {
		case fpll_type:
		case atx_type:
			arb_status = XCVR_REG_CAPAB_PLL_STAT;
			break;
		case cmu_cdr_type: // PMA
		case cmu_tx_type:
			arb_status = XCVR_REG_CAPAB_PMA;
			break;
		default:
			printf("%s: Pll type not defined!\n", __func__);
			return -1;
		}

	a10_pll_write(mypll, XCVR_REG_ARBITRATION, XCVR_ARBITRATION_GET_AVMM);

	do {
		a10_pll_read(mypll, arb_status, &status);
		if ((status & BIT(2)) == 0) {
			return 0;
		}
		mdelay(1);
	} while (timeout++ < 100);

	printf("%s: Failed to acquire arbitration\n", __func__);

	return -1;
}

/*******************************************************************************
* @brief altera_a10_release_arbitration
*******************************************************************************/
void altera_a10_release_arbitration(xcvr_pll *mypll,
	uint8_t run_calibration)
{
	a10_pll_write(mypll, XCVR_REG_ARBITRATION, (run_calibration ? 0x1 : 0x3));
}

/*******************************************************************************
* @brief pll_calibration_check
*******************************************************************************/
uint8_t pll_calibration_check(xcvr_pll *my_pll)
{
	uint32_t timeout = 0;
	uint32_t val;
	uint32_t mask;
	uint32_t arb_status;
	const char *msg;

	switch (my_pll->type) {
		case fpll_type:
			arb_status = XCVR_REG_CAPAB_PLL_STAT;
			mask = XCVR_CAPAB_PLL_CAL_BSY_MASK;
			msg = "FPLL calibration";
			break;
		case atx_type:
			arb_status = XCVR_REG_CAPAB_PLL_STAT;
			mask = XCVR_CAPAB_PLL_CAL_BSY_MASK;
			msg = "ATX PLL calibration";
			break;
		case cmu_cdr_type: // PMA RX
			arb_status = XCVR_REG_CAPAB_PMA;
			mask = XCVR_CAPAB_RX_CAL_BUSY_MASK;
			msg = "CMU/CDR PLL calibration";
			break;
		case cmu_tx_type: // PMA TX
			arb_status = XCVR_REG_CAPAB_PMA;
			mask = XCVR_CAPAB_TX_CAL_BUSY_MASK;
			msg = "CMU/TX PLL calibration";
			break;
		default:
			printf("%s: %x Pll type not defined!\n", __func__,
				my_pll->type);
			return -1;
		}
	/* Wait max 100ms for cal_busy to de-assert */
	do {
		udelay(100);

		/* Read PLL calibration status from capability register */
		a10_pll_read(my_pll, arb_status, &val);
		if ((val & mask) == 0x00) {
#ifdef DEBUG
			printf("%s OK (%d us, addr 0x%x)\n", msg,
				timeout * 100, my_pll->base_address);
#endif
			return 0;
		}
	} while (timeout++ < 1000);

	printf("%s FAILED (addr 0x%x)\n", msg,
		my_pll->base_address);

	return 1;
}

/*******************************************************************************
* @brief pll_is_enabled
*******************************************************************************/
uint32_t pll_is_enabled(xcvr_pll *mypll)
{
	switch (mypll->type) {
		case fpll_type:
		case atx_type:
			break;
		case cmu_cdr_type: // PMA RX
		case cmu_tx_type: // PMA TX
			printf("%s: PMA has no powerdown option!\n",
				__func__);
			return 0;
		default:
			printf("%s: Pll type not defined!\n", __func__);
			return -1;
		}
	uint32_t ret;
	a10_pll_read(mypll, 0x2e0, &ret);
	return ret != 0x3;
}

/*******************************************************************************
* @brief pll_enable
*******************************************************************************/
uint32_t pll_enable(xcvr_pll *mypll)
{
	switch (mypll->type) {
		case fpll_type:
		case atx_type:
			break;
		case cmu_cdr_type: // PMA RX
		case cmu_tx_type: // PMA TX
			printf("%s: PMA has no powerdown option!\n",
				__func__);
			return 0;
		default:
			printf("%s: Pll type not defined!\n", __func__);
			return -1;
		}
		a10_pll_write(mypll, 0x2e0, 0x2);
		return 0;
}

/*******************************************************************************
* @brief pll_disable
*******************************************************************************/
uint32_t pll_disable(xcvr_pll *mypll)
{
	switch (mypll->type) {
		case fpll_type:
		case atx_type:
			break;
		case cmu_cdr_type: // PMA RX
		case cmu_tx_type: // PMA TX
			printf("%s: PMA has no powerdown option!\n", __func__);
			return 0;
		default:
			printf("%s: Pll type not defined!\n", __func__);
			return -1;
		}
		a10_pll_write(mypll, 0x2e0, 0x3);
	return 0;
}
#endif

/*******************************************************************************/
/*******************************************************************************/
