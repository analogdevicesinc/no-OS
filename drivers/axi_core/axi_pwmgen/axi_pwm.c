/*******************************************************************************
 *   @file   axi_pwm.c
 *   @brief  Driver for the Analog Devices AXI PWM generator.
 *   @author Cristian Pop (cristian.pop@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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
 ******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include "no_os_pwm.h"
#include "axi_pwm_extra.h"
#include "no_os_axi_io.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "no_os_error.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define AXI_PWMGEN_REG_CORE_VERSION	0x00
#define AXI_PWMGEN_REG_ID		0x04
#define AXI_PWMGEN_REG_SCRATCHPAD	0x08
#define AXI_PWMGEN_REG_CORE_MAGIC	0x0C
#define AXI_PWMGEN_REG_CONFIG		0x10
#define AXI_PWMGEN_REG_NPWM		0x14
#define AXI_PWMGEN_CHX_PERIOD(p, ch) \
    (((p)->hw_major_ver == 1) ? (0x40 + 12 * (ch)) : (0x40 + 4 * (ch)))
#define AXI_PWMGEN_CHX_DUTY(p, ch) \
    (((p)->hw_major_ver == 1) ? (0x44 + 12 * (ch)) : (0x80 + 4 * (ch)))
#define AXI_PWMGEN_CHX_PHASE(p, ch) \
    (((p)->hw_major_ver == 1) ? (0x48 + 12 * (ch)) : (0xC0 + 4 * (ch)))
#define AXI_PWMGEN_TEST_DATA		0x5A0F0081
#define AXI_PWMGEN_LOAD_CONIG		NO_OS_BIT(1)
#define AXI_PWMGEN_RESET		NO_OS_BIT(0)
#define AXI_PWMGEN_CHANNEL_DISABLE	0
#define AXI_PWMGEN_MAX_CHANNELS(p)	(((p)->hw_major_ver == 1) ? 4 : 16)
#define NSEC_PER_USEC			1000L
#define USEC_PER_SEC			1000000L
#define AXI_PWMGEN_VERSION_MAJOR(x)	(((x) >> 16) & 0xff)
#define AXI_PWMGEN_VERSION_MINOR(x)	(((x) >> 8) & 0xff)
#define AXI_PWMGEN_VERSION_PATCH(x)	((x) & 0xff)

/******************************************************************************/
/************************** Functions Implementation **************************/
/******************************************************************************/

/**
 * @brief Write the pwmgen axi register using a bitmask
 *
 * @param [in] base - Device baseaddress
 * @param [in] offset - Device register offset
 * @param [in] mask - Bitmask used to write
 * @param [in] data - Data to write in register
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t axi_pwmgen_write_mask(uint32_t base, uint32_t offset,
				     uint32_t mask,
				     uint32_t data)
{
	int32_t ret;
	uint32_t temp;

	ret = no_os_axi_io_read(base, offset, &temp);
	if (ret != 0)
		return ret;

	return no_os_axi_io_write(base, offset, (temp & ~mask) | (data & mask));
}

/**
 * @brief Enable PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_enable(struct no_os_pwm_desc *desc)
{
	struct axi_pwm_desc *axi_desc = desc->extra;
	int ret;

	ret = no_os_axi_io_write(axi_desc->base_addr,
				 AXI_PWMGEN_CHX_PERIOD(axi_desc, axi_desc->channel),
				 axi_desc->ch_period);
	if (ret != 0)
		return ret;

	return no_os_axi_io_write(axi_desc->base_addr, AXI_PWMGEN_REG_CONFIG,
				  AXI_PWMGEN_LOAD_CONIG);
}

/**
 * @brief Disable PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_disable(struct no_os_pwm_desc *desc)
{
	struct axi_pwm_desc *axi_desc = desc->extra;
	int ret;

	ret = no_os_axi_io_write(axi_desc->base_addr,
				 AXI_PWMGEN_CHX_PERIOD(axi_desc, axi_desc->channel),
				 AXI_PWMGEN_CHANNEL_DISABLE);
	if (ret != 0)
		return ret;

	return no_os_axi_io_write(axi_desc->base_addr, AXI_PWMGEN_REG_CONFIG,
				  AXI_PWMGEN_LOAD_CONIG);
}

/**
 * @brief Set period of PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @param [in] period_ns - PWM period.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_set_period(struct no_os_pwm_desc *desc, uint32_t period_ns)
{
	struct axi_pwm_desc *axi_desc = desc->extra;
	uint32_t tmp, period_cnt;
	int32_t ret;

	/* Downscale by 1000 in order to avoid overflow when multiplying */
	tmp = (axi_desc->ref_clock_Hz / NSEC_PER_USEC) * period_ns;
	period_cnt = NO_OS_DIV_ROUND_UP(tmp, USEC_PER_SEC);
	axi_desc->ch_period = period_cnt;
	ret = no_os_axi_io_write(axi_desc->base_addr,
				 AXI_PWMGEN_CHX_PERIOD(axi_desc, axi_desc->channel),
				 desc->enabled ? period_cnt : 0);
	if (ret != 0)
		return ret;

	desc->period_ns = period_ns;

	return 0;
}

