/***************************************************************************//**
 *   @file   max42500.c
 *   @brief  Source file of MAX42500 Driver.
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "max42500.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define CRC8_PEC        0x07      /* Implements Polynomial X^8 + X^2 + X^1 +1 */
#define	ECOMM           9         /* General communication error */

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/
/******************************************************************************/

/**
 * @brief 8-bit CRC computation
 * @param data - Data buffer.
 * @param len - Buffer length.
 * @return 8-bit CRC value
 */
static uint8_t crc8(uint8_t *data, uint8_t len)
{
	uint8_t crc = 0;
	bool msb_set;

	for (uint8_t idx = 0; idx < len; idx++) {
		crc ^= data[idx];

		for (uint8_t loop=0; loop < 8; loop++) {
			msb_set = ((crc & 0x80) == 0x80);
			crc <<= 1;

			/* Divide by PEC if bit 8 was set */
			if (msb_set)
				crc ^= CRC8_PEC;
		}
	}

	return crc;
}

/**
 * @brief 8-bit watchdog key computation
 * @param curr_wd_key - Current watchdog key value.
 * @return New 8-bit watchdog key value
 */
static uint8_t max42500_new_watchdog_key(uint8_t curr_wd_key)
{
	return curr_wd_key;
}

/**
 * @brief Set device state through EN0 and EN1 pins
 * @param state - Device state.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_state(struct max42500_dev *desc, enum max42500_state state)
{
	if (!desc)
		return -EINVAL;

	int ret;
	uint8_t en0;
	uint8_t en1;

	switch (state) {
	case MAX42500_STATE_OFF:
		en0 = NO_OS_GPIO_LOW;
		en1 = NO_OS_GPIO_LOW;
		break;

	case MAX42500_STATE_SLEEP:
		en0 = NO_OS_GPIO_HIGH;
		en1 = NO_OS_GPIO_LOW;
		break;

	case MAX42500_STATE_ON:
		en0 = NO_OS_GPIO_HIGH;
		en1 = NO_OS_GPIO_HIGH;
		break;

	default:
		en0 = NO_OS_GPIO_HIGH;
		en1 = NO_OS_GPIO_HIGH;
		break;
	}

	ret = no_os_gpio_set_value(desc->en0, en0);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(desc->en1, en1);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Read a raw value from a register.
 * @param desc - The device structure.
 * @param reg - Register structure holding info about the register to read from.
 * @return 0 in case of success, error code otherwise
 */
int max42500_reg_read(struct max42500_dev *desc, struct max42500_reg_st *reg)
{
	int ret;
	uint8_t i2c_data[MAX42500_I2C_RD_FRAME_SIZE] = {0};
	uint8_t bytes_number;
	uint8_t crc;

	/* PEC is computed over entire I2C frame from the first START condition */
	i2c_data[0] = (desc->comm_desc->slave_address << 1);
	i2c_data[1] = reg->addr;
	i2c_data[2] = (desc->comm_desc->slave_address << 1) | 0x1;

	/* I2C write target address */
	bytes_number = 1;

	ret = no_os_i2c_write(desc->comm_desc, &i2c_data[1], bytes_number, 0);
	if (ret)
		return ret;

	/* Change read byte count if PECE is enabled (1-byte data. 1-byte PEC) */
	bytes_number = (desc->pece) ? 2 : bytes_number;

	ret = no_os_i2c_read(desc->comm_desc, &i2c_data[3], bytes_number, 1);
	if (ret)
		return ret;

	if (desc->pece) {
		/* Compute CRC over entire I2C frame */
		crc = crc8(i2c_data, (MAX42500_I2C_RD_FRAME_SIZE - 1));

		if (i2c_data[4] != crc)
			return -ECOMM;
	}

	/* Update register with new read value */
	reg->value = i2c_data[3];

	return 0;
}

/**
 * @brief Write a raw value to a register.
 * @param desc - The device structure.
 * @param reg - Register structure holding info about the register to be written.
 * @return 0 in case of success, error code otherwise
 */
int max42500_reg_write(struct max42500_dev *desc, struct max42500_reg_st reg)
{
	uint8_t i2c_data[MAX42500_I2C_WR_FRAME_SIZE] = {0};
	uint8_t bytes_number;

	bytes_number = (desc->pece) ? (MAX42500_I2C_WR_FRAME_SIZE - 1) : 2;
	i2c_data[0] = (desc->comm_desc->slave_address << 1);
	i2c_data[1] = reg.addr;
	i2c_data[2] = reg.value;

	if (desc->pece)
		i2c_data[3] = crc8(i2c_data, bytes_number);

	return no_os_i2c_write(desc->comm_desc, &i2c_data[1], bytes_number, 1);
}