/**
 * @brief Get period of PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @param [out] period_ns - PWM period.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_get_period(struct no_os_pwm_desc *desc, uint32_t *period_ns)
{

	*period_ns = desc->period_ns;

	return 0;
}

/**
 * @brief Set duty cycle of PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @param [in] duty_cycle_ns - PWM duty cycle.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_set_duty_cycle(struct no_os_pwm_desc *desc,
			       uint32_t duty_cycle_ns)
{
	struct axi_pwm_desc *axi_desc = desc->extra;
	uint32_t tmp, duty_cnt;
	int32_t ret;

	if (duty_cycle_ns > desc->period_ns)
		duty_cycle_ns = desc->period_ns;

	/* Downscale by 1000 */
	tmp = (axi_desc->ref_clock_Hz / NSEC_PER_USEC) * duty_cycle_ns;
	duty_cnt = NO_OS_DIV_ROUND_UP(tmp, USEC_PER_SEC);
	ret = no_os_axi_io_write(axi_desc->base_addr,
				 AXI_PWMGEN_CHX_DUTY(axi_desc, axi_desc->channel),
				 duty_cnt);
	if (ret != 0)
		return ret;

	desc->duty_cycle_ns = duty_cycle_ns;

	return 0;
}

/**
 * @brief Get duty cycle of PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @param [out] duty_cycle_ns - PWM duty cycle.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_get_duty_cycle(struct no_os_pwm_desc *desc,
			       uint32_t *duty_cycle_ns)
{
	*duty_cycle_ns = desc->duty_cycle_ns;

	return 0;
}

/**
 * @brief Set phase of PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @param [in] phase_ns - PWM phase.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_set_phase(struct no_os_pwm_desc *desc, uint32_t phase_ns)
{
	struct axi_pwm_desc *axi_desc = desc->extra;
	uint32_t tmp, phase_cnt;
	int32_t ret;

	/* Downscale by 1000 */
	tmp = (axi_desc->ref_clock_Hz / NSEC_PER_USEC) * phase_ns;
	phase_cnt = NO_OS_DIV_ROUND_UP(tmp, USEC_PER_SEC);
	ret = no_os_axi_io_write(axi_desc->base_addr,
				 AXI_PWMGEN_CHX_PHASE(axi_desc, axi_desc->channel),
				 phase_cnt);
	if (ret != 0)
		return ret;

	desc->phase_ns = phase_ns;

	return 0;
}