/**
 * @brief Wrap the write register function to give it a modern signature.
 * @param desc - The device structure
 * @param reg - Address of the register to be written.
 * @param writeval - New value for the register.
 * @return 0 in case of success, error code otherwise.
 */
int max42500_reg_write2(struct max42500_dev *desc,
			uint32_t reg,
			uint32_t writeval)
{
	desc->regs[reg].value = writeval;

	return max42500_reg_write(desc, desc->regs[reg]);
}

/**
 * @brief Set Packet Error Checking enable bit
 * @param desc - The device structure.
 * @param pece - The PECE bit value.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_pece(struct max42500_dev *desc, bool pece)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_CONFIG1];

	if (pece)
		reg.value |= MAX42500_CONFIG1_PECE_MASK;
	else
		reg.value &= ~MAX42500_CONFIG1_PECE_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_CONFIG1].value = reg.value;

	return 0;
}

/**
 * @brief Set built-in self-test mapping bit
 * @param desc - The device structure.
 * @param mbst - The MBST bit value.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_mbst(struct max42500_dev *desc, bool mbst)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_CONFIG1];

	if (mbst)
		reg.value |= MAX42500_CONFIG1_MBST_MASK;
	else
		reg.value &= ~MAX42500_CONFIG1_MBST_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_CONFIG1].value = reg.value;

	return 0;
}

/**
 * @brief Set OTP reload reset configuration bit
 * @param desc - The device structure.
 * @param rr - The RR bit value.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_rr(struct max42500_dev *desc, bool rr)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_CONFIG1];

	if (rr)
		reg.value |= MAX42500_CONFIG1_RR_MASK;
	else
		reg.value &= ~MAX42500_CONFIG1_RR_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_CONFIG1].value = reg.value;

	return 0;
}

/**
 * @brief Enable voltage monitoring on selected input
 * @param desc - The device structure.
 * @param vmon_en_mask - Mask corresponding to the enabled voltage inputs.
 *                       Bit set means VIN is enabled. Bit cleared is disabled.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_vmon_enable(struct max42500_dev *desc,
			     uint8_t vmon_en_mask)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_VMON];

	/* Bits set on the mask are enabled voltage inputs */
	reg.value = vmon_en_mask;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_VMON].value = reg.value;

	return 0;
}

/**
 * @brief Set voltage monitor power down
 * @param desc - The device structure.
 * @param vmpd_en - Enable/disable power down.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_vmon_pd_en(struct max42500_dev *desc,
			    bool vmpd_en)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_VMON];

	if (vmpd_en)
		reg.value |= MAX42500_VMON_VMPD_MASK;
	else
		reg.value &= ~MAX42500_VMON_VMPD_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_VMON].value = reg.value;

	return 0;
}

/**
 * @brief Enable input OV/UV mapping to reset assertion
 * @param desc - The device structure.
 * @param rst_map_mask - Mask corresponding to the mapped voltage inputs.
 *                       Bit set means VIN is mapped. Bit cleared is unmapped.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_rstmap_in(struct max42500_dev *desc,
			   uint8_t rstmap_mask)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_RSTMAP];

	/* Bits set on the mask are mapped voltage input assertions */
	reg.value = rstmap_mask;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_RSTMAP].value = reg.value;

	return 0;
}

/**
 * @brief Set parity check failure mapping to reset assertion
 * @param desc - The device structure.
 * @param parm - Enable/disable mapping.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_rstmap_parity(struct max42500_dev *desc,
			       bool parm_en)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_RSTMAP];

	if (parm_en)
		reg.value |= MAX42500_RSTMAP_PARM_MASK;
	else
		reg.value &= ~MAX42500_RSTMAP_PARM_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_RSTMAP].value = reg.value;

	return 0;
}

/**
 * @brief Set the watchdog mode
 * @param desc - The device structure.
 * @param wd_mode - Challenge/response or simple mode.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_mode(struct max42500_dev *desc,
			       enum max42500_wd_mode wd_mode)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_WDCDIV];

	if (wd_mode == MAX42500_WD_MODE_SIMPLE)
		reg.value |= MAX42500_WDCDIV_SWW_MASK;
	else
		reg.value &= ~MAX42500_WDCDIV_SWW_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_WDCDIV].value = reg.value;

	return 0;
}

/**
 * @brief Set the watchdog clock divider.
 * @param desc - The device structure.
 * @param wd_cdiv - Clock div fine tune value.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_clock_div(struct max42500_dev *desc,
				    uint8_t wd_cdiv)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_WDCDIV];

	reg.value &= ~MAX42500_WDCDIV_WDIC_MASK;
	reg.value |= wd_cdiv;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_WDCDIV].value = reg.value;

	return 0;
}

/**
 * @brief Set the watchdog open and close windows
 * @param desc - The device structure.
 * @param close_window - Watchdog close window.
 * @param open_window - Watchdog open window.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_window(struct max42500_dev *desc,
				 uint8_t close_window,
				 uint8_t open_window)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_WDCFG1];

	reg.value = ((close_window & 0xF) << 4) | (open_window & 0xF);

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_WDCFG1].value = reg.value;

	return 0;
}

/**
 * @brief Enable watchdog
 * @param desc - The device structure.
 * @param wd_enable - Enable/disable MAX42500 watchdog.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_enable(struct max42500_dev *desc, bool wd_enable)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_WDCFG2];

	if (!desc)
		return -EINVAL;

	if (wd_enable)
		reg.value |= MAX42500_WDCFG2_WDEN_MASK;
	else
		reg.value &= ~MAX42500_WDCFG2_WDEN_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_WDCFG2].value = reg.value;

	return 0;
}

/**
 * @brief Set watchdog first update window
 * @param desc - The device structure.
 * @param wd_1ud - First open window length after RESET.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_1ud_window(struct max42500_dev *desc, uint8_t wd_1ud)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_WDCFG2];

	reg.value &= ~MAX42500_WDCFG2_1UP_MASK;
	reg.value |= (wd_1ud & MAX42500_WDCFG2_1UP_MASK);

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_WDCFG2].value = reg.value;

	return 0;
}

/**
 * @brief Update the watchdog key based on the mode and current value
 * @param desc - The device structure.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_key(struct max42500_dev *desc)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_WDKEY];

	/* Compute new watchdog key for challenge/response mode */
	if (desc->wd_mode == MAX42500_WD_MODE_CH_RESP)
		reg.value = max42500_new_watchdog_key(reg.value);

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_WDKEY].value = reg.value;

	return 0;
}

/**
 * @brief Set watchdog lock
 * @param desc - The device structure.
 * @param wd_lock - Enable/disable MAX42500 watchdog register access.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_lock(struct max42500_dev *desc, bool wd_lock)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_WDLOCK];

	if (!desc)
		return -EINVAL;

	if (wd_lock)
		reg.value |= MAX42500_WDLOCK_LOCK_MASK;
	else
		reg.value &= ~MAX42500_WDLOCK_LOCK_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_WDLOCK].value = reg.value;

	return 0;
}

/**
 * @brief Set watchdog violation count before reset assertion
 * @param desc - The device structure.
 * @param mr1 - False - Assert after 1 violation
                True - Assert after 2 violations
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_mr1(struct max42500_dev *desc, bool mr1)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_RSTCTRL];

	if (!desc)
		return -EINVAL;

	if (mr1)
		reg.value |= MAX42500_RSTCTRL_MR1_MASK;
	else
		reg.value &= ~MAX42500_RSTCTRL_MR1_MASK;

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_RSTCTRL].value = reg.value;

	return 0;
}

/**
 * @brief Set watchdog reset hold time
 * @param desc - The device structure.
 * @param wd_1ud - First open window length after RESET.
 * @return 0 in case of success, error code otherwise
 */
int max42500_set_watchdog_rhld(struct max42500_dev *desc,
			       enum max42500_wd_rhld rhld)
{
	if (!desc)
		return -EINVAL;

	int ret;
	struct max42500_reg_st reg = desc->regs[MAX42500_RSTCTRL];

	reg.value &= ~MAX42500_RSTCTRL_RHLD_MASK;
	reg.value |= (rhld & MAX42500_RSTCTRL_RHLD_MASK);

	/* Write register */
	ret = max42500_reg_write(desc, reg);
	if (ret)
		return ret;

	/* Update register value */
	desc->regs[MAX42500_RSTCTRL].value = reg.value;

	return 0;
}