/**
 * @brief Get phase of PWM generator device.
 *
 * @param [in] desc - Decriptor containing PWM generator parameters.
 * @param [out] phase_ns - PWM phase.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_get_phase(struct no_os_pwm_desc *desc, uint32_t *phase_ns)
{
	*phase_ns = desc->phase_ns;

	return 0;
}

/**
 * @brief Initialize the pwm axi generator and the handler associated with it.
 *
 * @param [out] desc - Decriptor containing PWM generator parameters.
 * @param [in] param - Structure containing the PWM generator init parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t axi_pwm_init(struct no_os_pwm_desc **desc,
		     const struct no_os_pwm_init_param *param)
{
	struct no_os_pwm_desc *pwm_desc;
	struct axi_pwm_init_param *axi_init;
	struct axi_pwm_desc *axi_desc;
	uint32_t data;
	int32_t ret;

	if (!param)
		return -1;

	axi_init = param->extra;

	pwm_desc = (struct no_os_pwm_desc *)no_os_calloc(1, sizeof(*pwm_desc));
	if (!pwm_desc)
		return -1;

	axi_desc = (struct axi_pwm_desc *)no_os_calloc(1, sizeof(*axi_desc));
	if (!axi_desc)
		goto error_desc;

	axi_desc->base_addr = axi_init->base_addr;
	axi_desc->ref_clock_Hz = axi_init->ref_clock_Hz;
	axi_desc->channel = axi_init->channel;
	pwm_desc->extra = axi_desc;
	pwm_desc->duty_cycle_ns = param->duty_cycle_ns;
	pwm_desc->period_ns = param->period_ns;
	pwm_desc->polarity = param->polarity;
	pwm_desc->phase_ns = param->phase_ns;

	ret = no_os_axi_io_read(axi_desc->base_addr,
				AXI_PWMGEN_REG_NPWM,
				&data);
	if (ret != 0)
		goto error_xdesc;

	if (axi_init->channel > (data - 1))
		goto error_xdesc;

	/* Enable the core */
	ret = axi_pwmgen_write_mask(axi_desc->base_addr, AXI_PWMGEN_REG_CONFIG,
				    AXI_PWMGEN_RESET, 0);
	if (ret != 0)
		goto error_xdesc;

	ret = no_os_axi_io_write(axi_desc->base_addr, AXI_PWMGEN_REG_SCRATCHPAD,
				 AXI_PWMGEN_TEST_DATA);
	if (ret != 0)
		goto error_xdesc;

	/* Get Hardware version */
	ret = no_os_axi_io_read(axi_desc->base_addr,
				AXI_PWMGEN_REG_CORE_VERSION,
				&data);
	if (ret != 0)
		goto error_xdesc;

	axi_desc->hw_major_ver = AXI_PWMGEN_VERSION_MAJOR(data);

	/** Check if channel is out of hw available range */
	if (axi_desc->channel >= AXI_PWMGEN_MAX_CHANNELS(axi_desc))
		goto error_xdesc;

	ret = no_os_axi_io_read(axi_desc->base_addr,
				AXI_PWMGEN_REG_SCRATCHPAD,
				&data);
	if (ret != 0)
		goto error_xdesc;

	if (data != AXI_PWMGEN_TEST_DATA)
		goto error_xdesc;

	ret = axi_pwm_set_period(pwm_desc, pwm_desc->period_ns);
	if (ret != 0)
		goto error_xdesc;

	ret = axi_pwm_set_duty_cycle(pwm_desc, pwm_desc->duty_cycle_ns);
	if (ret != 0)
		goto error_xdesc;

	ret = axi_pwm_set_phase(pwm_desc, pwm_desc->phase_ns);
	if (ret != 0)
		goto error_xdesc;

	ret = axi_pwm_enable(pwm_desc);
	if (ret != 0)
		goto error_xdesc;

	*desc = pwm_desc;

	return 0;

error_xdesc:
	no_os_free(axi_desc);
error_desc:
	no_os_free(pwm_desc);

	return -1;
}

/**
 * @brief Free the memory allocated by axi_pwmgen_init().
 * @param [in] desc - Pointer to the device handler.
 * @return 0 in case of success, -1 otherwise
 */
int32_t axi_pwm_remove(struct no_os_pwm_desc *desc)
{
	struct axi_pwm_desc *axi_desc = desc->extra;
	int32_t ret;

	if (!desc)
		return -1;

	ret = no_os_axi_io_write(axi_desc->base_addr,
				 AXI_PWMGEN_REG_CONFIG,
				 AXI_PWMGEN_RESET);
	if (ret != 0)
		return -1;

	no_os_free(axi_desc);
	no_os_free(desc);

	return 0;
}

/**
* @brief AXI platform specific PWM platform ops structure
*/
const struct no_os_pwm_platform_ops axi_pwm_ops = {
	.pwm_ops_init = &axi_pwm_init,
	.pwm_ops_enable = &axi_pwm_enable,
	.pwm_ops_disable = &axi_pwm_disable,
	.pwm_ops_set_period = &axi_pwm_set_period,
	.pwm_ops_get_period = &axi_pwm_get_period,
	.pwm_ops_set_duty_cycle = &axi_pwm_set_duty_cycle,
	.pwm_ops_get_duty_cycle = &axi_pwm_get_duty_cycle,
	.pwm_ops_set_phase = &axi_pwm_set_phase,
	.pwm_ops_get_phase = &axi_pwm_get_phase,
	.pwm_ops_remove = &axi_pwm_remove
};