/**
 * @brief Initialize the device structure.
 * @param desc - The device structure to be initialized.
 * @param init_param - Initialization parameter for the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int max42500_init(struct max42500_dev **desc,
		  struct max42500_init_param *init_param)
{
	int ret;
	struct max42500_dev *descriptor;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	descriptor->regs = init_param->regs;
	descriptor->pece = false;

	/* Initialize GPIOs for device pins */
	ret = no_os_gpio_get(&descriptor->en0, &init_param->en0_param);
	if (ret)
		goto free_desc;

	ret = no_os_gpio_get(&descriptor->en1, &init_param->en1_param);
	if (ret)
		goto free_en0;

	ret = no_os_gpio_get(&descriptor->addr, &init_param->addr_param);
	if (ret)
		goto free_en1;

	/* Enable EN0 and EN1 GPIO output with default values */
	no_os_gpio_direction_output(descriptor->en0, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_addr;

	no_os_gpio_direction_output(descriptor->en1, NO_OS_GPIO_HIGH);
	if (ret)
		goto free_addr;

	/* Enable I2C addr selection */
	ret = no_os_gpio_direction_output(descriptor->addr, init_param->addr_sel);
	if (ret)
		goto free_addr;

	/* Initialize I2C communication */
	ret = no_os_i2c_init(&descriptor->comm_desc, &init_param->comm_param);
	if (ret)
		goto free_addr;

	/* Check device silicon ID */
	ret = max42500_reg_read(descriptor, &descriptor->regs[MAX42500_ID]);
	if (ret)
		goto free_i2c;

	if (descriptor->regs[MAX42500_ID].value != MAX42500_SILICON_ID) {
		ret = -ENODEV;
		goto free_i2c;
	}

	/* Initially read all registers */
	for (int8_t reg_id = MAX42500_CONFIG1; reg_id < MAX42500_REG_NO; reg_id++) {
		ret = max42500_reg_read(descriptor, &descriptor->regs[reg_id]);
		if (ret)
			goto free_i2c;
	}

	/* Configure PEC */
	ret = max42500_set_pece(descriptor, init_param->pece);
	if (ret)
		goto free_i2c;

	/* Set PEC enable/disable for subsequent register access */
	descriptor->pece = init_param->pece;

	/* Enable voltage monitor inputs */
	ret = max42500_set_vmon_enable(descriptor, init_param->vmon_en);
	if (ret)
		goto free_i2c;

	/* Enable voltage monitor power-down */
	ret = max42500_set_vmon_pd_en(descriptor, init_param->vmon_vmpd);
	if (ret)
		goto free_i2c;

	/* Enable input OV/UV mapping to reset pin */
	ret = max42500_set_rstmap_in(descriptor, init_param->reset_map);
	if (ret)
		goto free_i2c;

	/* Set watchdog mode */
	ret = max42500_set_watchdog_mode(descriptor, init_param->wd_mode);
	if (ret)
		goto free_i2c;

	/* Set watchdog clock div */
	ret = max42500_set_watchdog_clock_div(descriptor, init_param->wd_cdiv);
	if (ret)
		goto free_i2c;

	/* Set watchdog window */
	ret = max42500_set_watchdog_window(descriptor,
					   init_param->wd_close,
					   init_param->wd_open);
	if (ret)
		goto free_i2c;

	/* Set watchdog first update window */
	ret = max42500_set_watchdog_1ud_window(descriptor, init_param->wd_1ud);
	if (ret)
		goto free_i2c;

	/* Set watchdog enable */
	ret = max42500_set_watchdog_enable(descriptor, init_param->wd_en);
	if (ret)
		goto free_i2c;

	/* Initialize timer for watchdog */
	ret = no_os_timer_init(&descriptor->timer_desc, &init_param->timer_param);
	if (ret)
		goto free_i2c;

	/* Initialize IRQ controller */
	ret = no_os_irq_ctrl_init(&descriptor->irq_desc, &init_param->irq_param);
	if (ret)
		goto free_timer;

	/* Update watchdog parameters */
	descriptor->wd_mode = init_param->wd_mode;
	descriptor->wd_cdiv = init_param->wd_cdiv;
	descriptor->wd_close = init_param->wd_close;
	descriptor->wd_open = init_param->wd_open;
	descriptor->wd_1ud = init_param->wd_1ud;
	descriptor->wd_en = init_param->wd_en;

	/* Update descriptor */
	*desc = descriptor;

	return 0;

free_irq:
	no_os_irq_ctrl_remove(descriptor->irq_desc);
free_timer:
	no_os_timer_remove(descriptor->timer_desc);
free_i2c:
	no_os_i2c_remove(descriptor->comm_desc);
free_addr:
	no_os_gpio_remove(descriptor->addr);
free_en1:
	no_os_gpio_remove(descriptor->en1);
free_en0:
	no_os_gpio_remove(descriptor->en0);
free_desc:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the device descriptor.
 * @param desc - The device structure.
 * @return 0 in case of success, -EINVAL otherwise.
 */
int max42500_remove(struct max42500_dev *desc)
{
	int ret;

	if (!desc)
		return -EINVAL;

	ret = no_os_i2c_remove(desc->comm_desc);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(desc->addr);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(desc->en1);
	if (ret)
		return ret;

	ret = no_os_gpio_remove(desc->en0);
	if (ret)
		return ret;

	no_os_free(desc);

	return 0;
}
